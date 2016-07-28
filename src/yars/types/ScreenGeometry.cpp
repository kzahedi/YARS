#include "ScreenGeometry.h"

ScreenGeometry::ScreenGeometry()
{
  _width = 640;
  _height = 480;
#ifdef USE_THREAD_SAFE
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_dimensionMutex, &pthread_mutexattr_default);
#endif // USE_THREAD_SAFE
}

#ifndef _MSC_VER
ScreenGeometry::ScreenGeometry(ScreenGeometry& sg)
{
  *this = sg;
}
#endif // _MSC_VER

ScreenGeometry::ScreenGeometry(const ScreenGeometry& sg)
{
  *this = sg;
}

unsigned int ScreenGeometry::width()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  unsigned int r = _width;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ScreenGeometry::setWidth(unsigned int width)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _width = width;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

unsigned int ScreenGeometry::height()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  unsigned int r = _height;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ScreenGeometry::setHeight(unsigned int height)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _height = height;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

void ScreenGeometry::setDimensions(unsigned int width, unsigned int height)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _height = height;
  _width = width;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

void ScreenGeometry::getDimensions(unsigned int *width, unsigned int *height)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  *height = _height;
  *width  = _width;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

ScreenGeometry& ScreenGeometry::operator=(const ScreenGeometry &d)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _height = d._height;
  _width  = d._width;
  _x      = d._x;
  _y      = d._y;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  return *this;
};

int ScreenGeometry::x()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  int x = _x;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  return x;
}

int ScreenGeometry::y()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  int y = _y;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  return y;
}

void ScreenGeometry::setX(int x)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _x = x;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

void ScreenGeometry::setY(int y)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _y = y;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

void ScreenGeometry::setXY(int x, int y)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
  _x = x;
  _y = y;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_dimensionMutex);
#endif // USE_THREAD_SAFE
}

