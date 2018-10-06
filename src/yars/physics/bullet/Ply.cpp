#include <yars/physics/bullet/Ply.h>
#include <yars/physics/bullet/World.h>
#include <yars/physics/bullet/MyMotionState.h>
#include <yars/physics/bullet/MyMotionStateDebug.h>
#include <yars/util/YarsErrorHandler.h>

#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <iostream>
#include <iomanip>

Ply::Ply(DataPly *data) : Object(data)
{
  _data = data;

  _vertices = NULL;
  _indices = NULL;
  _indexVertexArrays = NULL;
  _trimesh = NULL;

  init();
}

void Ply::init()
{
  _pd = PlyLoader::instance()->get(_data->filename().c_str());
  _pd.removeDoubles();

  _indexVertexArrays = NULL;
  _trimesh = NULL;
  _nVertices = _pd.vertices.size();
  _nTriangles = _pd.triangles.size();

  _vertices = new btVector3[_nVertices];
  _indices = new int[_nTriangles * 3];

  for (int i = 0; i < _nVertices; i++)
  {
    _vertices[i].setValue(_pd.vertices[i].x, _pd.vertices[i].y, _pd.vertices[i].z);
  }

  for (int i = 0; i < _nTriangles; i++)
  {
    _indices[3 * i + 0] = _pd.triangles[i].a;
    _indices[3 * i + 1] = _pd.triangles[i].b;
    _indices[3 * i + 2] = _pd.triangles[i].c;
  }

#ifdef USE_SOFT_BODIES
  if (data->isSoft())
    __soft();
  else
    __rigid();
#else  // USE_SOFT_BODIES
  __rigid();
#endif // USE_SOFT_BODIES
}

Ply::~Ply()
{
  delete _vertices;
  delete _indices;
  if (_trimesh != NULL)
    delete _trimesh;
  if (_indexVertexArrays != NULL)
    delete _indexVertexArrays;
}

#ifdef USE_SOFT_BODIES
void Ply::__soft()
{
  Quaternion q(_data->pose().orientation);
  P3D p = _data->pose().position;
  btScalar mass = _data->physics()->mass();

  btScalar *vertices = new btScalar[_nVertices * 3];
  for (int j = 0, i = 0; i < _nVertices; i++)
  {
    vertices[j++] = _pd.vertices[i].x;
    vertices[j++] = _pd.vertices[i].y;
    vertices[j++] = _pd.vertices[i].z;
  }

  int indices[_nTriangles][3];
  for (int i = 0; i < _nTriangles; i++)
  {
    indices[i][0] = _pd.triangles[i].a;
    indices[i][1] = _pd.triangles[i].b;
    indices[i][2] = _pd.triangles[i].c;
  }

  _softBody = btSoftBodyHelpers::CreateFromTriMesh(World::instance()->world()->getWorldInfo(),
                                                   &vertices[0], &indices[0][0], _nTriangles);

  btSoftBody::Material *pm = _softBody->appendMaterial();
  pm->m_flags -= btSoftBody::fMaterial::DebugDraw;

  _softBody->generateBendingConstraints(2, pm);
  _softBody->randomizeConstraints();
  _softBody->rotate(btQuaternion(q.x, q.y, q.z, q.w));
  _softBody->translate(btVector3(p.x, p.y, p.z));
  _softBody->setTotalMass(mass, true);
  _softBody->generateClusters(64);
  _softBody->getCollisionShape()->setMargin(0.01);
  _softBody->setPose(true, false);

  pm->m_kLST = _data->parameter()->kLST;
  pm->m_kAST = _data->parameter()->kAST;
  pm->m_kVST = _data->parameter()->kVST;

  _softBody->m_cfg.kVCF = _data->parameter()->kVCF;
  _softBody->m_cfg.kDP = _data->parameter()->kDP;
  _softBody->m_cfg.kDG = _data->parameter()->kDG;
  _softBody->m_cfg.kLF = _data->parameter()->kLF;
  _softBody->m_cfg.kPR = _data->parameter()->kPR;
  _softBody->m_cfg.kVC = _data->parameter()->kVC;
  _softBody->m_cfg.kDF = _data->parameter()->kDF;
  _softBody->m_cfg.kMT = _data->parameter()->kMT;
  _softBody->m_cfg.kCHR = _data->parameter()->kCHR;
  _softBody->m_cfg.kKHR = _data->parameter()->kKHR;
  _softBody->m_cfg.kSHR = _data->parameter()->kSHR;
  _softBody->m_cfg.kAHR = _data->parameter()->kAHR;
  _softBody->m_cfg.kSRHR_CL = _data->parameter()->kSRHR_CL;
  _softBody->m_cfg.kSKHR_CL = _data->parameter()->kSKHR_CL;
  _softBody->m_cfg.kSSHR_CL = _data->parameter()->kSSHR_CL;
  _softBody->m_cfg.kSR_SPLT_CL = _data->parameter()->kSR_SPLT_CL;
  _softBody->m_cfg.kSK_SPLT_CL = _data->parameter()->kSK_SPLT_CL;
  _softBody->m_cfg.kSS_SPLT_CL = _data->parameter()->kSS_SPLT_CL;
  _softBody->m_cfg.maxvolume = _data->parameter()->maxvolume;
  _softBody->m_cfg.timescale = _data->parameter()->timescale;
  _softBody->m_cfg.viterations = _data->parameter()->viterations;
  _softBody->m_cfg.piterations = _data->parameter()->piterations;
  _softBody->m_cfg.diterations = _data->parameter()->diterations;
  _softBody->m_cfg.citerations = _data->parameter()->citerations;
  _softBody->m_clusters[0]->m_matching = _data->parameter()->matching;
  _softBody->m_clusters[0]->m_ndamping = _data->parameter()->ndamping;

  _softBody->m_cfg.collisions = btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;

  _data->setNumberOfVertices(_softBody->m_faces.size() * 3);
  _data->setNumberOfNormals(_softBody->m_faces.size() * 3);
  _data->setNumberOfIndices(_softBody->m_faces.size() * 3);

  postPhysicsUpdate(); // to update the vertices
}
#endif // USE_SOFT_BODIES

