#include <yars/configuration/xsd/graphviz/graph/XsdEnumerationGraphNode.h>


#include <yars/configuration/data/Data.h>

XsdEnumerationGraphNode::XsdEnumerationGraphNode(XsdEnumeration *spec)
{
  _spec = spec;
  for (auto v = spec->v_begin(); v != spec->v_end(); ++v)
  {
    _oss << "<tr> <td bgcolor=\"" << SPECIFICATION_BGCOLOR << "\"> " << *v << " </td> </tr>";
  }
  _type = "enum";
  _specification = _oss.str();
}

string XsdEnumerationGraphNode::customLabel(string label)
{
  _oss.str("");
  _oss << " [label=<";
  _oss << "<table bgcolor=\"" << ENUM_BGCOLOR << "\" border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"0\">";
  _oss << "<tr><td> " << label  << "</td></tr>";// << "&nbsp;:&nbsp;" << _type << "</td></tr>";
  _oss << _specification;
  _oss << "</table>";
  _oss << ">];";
  return _oss.str();
}

string XsdEnumerationGraphNode::content()
{
  _oss.str("");
  for (auto v = _spec->v_begin(); v != _spec->v_end(); ++v)
  {
    _oss << "<tr> <td> " << *v << " </td> </tr>";
  }
  return _oss.str();
}

string XsdEnumerationGraphNode::name()
{
  return _spec->name();
}

XsdEnumeration* XsdEnumerationGraphNode::spec()
{
  return _spec;
}
