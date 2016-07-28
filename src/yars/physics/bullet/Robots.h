#ifndef __ROBOTS_H__
#define __ROBOTS_H__

#include "Robot.h"

#include <vector>

using namespace std;

class Robots : public std::vector<Robot*>
{
  public:
    Robots();
    ~Robots();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void controllerUpdate();
    void reset();

    bool isReset();
    bool isQuit();
    int  seed();

  private:
    bool _reset;
    bool _quit;
    int  _seed;

};

#endif // __ROBOTS_H__

