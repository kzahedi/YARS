#include <yars/configuration/data/DataLogging.h>

#include <yars/util/YarsErrorHandler.h>

#include <sstream>

using namespace std;

#define YARS_STRING_SENSOR              (char*)"sensor"
#define YARS_STRING_SENSOR_DEFINITION   (char*)"sensor_logging_definition"

#define YARS_STRING_ACTUATOR            (char*)"actuator"
#define YARS_STRING_ACTUATOR_DEFINITION (char*)"actuator_logging_definition"

#define YARS_STRING_REF                 (char*)"ref"
#define YARS_STRING_NAME                (char*)"name"
#define YARS_STRING_PIPE                (char*)"pipe"
#define YARS_STRING_TYPE                (char*)"type"
#define YARS_STRING_LOGGING_TYPE_ENUM   (char*)"logging_type_enum"

// #define YARS_STRING_CONTROLLER            (char*)"controller"
// #define YARS_STRING_CONTROLLER_DEFINITION (char*)"controller_logging_definition"

DataLogging::DataLogging(DataNode *parent)
  : DataNode(parent)
{
}

DataLogging::~DataLogging()
{
  // nothing to be done
}

void DataLogging::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LOGGING))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_LOGGING_OBJECT))
  {
    DataLoggingObject *object = new DataLoggingObject(this);
    _objects.push_back(object);
    current = object;
    current->add(element);
  }

  if(element->opening(YARS_STRING_LOGGING_SENSOR))
  {
    DataLoggingSensor *sensor = new DataLoggingSensor(this);
    _sensors.push_back(sensor);
    current = sensor;
    current->add(element);
  }

  if(element->opening(YARS_STRING_LOGGING_ACTUATOR))
  {
    DataLoggingActuator *actuator = new DataLoggingActuator(this);
    _actuators.push_back(actuator);
    current = actuator;
    current->add(element);
  }

  if(element->opening(YARS_STRING_CONTROLLER))
  {
    DataLoggingController *controller = new DataLoggingController(this);
    _controllers.push_back(controller);
    current = controller;
    current->add(element);
  }

  if(element->opening(YARS_STRING_CONSOLE))
  {
    DataLoggingConsole *console = new DataLoggingConsole(this);
    _consolelogger.push_back(console);
    current = console;
    current->add(element);
  }

  if(element->opening(YARS_STRING_FILE_LOGGER))
  {
    DataLoggingFile *file = new DataLoggingFile(this);
    _filelogger.push_back(file);
    current = file;
    current->add(element);
  }

  if(element->opening(YARS_STRING_CSV_LOGGER))
  {
    DataLoggingCSV *csv = new DataLoggingCSV(this);
    _csvlogger.push_back(csv);
    current = csv;
    current->add(element);
  }

  if(element->opening(YARS_STRING_BLENDER_LOGGER))
  {
    DataLoggingBlender *blender = new DataLoggingBlender(this);
    _blenderlogger.push_back(blender);
    current = blender;
    current->add(element);
  }

  if(element->opening(YARS_STRING_GNUPLOT_LOGGER))
  {
    DataLoggingGnuplot *gnuplot = new DataLoggingGnuplot(this);
    _gnuplotlogger.push_back(gnuplot);
    current = gnuplot;
    current->add(element);
  }

  if(element->opening(YARS_STRING_SELFORG_LOGGER))
  {
    DataLoggingSelforg *selforg = new DataLoggingSelforg(this);
    _selforglogger.push_back(selforg);
    current = selforg;
    current->add(element);
  }

}

