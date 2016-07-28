#ifndef __XSD_GRAPHVIZ_EXPORTER_H__
#define __XSD_GRAPHVIZ_EXPORTER_H__

#include <yars/configuration/xsd/graphviz/XsdGraphvizGenerator.h>
#include <yars/configuration/data/DataRobotSimulationDescription.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace fs = boost::filesystem;

class XsdGraphvizExporter
{
  public:
    static void writeDotFile(string filetype = "pdf");

};

#endif // __XSD_GRAPHVIZ_EXPORTER_H__
