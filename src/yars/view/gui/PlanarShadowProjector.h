#ifndef YARS_VIEW_GUI_PLANAR_SHADOW_PROJECTOR_H
#define YARS_VIEW_GUI_PLANAR_SHADOW_PROJECTOR_H

#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreString.h>

#include <vector>

namespace Ogre {
    class SceneManager;
    class SceneNode;
    class Entity;
    class MovableObject;
    class ManualObject;
}

namespace yars {

/**
 * Renders planar projected shadows on the YARS floor plane for every
 * registered caster, using the classic SIGGRAPH 97 "shadow matrix"
 * technique.
 *
 * Architecture:
 *   - The receiver plane is the floor (Ogre world y=0, normal +Y).
 *   - The light direction is hardcoded to (-1,-1,-1) in Ogre world
 *     space, matching the directional sun light in OgreHandler.
 *   - For each registered caster (an Entity attached to a SceneNode):
 *       - A shadow proxy SceneNode is created as a child of the
 *         SceneManager's root.
 *       - A new Entity sharing the same Ogre::Mesh is attached, with
 *         material YARS/PlanarShadow (translucent black).
 *       - On each frame's update(), the proxy node's full transform
 *         is set to planarShadowMatrix() * caster.world.
 *
 * Limitations:
 *   - Floor only. Shadows do not appear on walls.
 *   - Hard-edged. No soft falloff.
 *   - Only works for entities that share a single Ogre::Mesh. For
 *     ManualObject-based casters (boxes, capsules, cylinders,
 *     muscles in YARS) the caller must convertToMesh() first or
 *     use a different registration path.
 *
 * Threading: not thread-safe. update() must be called from the same
 * thread that owns the SceneManager.
 */
class PlanarShadowProjector
{
public:
    PlanarShadowProjector(Ogre::SceneManager *sm,
                          const Ogre::Plane &floorPlane,
                          const Ogre::Vector3 &lightDirection);
    ~PlanarShadowProjector();

    /**
     * Register an Entity-based caster. Creates a parallel Entity
     * referencing the same Mesh, attached to a fresh proxy SceneNode.
     * @param casterNode The scene node whose world transform drives
     *                   the proxy's world transform each frame.
     * @param meshName   The Mesh from which to spawn a shadow Entity.
     *                   The proxy renders this mesh in translucent black.
     * @return true if the proxy was created, false if registration
     *         failed (e.g. mesh is null).
     */
    bool registerCaster(Ogre::SceneNode *casterNode,
                        const Ogre::String &meshName);

    /**
     * Register a ManualObject-based caster.
     * The ManualObject is converted to a Mesh (one-shot snapshot at
     * registration time) and then the same Entity-based mechanism is
     * used. Note: if the ManualObject's geometry changes after
     * registration, the shadow will not reflect the change.
     */
    bool registerCaster(Ogre::SceneNode *casterNode,
                        Ogre::ManualObject *manual,
                        const Ogre::String &meshNameHint);

    /// Per-frame update. Walks all registered casters and refreshes
    /// each proxy's full world transform.
    void update();

    /// Compute the shadow projection matrix from a plane and a light
    /// direction. Public so callers can sanity-check or compose with
    /// their own transforms.
    static Ogre::Matrix4 planarShadowMatrix(const Ogre::Plane &plane,
                                            const Ogre::Vector3 &lightDir);

private:
    struct Proxy {
        Ogre::SceneNode *casterNode;
        Ogre::SceneNode *proxyNode;
        Ogre::Entity    *proxyEntity;
    };

    Ogre::SceneManager  *_sm;
    Ogre::Plane          _floor;
    Ogre::Vector3        _lightDir; // already normalized
    Ogre::Matrix4        _shadowMatrix; // cached planarShadowMatrix
    std::vector<Proxy>   _proxies;
    int                  _nextProxyId; // for unique node names
};

} // namespace yars

#endif
