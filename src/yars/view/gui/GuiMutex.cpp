#include "GuiMutex.h"

#include <iostream>

GuiMutex* GuiMutex::_me = NULL;

void GuiMutex::lock()
{
  if(_me == NULL) _me = new GuiMutex();
  _me->_lock();
}

void GuiMutex::unlock()
{
  if(_me == NULL) _me = new GuiMutex();
  _me->_unlock();
}

void GuiMutex::lock(string file, int line)
{
  cout << "LOCK: " << file << ":" << line << endl;
  if(_me == NULL) _me = new GuiMutex();
  _me->_lock();
}

void GuiMutex::unlock(string file, int line)
{
  cout << "UNLOCK: " << file << ":" << line << endl;
  if(_me == NULL) _me = new GuiMutex();
  _me->_unlock();
}

GuiMutex::GuiMutex()
{
  YM_INIT;
  _count = 0;
}

void GuiMutex::_lock()
{
  // _count++;
  // cout << "lock: " << _count << endl;
  YM_LOCK;
}

void GuiMutex::_unlock()
{
  // _count--;
  // cout << "unlock: " << _count << endl;
  YM_UNLOCK;
}

