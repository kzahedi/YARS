#include <yars/util/NamedPipe.h>

#include <yars/util/macros.h>
#include <yars/util/YarsErrorHandler.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#if !defined(_MSC_VER) && !defined(_WIN32)
#  include <sys/time.h>
#endif // _MSC_VER

#include <sstream>
#include <iostream>

// Just required to put the bytes together again.
// It is not a real buffer size
#define __BUFFER_SIZE    8192

#define __DOUBLE_VALUE   'd'
#define __INTEGER_VALUE  'i'
#define __STRING_VALUE   's'

#define __DOUBLE_VECTOR  'D'
#define __INTEGER_VECTOR 'I'

# ifndef MSG_WAITALL
#  define MSG_WAITALL 0x0008
# endif


using namespace std;

inline void __printBytes(char c)
{
  for(int i = 0; i < 8; i++)
  {
    if(0x01 & (c >> i))
    {
      std::cout << "1";
    }
    else
    {
      std::cout << "0";
    }
  }
  std::cout << std::endl;
}

NamedPipe::NamedPipe()
{
  _fdIn  = -1;
  _fdOut = -1;
  YarsErrorHandler::instance();
}

NamedPipe::~NamedPipe()
{
  closePipe();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// BUFFER
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// BUFFER - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const Buffer &b) const
{
  char *buf = new char[b.size() + 1];
  buf[0] = b.label;

  for(unsigned int i = 0; i < b.size(); i++)
  {
    buf[1 + i] = b[i];
  }

  write(_fdOut, buf, b.size()+1);

  delete buf;

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(Buffer &b) const
{
  b.resize(0);
  char *buf       = new char [__BUFFER_SIZE];
  char *type      = new char[1];
  char *sizeBytes = new char[4];
  int r           = 0;
  int size        = -1;
  int readBytes   = 0;
  int toread      = 0;

  read(_fdIn, type, 1);
  ((NamedPipe*)this)->__check(b.label, type[0]);

  switch(type[0])
  {
    case __STRING_VALUE:
      read(_fdIn, sizeBytes, 4);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((NamedPipe*)this)->__coneverToInt(sizeBytes, &size);
      break;
    case __INTEGER_VECTOR:
      read(_fdIn, sizeBytes, 4);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((NamedPipe*)this)->__coneverToInt(sizeBytes, &size);
      size *= sizeof(int);
      break;
    case __DOUBLE_VECTOR:
      read(_fdIn, sizeBytes, 4);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((NamedPipe*)this)->__coneverToInt(sizeBytes, &size);
      size *= sizeof(yReal);
      break;
    case __INTEGER_VALUE:
      size = sizeof(int);
      break;
    case __DOUBLE_VALUE:
      size = sizeof(yReal);
      break;
  }

  readBytes = 0;
  while(readBytes < size)
  {
    toread = MIN(size - readBytes, __BUFFER_SIZE);
    r = read(_fdIn, buf, toread);
    readBytes += r;
    for(int i = 0; i < r; i++)
    {
      b.push_back(buf[i]);
    }
  }

  delete buf;
  delete type;
  delete sizeBytes;

  return *this;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DOUBLE
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DOUBLE - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const yReal &d) const
{
  Buffer b;
  b.resize(0);
  b.label = __DOUBLE_VALUE;
  ((NamedPipe*)this)->__writeDouble(&b, d);
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// DOUBLE - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(yReal& d) const
{
  d = 0;
  int size = sizeof(yReal);
  Buffer b;
  b.resize(size);
  b.label = __DOUBLE_VALUE;
  *this >> b;
  ((NamedPipe*)this)->__readDouble(&d, b, 0);
  return *this;
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// INTEGER
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// INTEGER - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const int &i) const
{
  Buffer b;
  b.label = __INTEGER_VALUE;
  b.resize(0);
  ((NamedPipe*)this)->__writeInteger(&b, i);
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// INTEGER - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(int& i) const
{
  i = 0;
  int size = sizeof(int);
  Buffer b;
  b.label = __INTEGER_VALUE;
  b.resize(size);
  *this >> b;
  ((NamedPipe*)this)->__readInteger(&i, b, 0);
  return *this;
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// STRING
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// STRING - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const std::string& s) const
{
  Buffer b;
  b.resize(0);
  b.label = __STRING_VALUE;
  ((NamedPipe*)this)->__writeInteger(&b, (int)s.length());
  for(unsigned int i = 0; i < s.length(); i++)
  {
    b.push_back(s[i]);
  }
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// STRING - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(std::string& s) const
{
  s = "";
  Buffer b;
  b.resize(0);
  b.label = __STRING_VALUE;
  *this >> b;

  stringstream oss;
  int size = 0;
  ((NamedPipe*)this)->__readInteger(&size, b, 0);
  for(unsigned int i = sizeof(int); i < b.size(); i++)
  {
    oss << b[i];
  }
  s = oss.str();
  return *this;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// VECTOR OF INTEGERS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// VECTOR OF INTEGERS - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const std::vector<int>& v) const
{
  Buffer b;
  b.label = __INTEGER_VECTOR;
  b.resize(0);
  int s = (int)v.size();
  ((NamedPipe*)this)->__writeInteger(&b, s);

  for(std::vector<int>::const_iterator i = v.begin(); i != v.end(); i++)
  {
    int value = *i;
    ((NamedPipe*)this)->__writeInteger(&b, value);
  }
  *this << b;
  return *this;
}


////////////////////////////////////////////////////////////////////////////////
// VECTOR OF INTEGERS - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(std::vector<int>& v) const
{
  v.clear();
  Buffer b;
  b.label = __INTEGER_VECTOR;
  b.resize(0);
  int vectorSize = 0;

  *this >> b;

  ((NamedPipe*)this)->__readInteger(&vectorSize, b, 0);

  for(int i = 0; i < vectorSize; i++)
  {
    int value = 0;
    ((NamedPipe*)this)->__readInteger(&value, b, (i+1) * sizeof(int));
    v.push_back(value);
  }
  return *this;
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// VECTOR OF DOUBLES
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// VECTOR OF DOUBLES - SEND
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator<<(const std::vector<yReal>& v) const
{
  Buffer b;
  b.label = __DOUBLE_VECTOR;
  b.resize(0);
  int s = (int)v.size();

  ((NamedPipe*)this)->__writeInteger(&b, s);

  for(std::vector<yReal>::const_iterator i = v.begin(); i != v.end(); i++)
  {
    yReal value = *i;
    ((NamedPipe*)this)->__writeDouble(&b, value);
  }
  *this << b;
  return *this;
}


////////////////////////////////////////////////////////////////////////////////
// VECTOR OF DOUBLES - RECEIVE
////////////////////////////////////////////////////////////////////////////////
const NamedPipe& NamedPipe::operator>>(std::vector<yReal>& v) const
{
  v.clear();
  Buffer b;
  b.label = __DOUBLE_VECTOR;
  b.resize(0);

  *this >> b;

  int vectorSize = 0;

  ((NamedPipe*)this)->__readInteger(&vectorSize, b, 0);

  for(int i = 0; i < vectorSize; i++)
  {
    yReal value = 0;
    ((NamedPipe*)this)->__readDouble(&value, b, i * sizeof(yReal) + sizeof(int));
    v.push_back(value);
  }
  return *this;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Buffer access functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void NamedPipe::__writeDouble(Buffer *b, yReal d)
{
  char *x_ptr=reinterpret_cast<char*>(&d);
  for(unsigned int count = 0;count < sizeof(yReal); count++)
  {
    b->push_back(*(x_ptr+count));
  }
}

void NamedPipe::__readDouble(yReal *d, Buffer b, int startIndex)
{
  *d = 0;
  char *x_ptr=reinterpret_cast<char *>(d);
  for(int count = sizeof(yReal)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=b[startIndex + count];
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void NamedPipe::__writeInteger(Buffer *b, int i)
{
  char *x_ptr=reinterpret_cast<char*>(&i);
  for(unsigned int count = 0; count < sizeof(int); count++)
  {
    b->push_back(*(x_ptr+count));
  }
}

void NamedPipe::__readInteger(int *i, Buffer b, int startIndex)
{
  *i = 0;
  char *x_ptr=reinterpret_cast<char *>(i);
  for(int count = sizeof(int)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=b[startIndex + count];
  }
}

void NamedPipe::__coneverToInt(char *c, int *i)
{
  *i = 0;
  char *x_ptr=reinterpret_cast<char *>(i);
  for(int count = sizeof(int)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=c[count];
  }
}

void NamedPipe::__check(const char a, const char b)
{
  if(a == b) return; // everything ok
  stringstream oss;
  oss << "NamedPipe communication error. Awaited ";
  switch(a)
  {
    case __DOUBLE_VALUE:
      oss << "<yReal value> '" << __DOUBLE_VALUE << "'";
      break;
    case __INTEGER_VALUE:
      oss << "<integer value> '" << __INTEGER_VALUE << "'";
      break;
    case __STRING_VALUE:
      oss << "<string value> '" << __STRING_VALUE << "'";
      break;
    case __DOUBLE_VECTOR:
      oss << "<yReal vector> '" << __DOUBLE_VECTOR << "'";
      break;
    case __INTEGER_VECTOR:
      oss << "<integer vector> '" << __INTEGER_VECTOR << "'";
      break;
    default:
      oss << "<unknown \"" << (int)b << "\">";
      break;
  }
  oss << " but received ";
  switch(b)
  {
    case __DOUBLE_VALUE:
      oss << "<yReal value> '" << __DOUBLE_VALUE << "'";
      break;
    case __INTEGER_VALUE:
      oss << "<integer value> '" << __INTEGER_VALUE << "'";
      break;
    case __STRING_VALUE:
      oss << "<string value> '" << __STRING_VALUE << "'";
      break;
    case __DOUBLE_VECTOR:
      oss << "<yReal vector> '" << __DOUBLE_VALUE << "'";
      break;
    case __INTEGER_VECTOR:
      oss << "<integer vector> '" << __INTEGER_VECTOR << "'";
      break;
    default:
      oss << "<unknown \"" << b << "\">";
      break;
  }
  YarsErrorHandler::push(oss.str());
  closePipe();
}



void NamedPipe::closePipe()
{
  if(_created)
  {
    close(_fdIn);
    close(_fdOut);
  }
  unlink(_inName.c_str());
  unlink(_outName.c_str());
  _fdIn  = -1;
  _fdOut = -1;
}

void NamedPipe::init(string name, bool create)
{
  stringstream oss;
  oss << name.c_str() << "-in";
  _inName = oss.str();
  oss.str("");
  oss << name.c_str() << "-out";
  _outName = oss.str();

  _created = create;
  if(create)
  {
    cout << "creating in pipe" << endl;
    if (mkfifo(_inName.c_str(), 0666)<0)
    {
      unlink(_inName.c_str());
      if (mkfifo(_inName.c_str(), 0666)<0)
      {
        perror("FIFO (named pipe) could not be created.");
        exit(-1);
      }
    }
    else printf("\nIn pipe has been created...");

    cout << "creating out pipe" << endl;
    if (mkfifo(_outName.c_str(), 0666)<0)
    {
      unlink(_outName.c_str());
      if (mkfifo(_outName.c_str(), 0666)<0)
      {
        perror("FIFO (named pipe) could not be created.");
        exit(-1);
      }
    }
    else printf("\nOut pipe has been created...");

    if((_fdIn = open(_inName.c_str(), O_RDWR)) < 0)
    {
      perror("Could not open named pipe.");
      exit(-1);
    }
    if((_fdOut = open(_outName.c_str(), O_RDWR)) < 0)
    {
      perror("Could not open named pipe.");
      exit(-1);
    }
  }
  else
  {
    if((_fdIn = open(_outName.c_str(), O_RDWR)) < 0)
    {
      perror("Could not open named pipe.");
      exit(-1);
    }
    else printf("In pipe created\n");
    if((_fdOut = open(_inName.c_str(), O_RDWR)) < 0)
    {
      perror("Could not open named pipe.");
      exit(-1);
    }
    else printf("Out pipe created\n");
  }
}
