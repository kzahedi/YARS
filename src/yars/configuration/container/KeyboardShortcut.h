#ifndef __KEYBOARD_SHORT_CUT_H__
#define __KEYBOARD_SHORT_CUT_H__

#include <yars/util/YarsErrorHandler.h>

#include <string>
#include <iostream>

#include "boost/function.hpp"

typedef boost::function<void(void)> FuncPtrVoidVoid;

using namespace std;

class KeyboardShortcut
{
  public:
    KeyboardShortcut();
    void set();
    void toString(string *s);

    unsigned char   key;
    bool            ctrl;
    bool            alt;
    bool            shift;
    unsigned char   metaChar;
    string          description;
    string          keyCode;
    FuncPtrVoidVoid function;
    int             id;

  private:
    void __past(string s, string *t, int length);
};

#endif // __KEYBOARD_SHORT_CUT_H__
