#include "PlanarShadowProjector.h"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreStringConverter.h>
#include <OGRE/OgreTechnique.h>

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
        const int idx = _nextProxyId++;
        Ogre::String proxyEntName  = "YarsShadowProxy_E_" +
                                     Ogre::StringConverter::toString(idx);
        Ogre::String proxyNodeName = "YarsShadowProxy_N_" +
                                     Ogre::StringConverter::toString(idx);
        Ogre::String proxyMatName  = "YARS/PlanarShadow_" +
                                     Ogre::StringConverter::toString(idx);

        // Clone the base material so this proxy has its own
        // casterWorldMatrix uniform. If we re-used the singleton
        // YARS/PlanarShadow, every proxy would end up rendering
        // with the LAST caster's matrix.
        Ogre::MaterialPtr baseMat =
            Ogre::MaterialManager::getSingleton().getByName(
                "YARS/PlanarShadow");
        if (!baseMat) {
            std::cerr << "PlanarShadowProjector::registerCaster: "
                         "YARS/PlanarShadow material not found"
                      << std::endl;
            return false;
        }
        baseMat->load();
        Ogre::MaterialPtr clonedMat = baseMat->clone(proxyMatName);
        clonedMat->load();

        // Seed planarShadowMatrix once; this uniform is constant per
        // projector instance, so we don't need to push it every frame.
        if (Ogre::Technique *tech = clonedMat->getTechnique(0)) {
            if (Ogre::Pass *pass = tech->getPass(0)) {
                if (pass->hasVertexProgram()) {
                    Ogre::GpuProgramParametersSharedPtr vparams =
                        pass->getVertexProgramParameters();
                    try {
                        vparams->setNamedConstant("planarShadowMatrix",
                                                  _shadowMatrix);
                    } catch (const Ogre::Exception &) {
                        // Some drivers/optimizers may strip unused
                        // uniforms; we'll fall back to setting it
                        // each frame in update().
                    }
                }
            }
        }

        Ogre::Entity *proxyEnt = _sm->createEntity(proxyEntName, meshName);
        proxyEnt->setMaterialName(proxyMatName);
        proxyEnt->setCastShadows(false); // shadow proxies don't recurse.

        Ogre::SceneNode *proxyNode = _sm->getRootSceneNode()
                                        ->createChildSceneNode(proxyNodeName);
        proxyNode->attachObject(proxyEnt);

        _proxies.push_back({casterNode, proxyNode, proxyEnt, clonedMat});
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
        if (!p.casterNode || !p.proxyMaterial) continue;

        // The proxy scene node only matters for culling — the shader
        // computes the actual projected world position. Park the proxy
        // node at the caster's derived position so it lies within the
        // camera frustum (otherwise Ogre culls the proxy by its
        // bounding box, which would still be at the proxy node's
        // origin if we left it at the world root).
        const Ogre::Vector3 casterPos = p.casterNode->_getDerivedPosition();
        p.proxyNode->setPosition(casterPos);

        // Push the caster's current world matrix to the proxy's
        // vertex program. The shader applies the planar shadow
        // projection per-vertex and does the perspective divide.
        const Ogre::Affine3 &cw = p.casterNode->_getFullTransform();
        const Ogre::Matrix4  casterWorld(cw);

        Ogre::Technique *tech = p.proxyMaterial->getTechnique(0);
        if (!tech) continue;
        Ogre::Pass *pass = tech->getPass(0);
        if (!pass || !pass->hasVertexProgram()) continue;
        Ogre::GpuProgramParametersSharedPtr vparams =
            pass->getVertexProgramParameters();
        try {
            vparams->setNamedConstant("casterWorldMatrix", casterWorld);
        } catch (const Ogre::Exception &) {
            // Uniform optimized out; nothing to do.
        }
    }
}

} // namespace yars
