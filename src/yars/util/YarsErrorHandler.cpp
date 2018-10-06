#include "YarsErrorHandler.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <string.h>

YarsErrorHandler *YarsErrorHandler::_me = NULL;

YarsErrorHandler::YarsErrorHandler()
{
}

YarsErrorHandler *YarsErrorHandler::instance()
{
  if (_me == NULL)
  {
    _me = new YarsErrorHandler();
  }
  return _me;
}

string YarsErrorHandler::message()
{
  return str();
}

void YarsErrorHandler::push()
{
  cerr << _me->str().c_str() << endl;
  exit(-1);
}

void YarsErrorHandler::push(string message)
{
  (*_me) << message;
  _me->push();
}

void YarsErrorHandler::push(const char *message, ...)
{
  va_list ap;
  va_start(ap, message);
  int result = -1;
  int length = 256;
  char *buffer = 0;
  while (result == -1)
  {
    if (buffer)
      delete[] buffer;
    buffer = new char[length + 1];
    memset(buffer, 0, length + 1);
    result = vsnprintf(buffer, length, message, ap);
    length *= 2;
  }
  std::string s(buffer);
  delete[] buffer;
  va_end(ap);
  (*_me) << s;
  _me->push();
}
