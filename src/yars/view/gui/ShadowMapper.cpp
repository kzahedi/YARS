#include "ShadowMapper.h"

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMovableObject.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <iostream>

namespace yars {

const char *ShadowMapper::RTT_NAME = "YarsShadowRTT";
const char *ShadowMapper::CAST_SCHEME = "yars-shadow-cast";

namespace {

/// When the active scheme is "yars-shadow-cast" and a material doesn't
/// have a technique tagged with that scheme, return a copy of the
/// YARS/CustomShadowCast material's technique. This avoids having to
/// manually annotate every .material file.
class ShadowCastSchemeListener : public Ogre::MaterialManager::Listener
{
public:
    Ogre::Technique *handleSchemeNotFound(
        unsigned short /*schemeIndex*/,
        const Ogre::String &schemeName,
        Ogre::Material *originalMaterial,
        unsigned short /*lodIndex*/,
        const Ogre::Renderable * /*rend*/) override
    {
        if (schemeName != ShadowMapper::CAST_SCHEME) return nullptr;
        if (originalMaterial->getName() == "YARS/CustomShadowCast") return nullptr;

        Ogre::MaterialPtr castMat = Ogre::MaterialManager::getSingleton()
            .getByName("YARS/CustomShadowCast");
        if (!castMat) return nullptr;
        castMat->load();
        // Return the first technique of YARS/CustomShadowCast.
        return castMat->getNumTechniques() > 0 ? castMat->getTechnique(0) : nullptr;
    }
};

static ShadowCastSchemeListener *gShadowCastListener = nullptr;

} // anonymous namespace

ShadowMapper::ShadowMapper(Ogre::SceneManager *sm, float arenaSize)
    : _sm(sm), _arenaSize(arenaSize), _shadowCam(nullptr),
      _shadowCamNode(nullptr), _shadowVp(nullptr)
{
    // Install the shadow-cast scheme listener so every material gets a
    // silhouette technique on demand.
    if (!gShadowCastListener) {
        gShadowCastListener = new ShadowCastSchemeListener();
        Ogre::MaterialManager::getSingleton().addListener(gShadowCastListener);
    }

    // 1) Create the RTT.
    //
    // Resource group "YARS" (not the default General group): the
    // YARSGroundShadowed.material lives in the YARS group and Ogre's
    // material parser resolves textures against the same group during
    // initialiseResourceGroup. With the RTT in the General group, the
    // parser tries to file-load YarsShadowRTT from the YARS archive
    // and emits a misleading "Cannot locate resource YarsShadowRTT in
    // resource group YARS" line in ogre.log even though Ogre
    // re-resolves the texture by name later and shadows still render.
    //
    // The companion change in materials/YARSGroundShadowed.material
    // removes the `texture YarsShadowRTT` line from the shadow
    // texture_unit; the texture is bound programmatically below (and
    // re-bound whenever the material is reloaded, via the manual
    // setTextureName call) so the material script no longer tries to
    // declare a texture that has different parameters from the manual
    // one — which would trigger an "overriding previous declarations"
    // script-compiler warning.
    _rtt = Ogre::TextureManager::getSingleton().createManual(
        RTT_NAME,
        "YARS",
        Ogre::TEX_TYPE_2D,
        1024, 1024,
        0, // no mipmaps
        Ogre::PF_R8,
        Ogre::TU_RENDERTARGET);

    // 1b) Bind the RTT to the GroundShadowed material's shadow
    // texture_unit. The material script intentionally leaves the
    // shadow texture_unit's texture name unset (see comment above).
    {
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton()
            .getByName("YARS/GroundShadowed", "YARS");
        if (mat)
        {
            mat->load();
            if (mat->getNumTechniques() > 0)
            {
                Ogre::Technique *tech = mat->getTechnique(0);
                if (tech && tech->getNumPasses() > 0)
                {
                    Ogre::Pass *pass = tech->getPass(0);
                    // Unit 1 is the shadow RTT (Unit 0 is the ground
                    // diffuse texture).
                    if (pass && pass->getNumTextureUnitStates() > 1)
                    {
                        pass->getTextureUnitState(1)->setTextureName(RTT_NAME);
                    }
                }
            }
        }
    }

    Ogre::RenderTarget *rt = _rtt->getBuffer()->getRenderTarget();
    rt->setAutoUpdated(false); // we drive it manually each frame

    // 2) Create the top-down orthographic camera.
    _shadowCam = _sm->createCamera("YarsShadowCam");
    _shadowCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    // Ortho window covers [-arenaSize, +arenaSize] in both X and Z
    // when viewed from above (Ogre world).
    _shadowCam->setOrthoWindow(_arenaSize * 2.0f, _arenaSize * 2.0f);
    _shadowCam->setNearClipDistance(0.5f);
    _shadowCam->setFarClipDistance(100.0f);
    // Camera looks straight down (-Y in Ogre world).
    _shadowCamNode = _sm->getRootSceneNode()->createChildSceneNode("YarsShadowCamNode");
    _shadowCamNode->setPosition(0.0f, 50.0f, 0.0f);
    _shadowCamNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD);
    _shadowCamNode->attachObject(_shadowCam);

    // 3) Attach a viewport that clears to white (1,1,1 = "no shadow").
    _shadowVp = rt->addViewport(_shadowCam);
    _shadowVp->setBackgroundColour(Ogre::ColourValue::White);
    _shadowVp->setClearEveryFrame(true);
    _shadowVp->setOverlaysEnabled(false);
    _shadowVp->setShadowsEnabled(false);

