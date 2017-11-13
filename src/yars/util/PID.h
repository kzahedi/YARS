#ifndef __PID_H__
#define __PID_H__

#include <yars/defines/types.h>

#include <vector>
#include <ostream>

using namespace std;

class PID
{
  public:
    PID(double p = 1, double i = 0, double d = 0, double c = 0, int size = 3);

    PID(const PID &p);
    PID & operator=(const PID &b);

    double update(double value, bool debug = false);

    void setPID(double p, double i, double d, int size = 3);
    void setP(double p);
    void setI(double i);
    void setD(double d);
    void setSize(int size);
    void setInitialValue(double value);
    void reset();

    double p();
    double i();
    double d();

    double c(); // current value
    double e(); // error value

    int size();

    friend std::ostream& operator<<(std::ostream& str, const PID& p)
    {
      str << p._p << ", " << p._i << ", " << p._d;
      return str;
    };


  private:

    void __resize(int size);
    void __reset(double c);

    double         _currentValue;
    double         _error;
    double         _lastError;
    double         _errorSum;
    double         _derivative;
    double         _integral;
    std::vector<double> _errors;

    double _p;
    double _i;
    double _d;

    int _currentErrorIndex;

};

#endif // __PID_H__
