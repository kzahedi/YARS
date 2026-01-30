#include "Data.h"

Data* Data::_me = nullptr;

Data* Data::instance()
{
  if(_me == nullptr)
  {
    _me = new Data();
  }
  return _me;
}

DataRobotSimulationDescription* Data::current()
{
  return _current;
}

Data::Data()
{
  _current = nullptr;
}

Data::~Data()
{
  for(std::vector<DataRobotSimulationDescription*>::iterator i = _description.begin(); i != _description.end(); i++)
  {
    delete *i;
  }
  _description.clear();
}

DataRobotSimulationDescription* Data::newSpecification()
{
  DataRobotSimulationDescription *desc = new DataRobotSimulationDescription(nullptr);
  _description.push_back(desc);
  return desc;
}

XsdSpecification* Data::xsd()
{
  XsdSpecification *spec = new XsdSpecification();
  DataRobotSimulationDescription::createXsd(spec);
  return spec;
}

void Data::clear()
{
  for(std::vector<DataRobotSimulationDescription*>::iterator i = _description.begin(); i != _description.end(); i++)
  {
    delete *i;
  }
  _description.clear();
  _current = nullptr;
}

DataRobotSimulationDescription* Data::get(int index)
{
  return _description[index]->copy();
}

DataRobotSimulationDescription* Data::last()
{
  return _description[size()-1];
}

int Data::size()
{
  return (int)_description.size();
}

void Data::initialise(int index)
{
  if(_current != nullptr)
  {
    _current->resetTo(_description[index]); // get the initial values (e.g. in case of reset)
  }
  else
  {
    _current = _description[index]->copy();
  }
}

void Data::close()
{
  if(_me != nullptr) delete _me;
}
