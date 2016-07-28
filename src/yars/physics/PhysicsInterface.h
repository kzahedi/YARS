#ifndef __PHYSICS_INTERFACE_H__
#define __PHYSICS_INTERFACE_H__

class PhysicsInterface
{
  public:
    virtual ~PhysicsInterface() { };
    virtual void init()    = 0;
    virtual void step()    = 0;
    virtual void reset()   = 0;
    virtual void close()   = 0;
    virtual bool isReset() = 0; // true if reset is called
    virtual bool isQuit()  = 0; // true if quit is called
};
#endif // __PHYSICS_INTERFACE_H__



