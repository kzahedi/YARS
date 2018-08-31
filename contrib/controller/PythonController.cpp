#include "PythonController.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

void PythonController::update()
{
  _pSensors = PyList_New(sensors.size());
  if(_pSensors == NULL) PyErr_Print();

  for(int i = 0; i < (int)sensors.size(); i++)
  {
    _pValue = PyFloat_FromDouble(sensors[i]);
    if(_pValue == NULL) PyErr_Print();
    PyList_SetItem(_pSensors, i, _pValue);
  }
  _pArgs = PyTuple_Pack(1, _pSensors);

  _pActuators = PyObject_CallObject(_pUpdate, _pArgs);
  if(_pActuators == NULL) PyErr_Print();

  for(int i = 0; i < (int)motors.size(); i++)
  {
    _pValue = PyList_GetItem(_pActuators, i);
    if(_pValue == NULL) PyErr_Print();
    motors[i] = PyFloat_AsDouble(_pValue);
  }
  Py_DECREF(_pActuators);
  Py_DECREF(_pSensors);
}

void PythonController::init()
{
  stringstream sst;
  parameter.set("module",            _module,     "");
  parameter.set("working directory", _workingDir, ".");

  Py_Initialize();
#ifndef __APPLE__
  // import_array()
#endif
  PyObject *sys = PyImport_ImportModule("sys");
  if(sys == NULL) PyErr_Print();
  PyObject *path = PyObject_GetAttrString(sys, "path");
  if(path == NULL) PyErr_Print();
  // PyList_Append(path, PyString_FromString(_workingDir.c_str()));
  PyList_Append(path, PyUnicode_FromString(_workingDir.c_str()));

  // Build the name object
  // _pName   = PyString_FromString(_module.c_str());
  // _pName   = PyBytes_FromString(_module.c_str());
  // if(_pName == NULL) PyErr_Print();
  if(_pModule == NULL) PyErr_Print();
  _pModule = PyImport_ImportModule(_module.c_str());
  if(_pModule == NULL) PyErr_Print();
  _pDict   = PyModule_GetDict(_pModule);
  if(_pModule == NULL) PyErr_Print();

  _pInit   = PyDict_GetItemString(_pDict, "init");
  // if (PyCallable_Check(_pInit)) {PyObject_CallObject(_pInit, NULL);}
  // else PyErr_Print();

  _pUpdate = PyDict_GetItemString(_pDict, "update");
  // if (PyCallable_Check(_pUpdate)) {PyObject_CallObject(_pUpdate, NULL);}
  // else PyErr_Print();

  _pClose  = PyDict_GetItemString(_pDict, "close");
  // if (PyCallable_Check(_pClose)) {PyObject_CallObject(_pClose, NULL);}
  // else PyErr_Print();

  _pReset  = PyDict_GetItemString(_pDict, "reset");
  // if (PyCallable_Check(_pReset)) {PyObject_CallObject(_pReset, NULL);}
  // else PyErr_Print();

  if(PyObject_CallObject(_pInit, NULL) == NULL) PyErr_Print();
}

void PythonController::reset()
{
  if(PyObject_CallObject(_pReset, NULL) == NULL) PyErr_Print();
}

void PythonController::close()
{
  if(PyObject_CallObject(_pClose, NULL) == NULL) PyErr_Print();
  Py_DECREF(_pModule);
  Py_DECREF(_pName);
  Py_DECREF(_pSensors);
  Py_Finalize();
}

// the class factories
extern "C" RobotController* create()
{
  PythonController *b = new PythonController();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  //delete controller;
}

//  int main(int argc, char *argv[])
//  {
//      PyObject *pName, *pModule, *pDict, *pFunc, *pValue;
//
//      if (argc < 3)
//      {
//          printf("Usage: exe_name python_source function_name\n");
//          return 1;
//      }
//
//      // Initialize the Python Interpreter
//      Py_Initialize();
//
//      // Build the name object
//      pName = PyString_FromString(argv[1]);
//
//      // Load the module object
//      pModule = PyImport_Import(pName);
//
//      // pDict is a borrowed reference
//      pDict = PyModule_GetDict(pModule);
//
//      // pFunc is also a borrowed reference
//      pFunc = PyDict_GetItemString(pDict, argv[2]);
//
//      if (PyCallable_Check(pFunc))
//      {
//          PyObject_CallObject(pFunc, NULL);
//      } else
//      {
//          PyErr_Print();
//      }
//
//      // Clean up
//      Py_DECREF(pModule);
//      Py_DECREF(pName);
//
//      // Finish the Python Interpreter
//      Py_Finalize();
//
//      return 0;
//  }
//