void Ply::__rigid()
{
  if (_data->isConvex())
  {
    YarsErrorHandler::push("Ply convex not implemented yet!");

    _trimesh = new btTriangleMesh();
    for (int i = 0; i < _nTriangles; i++)
    {
      _trimesh->addTriangle(_vertices[3 * i], _vertices[3 * i + 1], _vertices[3 * i + 2]);
    }

    // cout << "using convex approximation" << endl;
    // cout << "number of _vertices: " << _nVertices << endl;
    // cout << "number of triangles: " << _nTriangles << endl;

    // btConvexShape* tmpConvexShape = new btConvexTriangleMeshShape(_trimesh);
    btConvexTriangleMeshShape *tmpConvexShape = new btConvexTriangleMeshShape(_trimesh);

    btShapeHull *hull = new btShapeHull(tmpConvexShape);
    btScalar margin = tmpConvexShape->getMargin();
    hull->buildHull(margin);
    tmpConvexShape->setUserPointer(hull);

    printf("new numTriangles = %d\n", hull->numTriangles());
    printf("new numIndices   = %d\n", hull->numIndices());
    printf("new numVertices  = %d\n", hull->numVertices());

    btConvexHullShape *convexShape = new btConvexHullShape();
    for (int i = 0; i < hull->numVertices(); i++)
    {
      convexShape->addPoint(hull->getVertexPointer()[i]);
    }

    delete tmpConvexShape;
    delete hull;

    // _collisionShape = tmpConvexShape;
    setCollisionShape(tmpConvexShape);
  }
  else
  {
    int indexStride = 3 * sizeof(int);
    int vertStride = sizeof(btVector3);

    _indexVertexArrays = new btTriangleIndexVertexArray(
        _nTriangles, _indices, indexStride,
        _nVertices, (btScalar *)&_vertices[0], vertStride);

    btGImpactMeshShape *mesh = new btGImpactMeshShape(_indexVertexArrays);
    mesh->setMargin(0.00);
    mesh->updateBound();
    setCollisionShape(mesh);
    // _collisionShape = mesh;
    btCollisionDispatcher *dispatcher = static_cast<btCollisionDispatcher *>(World::instance()->world()->getDispatcher());
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
  }

  Quaternion q(_data->pose().orientation);
  P3D p = _data->pose().position;

  MyMotionState *myMotionState = new MyMotionState(_data);
  myMotionState->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
  setMotionState(myMotionState);

  btCollisionShape *shape = collisionShape();

  btScalar mass = _data->physics()->mass();
  P3D localInertia = _data->physics()->centreOfMass();
  btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
  shape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, myMotionState, shape, inertia);

  if (_data->physics()->useFriction())
    rigidBodyCI.m_friction = _data->physics()->friction();

  // cout << shape << " : " << rigidBodyCI.m_friction << endl;

  if (_data->physics()->useLinearDamping())
    rigidBodyCI.m_linearDamping = _data->physics()->linearDamping();

  if (_data->physics()->useAngularDamping())
    rigidBodyCI.m_angularDamping = _data->physics()->angularDamping();

  if (_data->physics()->useRollingFriction())
    rigidBodyCI.m_rollingFriction = _data->physics()->rollingFriction();

  if (_data->physics()->useRestitution())
    rigidBodyCI.m_restitution = _data->physics()->restitution();

  _rigidBody = new btRigidBody(rigidBodyCI);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

void Ply::postPhysicsUpdate()
{
  if (_data->isSoft())
  {
    int index = 0;
    _data->lock();
    for (int f = 0; f < _softBody->m_faces.size(); f++)
    {
      for (int n = 0; n < 3; n++)
      {
        btVector3 normal = _softBody->m_faces[f].m_normal;
        btVector3 position = _softBody->m_faces[f].m_n[n]->m_x;
        _data->setVertex(index, position[0], position[1], position[2]);
        _data->setNormal(index, normal[0], normal[1], normal[2]);
        // _data->setIndex(index, index++);
      }
    }
    _data->unlock();
  }
  else
  {
    const btVector3 vel = _rigidBody->getLinearVelocity();
    const btVector3 ang = _rigidBody->getAngularVelocity();
    const btVector3 force = _rigidBody->getTotalForce();
    const btVector3 torque = _rigidBody->getTotalTorque();

    _data->setCurrentForce(force[0], force[1], force[2]);
    _data->setCurrentVelocity(vel[0], vel[1], vel[2]);
    _data->setCurrentTorque(torque[0], torque[1], torque[2]);
    _data->setCurrentAngularVelocity(ang[0], ang[1], ang[2]);
  }
}
