#include <yars/logging/YarsLoggingModel.h>


#include <yars/logging/ConsoleLogger.h>
#include <yars/logging/FileLogger.h>
#include <yars/logging/CSVLogger.h>
#include <yars/logging/GnuplotLogger.h>
#include <yars/logging/SelforgLogger.h>
#include <yars/logging/BlenderLogger.h>

#include <yars/logging/LoggingModuleObject.h>
#include <yars/logging/LoggingModuleSensor.h>
#include <yars/logging/LoggingModuleActuator.h>
#include <yars/logging/LoggingModuleController.h>
#include <yars/configuration/data/DataLogging.h>
#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>

YarsLoggingModel::YarsLoggingModel()
{
  _traces         = nullptr;
  _loggingHandler = nullptr;
}

YarsLoggingModel::~YarsLoggingModel()
{
  __cleanup();
}


void YarsLoggingModel::reset()
{
  // __cleanup();
  // init();

  if(_traces != nullptr)
  {
    for(auto l = _traces->l_begin(); l != _traces->l_end(); l++)
    {
      (*l)->reset();
    }
    for(auto p = _traces->p_begin(); p != _traces->p_end(); p++)
    {
      (*p)->reset();
    }
  }
  // TODO: this is a quick hack. needs to be set in RoSiML
  if(_loggingHandler != nullptr)
  {
    _loggingHandler->reset();
  }
}

void YarsLoggingModel::init()
{
  DataLogging *loggingData = Data::instance()->current()->logging();
  if(loggingData != nullptr)
  {
    _loggingHandler = new LoggingHandler();

    for(auto o = loggingData->lo_begin(); o != loggingData->lo_end(); o++)
    {
      LoggingModuleObject *obj = new LoggingModuleObject(*o);
      for(auto v = (*o)->begin(); v != (*o)->end(); v++)
        obj->useVariable(*v);
      _loggingHandler->addModule(obj);
    }

    for(auto s = loggingData->ls_begin(); s != loggingData->ls_end(); s++)
      _loggingHandler->addModule(new LoggingModuleSensor(*s));

    for(auto a = loggingData->la_begin(); a != loggingData->la_end(); a++)
      _loggingHandler->addModule(new LoggingModuleActuator(*a));

    for(auto c = loggingData->lc_begin(); c != loggingData->lc_end(); c++)
    {
      LoggingModuleController *controller = new LoggingModuleController(*c);
      _loggingHandler->addModule(controller);
      for(auto v = (*c)->begin(); v != (*c)->end(); v++)
        controller->useVariable(*v);
    }

    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////

    for(auto c = loggingData->c_begin(); c != loggingData->c_end(); c++)
      _loggingHandler->addLogger(new ConsoleLogger(*c));

    for(auto b = loggingData->b_begin(); b != loggingData->b_end(); b++)
      _loggingHandler->addLogger(new BlenderLogger(*b));

    for(auto f = loggingData->f_begin(); f != loggingData->f_end(); f++)
      _loggingHandler->addLogger(new FileLogger(*f));

    for(auto f = loggingData->csv_begin(); f != loggingData->csv_end(); f++)
      _loggingHandler->addLogger(new CSVLogger(*f));

    int index = 0;
    for(auto g = loggingData->g_begin(); g != loggingData->g_end(); g++)
      _loggingHandler->addLogger(new GnuplotLogger(*g, index++));

    index = 0;
    for(auto so = loggingData->so_begin(); so != loggingData->so_end(); so++)
      _loggingHandler->addLogger(new SelforgLogger(*so, index++));

    // for(std::vector<DataLoggingSelforg*>::iterator
    //     s  = loggingData->selforg_begin();
    //     s != loggingData->selforg_end();
    //     s++)
    // {
    //   SelforgLogging *sol = new SelforgLogging();
    //   // only one is not nullptr
    //   sol->set((*s)->object());
    //   sol->set((*s)->sensor());
    //   sol->set((*s)->actuator());
    //   sol->set((*s)->controller());
    //   for(std::vector<string>::iterator v = (*s)->v_begin(); v != (*s)->v_end(); v++)
    //   {
    //     sol->useVariable(*v);
    //   }
    //   _logger.push_back(sol);
    // }
    _loggingHandler->init();
  }
  if(__YARS_GET_USE_VISUALISATION)
  {
    _traces = Data::instance()->current()->traces();

    if(_traces != nullptr)
    {
      for(auto l = _traces->l_begin(); l != _traces->l_end(); l++)
        if (*l != nullptr) (*l)->update();
    }
  }
}

void YarsLoggingModel::step()
{
  if(!__YARS_GET_USE_PAUSE)
  {
    if(__YARS_GET_USE_VISUALISATION)
    {
      _traces = Data::instance()->current()->traces();

      if(_traces != nullptr && !__YARS_GET_USE_PAUSE)
      {
        for(auto p = _traces->p_begin(); p != _traces->p_end(); p++) (*p)->update();
        for(auto l = _traces->l_begin(); l != _traces->l_end(); l++) (*l)->update();
      }
    }
  }

  // if(__YARS_GET_STEP % __YARS_GET_BEHAVIOUR_FREQUENCY == 0)
  // {
  if(_loggingHandler != nullptr) _loggingHandler->update();
  // }
    
}

void YarsLoggingModel::__cleanup()
{
  if(_loggingHandler != nullptr) _loggingHandler->close();
  if(_traces         != nullptr) delete _traces;
  //if(_loggingHandler != nullptr) delete _loggingHandler;
}

