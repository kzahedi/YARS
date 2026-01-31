#include <yars/configuration/xsd/graphviz/graph/XsdGraph.h>

#include <yars/configuration/data/Data.h>
#include <yars/configuration/xsd/specification/XsdSpecification.h>

#include <yars/util/YarsErrorHandler.h>


#include <yars/configuration/xsd/graphviz/graph/XsdSequenceGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdChoiceGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdEnumerationGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdRegularExpressionGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdIntervalGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdElementGraphNode.h>

class XsdSequenceGraphNode;
class XsdRegularExpressionGraphNode;
class XsdChoiceGraphNode;
class XsdIntervalGraphNode;


#define DO(a) if((a)->name() == destination->name()) __addChild(destination, a)

XsdGraph::XsdGraph()
{
  _spec                      = Data::instance()->xsd();

  for (auto s = _spec->s_begin(); s != _spec->s_end(); ++s) __add(*s);
  for (auto c = _spec->c_begin(); c != _spec->c_end(); ++c) __add(*c);
  for (auto e = _spec->e_begin(); e != _spec->e_end(); ++e) __add(*e);
  for (auto i = _spec->i_begin(); i != _spec->i_end(); ++i) __add(*i);
  for (auto r = _spec->r_begin(); r != _spec->r_end(); ++r) __add(*r);

  __createGraph();
}

XsdGraph::~XsdGraph()
{
  for(std::vector<XsdGraphNode*>::iterator n = _nodes.begin(); n != _nodes.end(); n++) delete *n;
}


void XsdGraph::__add(XsdSequence *seq)
{
  if(seq->name() == _spec->root()->name()) return;
  XsdSequenceGraphNode *s = new XsdSequenceGraphNode(this, seq);
  _nodes.push_back(s);
}

void XsdGraph::__add(XsdChoice *choice)
{
  XsdChoiceGraphNode *c = new XsdChoiceGraphNode(this, choice);
  _nodes.push_back(c);
}

void XsdGraph::__add(XsdEnumeration *enu)
{
  XsdEnumerationGraphNode *e = new XsdEnumerationGraphNode(enu);
  _nodes.push_back(e);
}

void XsdGraph::__add(XsdInterval *interval)
{
  XsdIntervalGraphNode *i = new XsdIntervalGraphNode(interval);
  _nodes.push_back(i);
}

void XsdGraph::__add(XsdRegularExpression *regexp)
{
  XsdRegularExpressionGraphNode *r = new XsdRegularExpressionGraphNode(regexp);
  _nodes.push_back(r);
}

std::vector<XsdGraphNodeInstance*>::iterator XsdGraph::i_begin()
{
  return _instances.begin();
}

std::vector<XsdGraphNodeInstance*>::iterator XsdGraph::i_end()
{
  return _instances.end();
}

std::vector<XsdGraphNode*>::iterator XsdGraph::n_begin()
{
  return _nodes.begin();
}

std::vector<XsdGraphNode*>::iterator XsdGraph::n_end()
{
  return _nodes.end();
}
XsdGraphNodeInstance* XsdGraph::get(string parent, string name)
{
  for (auto* i : _instances)
  {
    if (i->name() == parent)
    {
      for (auto c = i->begin(); c != i->end(); ++c)
      {
        if ((*c)->name() == name)
        {
          return (*c);
        }
      }
    }
  }
  return _root;
}