void DataLogging::createXsd(XsdSpecification *spec)
{
  XsdSequence *loggingDefinition = new XsdSequence(YARS_STRING_LOGGING_DEFINITION);
  spec->add(loggingDefinition);

  XsdChoice *loggingObjectChoice = new XsdChoice("",              "0", YARS_STRING_XSD_UNBOUNDED);
  loggingObjectChoice->add(XE(YARS_STRING_LOGGING_OBJECT, YARS_STRING_LOGGING_OBJECT_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingObjectChoice->add(XE(YARS_STRING_LOGGING_SENSOR, YARS_STRING_LOGGING_SENSOR_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingObjectChoice->add(XE(YARS_STRING_LOGGING_ACTUATOR, YARS_STRING_LOGGING_ACTUATOR_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingObjectChoice->add(XE(YARS_STRING_LOGGING_CONTROLLER, YARS_STRING_LOGGING_CONTROLLER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingDefinition->add(loggingObjectChoice);

  XsdChoice *loggingOutputChoice = new XsdChoice("",              "0", YARS_STRING_XSD_UNBOUNDED);
  loggingOutputChoice->add(XE(YARS_STRING_CONSOLE, YARS_STRING_CONSOLE_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingOutputChoice->add(XE(YARS_STRING_FILE_LOGGER, YARS_STRING_FILE_LOGGER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingOutputChoice->add(XE(YARS_STRING_GNUPLOT_LOGGER, YARS_STRING_GNUPLOT_LOGGER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingOutputChoice->add(XE(YARS_STRING_SELFORG_LOGGER, YARS_STRING_SELFORG_LOGGER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingOutputChoice->add(XE(YARS_STRING_BLENDER_LOGGER, YARS_STRING_BLENDER_LOGGER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingOutputChoice->add(XE(YARS_STRING_CSV_LOGGER, YARS_STRING_CSV_LOGGER_DEFINITION,
        0, YARS_STRING_XSD_UNBOUNDED));
  loggingDefinition->add(loggingOutputChoice);

  DataLoggingSelforg::createXsd(spec);
  DataLoggingConsole::createXsd(spec);
  DataLoggingFile::createXsd(spec);
  DataLoggingCSV::createXsd(spec);
  DataLoggingGnuplot::createXsd(spec);
  DataLoggingBlender::createXsd(spec);

  DataLoggingObject::createXsd(spec);
  DataLoggingSensor::createXsd(spec);
  DataLoggingActuator::createXsd(spec);
  DataLoggingController::createXsd(spec);
}

DataLogging* DataLogging::copy()
{
  DataLogging *copy = new DataLogging(nullptr);
  for (auto* lo : _objects)
    copy->_objects.push_back(lo->copy());
  for (auto* ls : _sensors)
    copy->_sensors.push_back(ls->copy());
  for (auto* la : _actuators)
    copy->_actuators.push_back(la->copy());
  for (auto* lc : _controllers)
    copy->_controllers.push_back(lc->copy());
  for (auto* lc : _consolelogger)
    copy->_consolelogger.push_back(lc->copy());
  for (auto* lf : _filelogger)
    copy->_filelogger.push_back(lf->copy());
  for (auto* lc : _csvlogger)
    copy->_csvlogger.push_back(lc->copy());
  for (auto* lb : _blenderlogger)
    copy->_blenderlogger.push_back(lb->copy());
  for (auto* lg : _gnuplotlogger)
    copy->_gnuplotlogger.push_back(lg->copy());
  for (auto* ls : _selforglogger)
    copy->_selforglogger.push_back(ls->copy());
  return copy;
}


void DataLogging::checkGeoms(DataObjects *geoms)
{
  for (auto* geom : *geoms)
  {
    for (auto* lo : _objects)
    {
      if (geom->name() == lo->target())
      {
        lo->set(geom);
        break;
      }
    }
  }
}

void DataLogging::checkSensors(DataSensors *sensors)
{
  for (auto* sensor : *sensors)
  {
    for (auto* ls : _sensors)
    {
      if (sensor->name() == ls->target())
      {
        ls->set(sensor);
        break;
      }
    }
  }
}

void DataLogging::checkActuators(DataActuators *actuators)
{
  for (auto* actuator : *actuators)
  {
    for (auto* la : _actuators)
    {
      if (actuator->name() == la->target())
      {
        la->set(actuator);
        break;
      }
    }
  }
}

void DataLogging::checkControllers(DataControllers *controllers)
{
  for (auto* controller : *controllers)
  {
    for (auto* lc : _controllers)
    {
      if (controller->name() == lc->target())
      {
        lc->set(controller);
        break;
      }
    }
  }
}

void DataLogging::finish()
{
  bool missing = false;
  stringstream oss;
  for (auto* o : _objects)
  {
    if (o->object() == nullptr)
    {
      missing = true;
      oss << "Object logger \"" << o->target() << "\" refers to non-existing object." << endl;
    }
  }
  for (auto* s : _sensors)
  {
    if (s->sensor() == nullptr)
    {
      missing = true;
      oss << "Sensor logger \"" << s->target() << "\" refers to non-existing sensor." << endl;
    }
  }
  for (auto* a : _actuators)
  {
    if (a->actuator() == nullptr)
    {
      missing = true;
      oss << "Actuator logger \"" << a->target() << "\" refers to non-existing actuator." << endl;
    }
  }
  for (auto* c : _controllers)
  {
    if (c->controller() == nullptr)
    {
      missing = true;
      oss << "Controller logger \"" << c->target() << "\" refers to non-existing controller." << endl;
    }
  }
  if (missing) YarsErrorHandler::push(oss.str());
}

std::vector<DataLoggingObject*>::iterator DataLogging::lo_begin()
{
  return _objects.begin();
}

std::vector<DataLoggingObject*>::iterator DataLogging::lo_end()
{
  return _objects.end();
}

int DataLogging::lo_size()
{
  return _objects.size();
}

std::vector<DataLoggingConsole*>::iterator DataLogging::c_begin()
{
  return _consolelogger.begin();
}

std::vector<DataLoggingConsole*>::iterator DataLogging::c_end()
{
  return _consolelogger.end();
}

int DataLogging::c_size()
{
  return _consolelogger.size();
}

std::vector<DataLoggingSensor*>::iterator DataLogging::ls_begin()
{
  return _sensors.begin();
}

std::vector<DataLoggingSensor*>::iterator DataLogging::ls_end()
{
  return _sensors.end();
}

int DataLogging::ls_size()
{
  return _sensors.size();
}

std::vector<DataLoggingActuator*>::iterator DataLogging::la_begin()
{
  return _actuators.begin();
}

std::vector<DataLoggingActuator*>::iterator DataLogging::la_end()
{
  return _actuators.end();
}

int DataLogging::la_size()
{
  return _actuators.size();
}

std::vector<DataLoggingController*>::iterator DataLogging::lc_begin()
{
  return _controllers.begin();
}

std::vector<DataLoggingController*>::iterator DataLogging::lc_end()
{
  return _controllers.end();
}

int DataLogging::lc_size()
{
  return _controllers.size();
}

std::vector<DataLoggingFile*>::iterator DataLogging::f_begin()
{
  return _filelogger.begin();
}

std::vector<DataLoggingFile*>::iterator DataLogging::f_end()
{
  return _filelogger.end();
}

int DataLogging::f_size()
{
  return _filelogger.size();
}

std::vector<DataLoggingCSV*>::iterator DataLogging::csv_begin()
{
  return _csvlogger.begin();
}

std::vector<DataLoggingCSV*>::iterator DataLogging::csv_end()
{
  return _csvlogger.end();
}

int DataLogging::csv_size()
{
  return _csvlogger.size();
}

std::vector<DataLoggingBlender*>::iterator DataLogging::b_begin()
{
  return _blenderlogger.begin();
}

std::vector<DataLoggingBlender*>::iterator DataLogging::b_end()
{
  return _blenderlogger.end();
}

int DataLogging::b_size()
{
  return _blenderlogger.size();
}

std::vector<DataLoggingGnuplot*>::iterator DataLogging::g_begin()
{
  return _gnuplotlogger.begin();
}

std::vector<DataLoggingGnuplot*>::iterator DataLogging::g_end()
{
  return _gnuplotlogger.end();
}

int DataLogging::g_size()
{
  return _gnuplotlogger.size();
}

std::vector<DataLoggingSelforg*>::iterator DataLogging::so_begin()
{
  return _selforglogger.begin();
}

std::vector<DataLoggingSelforg*>::iterator DataLogging::so_end()
{
  return _selforglogger.end();
}

int DataLogging::so_size()
{
  return _selforglogger.size();
}

