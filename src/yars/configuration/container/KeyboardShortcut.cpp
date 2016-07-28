#include "KeyboardShortcut.h"

KeyboardShortcut::KeyboardShortcut()
{
  key           = 0; // cannot be pressed on the keyboard
  ctrl          = false;
  alt           = false;
  shift         = false;
  metaChar      = 0; // cannot be pressed on the keyboard
  description   = "";
  keyCode       = "";
  function      = NULL;
  id            = -1;
}

void KeyboardShortcut::set()
{
  if(keyCode[0] != '<')
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "Keycode " << keyCode << " does not start with '<'" << endl;
    YarsErrorHandler::push();
  }

  if(keyCode[keyCode.length()-1] != '>')
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "Keycode " << keyCode << " does not end with '>'" << endl;
    YarsErrorHandler::push();
  }

  for(int i = 0; i < (int)keyCode.length(); i++)
  {
    keyCode[i] = toupper(keyCode[i]);
  }

  string::size_type foundShiftStr   = keyCode.find( "S-", 0 );
  string::size_type foundControlStr = keyCode.find( "C-", 0 );
  string::size_type foundAltStr     = keyCode.find( "A-", 0 );


  if( foundShiftStr != string::npos )
  {
    shift = true;
  }

  if( foundControlStr != string::npos )
  {
    ctrl = true;
  }

  if( foundAltStr != string::npos )
  {
    alt = true;
  }

  key = keyCode[keyCode.length()-2];

}

void KeyboardShortcut::__past(string s, string *t, int length)
{
  *t = s;
  for(int i = 0; i < (int)(length - s.length()); i++)
  {
    *t += " ";
  }
}

void KeyboardShortcut::toString(string *s)
{
  stringstream oss;
  string t;
  __past(keyCode, &t, 7);
  oss << t << " " << description;
  *s = oss.str();
}
