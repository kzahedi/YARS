#include "ShadowMapper.h"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMatrix3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMovableObject.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <cmath>
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

    // 1b) Defer GroundShadowed material binding to bindToGroundMaterial(),
    // which OgreHandler must call after rgm.initialiseResourceGroup("YARS")
    // has parsed the material script. Trying to bind here returns a null
    // MaterialPtr because the YARS resource group hasn't been initialised
    // yet (this constructor runs BEFORE group init, on purpose, so the
    // RTT texture exists by the time the material script references it).
    // Without that deferred call, unit 1 stays unbound — the floor then
    // samples whatever stale texture is in sampler unit 1, producing
    // checkered nonsense instead of shadows.

    Ogre::RenderTarget *rt = _rtt->getBuffer()->getRenderTarget();
    rt->setAutoUpdated(false); // we drive it manually each frame

    // 2) Create the light-direction-aligned orthographic camera.
    //
    // Light rays travel in lightDirection = (-1,-1,-1) (normalised). The
    // shadow camera is positioned at -lightDirection * cameraDistance
    // (i.e. on the side the light source comes from) and looks along
    // lightDirection (in the direction the light travels), so that
    // silhouettes project onto receivers along the actual light vector.
    //
    // We build the camera basis explicitly via a quaternion so the
    // result is deterministic — Ogre's lookAt with an implicit up
    // vector can pick a different basis depending on degeneracies.
    _shadowCam = _sm->createCamera("YarsShadowCam");
    _shadowCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    // Ortho window: the projected footprint of the 8x8 arena at a 45°
    // light angle is larger than the arena itself. 16x16 covers the
    // arena plus margin for taller dynamic geometry; increase if
    // shadows clip at the edges.
    _shadowCam->setOrthoWindow(16.0f, 16.0f);
    // Near clip 1.0 — the camera is ~50 units from origin, so 0.5 is
    // unnecessarily tight (and can produce z-fighting in the depth
    // tests done by the silhouette pass).
    _shadowCam->setNearClipDistance(1.0f);
    _shadowCam->setFarClipDistance(200.0f);

    const Ogre::Real cameraDistance = 50.0f;
    Ogre::Vector3 lightDirection(-1.0f, -1.0f, -1.0f);
    lightDirection.normalise();
    const Ogre::Vector3 camPos = -lightDirection * cameraDistance;

    _shadowCamNode = _sm->getRootSceneNode()->createChildSceneNode("YarsShadowCamNode");
    _shadowCamNode->setPosition(camPos);

    // Build the camera basis explicitly via a quaternion so the result
    // is deterministic. Ogre cameras look along their local -Z axis;
    // local +Y is up, local +X is right. We want:
    //   local -Z (forward in world) = lightDirection
    //   local +Y (up in world)      = orthogonal to forward, derived
    //                                 from a world up vector
    //
    // Deterministic basis: pick world +Y as up. If the light direction
    // is (nearly) parallel to +Y, fall back to +Z to avoid a degenerate
    // cross product. For (-1,-1,-1) the dot with +Y is ~-0.577 so the
    // primary branch applies.
    Ogre::Vector3 worldUp = Ogre::Vector3::UNIT_Y;
    if (std::abs(worldUp.dotProduct(lightDirection)) > 0.95f) {
        worldUp = Ogre::Vector3::UNIT_Z;
    }
    const Ogre::Vector3 zAxis = -lightDirection;          // local +Z
    Ogre::Vector3 xAxis = worldUp.crossProduct(zAxis);    // local +X = up x z
    xAxis.normalise();
    const Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis); // local +Y = z x x

    Ogre::Matrix3 rot;
    rot.SetColumn(0, xAxis);
    rot.SetColumn(1, yAxis);
    rot.SetColumn(2, zAxis);
    Ogre::Quaternion orientation(rot);
    _shadowCamNode->setOrientation(orientation);

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
              << " (1024^2 R8), ortho 16x16, cam pos ("
              << camPos.x << ", " << camPos.y << ", " << camPos.z
              << "), light dir (" << lightDirection.x << ", "
              << lightDirection.y << ", " << lightDirection.z << ")"
              << std::endl;
}

Ogre::Matrix4 ShadowMapper::getShadowViewProjMatrix() const
{
    // Bias matrix maps clip-space [-1,+1] to texture-space [0,1].
    // The convention matches what the GLSL fragment shader expects after
    // perspective divide: shadowClipPos.xy / shadowClipPos.w in [0,1].
    static const Ogre::Matrix4 BIAS(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f);
    if (!_shadowCam) return Ogre::Matrix4::IDENTITY;
    // getProjectionMatrixWithRSDepth returns the projection matrix with
    // the render system's actual depth-range applied (GL: [-1,+1] z;
    // others may use [0,1]). Combined with the bias above, the result
    // lands in [0,1] for shaders regardless of API.
    return BIAS
           * _shadowCam->getProjectionMatrixWithRSDepth()
           * _shadowCam->getViewMatrix();
}

void ShadowMapper::bindToGroundMaterial()
{
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton()
        .getByName("YARS/GroundShadowed", "YARS");
    if (!mat) {
        std::cerr << "ShadowMapper::bindToGroundMaterial: material "
                     "YARS/GroundShadowed not found — shadows will not "
                     "render correctly." << std::endl;
        return;
    }
    mat->load();
    if (mat->getNumTechniques() == 0) return;
    Ogre::Technique *tech = mat->getTechnique(0);
    if (!tech || tech->getNumPasses() == 0) return;
    Ogre::Pass *pass = tech->getPass(0);
    if (!pass || pass->getNumTextureUnitStates() <= 1) return;
    // Unit 1 is the shadow RTT (Unit 0 is the ground diffuse texture).
    pass->getTextureUnitState(1)->setTextureName(RTT_NAME);
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

    // Push the current shadow view-projection-bias matrix to the
    // receiver shader as a named constant. The matrix is set per-frame
    // (rather than via a material-script `param_named_auto`) because
    // Ogre 14 has no built-in auto-param for an arbitrary off-screen
    // camera's view+proj product, and the documented
    // `texture_worldviewproj_matrix` auto-param is broken on GL3+ core
    // (see docs/planning/shadows_attempts_log.md).
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton()
        .getByName("YARS/GroundShadowed", "YARS");
    if (mat && mat->getNumTechniques() > 0) {
        Ogre::Technique *tech = mat->getTechnique(0);
        if (tech && tech->getNumPasses() > 0) {
            Ogre::Pass *pass = tech->getPass(0);
            if (pass && pass->hasVertexProgram()) {
                Ogre::GpuProgramParametersSharedPtr params =
                    pass->getVertexProgramParameters();
                if (params) {
                    params->setNamedConstant("shadowViewProjMatrix",
                                             getShadowViewProjMatrix());
                }
            }
        }
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
