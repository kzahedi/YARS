#ifndef __MACROS_H__
#define __MACROS_H__

#ifdef _MSC_VER
#  define _USE_MATH_DEFINES
#  include <cmath>
#else
#  include <math.h>
#endif

#  define RAD_TO_DEG(x) (( (double)(x) / (M_PI)) * 180.0)
#  define DEG_TO_RAD(x) (( (double)(x) / 180.0) * (M_PI))

#ifndef MIN
#  define MIN(a,b) (((a)>(b))?(b):(a))
#endif // MIN

#ifndef MAX
#  define MAX(a,b) (((a)>(b))?(a):(b))
#endif // MAX

#  define SIGN(a) (((a)>=(0))?(1):(-1))

#  define PNT3F_TO_P3D(a,b)         {a.x  =  b[0]; a.y  =  b[1]; a.z  =  b[2];} // one to one mapping
#  define P3D_TO_PNT3F(a,b)         {a[0] =  b.x;  a[1] =  b.y;  a[2] =  b.z;}  // one to one mapping

#define OGRE_TO_YARS(source, destination) \
  destination.x =  source[0]; \
  destination.y = -source[2]; \
  destination.z =  source[1];

#define YARS_TO_OGRE(source, destination) \
  destination[0] =  source.x; \
  destination[1] =  source.z; \
  destination[2] = -source.y;


#endif // __MACROS_H__
