#ifndef YARS_VIEW_GUI_SHADOW_MAPPER_H
#define YARS_VIEW_GUI_SHADOW_MAPPER_H

#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreSceneManager.h>

#include <memory>

namespace Ogre {
    class Camera;
}

namespace yars {

/**
 * Owns the per-frame logic of pushing the shadow camera's world-view-proj
 * matrix into the YARS/ShadowReceiver material as a custom uniform.
 * Ogre's SHADOWTYPE_TEXTURE_MODULATIVE framework handles the actual
 * shadow RTT rendering; this class just makes sure the receiver shader
 * has the correct projection matrix.
 *
 * Why a custom uniform instead of Ogre's texture_worldviewproj_matrix
 * auto-param: that auto-param is broken on macOS arm64 GL3+ core - it
 * computes UVs that don't agree with where the caster pass actually
 * wrote silhouettes in the shadow texture. By snapshotting the matrix
 * via Ogre's ShadowTextureListener::shadowTexturesUpdated() callback
 * (which fires AFTER caster rendering, BEFORE receiver rendering) we
 * capture the same matrix Ogre actually used for the caster pass.
 */
class ShadowMapper : public Ogre::ShadowTextureListener
{
public:
    ShadowMapper(Ogre::SceneManager *sm,
                 const Ogre::String &receiverMaterialName,
                 const Ogre::Vector3 &lightDirection);
    ~ShadowMapper() override;

    /// Listener callback - fires after Ogre has rendered all shadow
    /// textures but before any receivers are drawn. We snapshot the
    /// shadow camera's view-projection matrix here and push it into
    /// the receiver material uniforms so the modulating pass uses
    /// exactly the same matrix the caster pass did.
    void shadowTexturesUpdated(size_t numberOfShadowTextures) override;

private:
    Ogre::SceneManager *_sm;
    Ogre::MaterialPtr   _receiverMaterial;
    Ogre::Vector3       _lightDir;   // normalised
};

} // namespace yars

#endif