void XsdGraph::__createGraph()
{
  XsdSequence          *seq  = _spec->root();
  XsdSequenceGraphNode *root = new XsdSequenceGraphNode(this, seq);
  _root                      = new XsdGraphNodeInstance(root->name(), root->name(), root, "");
  _instances.push_back(_root);

  int index = 1;
  // attributes not used
  for (auto e = seq->e_begin(); e != seq->e_end(); ++e) __add(_root, *e);
  // int index = 0;
  // FOREACHP(XsdGraphNodeInstance*, n, _root)
  // {
    // (*n)->setPort(index++);
  // }
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdElement *element)
{
  XsdGraphNode *node             = __findNode(element->type());
  XsdGraphNodeInstance *instance = nullptr;
  stringstream sst;
  sst << element->minOccurs() << ":" << element->maxOccurs();

  int index = 0;
  if(node != nullptr)
  {
    instance = new XsdGraphNodeInstance(element->name(), element->type(), node, sst.str());
    __add(instance, node->spec());
  }
  else
  {
    XsdElementGraphNode *node = new XsdElementGraphNode(this, element);
    instance = new XsdGraphNodeInstance(element->name(), element->name(), node, sst.str());
    // attributes not used
    for (auto n = instance->begin(); n != instance->end(); ++n)
    {
      (*n)->setPort(index++);
    }
  }

  _instances.push_back(instance);
  parent->push_back(instance);


}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdSequence *seq)
{
  int index = 1;
  // attributes not used
  for (auto e = seq->e_begin(); e != seq->e_end(); ++e) __add(parent, *e);
  for (auto c = seq->c_begin(); c != seq->c_end(); ++c) __add(parent, *c);
  for (auto r = seq->r_begin(); r != seq->r_end(); ++r) __add(parent, *r);
  for (auto i = seq->i_begin(); i != seq->i_end(); ++i) __add(parent, *i);
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdChoice *choice)
{
  int index = 0;
  // attributes not used
  for (auto e = choice->e_begin(); e != choice->e_end(); ++e) __add(parent, *e);

  std::vector<XsdGraphNodeInstance*> v;
  for (auto s = choice->s_begin(); s != choice->s_end(); ++s)
  {
    XsdGraphNodeInstance *i = new XsdGraphNodeInstance("", "", nullptr, "");
    __add(i, *s);
    v.push_back(i);
  }

  for (auto* p : v)
  {
    for (auto i = p->begin(); i != p->end(); ++i)
    {
      parent->push_back(*i);
      (*i)->setPort(index);
    }
    index++;
  }
  v.clear();
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdAttribute *attribute, int index)
{
  XsdGraphNode *node             = __findNode(attribute->type());
  XsdGraphNodeInstance *instance = nullptr;

  if(node != nullptr)
  {
    instance = new XsdGraphNodeInstance(attribute->name(), attribute->type(), node, "");
    instance->setPort(index);
    __add(instance, node->spec());

    _instances.push_back(instance);
    parent->push_back(instance);
  }
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdRegularExpression *regexp)
{
  XsdGraphNode *node             = __findNode(regexp->type());
  XsdGraphNodeInstance *instance = nullptr;

  if(node != nullptr)
  {
    instance = new XsdGraphNodeInstance(regexp->name(), regexp->type(), node, "");
    __add(instance, node->spec());

    _instances.push_back(instance);
    parent->push_back(instance);
  }
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdInterval *interval)
{
  XsdGraphNode *node             = __findNode(interval->type());
  XsdGraphNodeInstance *instance = nullptr;

  if(node != nullptr)
  {
    instance = new XsdGraphNodeInstance(interval->name(), interval->type(), node, "");
    __add(instance, node->spec());

    _instances.push_back(instance);
    parent->push_back(instance);
  }
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdEnumeration *enumeration)
{
  XsdGraphNode *node             = __findNode(enumeration->type());
  XsdGraphNodeInstance *instance = nullptr;

  if(node != nullptr)
  {
    instance = new XsdGraphNodeInstance(enumeration->name(), enumeration->type(), node, "");
    __add(instance, node->spec());

    _instances.push_back(instance);
    parent->push_back(instance);
  }
}

void XsdGraph::__add(XsdGraphNodeInstance *parent, XsdNode *node)
{
  switch(node->nodeType())
  {
    case XSD_NODE_TYPE_SEQUENCE:
      __add(parent, (XsdSequence*)node);
      break;
    case XSD_NODE_TYPE_CHOICE:
       __add(parent, (XsdChoice*)node);
      break;
    case XSD_NODE_TYPE_ELEMENT:
       __add(parent, (XsdElement*)node);
      break;
    case XSD_NODE_TYPE_REG_EXP:
       __add(parent, (XsdRegularExpression*)node);
      break;
    case XSD_NODE_TYPE_INTERVAL:
       __add(parent, (XsdInterval*)node);
      break;
    case XSD_NODE_TYPE_ATTRIBUTE:
       // __add(parent, (XsdAttribute*)node, 0);
      break;
    case XSD_NODE_TYPE_ENUMERATION:
       __add(parent, (XsdEnumeration*)node);
      break;
    default:
      cout << "uncaught " << node->nodeType() << endl;
      exit(-1);
  }
}


XsdGraphNode* XsdGraph::__findNode(string name)
{
  for (auto* n : _nodes)
  {
    if (n->name() == name)
    {
      return n;
    }
  }
  return nullptr;
}
