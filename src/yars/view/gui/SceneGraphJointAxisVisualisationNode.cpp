#include "SceneGraphJointAxisVisualisationNode.h"

#include <yars/defines/defaults.h>
#include <yars/configuration/data/Data.h>

#define SET_INDICES\
  for(unsigned int i = 0; i < indices.size()-2; i++)\
  {\
    if(i%2==0)\
    {\
      manual->index(indices[i+0]);\
      manual->index(indices[i+1]);\
      manual->index(indices[i+2]);\
    }\
    else\
    {\
      manual->index(indices[(i-1)+1]);\
      manual->index(indices[(i-1)+3]);\
      manual->index(indices[(i-1)+2]);\
    }\
  }

#define SET_INDICES_2\
  for(unsigned int i = 0; i < indices.size()-2; i++)\
  {\
      manual->index(indices[i+0]);\
      manual->index(indices[i+1]);\
      manual->index(indices[i+2]);\
  }

SceneGraphJointAxisVisualisationNode::SceneGraphJointAxisVisualisationNode(
    DataActuator *data, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r,sm)
{
  _data = data;
  _dimension = Data::instance()->current()->screens()->jointsVisualisationDimension();

  stringstream oss;

  _node   = _root->createChildSceneNode();

  oss.str("");
  oss << _data->name() << " visualisation X";
  _manualX = _sceneManager->createManualObject(oss.str());
  __topCap(_manualX);
  __bottomCap(_manualX);
  __body(_manualX);
  _manualX->setMaterialName(0, Data::instance()->current()->screens()->xAxisTexture());
  _manualX->setMaterialName(1, Data::instance()->current()->screens()->xAxisTexture());
  _manualX->setMaterialName(2, Data::instance()->current()->screens()->xAxisTexture());
  ::Quaternion qX(P3D(0, M_PI_2, 0));
  Ogre::SceneNode *nodeX = _node->createChildSceneNode();
  nodeX->setOrientation(Ogre::Quaternion(qX.w, qX.x, qX.y, qX.z));
  nodeX->attachObject(_manualX);

  oss.str("");
  oss << _data->name() << " visualisation Y";
  _manualY = _sceneManager->createManualObject(oss.str());
  __topCap(_manualY);
  __bottomCap(_manualY);
  __body(_manualY);
  _manualY->setMaterialName(0, Data::instance()->current()->screens()->yAxisTexture());
  _manualY->setMaterialName(1, Data::instance()->current()->screens()->yAxisTexture());
  _manualY->setMaterialName(2, Data::instance()->current()->screens()->yAxisTexture());
  ::Quaternion qY(P3D(-M_PI_2, 0, 0));
  Ogre::SceneNode *nodeY = _node->createChildSceneNode();
  nodeY->setOrientation(Ogre::Quaternion(qY.w, qY.x, qY.y, qY.z));
  nodeY->attachObject(_manualY);

  oss.str("");
  oss << _data->name() << " visualisation Z";
  _manualZ = _sceneManager->createManualObject(oss.str());
  __topCap(_manualZ);
  __bottomCap(_manualZ);
  __body(_manualZ);
  _manualZ->setMaterialName(0, Data::instance()->current()->screens()->zAxisTexture());
  _manualZ->setMaterialName(1, Data::instance()->current()->screens()->zAxisTexture());
  _manualZ->setMaterialName(2, Data::instance()->current()->screens()->zAxisTexture());
  ::Quaternion qZ(P3D(0, 0, 0));
  Ogre::SceneNode *nodeZ = _node->createChildSceneNode();
  nodeZ->setOrientation(Ogre::Quaternion(qZ.w, qZ.x, qZ.y, qZ.z));
  nodeZ->attachObject(_manualZ);

  update();
}

