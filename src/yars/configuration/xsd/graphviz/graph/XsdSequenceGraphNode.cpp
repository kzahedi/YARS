#include <yars/configuration/xsd/graphviz/graph/XsdSequenceGraphNode.h>
#include <yars/configuration/data/Data.h>

#include <yars/util/stl_macros.h>

#include <iostream>

using namespace std;

XsdSequenceGraphNode::XsdSequenceGraphNode(XsdGraph* graph, XsdSequence* spec)
{
  _spec  = spec;
  _graph = graph;
}

string XsdSequenceGraphNode::customLabel(string label)
{
  _oss.str("");
  _oss << " [label=<";
  _oss << "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"0\">";
  _oss << "<tr><td>";
  _oss << "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
  _oss << "<tr><td port=\"-1\" colspan=\"" << MAX(_spec->a_size(),1) << "\" bgcolor=\"" << ELEMENT_BGCOLOR << "\">"<< label << "</td></tr>";
  int portIndex = 1;
  stringstream s_label;
  stringstream s_optional;
  stringstream s_type;
  FOREACHF(XsdAttribute*, a, _spec, ->a_begin(), ->a_end())
  {
    s_label << "<td bgcolor=\"" << ATTRIBUTE_BGCOLOR << "\">"<< (*a)->name() << "</td>";
    if((*a)->required())
    {
      s_optional << "<td bgcolor=\"" << ATTRIBUTE_BGCOLOR << "\"> <font color=\"" << REQUIRED_COLOR << "\"> required </font> </td>";
    }
    else
    {
      s_optional << "<td bgcolor=\"" << ATTRIBUTE_BGCOLOR << "\"> <font color=\"" << OPTIONAL_COLOR << "\"> optional </font> </td>";
    }
    if(!hasDefinition((*a)->type()))
    {
      s_type << "<td port=\"" << portIndex++ << "\" bgcolor=\"" << ATTRIBUTE_BGCOLOR << "\" valign=\"top\">"<< (*a)->type() << "</td>";
    }
    else
    {
      XsdGraphNode *node      = __findNode((*a)->type());

      if(node != NULL)
      {
        s_type << "<td bgcolor=\"" << ATTRIBUTE_BGCOLOR << "\" valign=\"top\">";
        s_type << "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"0\">";
        s_type << node->content();
        s_type << "</table>";
        s_type << "</td>";
      }
    }
  }
  if(s_label.str().length()    > 0) _oss << "<tr>" << s_label.str()    << "</tr>";
  if(s_optional.str().length() > 0) _oss << "<tr>" << s_optional.str() << "</tr>";
  if(s_type.str().length()     > 0) _oss << "<tr>" << s_type.str()     << "</tr>";
  _oss << "</table>";
  _oss << "</td>";
  _oss << "</tr>";
  _oss << "</table>";
  _oss << ">];";
  return _oss.str();
}

string XsdSequenceGraphNode::name()
{
  return _spec->name();
}

XsdSequence* XsdSequenceGraphNode::spec()
{
  return _spec;
}

XsdGraphNode* XsdSequenceGraphNode::__findNode(string name)
{
  for(vector<XsdGraphNode*>::iterator n = _graph->n_begin(); n != _graph->n_end(); n++)
  {
    if((*n)->name() == name)
    {
      return (*n);
    }
  }
  return NULL;
}

string XsdSequenceGraphNode::content()
{
  _oss.str("");
  _oss << "sequence";
  return _oss.str();
}
