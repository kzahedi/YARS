#include <yars/configuration/xsd/graphviz/XsdGraphvizExporter.h>

#include <yars/util/FileSystemOperations.h>

class Pair
{
  public:
    Pair(string p, string n, bool rl = false, int d = -1, string f = "")
    {
      parent   = p;
      name     = n;
      depth    = d;
      lr       = rl;
      if(f == "")
      {
        stringstream oss;
        oss << p << "_" << n;
        filename = oss.str();
      }
      else
      {
        filename = f;
      }
    }

    string parent;
    string name;
    string filename;
    int    depth;
    bool   lr;
};

void XsdGraphvizExporter::writeDotFile(string filetype)
{

  cout << "Exporting to ./" << filetype << endl;

  std::vector<Pair*> name;

  name.push_back(new Pair("", (char*)"rosiml", false, 2,  "rosiml"));
  name.push_back(new Pair("", (char*)"rosiml", false, -1, "rosiml_complete"));

  name.push_back(new Pair((char*)"rosiml", (char*)"simulator",   true,  2));
  name.push_back(new Pair((char*)"rosiml", (char*)"screens",     true,  2));
  name.push_back(new Pair((char*)"rosiml", (char*)"environment", true,  2));
  name.push_back(new Pair((char*)"rosiml", (char*)"robots",      false, 3));
  name.push_back(new Pair((char*)"rosiml", (char*)"traces",      true,  2));
  name.push_back(new Pair((char*)"rosiml", (char*)"macros",      false, 3));
  name.push_back(new Pair((char*)"rosiml", (char*)"logging",     true,  2));

  name.push_back(new Pair((char*)"screens", (char*)"screen",      true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"cameras",     false, 2));
  name.push_back(new Pair((char*)"screens", (char*)"followables", true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"recording",   true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"visualise",   true,  2));
  name.push_back(new Pair((char*)"screen",  (char*)"osd",         true,  2));

  name.push_back(new Pair((char*)"cameras", (char*)"orbit", true, 2));

  name.push_back(new Pair((char*)"environment", (char*)"ground", true));
  name.push_back(new Pair((char*)"environment", (char*)"lightSource"));
  name.push_back(new Pair((char*)"environment", (char*)"macro"));

  name.push_back(new Pair((char*)"robot", (char*)"body",      false, 2));
  name.push_back(new Pair((char*)"robot", (char*)"actuators", false, 2));
  name.push_back(new Pair((char*)"robot", (char*)"sensors",   false, 2));

  name.push_back(new Pair((char*)"sensors", (char*)"proximity",         false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"camera",            false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"ambientLight",      false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"internalEnergy",    false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"orientation",       false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"position",          false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"ldr",               false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"sharpDM2Y3A003K0F", false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"sharpGP2D12_37",    false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"feedback",          false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"velocity",          false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"deflection",        false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"signal",            false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"binaryContact",     false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"generic",           false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"ov",                false, 2));
  name.push_back(new Pair((char*)"sensors", (char*)"oav",               false, 2));

  name.push_back(new Pair((char*)"body", (char*)"box",      false, 2));
  name.push_back(new Pair((char*)"body", (char*)"capsule",  false, 2));
  name.push_back(new Pair((char*)"body", (char*)"cylinder", false, 2));
  name.push_back(new Pair((char*)"body", (char*)"sphere",   false, 2));
  name.push_back(new Pair((char*)"body", (char*)"mesh",     false, 2));
  name.push_back(new Pair((char*)"body", (char*)"ply",      false, 2));

  name.push_back(new Pair((char*)"box", (char*)"visualisation", false, 2, "visualisation"));
  name.push_back(new Pair((char*)"box", (char*)"physics",      false, -1, "physics"));

  name.push_back(new Pair((char*)"rosiml", (char*)"traces", true,  2));
  name.push_back(new Pair((char*)"traces", (char*)"trace",  false, 2));

  name.push_back(new Pair((char*)"logging", (char*)"console",    true, -1, "logging_console"));
  name.push_back(new Pair((char*)"logging", (char*)"gnuplot",    true, -1, "logging_gnuplot"));
  name.push_back(new Pair((char*)"logging", (char*)"selforg",    true, -1, "logging_selforg"));
  name.push_back(new Pair((char*)"logging", (char*)"matrixviz",  true, -1, "logging_matrixviz"));
  name.push_back(new Pair((char*)"logging", (char*)"object",     true, -1, "logging_object"));
  name.push_back(new Pair((char*)"logging", (char*)"actuator",   true, -1, "logging_actuator"));
  name.push_back(new Pair((char*)"logging", (char*)"sensor",     true, -1, "logging_sensor"));
  name.push_back(new Pair((char*)"logging", (char*)"controller", true, -1, "logging_controller"));
  name.push_back(new Pair((char*)"logging", (char*)"csv",        true, -1, "logging_csv"));
  name.push_back(new Pair((char*)"logging", (char*)"blender",    true, -1, "logging_blender"));

  // name.push_back(new   Pair( (char*)"signals",     (char*)"keyboard"));
  // name.push_back(new   Pair( (char*)"signals",     (char*)"periodic"));
  // name.push_back(new   Pair( (char*)"signals",     (char*)"triggered"));


  XsdGraphvizGenerator *xsd = new XsdGraphvizGenerator();

  stringstream dir;
  dir << "./" << filetype;
  fs::path dotDir = "./dot";
  fs::path exportDir = dir.str();


  if(!fs::exists(dotDir))    fs::create_directory(dotDir);
  if(!fs::exists(exportDir)) fs::create_directory(exportDir);

  ofstream myfile;

  stringstream filename;
  for(std::vector<Pair*>::iterator i = name.begin(); i != name.end(); i++)
  {
    filename.str("");
    if((*i)->filename.length() == 0) filename << "./dot/" << (*i)->name     << ".dot";
    else                             filename << "./dot/" << (*i)->filename << ".dot";
    xsd->generate((*i)->parent, (*i)->name, (*i)->lr, (*i)->depth);
    myfile.open(filename.str().c_str());
    myfile << (*xsd) << endl;
    myfile.close();
    cout << "writing " << filename.str() << endl;
  }

  if (FileSystemOperations::doesExecutableExist(filetype))
  {
    cout << "Found dot executable. Exporting dot -> " << filetype << endl;
    stringstream oss;
    for(std::vector<Pair*>::iterator i = name.begin(); i != name.end(); i++)
    {
      oss.str("");
      filename.str("");
      if((*i)->filename.length() == 0) filename << "./dot/" << (*i)->name     << ".dot";
      else                             filename << "./dot/" << (*i)->filename << ".dot";
      oss << "dot -T" << filetype << " " << filename.str() << " -o ";
      filename.str("");
      if((*i)->filename.length() == 0) filename << filetype << "/" << (*i)->name << "." << filetype;
      else                             filename << filetype << "/" << (*i)->filename << "." << filetype;
      oss << filename.str();
      system(oss.str().c_str());
      cout << oss. str() << endl;
    }
  }
  else
  {
    cout << "Cannot find dot executable." << endl;
  }

  cout << "done." << endl;
}
