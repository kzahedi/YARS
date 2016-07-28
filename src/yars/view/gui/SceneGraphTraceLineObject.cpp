#include "SceneGraphTraceLineObject.h"
#include "configuration/YarsConfiguration.h"

#include <sstream>


SceneGraphTraceLineObject::SceneGraphTraceLineObject(
    DataTraceLine *trace, Ogre::SceneNode* root, Ogre::SceneManager* sceneManager, int index)
  : SceneGraphObjectNode(root, sceneManager)
{
  _trace      = trace;
  _node       = _root->createChildSceneNode();
  _billboard  = NULL;
  _particles  = NULL;
  _trail      = NULL;
  _reset      = false;
  update();

  stringstream oss;
  Colour c  = _trace->color();
  if(_trace->useLine())
  {
    oss << _trace->target() << " trace line " << index << endl;
    Colour fc = _trace->finalColour();
    yReal f = (yReal)_trace->length();
    yReal r = (c.red()   - fc.red())   / f;
    yReal g = (c.green() - fc.green()) / f;
    yReal b = (c.blue()  - fc.blue())  / f;
    yReal a = (c.alpha() - fc.alpha()) / f;
    yReal w = (_trace->lineWidth() - trace->finalWidth()) / f;
    _trail = sceneManager->createRibbonTrail(oss.str());
    _trail->setVisible(true);
    _trail->setMaterialName(_trace->texture());
    _trail->setTrailLength(_trace->length());
    _trail->setMaxChainElements(_trace->nrOfElements());
    _trail->setInitialColour(0, Ogre::ColourValue(c.red(), c.green(), c.blue(), c.alpha()));
    _trail->setColourChange(0,  Ogre::ColourValue(r,       g,         b,        a));
    _trail->setInitialWidth(0,  _trace->lineWidth());
    _trail->setWidthChange(0,   w);
    _trail->addNode(_node);
    sceneManager->getRootSceneNode()->attachObject(_trail);
    _trail->setFaceCamera(true);
  }

  if(_trace->useBillboard())
  {
    oss.str("");
    oss << _trace->target() << " billboard " << index << endl;
    _billboard = sceneManager->createBillboardSet(oss.str());
    _billboard->setMaterialName(_trace->billboard());
    _billboard->setDefaultDimensions(10.0 * _trace->lineWidth(), 10.0 * _trace->lineWidth());
    _billboard->createBillboard(0,0,0);
    _billboard->getBillboard(0)->setColour(Ogre::ColourValue(c.red(),  c.green(),  c.blue(),  c.alpha()));
    sceneManager->getRootSceneNode()->attachObject(_billboard);
  }

  if(_trace->useParticles())
  {
    oss.str("");
    oss << _trace->target() << " particles " << index << endl;
    _particles = sceneManager->createParticleSystem(oss.str(), _trace->particles());
    oss.str("");
    oss << "Particle node " << _trace->target() << " particles" << endl;
    Ogre::SceneNode* particleNode = _node->createChildSceneNode(oss.str());
    particleNode->attachObject(_particles);

    Ogre::ParticleEmitter *emitter = _particles->getEmitter(0);
    emitter->setColour(Ogre::ColourValue(c.red(), c.green(), c.blue(), c.alpha()));
    emitter->setMinTimeToLive(_trace->time()/2.0);
    emitter->setMaxTimeToLive(_trace->time());
  }
}

SceneGraphTraceLineObject::~SceneGraphTraceLineObject()
{
}

void SceneGraphTraceLineObject::update()
{
  if(_trace->size() > 0)
  {
    P3D p = _trace->position();
    _node->setPosition(p.x, p.y, p.z);
    if(_billboard != NULL) _billboard->getBillboard(0)->setPosition(p.x, p.z, -p.y);
    
  }
  if(_reset == true)
  {
    _reset = false;
    _trail->removeNode(_node);
    _sceneManager->getRootSceneNode()->detachObject(_trail);

    if(_trace->useLine())
    {
      for(int i = 0; i < _trail->getNumberOfChains(); i++)
      {
        _trail->clearChain(i);
      }
    }
    if(_trace->useParticles())
    {
      _particles->clear();
    }

    _trail->addNode(_node);
    _sceneManager->getRootSceneNode()->attachObject(_trail);
  }
}

void SceneGraphTraceLineObject::reset()
{
  _reset = true;

  // _trail->removeNode(_node);
  // _sceneManager->getRootSceneNode()->detachObject(_trail);

  // if(_trace->useLine())
  // {
    // cout << "SceneGraphTraceLineObject: reset" << endl;
    // for(int i = 0; i < _trail->getNumberOfChains(); i++)
    // {
      // _trail->clearChain(i);
    // }
  // }
  // if(_trace->useParticles())
  // {
    // _particles->clear();
  // }

  // _trail->addNode(_node);
  // _sceneManager->getRootSceneNode()->attachObject(_trail);

}
