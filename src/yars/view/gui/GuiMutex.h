#ifndef __GUI_MUTEX_H__
#define __GUI_MUTEX_H__

#include <yars/defines/mutex.h>

#define LOCK       GuiMutex::lock()
#define UNLOCK     GuiMutex::unlock()

// #define mLOCK   GuiMutex::lock(__FILE__,__LINE__)
// #define mUNLOCK GuiMutex::unlock(__FILE__,__LINE__)

#define mLOCK   LOCK
#define mUNLOCK UNLOCK

#include <string>

using namespace std;

class GuiMutex
{
  public:

    static void lock();
    static void unlock();

    static void lock(string file, int line);
    static void unlock(string file, int line);

  private:
    GuiMutex();

    void _lock();
    void _unlock();

    static GuiMutex* _me;
    pthread_mutex_t  _mutex;
    int              _count;
};

#endif // __GUI_MUTEX_H__
