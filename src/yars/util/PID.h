#ifndef __PID_H__
#define __PID_H__

#include <yars/defines/types.h>

#include <vector>
#include <ostream>

using namespace std;

class PID
{
  public:
    PID(yReal p = 1, yReal i = 0, yReal d = 0, yReal c = 0, int size = 10);

    PID(const PID &p);
    PID & operator=(const PID &b);

    yReal update(yReal value, bool debug = false);

    void setPID(yReal p, yReal i, yReal d, int size = 3);
    void setP(yReal p);
    void setI(yReal i);
    void setD(yReal d);
    void setSize(int size);
    void setInitialValue(yReal value);

    yReal p();
    yReal i();
    yReal d();

    yReal c(); // current value
    yReal e(); // error value

    int size();

    friend std::ostream& operator<<(std::ostream& str, const PID& p)
    {
      str << p._p << ", " << p._i << ", " << p._d;
      return str;
    };


  private:

    void __resize(int size);
    void __reset(yReal c);

    yReal         _currentValue;
    yReal         _error;
    yReal         _lastError;
    yReal         _errorSum;
    yReal         _derivative;
    yReal         _integral;
    std::vector<yReal> _errors;

    yReal _p;
    yReal _i;
    yReal _d;

    int _currentErrorIndex;

};

#endif // __PID_H__
