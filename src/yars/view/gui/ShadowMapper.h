#ifndef YARS_VIEW_GUI_SHADOW_MAPPER_H
#define YARS_VIEW_GUI_SHADOW_MAPPER_H

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreMaterialManager.h>

#include <vector>

namespace Ogre {
class MovableObject;
}

namespace yars {

/**
 * Custom light-direction-aligned render-to-texture shadow pipeline.
 *
 * Ogre 14's built-in texture_worldviewproj_matrix auto-param is broken
 * on GL3+ core (see docs/planning/shadows_attempts_log.md), so we
 * sidestep Ogre's shadow framework entirely:
 *
 *   1. Create a dedicated 1024x1024 R8 RTT (`shadow_rt`) positioned
 *      along the light direction with an orthographic camera looking
 *      in the direction the light travels (-1,-1,-1 in Ogre world).
 *   2. Each frame: render the scene into the RTT with every entity
 *      painted black (silhouette pass).
 *   3. The ground material (`YARS/GroundShadowed`) samples this RTT
 *      using a shadow view+proj matrix uniform pushed every frame
 *      from C++ — the receiver shader projects each ground vertex
 *      into shadow texture space rather than relying on Ogre's broken
 *      texture-space auto-params.
 *
 * Because the shadow camera is aligned with the actual light vector,
 * silhouettes project along that vector and shadows extend away from
 * casters in the light's travel direction — not directly under them
 * as the previous top-down ortho variant did.
 */
class ShadowMapper : public Ogre::RenderTargetListener
{
public:
    /**
     * Configures the RTT and orthographic camera. The supplied scene
     * manager must already exist and have a viewport.
     *
     * @param sm The Ogre scene manager owning the scene to be shadowed.
     * @param arenaSize Arena half-extent in Ogre world units (e.g. 6.0
     *                  for a 12x12 m arena centered on origin).
     */
    ShadowMapper(Ogre::SceneManager *sm, float arenaSize);
    ~ShadowMapper() override;

    /// Force a shadow re-render this frame. Call from OgreHandler::step.
    /// Also pushes the current shadow view-projection-bias matrix to
    /// the receiver material as a named vertex-shader constant.
    void update();

    /// Bind the RTT to the YARS/GroundShadowed material's shadow texture
    /// unit. Must be called AFTER the YARS resource group is initialised
    /// (so the material has been parsed). The constructor runs before
    /// group init, so the binding has to be deferred to here.
    void bindToGroundMaterial();

    /// Compute the bias * proj * view matrix product for the shadow
    /// camera, so a world-space vertex multiplied through it lands in
    /// texture-space [0,1] (after perspective divide, which for an
    /// ortho projection is a no-op).
    Ogre::Matrix4 getShadowViewProjMatrix() const;

    /// Name of the RTT texture (so ground material can reference it).
    static const char *RTT_NAME;
    /// Name of the shadow-cast material scheme.
    static const char *CAST_SCHEME;

    // RenderTargetListener overrides
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;

private:
    Ogre::SceneManager *_sm;
    float _arenaSize;
    Ogre::TexturePtr _rtt;
    Ogre::Camera *_shadowCam;
    Ogre::SceneNode *_shadowCamNode;
    Ogre::Viewport *_shadowVp;
    Ogre::String _previousScheme; // restored in postRenderTargetUpdate

    // MovableObjects whose visibility we temporarily disabled for the
    // shadow pass (those with getCastShadows()==false, e.g. the ground).
    // Restored in postRenderTargetUpdate.
    std::vector<Ogre::MovableObject *> _hiddenForCast;
};

} // namespace yars

#endif
