#ifndef __YARS_EXCEPTION_H__
#define __YARS_EXCEPTION_H__

#include <string>
#include <exception>
#include <iostream>

using namespace std;

class YarsException : public std::exception
{
  public:
    explicit YarsException(const string& what)
      :
        m_what(what)
  {}

    virtual ~YarsException() throw() {}

    virtual const char * what() const throw()
    {
      return m_what.c_str();
    }

    virtual void message() const throw()
    {
      cerr << "YarsException: " << m_what << endl;
    }

  private:
    string m_what;
};

#endif // __YARS_EXCEPTION_H__

