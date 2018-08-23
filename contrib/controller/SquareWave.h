#ifndef __SQUARE_WAVE_H__
#define __SQUARE_WAVE_H__

#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class SquareWave: public RobotController
{
  public:
    void init();
    void close();
    void update();
    void reset();

  private:
    void __readValues();

    vector<double> _values;
    int _count;
    int _nr_of_outputs;
    vector<int> _period;
    vector<int> _phaseShift;
    vector<double> _min;
    vector<double> _max;
    bool           _debug;
};

#endif // __SQUARE_WAVE_H__



