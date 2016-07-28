#ifndef __Python_TEST_CONTROLLER_H__
#define __Python_TEST_CONTROLLER_H__

#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>

#include <Python.h>

using namespace std;

class PythonController: public RobotController
{
  public:
    void init();
    void close();
    void reset();
    void update();

  private:
    string _module;
    string _workingDir;

    PyObject *_pName;
    PyObject *_pModule;
    PyObject *_pDict;
    PyObject *_pInit;
    PyObject *_pUpdate;
    PyObject *_pClose;
    PyObject *_pReset;
    PyObject *_pValue;
    PyObject *_pSensors;
    PyObject *_pActuators;
    PyObject *_pArgs;
};

#endif


