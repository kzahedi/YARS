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

  name.push_back(new Pair("",               (char*)"rosiml",      false, 2,  "rosiml"));
  name.push_back(new Pair("",               (char*)"rosiml",      false, -1, "rosiml_complete"));
  name.push_back(new Pair((char*)"rosiml",  (char*)"simulator",    true,  2));
  name.push_back(new Pair((char*)"rosiml",  (char*)"screens",     true,  2));
  name.push_back(new Pair((char*)"rosiml",  (char*)"environment", true,  2));
  name.push_back(new Pair((char*)"rosiml",  (char*)"robots",      false, 3));
  name.push_back(new Pair((char*)"rosiml",  (char*)"traces",      true,  2));
  name.push_back(new Pair((char*)"rosiml",  (char*)"macros",      false,  3));
  name.push_back(new Pair((char*)"rosiml",  (char*)"logging",     true,  2));

  name.push_back(new Pair((char*)"screens", (char*)"screen",      true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"cameras",     false,  2)); 
  name.push_back(new Pair((char*)"screens", (char*)"followables", true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"recording",   true,  2));
  name.push_back(new Pair((char*)"screens", (char*)"visualise",   true,  2));

  name.push_back(new Pair((char*)"screen",  (char*)"osd",         true,  2));
  name.push_back(new Pair((char*)"cameras", (char*)"orbit",       true,  2));

  name.push_back(new Pair((char*)"environment",  (char*)"ground", true));

  name.push_back(new Pair((char*)"robot",  (char*)"body",      false, 2));
  name.push_back(new Pair((char*)"robot",  (char*)"actuators", false, 2));
  name.push_back(new Pair((char*)"actuators",  (char*)"hinge", false, 2));

  // name.push_back(new   Pair( (char*)"screens",     (char*)"sky"));
  // name.push_back(new   Pair( (char*)"screens",     (char*)"followables"));
  // name.push_back(new   Pair( (char*)"screens",     (char*)"shadow"));
  // name.push_back(new   Pair( (char*)"screens",     (char*)"cameras",   false, 2));
  // name.push_back(new   Pair( (char*)"screens",     (char*)"recording"));
  // name.push_back(new   Pair( (char*)"screens",     (char*)"visualise"));
  // name.push_back(new   Pair( (char*)"cameras",     (char*)"orbit"));
  // name.push_back(new   Pair( (char*)"cameras",     (char*)"centre"));
  // name.push_back(new   Pair( (char*)"cameras",     (char*)"offset"));

  // name.push_back(new   Pair( (char*)"traces",      (char*)"trace"));

  // name.push_back(new   Pair( (char*)"rosiml",      (char*)"environment", true,   2));
  // name.push_back(new   Pair( (char*)"environment", (char*)"ground"));
  // name.push_back(new   Pair( (char*)"environment", (char*)"lightSource"));
  // name.push_back(new   Pair( (char*)"environment", (char*)"macro"));

  // name.push_back(new   Pair( (char*)"robot",     (char*)"sensors",     true,   2));
  // name.push_back(new   Pair( (char*)"robot",     (char*)"actuators",   true,   2));
  // name.push_back(new   Pair( (char*)"robot",     (char*)"controller"));
  // name.push_back(new   Pair( (char*)"body",        (char*)"box"));
  // name.push_back(new   Pair( (char*)"body",        (char*)"capsule"));
  // name.push_back(new   Pair( (char*)"body",        (char*)"cylinder"));
  // name.push_back(new   Pair( (char*)"body",        (char*)"sphere"));
  // name.push_back(new   Pair( (char*)"body",        (char*)"mesh"));

  // name.push_back(new   Pair( (char*)"sensors",     (char*)"proximity"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"camera"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"ambientLight"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"internalEnergy"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"orientation"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"position"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"lightDependentResistor"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"sharpDM2Y3A003K0F"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"sharpGP2D12_37"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"feedback"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"velocity"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"deflection"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"signal"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"binaryContact"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"generic", false, -1, "sensors_generic"));

  // name.push_back(new   Pair( (char*)"sensors",     (char*)"proximity"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"camera"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"ambientLight"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"internalEnergy"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"lightDependentResistor"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"sharpDM2Y3A003K0F"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"sharpGP2D12_37"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"feedback"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"velocity"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"deflection"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"signal"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"binaryContact"));
  // name.push_back(new   Pair( (char*)"sensors",     (char*)"universal", true, -1, "sensors_universal"));

  // name.push_back(new   Pair( (char*)"logging",     (char*)"console", true, -1, "logging_console"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"gnuplot", true, -1, "logging_gnuplot"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"selforg", true, -1, "logging_selforg"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"matrixviz", true, -1, "logging_matrixviz"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"object", true, -1, "logging_object"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"actuator", true, -1, "logging_actuator"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"sensor", true, -1, "logging_sensor"));
  // name.push_back(new   Pair( (char*)"logging",     (char*)"controller", true, -1, "logging_controller"));

  // name.push_back(new   Pair( (char*)"actuators",   (char*)"hinge"));
  // name.push_back(new   Pair( (char*)"actuators",   (char*)"slider"));
  // name.push_back(new   Pair( (char*)"actuators",   (char*)"generic", true, -1, "actuators_generic"));

  // name.push_back(new   Pair( (char*)"hinge",       (char*)"noise"));
  // name.push_back(new   Pair( (char*)"hinge",       (char*)"filter"));

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
