#ifndef _MOVING_AVERAGE_H_
#define _MOVING_AVERAGE_H_

#include <stdlib.h>
#include <math.h>
#include <vector>

#include <yars/defines/types.h>

#ifdef __APPLE__
#  include <limits.h>
#  include <float.h>
#else
#  ifndef _MSC_VER
#    include <values.h>
#  endif
#endif

using namespace std;

/**
 * This class constitutes a simple moving average filter wich may be constructed
 * with the following parameters
 *
 * - numberOfPoints
 * - initial value of points not yet filled
 * - recursive calculation on / off
 * - iterations after wich full calculation is redone (0 means never)
 */
class MovingAverage
{
 public:
   MovingAverage(int numberOfPoints, double initialValue,
       bool recursive, unsigned int itersBeforeFullRecalc);
  ~MovingAverage();

  double getFilteredOutput(double unfilteredInput);

  void   reset(double resetValue);

 private:
  int  numPoints_;
  bool recursive_;        // if this is true we will do a fast update
  int  maxIters_;         // in case we get problems with roundoff errors we max
                          //   increase this value (via the constructor)
  int  iters_;            // iterations passed without full new recalc

  std::vector<double> points_; // vector holding the raw input values
  int pointPos_;          // keep track of where we are

  double accumulator_;    // holds the actual accumulated sum
};

#endif
