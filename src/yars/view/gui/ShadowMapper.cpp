#include "ShadowMapper.h"

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <iostream>

namespace yars {

const char *ShadowMapper::RTT_NAME = "YarsShadowRTT";
const char *ShadowMapper::CAST_SCHEME = "yars-shadow-cast";

ShadowMapper::ShadowMapper(Ogre::SceneManager *sm, float arenaSize)
    : _sm(sm), _arenaSize(arenaSize), _shadowCam(nullptr),
      _shadowCamNode(nullptr), _shadowVp(nullptr)
{
    // 1) Create the RTT.
    _rtt = Ogre::TextureManager::getSingleton().createManual(
        RTT_NAME,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D,
        1024, 1024,
        0, // no mipmaps
        Ogre::PF_R8,
        Ogre::TU_RENDERTARGET);

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
}

void ShadowMapper::update()
{
    if (!_rtt) return;
    // Catch RTT render exceptions locally so they don't abort the main
    // frame in OgreHandler::step. Until Task 6 installs the scheme
    // listener that auto-generates a black-silhouette technique for
    // every material, the shadow pass will throw "technique has no
    // Vertex Shader" — that's expected here; just swallow it once.
    //
    // Also: if RenderTarget::update throws mid-render, Ogre's
    // firePostUpdate (which calls our postRenderTargetUpdate) never
    // fires, leaving the active material scheme stuck on
    // "yars-shadow-cast" and the main viewport renders broken on the
    // next frame. Restore the scheme defensively here.
    try {
        _rtt->getBuffer()->getRenderTarget()->update();
    } catch (const Ogre::Exception &e) {
        if (!_previousScheme.empty()) {
            Ogre::MaterialManager::getSingleton().setActiveScheme(_previousScheme);
        }
        static bool reported = false;
        if (!reported) {
            Ogre::LogManager::getSingleton().logMessage(
                "ShadowMapper::update swallowed (expected pre-Task-6): "
                    + Ogre::String(e.what()),
                Ogre::LML_WARNING);
            reported = true;
        }
    } catch (const std::exception &e) {
        if (!_previousScheme.empty()) {
            Ogre::MaterialManager::getSingleton().setActiveScheme(_previousScheme);
        }
        static bool reported = false;
        if (!reported) {
            Ogre::LogManager::getSingleton().logMessage(
                "ShadowMapper::update swallowed (expected pre-Task-6): "
                    + Ogre::String(e.what()),
                Ogre::LML_WARNING);
            reported = true;
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
}

void ShadowMapper::postRenderTargetUpdate(const Ogre::RenderTargetEvent &)
{
    Ogre::MaterialManager::getSingleton().setActiveScheme(_previousScheme);
}

} // namespace yars
