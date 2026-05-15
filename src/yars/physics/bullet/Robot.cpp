#include <yars/physics/bullet/Robot.h>

#include <yars/physics/bullet/ObjectFactory.h>
#include <yars/physics/bullet/ActuatorFactory.h>
#include <yars/physics/bullet/SensorFactory.h>

#include <yars/view/console/ConsoleView.h>

Robot::Robot(DataRobot *robot)
{
  _data = robot;
  _controllerLib = nullptr;
  _create_controller = nullptr;
  _controllerFrequency = 1;
  _reset = false;
  _quit = false;
  _seed = -1;

  __createBody();
  __createActuators();
  __createSensors();
  if (__YARS_GET_USE_CONTROLLER == true)
  {
    __createController();
  }
}

Robot::~Robot()
{
  // unique_ptr handles _objects, _actuators, _sensors automatically.
  // Controller needs an explicit close() before destruction, then the
  // dynamic library can be unloaded once the controller is destroyed.
  if (_controller)
  {
    _controller->close();
    _controller.reset();
  }
  if (_controllerLib != nullptr)
  {
    dlclose(_controllerLib);
  }
}

void Robot::__createBody()
{
  for (auto i = _data->o_begin(); i != _data->o_end(); ++i)
  {
    _objects.emplace_back(ObjectFactory::create(*i));
  }
}

void Robot::__createActuators()
{
  for (auto i = _data->a_begin(); i != _data->a_end(); ++i)
  {
    Actuator *a = ActuatorFactory::create(*i, this);
    cout << "added " << a->data()->name() << " is active: " << a->data()->isActive(0) << endl;
    _actuators.emplace_back(a);
  }
}

void Robot::__createSensors()
{
  for (auto i = _data->s_begin(); i != _data->s_end(); ++i)
  {
    Sensor *s = SensorFactory::create(*i, this);
    _sensors.emplace_back(s);
  }
}

void Robot::prePhysicsUpdate()
{
  _data->updateActuatorValues();
  for (auto &a : _actuators) a->prePhysicsUpdate();
  for (auto &s : _sensors)   s->prePhysicsUpdate();
}

void Robot::postPhysicsUpdate()
{
  for (auto &o : _objects) o->postPhysicsUpdate();

  for (auto &o : _objects)
    for (auto *oo : *o)
      oo->postPhysicsUpdate();

  for (auto &a : _actuators) a->postPhysicsUpdate();
  for (auto &s : _sensors)   s->postPhysicsUpdate();
  _data->updateSensorValues();
}

void Robot::controllerUpdate()
{
  if (_controller)
  {
    if (__YARS_GET_STEP % _controllerFrequency == 0)
    {
      _controller->setSensorValues(_data->sv_begin(), _data->sv_end());
      _controller->unsetStatus();
      _controller->update();
      _reset = _controller->isReset();
      _quit = _controller->isQuit();
      _seed = _controller->getSeed();
      if (!_reset && !_quit)
      {
        _controller->setMotorValues(_data->av_begin(), _data->av_end());
      }
    }
  }
}

void Robot::__createController()
{
  if (_data->controller() == NULL)
    return;
  string c = _data->controller()->module();
  _controllerLib = dlopen(c.c_str(), RTLD_LAZY);
  if (!_controllerLib)
  {
    Y_FATAL("failed to load \"%s\" with error message \"%s\"\n", c.c_str(), dlerror());
    exit(-1);
    return;
  }

  dlerror();

  _create_controller = (create_c *)dlsym(_controllerLib, "create");
  dlerror(); // TODO: error handling
  if (!_controllerLib)
  {
    Y_DEBUG("Robot::Cannot load symbol create");
    return;
  }

  _controller.reset(_create_controller());

  __setupController();

  _controller->init();
}

std::vector<std::unique_ptr<Object>>::iterator Robot::o_begin()
{
  return _objects.begin();
}

std::vector<std::unique_ptr<Object>>::iterator Robot::o_end()
{
  return _objects.end();
}

std::vector<std::unique_ptr<Actuator>>::iterator Robot::a_begin()
{
  return _actuators.begin();
}

std::vector<std::unique_ptr<Actuator>>::iterator Robot::a_end()
{
  return _actuators.end();
}

std::vector<std::unique_ptr<Sensor>>::iterator Robot::s_begin()
{
  return _sensors.begin();
}

std::vector<std::unique_ptr<Sensor>>::iterator Robot::s_end()
{
  return _sensors.end();
}

bool Robot::isReset()
{
  return _reset;
}

bool Robot::isQuit()
{
  return _quit;
}

int Robot::seed()
{
  return _seed;
}

void Robot::reset()
{
  for (auto &o : _objects)   o->reset();
  for (auto &a : _actuators) a->reset();
  for (auto &s : _sensors)   s->reset();
  _seed = -1;

  if (_controller)
  {
    _controller->reset();
    __setupController();
  }
}

bool Robot::selfCollide()
{
  return _data->selfCollide();
}

DataRobot *Robot::data()
{
  return _data;
}

void Robot::__setupController()
{
  std::vector<NameDimensionDomain> sensors;
  std::vector<NameDimensionDomain> motors;

  for (auto a = _data->a_begin(); a != _data->a_end(); ++a)
  {
    int n = 0;
    for (uint j = 0; j < (*a)->dimension(); j++)
      if ((*a)->isActive(j))
        n++;
    NameDimensionDomain nd = NameDimensionDomain((*a)->name(), n);
    nd.names.resize((*a)->dimension());
    for (uint j = 0; j < (*a)->dimension(); j++)
    {
      if ((*a)->isActive(j))
      {
        nd.internal.push_back((*a)->getInternalDomain(j));
        nd.external.push_back((*a)->getExternalDomain(j));
      }
    }

    if (nd.internal.size() > 0)
      motors.push_back(nd);
  }

  for (auto s = _data->s_begin(); s != _data->s_end(); ++s)
  {
    NameDimensionDomain nd = NameDimensionDomain((*s)->name(), (*s)->dimension());
    nd.names.resize((*s)->dimension());
    for (uint j = 0; j < (*s)->dimension(); j++)
    {
      nd.internal.push_back((*s)->getInternalDomain(j));
      nd.external.push_back((*s)->getExternalDomain(j));
    }
    if ((*s)->type() == DATA_MUSCLE_SENSOR)
    {
      nd.names[0] = "Force";
      nd.names[1] = "Fv";
      nd.names[2] = "Fl";
      nd.names[3] = "Velocity";
      nd.names[4] = "Length";
    }
    sensors.push_back(nd);
  }

  _controller->setSensors(sensors);
  _controller->setMotors(motors);
  _controller->setData(_data->controller());

  _controller->setNumberOfMotorsValues(_data->av_size());
  _controller->setNumberOfSensorValues(_data->sv_size());
  _controllerFrequency = _data->controller()->frequency();
  _controller->setFrequency(_controllerFrequency);

  RobotControllerParameter rcp;
  for (auto *i : *_data->controller())
  {
    rcp.add(i->name(), i->value());
  }
  _controller->setParameter(rcp);
  _controller->setArguments(__YARS_GET_ARGC, __YARS_GET_ARGV);
}