void SceneGraphJointAxisVisualisationNode::update()
{
  P3D        p = _data->getCurrentAxisPosition();
  Quaternion q = _data->getCurrentAxisOrientation();
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

void SceneGraphJointAxisVisualisationNode::__topCap(Ogre::ManualObject *manual)
{
  double z     = _dimension.height / 2.0;
  double delta = M_PI / (double)DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY;
  double r     = _dimension.radius;

  int index = 0;
  std::vector<int> indices;
  manual->begin("top", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  manual->position(0,0,z);
  manual->normal(0,0,1);
  manual->textureCoord(0.5,0.5);
  indices.push_back(index++);
  for(int i = 0; i < 2*DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY + 1; i++)
  {
    double x = sin(delta * i) * r;
    double y = cos(delta * i) * r;
    double u = sin(delta * i) * 0.5 + 0.5;
    double v = cos(delta * i) * 0.5 + 0.5;

    manual->position(x,y,z);
    manual->normal(0,0,1);
    manual->textureCoord(u,v);
    indices.push_back(index++);
  }

  for(unsigned int i = 0; i < indices.size()-1; i++)
  {
    manual->index(0);
    manual->index(indices[i+1]);
    manual->index(indices[i]);
  }

  // manual->index(0);
  // manual->index(indices[indices.size()]);
  // manual->index(indices[1]);

  manual->setVisible(true);
  manual->setCastShadows(false);
  manual->end();
}

void SceneGraphJointAxisVisualisationNode::__bottomCap(Ogre::ManualObject *manual)
{

  double z     = -_dimension.height / 2.0;
  double delta = M_PI / (double)DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY;
  double r     = _dimension.radius;

  int index = 0;
  std::vector<int> indices;
  manual->begin("bottom", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  manual->position(0,0,0);
  manual->normal(0,0,-1);
  manual->textureCoord(0.5,0.5);
  indices.push_back(index++);
  for(int i = 0; i < 2*DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY + 1; i++)
  {
    double x = sin(delta * i) * r;
    double y = cos(delta * i) * r;
    double u = sin(delta * i) * 0.5 + 0.5;
    double v = cos(delta * i) * 0.5 + 0.5;

    manual->position(x,y,0);
    manual->normal(0,0,-1);
    manual->textureCoord(u,v);
    indices.push_back(index++);
  }

  for(unsigned int i = 0; i < indices.size()-1; i++)
  {
    manual->index(0);
    manual->index(indices[i]);
    manual->index(indices[i+1]);
  }

  // manual->index(0);
  // manual->index(indices[1]);
  // manual->index(indices[indices.size()]);

  manual->setVisible(true);
  manual->setCastShadows(false);
  manual->end();
}

void SceneGraphJointAxisVisualisationNode::__body(Ogre::ManualObject *manual)
{
  double x, y;
  double z     = _dimension.height / 2.0;
  double delta = 2.0 * M_PI / (double)DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY;
  double r     = _dimension.radius;
  double du    = 1.0/(double)DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY;
  double u     = 0.0;

  manual->begin("body", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  std::vector<int> indices;
  int index = 0;
  for(int i = 0; i < DEFAULT_CAPPED_JOINT_AXIS_VISUALISATION_QUALITY + 1; i++)
  {
    x = sin(delta * i);
    y = cos(delta * i);

    manual->position(x*r,y*r,0);
    manual->normal(x,y,0);
    manual->textureCoord(u,1);
    indices.push_back(index++);

    manual->position(x*r,y*r,z);
    manual->normal(x,y,0);
    manual->textureCoord(u,0);
    indices.push_back(index++);

    u += du;
  }

  SET_INDICES;

  manual->setVisible(true);
  manual->setCastShadows(false);
  manual->end();
}

Ogre::Quaternion SceneGraphJointAxisVisualisationNode::__calculateAxisX(P3D axis)
{
  Ogre::Vector3 defaultAxis(1.0, 0.0, 0.0);
  Ogre::Vector3 desiredAxis(axis.x, axis.y, axis.z);
  defaultAxis.normalise();
  desiredAxis.normalise();
  Ogre::Vector3 rotationAxis = defaultAxis.crossProduct(desiredAxis);
  Ogre::Quaternion q;

  if(rotationAxis.length() < 0.0000001)
  {
    if(desiredAxis.x > 0) q = Ogre::Quaternion(Ogre::Radian(0.0),  Ogre::Vector3(0.0,1.0,0.0));
    else                  q = Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(0.0,1.0,0.0));
  }
  else
  {
    Ogre::Real angle = acos(defaultAxis.dotProduct(desiredAxis));
    q                = Ogre::Quaternion(Ogre::Radian(angle), rotationAxis);
  }
  return q;
}
