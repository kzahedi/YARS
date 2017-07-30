#include "MovingAverage.h"


/**
 * This class constitutes a simple moving average filter wich may be constructed
 * with the following parameters
 *
 * - numberOfPoints
 * - initial value of points not yet filled
 * - recursive calculation on / off
 * - iterations after wich full calculation is redone (0 means never)
 */
MovingAverage::MovingAverage(int numberOfPoints, double initialValue,
    bool recursive, unsigned int itersBeforeFullRecalc)
{
  numPoints_     = numberOfPoints;
  if(numPoints_ < 1)
    numPoints_ = 1;
  recursive_     = recursive;
  maxIters_      = itersBeforeFullRecalc;
  if(maxIters_ == 0)
    maxIters_ = 65535;

  points_.resize(numPoints_, initialValue);

  iters_         = 0;
  pointPos_      = 0;

  accumulator_ = numPoints_ * initialValue;
}

MovingAverage::~MovingAverage()
{
}

double MovingAverage::getFilteredOutput(double unfilteredInput)
{
  if(!recursive_ || iters_ >= maxIters_)
  {
    iters_       = 0;
    accumulator_ = 0;

    points_[pointPos_] = unfilteredInput; // replace oldest value

    for(int i = 0; i < numPoints_; i++)
    {
      accumulator_ += points_[i];
    }
  }
  else // recursive case
  {
    accumulator_      -= points_[pointPos_];
    points_[pointPos_] = unfilteredInput; // replace oldest value
    accumulator_      += points_[pointPos_];

    iters_++;
  }

  // move on
  pointPos_++;
  if(pointPos_ >= numPoints_)
    pointPos_ = 0;

  return (accumulator_ / numPoints_);
}

void MovingAverage::reset(double resetValue)
{
  for(unsigned int i=0; i < points_.size(); i++)
  {
    points_[i] = resetValue;
  }
  accumulator_ = points_.size() * resetValue;
}
