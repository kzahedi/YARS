#include "PlanarShadowProjector.h"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreStringConverter.h>

#include <iostream>

namespace yars {

PlanarShadowProjector::PlanarShadowProjector(Ogre::SceneManager *sm,
                                             const Ogre::Plane &floorPlane,
                                             const Ogre::Vector3 &lightDirection)
    : _sm(sm), _floor(floorPlane),
      _lightDir(lightDirection.normalisedCopy()),
      _shadowMatrix(planarShadowMatrix(floorPlane, _lightDir)),
      _nextProxyId(0)
{
    std::cerr << "PlanarShadowProjector: floor n="
              << _floor.normal << " d=" << _floor.d
              << " light=" << _lightDir << std::endl;
}

PlanarShadowProjector::~PlanarShadowProjector()
{
    for (auto &p : _proxies) {
        if (p.proxyNode && p.proxyEntity) {
            p.proxyNode->detachAllObjects();
            _sm->destroyEntity(p.proxyEntity);
        }
        if (p.proxyNode) {
            _sm->destroySceneNode(p.proxyNode);
        }
    }
    _proxies.clear();
}

Ogre::Matrix4
PlanarShadowProjector::planarShadowMatrix(const Ogre::Plane &plane,
                                          const Ogre::Vector3 &lightDir)
{
    // Reference: SIGGRAPH 97 projection-shadow notes.
    //   M[i][j] = (i==j ? n.l : 0) - l[i] * n_extended[j]
    // with n_extended = (n.x, n.y, n.z, d) and l a 4-vector with w=0
    // for directional light.

    Ogre::Real n[4] = {plane.normal.x, plane.normal.y, plane.normal.z, plane.d};
    Ogre::Real l[4] = {lightDir.x, lightDir.y, lightDir.z, 0.0f};

    Ogre::Real ndotl = n[0]*l[0] + n[1]*l[1] + n[2]*l[2] + n[3]*l[3];

    Ogre::Matrix4 m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j ? ndotl : 0.0f) - l[i] * n[j];
        }
    }
    return m;
}

bool PlanarShadowProjector::registerCaster(Ogre::SceneNode *casterNode,
                                           const Ogre::String &meshName)
{
    if (!casterNode || meshName.empty()) return false;
    if (!Ogre::MeshManager::getSingleton().resourceExists(
            meshName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)) {
        return false;
    }

    try {
        Ogre::String proxyEntName = "YarsShadowProxy_E_" +
                                    Ogre::StringConverter::toString(_nextProxyId);
        Ogre::String proxyNodeName = "YarsShadowProxy_N_" +
                                     Ogre::StringConverter::toString(_nextProxyId);
        ++_nextProxyId;

        Ogre::Entity *proxyEnt = _sm->createEntity(proxyEntName, meshName);
        proxyEnt->setMaterialName("YARS/PlanarShadow");
        proxyEnt->setCastShadows(false); // shadow proxies don't recurse.

        Ogre::SceneNode *proxyNode = _sm->getRootSceneNode()
                                        ->createChildSceneNode(proxyNodeName);
        proxyNode->attachObject(proxyEnt);

        _proxies.push_back({casterNode, proxyNode, proxyEnt});
        return true;
    } catch (const Ogre::Exception &e) {
        std::cerr << "PlanarShadowProjector::registerCaster failed: "
                  << e.what() << std::endl;
        return false;
    }
}

bool PlanarShadowProjector::registerCaster(Ogre::SceneNode *casterNode,
                                           Ogre::ManualObject *manual,
                                           const Ogre::String &meshNameHint)
{
    if (!casterNode || !manual) return false;
    try {
        // One-shot snapshot of the ManualObject geometry to a Mesh.
        // If the manual object's geometry later changes, the shadow
        // won't follow. For YARS' use cases (boxes, capsules,
        // cylinders) this is OK — geometry is static after creation;
        // only the transform changes.
        Ogre::String meshName = "YarsShadowProxyMesh_" + meshNameHint + "_" +
                                Ogre::StringConverter::toString(_nextProxyId);
        manual->convertToMesh(meshName);
        return registerCaster(casterNode, meshName);
    } catch (const Ogre::Exception &e) {
        std::cerr << "PlanarShadowProjector::registerCaster(manual) failed: "
                  << e.what() << std::endl;
        return false;
    }
}

void PlanarShadowProjector::update()
{
    for (auto &p : _proxies) {
        if (!p.casterNode || !p.proxyNode) continue;
        // proxy.world = shadowMatrix * caster.world
        // We compose the full Affine3 from the caster's derived transform
        // and apply the (non-affine) shadow matrix on top, then push the
        // 4x4 to the proxy node.
        //
        // Implementation note: Ogre 14 only exposes
        // `decomposition(pos, scale, orientation)` on Affine3, not on
        // Matrix4. We construct the proxy world matrix as a Matrix4
        // (because the shadow projection itself is not affine in
        // general), then re-wrap it as an Affine3. For our floor=y+0
        // light=(-1,-1,-1) configuration the bottom row of the shadow
        // matrix is `[0,0,0,1]` after normalisation (see derivation in
        // header / commit), so reinterpretation is exact. The 3x3
        // linear part is rank-deficient — decomposition yields a
        // degenerate scale on one axis, which flattens the mesh onto
        // the floor (that is the desired visual outcome).
        const Ogre::Affine3 &casterWorld = p.casterNode->_getFullTransform();
        const Ogre::Matrix4 proxyWorld   = _shadowMatrix * Ogre::Matrix4(casterWorld);
        const Ogre::Affine3 proxyAffine(proxyWorld);

        Ogre::Vector3 pos, scale;
        Ogre::Quaternion rot;
        proxyAffine.decomposition(pos, scale, rot);
        p.proxyNode->setPosition(pos);
        p.proxyNode->setOrientation(rot);
        p.proxyNode->setScale(scale);
    }
}

} // namespace yars