    // 4) Install listener so we can swap material schemes around the
    // shadow pass.
    rt->addListener(this);

    std::cerr << "ShadowMapper: initialised RTT " << RTT_NAME
              << " (1024^2 R8), ortho " << _arenaSize * 2.0f
              << " x " << _arenaSize * 2.0f << std::endl;
}

ShadowMapper::~ShadowMapper()
{
    if (_rtt) {
        Ogre::RenderTarget *rt = _rtt->getBuffer()->getRenderTarget();
        if (rt) rt->removeListener(this);
    }
    if (_shadowCamNode) {
        _shadowCamNode->detachAllObjects();
        _sm->getRootSceneNode()->removeAndDestroyChild(_shadowCamNode);
    }
    if (_shadowCam) {
        _sm->destroyCamera(_shadowCam);
    }
    if (_rtt) {
        Ogre::TextureManager::getSingleton().remove(_rtt);
    }

    if (gShadowCastListener) {
        Ogre::MaterialManager::getSingleton().removeListener(gShadowCastListener);
        delete gShadowCastListener;
        gShadowCastListener = nullptr;
    }
}

namespace {

/// Defensive cleanup + one-shot log when the RTT render throws. Called
/// from both the Ogre::Exception and std::exception catch arms so the
/// "log once total" property holds across exception types. Restores both
/// the active material scheme and the visibility of any objects that
/// preRenderTargetUpdate temporarily hid — if the throw happens after
/// preRenderTargetUpdate but before Ogre fires postRenderTargetUpdate,
/// those objects would otherwise stay invisible forever (the next
/// frame's preRenderTargetUpdate clears _hiddenForCast, dropping the
/// references).
void handleRenderException(const char *what,
                           const Ogre::String &previousScheme,
                           std::vector<Ogre::MovableObject *> &hiddenForCast)
{
    if (!previousScheme.empty()) {
        Ogre::MaterialManager::getSingleton().setActiveScheme(previousScheme);
    }
    for (Ogre::MovableObject *mo : hiddenForCast) {
        if (mo) mo->setVisible(true);
    }
    hiddenForCast.clear();
    static bool reported = false;
    if (!reported) {
        Ogre::LogManager::getSingleton().logMessage(
            Ogre::String("ShadowMapper::update swallowed unexpected "
                         "exception: ")
                + what,
            Ogre::LML_WARNING);
        reported = true;
    }
}

} // anonymous namespace

void ShadowMapper::update()
{
    if (!_rtt) return;
    // Catch RTT render exceptions locally so they don't abort the main
    // frame in OgreHandler::step. Post-Task-6 the listener should
    // satisfy every material query, so this catch is now belt-and-
    // braces — left in place because if RenderTarget::update throws
    // mid-render, Ogre's firePostUpdate (which calls our
    // postRenderTargetUpdate) never fires, leaving the active material
    // scheme stuck on "yars-shadow-cast" and the main viewport
    // renders broken on the next frame. Restore the scheme defensively.
    try {
        _rtt->getBuffer()->getRenderTarget()->update();
    } catch (const Ogre::Exception &e) {
        handleRenderException(e.what(), _previousScheme, _hiddenForCast);
    } catch (const std::exception &e) {
        handleRenderException(e.what(), _previousScheme, _hiddenForCast);
    }
}

void ShadowMapper::preRenderTargetUpdate(const Ogre::RenderTargetEvent &)
{
    // Swap viewport material scheme: every material is rendered using
    // its "yars-shadow-cast" technique (a single black opaque pass).
    // Materials without that technique use the MaterialManager listener
    // to auto-generate one.
    _previousScheme = Ogre::MaterialManager::getSingleton().getActiveScheme();
    Ogre::MaterialManager::getSingleton().setActiveScheme(CAST_SCHEME);
    if (_shadowVp) _shadowVp->setMaterialScheme(CAST_SCHEME);

    // Temporarily hide MovableObjects flagged as non-shadow-casters
    // (e.g. the ground plane, sensor visualizations). Without this, the
    // 50x50 ground mesh covers the entire 12x12 ortho shadow viewport
    // and paints it solid black via the silhouette technique. Restored
    // in postRenderTargetUpdate. We can't filter via the scheme listener
    // alone because most YARS renderables are ManualObject sections,
    // whose default Renderable::getCastsShadows() returns false even
    // when their parent MovableObject has setCastShadows(true).
    _hiddenForCast.clear();
    for (const char *type : {"Entity", "ManualObject", "BillboardSet", "ParticleSystem"}) {
        const Ogre::SceneManager::MovableObjectMap &objs =
            _sm->getMovableObjects(type);
        for (const auto &kv : objs) {
            Ogre::MovableObject *mo = kv.second;
            if (mo && mo->getVisible() && !mo->getCastShadows()) {
                mo->setVisible(false);
                _hiddenForCast.push_back(mo);
            }
        }
    }
}

void ShadowMapper::postRenderTargetUpdate(const Ogre::RenderTargetEvent &)
{
    Ogre::MaterialManager::getSingleton().setActiveScheme(_previousScheme);

    // Restore visibility of any objects we temporarily hid.
    for (Ogre::MovableObject *mo : _hiddenForCast) {
        if (mo) mo->setVisible(true);
    }
    _hiddenForCast.clear();
}

} // namespace yars
