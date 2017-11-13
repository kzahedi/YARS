#include "Colour.h"

#include <sstream>
#include <iostream>

Colour Colour::BLACK  (0.0f,  0.0f,  0.0f,  1.0f);
Colour Colour::BLUE   (0.0f,  0.0f,  1.0f,  1.0f);
Colour Colour::YELLOW (0.97f, 0.98f, 0.66f, 1.0f);
Colour Colour::WHITE  (1.0f,   1.0f,   1.0f,   1.0f);
Colour Colour::RED    (1.0f,   0.0f,   0.0f,   1.0f);

Colour::Colour()
{
  _red   = 0.0;
  _green = 0.0;
  _blue  = 0.0;
  _alpha = 1.0;
#ifdef USE_THREAD_SAFE
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_colorMutex, &pthread_mutexattr_default);
#endif // USE_THREAD_SAFE

}

Colour::Colour(float red, float green, float blue, float alpha)
{
  __init(red, green, blue, alpha);
}

Colour::Colour(double red, double green, double blue, double alpha)
{
  __init(red, green, blue, alpha);
}

void Colour::__init(double red, double green, double blue, double alpha)
{
  __check("red",   red);
  __check("green", green);
  __check("blue",  blue);
  __check("alpha", alpha);
  _red   = red;
  _green = green;
  _blue  = blue;
  _alpha = alpha;
}

void Colour::__check(std::string name, double value)
{
  if(value < 0.0 || value > 1.0)
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "Invalid color value " << name << ": ";
    if(value > 1.0)
    {
      (*e) << "too large: " << value;
    }
    if(value < 0.0)
    {
      (*e) << "is negative: " << value;
    }
    YarsErrorHandler::push();
  }
}

void Colour::setRed(double red)
{
  __setRed(red);
}
void Colour::setRed(float red)
{
  __setRed(red);
}
void Colour::__setRed(double red)
{
  __check("red", red);
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _red = red;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE
}

void Colour::setGreen(double green)
{
  __setGreen(green);
}
void Colour::setGreen(float green)
{
  __setGreen(green);
}
void Colour::__setGreen(double green)
{
  __check("green", green);
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _green = green;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE
}

void Colour::setBlue(double blue)
{
  __setBlue(blue);
}
void Colour::setBlue(float blue)
{
  __setBlue(blue);
}
void Colour::__setBlue(double blue)
{
  __check("blue", blue);
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _blue = blue;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE
}

void Colour::setAlpha(double alpha)
{
  __setAlpha(alpha);
}
void Colour::setAlpha(float alpha)
{
  __setAlpha(alpha);
}
void Colour::__setAlpha(double alpha)
{
  __check("alpha", alpha);
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _alpha = alpha;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE
}

double Colour::red()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  double r = _red;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE

  return r;
}

double Colour::green()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  double r = _green;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE

  return r;
}

double Colour::blue()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  double r = _blue;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE

  return r;
}

double Colour::alpha()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  double r = _alpha;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE

  return r;
}

Colour& Colour::operator= (const Colour &c)
{
  if(this == &c) return *this;

#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _red = c._red;
  _green = c._green;
  _blue = c._blue;
  _alpha = c._alpha;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE

  return *this;
}

void Colour::inverse()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_colorMutex);
#endif // USE_THREAD_SAFE

  _red   = 1 - _red;
  _green = 1 - _green;
  _blue  = 1 - _blue;

#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_colorMutex);
#endif // USE_THREAD_SAFE
}
