#ifndef YARS_VIEW_GUI_SHADOW_MAPPER_H
#define YARS_VIEW_GUI_SHADOW_MAPPER_H

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreMaterialManager.h>

#include <vector>

namespace Ogre {
class MovableObject;
}

namespace yars {

/**
 * Custom top-down render-to-texture shadow pipeline.
 *
 * Ogre 14's built-in texture_worldviewproj_matrix auto-param is broken
 * on GL3+ core (see docs/planning/shadows_attempts_log.md), so we
 * sidestep Ogre's shadow framework entirely:
 *
 *   1. Create a dedicated 1024x1024 R8 RTT (`shadow_rt`) above the
 *      arena, with an orthographic camera looking straight down.
 *   2. Each frame: render the scene into the RTT with every entity
 *      painted black (silhouette pass).
 *   3. The ground material (`YARS/GroundShadowed`) samples this RTT
 *      using `uv = (worldXZ - arenaMin) / arenaSize` — UV math is in
 *      our shader, not in Ogre's auto-param.
 *
 * The light direction is approximated by a top-down ortho. For the
 * YARS scene (8x8 arena, ~0.5m wall height, single directional light)
 * this is visually close enough; walls cast small shadow strips on
 * the floor exactly where they would under the directional light.
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
    void update();

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
