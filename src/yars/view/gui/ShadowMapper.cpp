#include "ShadowMapper.h"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreViewport.h>

#include <iostream>

namespace yars {

ShadowMapper::ShadowMapper(Ogre::SceneManager *sm,
                           const Ogre::String &receiverMaterialName,
                           const Ogre::Vector3 &lightDirection)
    : _sm(sm),
      _lightDir(lightDirection.normalisedCopy())
{
    _receiverMaterial = Ogre::MaterialManager::getSingleton()
        .getByName(receiverMaterialName);
    if (!_receiverMaterial) {
        std::cerr << "ShadowMapper: receiver material '"
                  << receiverMaterialName << "' not found" << std::endl;
    } else {
        _receiverMaterial->load();
    }

    // Register for the post-caster-render callback. This is critical -
    // we must read the shadow camera's matrix AFTER Ogre has set up the
    // caster pass (during which it may have moved/refocused the camera)
    // so the matrix we push to receivers matches the one Ogre used.
    _sm->addShadowTextureListener(this);

    std::cerr << "ShadowMapper: initialised with light="
              << _lightDir << std::endl;
}

ShadowMapper::~ShadowMapper()
{
    if (_sm) _sm->removeShadowTextureListener(this);
}

void ShadowMapper::shadowTexturesUpdated(size_t /*numTex*/)
{
    if (!_receiverMaterial || _sm->getShadowTextureCount() == 0) return;

    // Read the shadow camera that Ogre just used to render the caster
    // pass. Ogre stores it on the shadow texture's viewport.
    Ogre::TexturePtr shadowTex = _sm->getShadowTexture(0);
    if (!shadowTex) return;
    Ogre::RenderTarget *rt = shadowTex->getBuffer()->getRenderTarget();
    if (!rt || rt->getNumViewports() == 0) return;
    Ogre::Camera *shadowCam = rt->getViewport(0)->getCamera();
    if (!shadowCam) return;

    // World-to-shadow-clip matrix. Use the plain projection matrix
    // (without RS depth adjustment) — our hand-written GLSL targets
    // the standard GL clip-space depth range [-1,1] directly.
    Ogre::Matrix4 shadowViewProj =
        shadowCam->getProjectionMatrix() *
        shadowCam->getViewMatrix();

    for (unsigned short t = 0; t < _receiverMaterial->getNumTechniques(); ++t) {
        Ogre::Technique *tech = _receiverMaterial->getTechnique(t);
        for (unsigned short p = 0; p < tech->getNumPasses(); ++p) {
            Ogre::Pass *pass = tech->getPass(p);
            if (pass->hasVertexProgram()) {
                auto params = pass->getVertexProgramParameters();
                try {
                    params->setNamedConstant("shadowViewProjMatrix",
                                             shadowViewProj);
                } catch (const Ogre::Exception&) { /* uniform optimised out */ }
            }
            if (pass->hasFragmentProgram()) {
                auto params = pass->getFragmentProgramParameters();
                try {
                    params->setNamedConstant("shadowViewProjMatrix",
                                             shadowViewProj);
                } catch (const Ogre::Exception&) { /* uniform optimised out */ }
            }
        }
    }
}

} // namespace yars
