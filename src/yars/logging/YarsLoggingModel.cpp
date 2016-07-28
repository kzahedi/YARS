#include <yars/logging/YarsLoggingModel.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/util/stl_macros.h>

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

YarsLoggingModel::YarsLoggingModel()
{
  _traces         = NULL;
  _loggingHandler = NULL;
}

YarsLoggingModel::~YarsLoggingModel()
{
  __cleanup();
}


void YarsLoggingModel::reset()
{
  // __cleanup();
  // init();

  if(_traces != NULL)
  {
    for(vector<DataTraceLine*>::iterator l = _traces->l_begin(); l != _traces->l_end(); l++)
    {
      (*l)->reset();
      // (*l)->update();
    }
    for(vector<DataTracePoint*>::iterator p = _traces->p_begin(); p != _traces->p_end(); p++)
    {
      (*p)->reset();
      // (*p)->update();
    }
  }
}

void YarsLoggingModel::init()
{
  DataLogging *loggingData = Data::instance()->current()->logging();
  if(loggingData != NULL)
  {
    _loggingHandler = new LoggingHandler();

    for(std::vector<DataLoggingObject*>::iterator o = loggingData->lo_begin(); o != loggingData->lo_end(); o++)
    {
      LoggingModuleObject *obj = new LoggingModuleObject(*o);
      for(std::vector<string>::iterator v = (*o)->begin(); v != (*o)->end(); v++)
      {
        obj->useVariable(*v);
      }
      _loggingHandler->addModule(obj);
    }

    for(std::vector<DataLoggingSensor*>::iterator s = loggingData->ls_begin(); s != loggingData->ls_end(); s++)
    {
      _loggingHandler->addModule(new LoggingModuleSensor((*s)));
    }

    for(std::vector<DataLoggingActuator*>::iterator a = loggingData->la_begin(); a != loggingData->la_end(); a++)
    {
      _loggingHandler->addModule(new LoggingModuleActuator((*a)));
    }

    for(std::vector<DataLoggingController*>::iterator c = loggingData->lc_begin(); c != loggingData->lc_end(); c++)
    {
      LoggingModuleController *controller = new LoggingModuleController((*c));
      _loggingHandler->addModule(controller);
      for(std::vector<string>::iterator v = (*c)->begin(); v != (*c)->end(); v++)
      {
        controller->useVariable(*v);
      }
    }

    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////

    for(std::vector<DataLoggingConsole*>::iterator c = loggingData->c_begin(); c != loggingData->c_end(); c++)
    {
      _loggingHandler->addLogger(new ConsoleLogger(*c));
    }
    for(std::vector<DataLoggingBlender*>::iterator b = loggingData->b_begin(); b != loggingData->b_end(); b++)
    {
      _loggingHandler->addLogger(new BlenderLogger(*b));
    }
    for(std::vector<DataLoggingFile*>::iterator f = loggingData->f_begin(); f != loggingData->f_end(); f++)
    {
      _loggingHandler->addLogger(new FileLogger(*f));
    }
    for(std::vector<DataLoggingCSV*>::iterator f = loggingData->csv_begin(); f != loggingData->csv_end(); f++)
    {
      _loggingHandler->addLogger(new CSVLogger(*f));
    }
    int index = 0;
    for(std::vector<DataLoggingGnuplot*>::iterator g = loggingData->g_begin(); g != loggingData->g_end(); g++)
    {
      _loggingHandler->addLogger(new GnuplotLogger(*g, index++));
    }
    index = 0;
    for(std::vector<DataLoggingSelforg*>::iterator so = loggingData->so_begin(); so != loggingData->so_end(); so++)
    {
      _loggingHandler->addLogger(new SelforgLogger(*so, index++));
    }

    // for(std::vector<DataLoggingSelforg*>::iterator
    //     s  = loggingData->selforg_begin();
    //     s != loggingData->selforg_end();
    //     s++)
    // {
    //   SelforgLogging *sol = new SelforgLogging();
    //   // only one is not NULL
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

    if(_traces != NULL)
    {
      // for(std::vector<DataTracePoint*>::iterator p = _traces->p_begin(); p != _traces->p_end(); p++) (*p)->update();
      for(std::vector<DataTraceLine*>::iterator  l = _traces->l_begin(); l != _traces->l_end(); l++)
        if ((*l) != NULL) (*l)->update();
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

      if(_traces != NULL && !__YARS_GET_USE_PAUSE)
      {
        for(std::vector<DataTracePoint*>::iterator p = _traces->p_begin(); p != _traces->p_end(); p++) (*p)->update();
        for(std::vector<DataTraceLine*>::iterator  l = _traces->l_begin(); l != _traces->l_end(); l++) (*l)->update();
      }
    }
  }

  // if(__YARS_GET_STEP % __YARS_GET_BEHAVIOUR_FREQUENCY == 0)
  // {
  if(_loggingHandler != NULL) _loggingHandler->update();
  // }
    
}

void YarsLoggingModel::__cleanup()
{
  if(_loggingHandler != NULL) _loggingHandler->close();
  if(_traces         != NULL) delete _traces;
  //if(_loggingHandler != NULL) delete _loggingHandler;
}

