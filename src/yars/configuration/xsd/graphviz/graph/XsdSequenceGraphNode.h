#ifndef __XSD_SEQUENCE_GRAPH_NODE_H__
#define __XSD_SEQUENCE_GRAPH_NODE_H__

#include <yars/configuration/xsd/graphviz/graph/XsdGraphNode.h>
#include <yars/configuration/xsd/graphviz/graph/XsdGraph.h>
#include <yars/configuration/xsd/specification/XsdSequence.h>

#include <string>
#include <sstream>

using namespace std;

class XsdSequenceGraphNode : public XsdGraphNode
{
  public:
    XsdSequenceGraphNode(XsdGraph* root, XsdSequence *spec);
    string customLabel(string label);
    string name();
    string content();
    XsdSequence* spec();

  private:
    XsdGraphNode* __findNode(string name);
    stringstream _oss;
    XsdSequence* _spec;
    XsdGraph*    _graph;
};

#endif // __XSD_SEQUENCE_GRAPH_NODE_H__
