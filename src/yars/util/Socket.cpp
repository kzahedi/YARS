#include <yars/util/Socket.h>
#include <yars/util/macros.h>
#include <yars/util/YarsErrorHandler.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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

Socket::Socket()
{
  _sock           = -1;
  _mysock         = -1;
  YarsErrorHandler::instance();
}

Socket::~Socket()
{
  close();
}

/** \brief Function for the client
 *
 *  Closes sockets, if they are still open.
 **/
void Socket::connect(const std::string host, const int port)
{
  struct hostent *h;
  struct in_addr **addr_list;
  int flag = 1;

  if ((_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    YarsErrorHandler::push("Socket::connect: ECHOCLNT: Error creating listening socket.\n");
  }

  if ((h = gethostbyname(host.c_str())) == NULL)  // get the host info
  {
    YarsErrorHandler::push("Socket::connect: gethostbyname, cannot resolve hostname");
  }

  addr_list = (struct in_addr **)h->h_addr_list;

#ifdef __APPLE__
  if (inet_aton(inet_ntoa(*addr_list[0]), &_peer.sin_addr) <= 0)
  {
    YarsErrorHandler::push("Socket::connect: invalid address given.");
  }
#else
  // to do
#endif // __APPLE__

  memset(&_peer, 0, sizeof(_peer));
  _peer.sin_family      = AF_INET;
  _peer.sin_port        = htons(port);

  if ((_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    YarsErrorHandler::push("Socket::connect: Error creating listening socket.");
  }

  if ( ::connect(_sock, (struct sockaddr *) &_peer, sizeof(_peer) ) < 0 )
  {
    YarsErrorHandler::push("Socket::connect: Error calling connect()");
  }

  if(setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0)
  {
    YarsErrorHandler::push("Socket::connect: Error setting setsockopt.");
  }
}


int Socket::accept(const int port)
{
  int flag = 1;
  int p    = port - 1;
  memset(&_peer, 0, sizeof(_peer));
  _peer.sin_family      = AF_INET;
  _peer.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((_mysock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    YarsErrorHandler::push("Socket::accept ECHOSERV: Error creating listening socket.");
  }

#if defined(_MSC_VER) || defined(_WIN32)
  cout << "Socket not yet implemented in Windows version" << endl;
  exit(-1);
#else
  do { _peer.sin_port = htons(++p); }
  while(::bind(_mysock, (struct sockaddr *) &_peer, sizeof(_peer)) < 0 );
#endif

  cout << "  --> port " << p << " opened" << endl;

  if ( listen(_mysock, 1) < 0 ) { // only one connection on this port
    YarsErrorHandler::push("Socket::accept ECHOSERV: Error calling listen()");
  }

  if ( (_sock = ::accept(_mysock, NULL, NULL) ) < 0 ) { // ::accept != Socket::accept
    YarsErrorHandler::push("Socket::accept ECHOSERV: Error calling accept()");
  }

  if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0)
  {
    YarsErrorHandler::push("Socket::connect: Error setting setsockopt on my socket.");
  }

  if(setsockopt(_mysock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0)
  {
    YarsErrorHandler::push("Socket::connect: Error setting setsockopt on client socket.");
  }
  return p;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// BUFFER
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// BUFFER - SEND
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator<<(const Buffer &b) const
{
  char *buf = new char[b.size() + 1];
  buf[0] = b.label;

  for(unsigned int i = 0; i < b.size(); i++)
  {
    buf[1 + i] = b[i];
  }

  send(_sock, buf, b.size() + 1, 0);

  delete[] buf;

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// BUFFER - RECEIVE
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator>>(Buffer &b) const
{
  b.resize(0);
  char *buf       = new char[__BUFFER_SIZE];
  char *type      = new char[1];
  char *sizeBytes = new char[4];
  int r           = 0;
  int size        = -1;
  int read        = 0;
  int toread      = 0;

  recv(_sock, type, 1, MSG_WAITALL);
  ((Socket*)this)->__check(b.label, type[0]);

  switch(type[0])
  {
    case __STRING_VALUE:
      recv(_sock, sizeBytes, 4, MSG_WAITALL);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((Socket*)this)->__coneverToInt(sizeBytes, &size);
      break;
    case __INTEGER_VECTOR:
      recv(_sock, sizeBytes, 4, MSG_WAITALL);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((Socket*)this)->__coneverToInt(sizeBytes, &size);
      size *= sizeof(int);
      break;
    case __DOUBLE_VECTOR:
      recv(_sock, sizeBytes, 4, MSG_WAITALL);
      for(int i = 0; i < 4; i++) b.push_back(sizeBytes[i]);
      ((Socket*)this)->__coneverToInt(sizeBytes, &size);
      size *= sizeof(double);
      break;
    case __INTEGER_VALUE:
      size = sizeof(int);
      break;
    case __DOUBLE_VALUE:
      size = sizeof(double);
      break;
  }

  read = 0;
  while(read < size)
  {
    toread = MIN(size - read, __BUFFER_SIZE);
    r = recv(_sock, buf, toread, MSG_WAITALL);
    read += r;
    for(int i = 0; i < r; i++)
    {
      b.push_back(buf[i]);
    }
  }

  delete[] buf;
  delete[] type;
  delete[] sizeBytes;

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

const Socket& Socket::operator<<(const double &d) const
{
  Buffer b;
  b.resize(0);
  b.label = __DOUBLE_VALUE;
  ((Socket*)this)->__writeDouble(&b, d);
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// DOUBLE - RECEIVE
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator>>(double& d) const
{
  d = 0;
  int size = sizeof(double);
  Buffer b;
  b.resize(size);
  b.label = __DOUBLE_VALUE;
  *this >> b;
  ((Socket*)this)->__readDouble(&d, b, 0);
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

const Socket& Socket::operator<<(const int &i) const
{
  Buffer b;
  b.label = __INTEGER_VALUE;
  b.resize(0);
  ((Socket*)this)->__writeInteger(&b, i);
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// INTEGER - RECEIVE
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator>>(int& i) const
{
  i = 0;
  int size = sizeof(int);
  Buffer b;
  b.label = __INTEGER_VALUE;
  b.resize(size);
  *this >> b;
  ((Socket*)this)->__readInteger(&i, b, 0);
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

const Socket& Socket::operator<<(const std::string& s) const
{
  Buffer b;
  b.resize(0);
  b.label = __STRING_VALUE;
  ((Socket*)this)->__writeInteger(&b, (int)s.length());
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

const Socket& Socket::operator>>(std::string& s) const
{
  s = "";
  Buffer b;
  b.resize(0);
  b.label = __STRING_VALUE;
  *this >> b;

  stringstream oss;
  int size = 0;
  ((Socket*)this)->__readInteger(&size, b, 0);
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

const Socket& Socket::operator<<(const std::vector<int>& v) const
{
  Buffer b;
  b.label = __INTEGER_VECTOR;
  b.resize(0);
  int s = (int)v.size();
  ((Socket*)this)->__writeInteger(&b, s);

  for(std::vector<int>::const_iterator i = v.begin(); i != v.end(); i++)
  {
    int value = *i;
    ((Socket*)this)->__writeInteger(&b, value);
  }
  *this << b;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
// VECTOR OF INTEGERS - RECEIVE
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator>>(std::vector<int>& v) const
{
  v.clear();
  Buffer b;
  b.label = __INTEGER_VECTOR;
  b.resize(0);
  int vectorSize = 0;

  *this >> b;

  ((Socket*)this)->__readInteger(&vectorSize, b, 0);

  for(int i = 0; i < vectorSize; i++)
  {
    int value = 0;
    ((Socket*)this)->__readInteger(&value, b, (i+1) * sizeof(int));
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
const Socket& Socket::operator<<(const std::vector<double>& v) const
{
  Buffer b;
  b.label = __DOUBLE_VECTOR;
  b.resize(0);
  int s = (int)v.size();

  ((Socket*)this)->__writeInteger(&b, s);

  for(std::vector<double>::const_iterator i = v.begin(); i != v.end(); i++)
  {
    double value = *i;
    ((Socket*)this)->__writeDouble(&b, value);
  }
  *this << b;
  return *this;
}


////////////////////////////////////////////////////////////////////////////////
// VECTOR OF DOUBLES - RECEIVE
////////////////////////////////////////////////////////////////////////////////

const Socket& Socket::operator>>(std::vector<double>& v) const
{
  v.clear();
  Buffer b;
  b.label = __DOUBLE_VECTOR;
  b.resize(0);

  *this >> b;

  int vectorSize = 0;

  ((Socket*)this)->__readInteger(&vectorSize, b, 0);

  for(int i = 0; i < vectorSize; i++)
  {
    double value = 0;
    ((Socket*)this)->__readDouble(&value, b, i * sizeof(double) + sizeof(int));
    v.push_back(value);
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Buffer access functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Socket::__writeDouble(Buffer *b, double d)
{
  char *x_ptr=reinterpret_cast<char*>(&d);
  for(unsigned int count = 0;count < sizeof(double); count++)
  {
    b->push_back(*(x_ptr+count));
  }
}

void Socket::__readDouble(double *d, Buffer b, int startIndex)
{
  *d = 0;
  char *x_ptr=reinterpret_cast<char *>(d);
  for(int count = sizeof(double)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=b[startIndex + count];
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Socket::__writeInteger(Buffer *b, int i)
{
  char *x_ptr=reinterpret_cast<char*>(&i);
  for(unsigned int count = 0; count < sizeof(int); count++)
  {
    b->push_back(*(x_ptr+count));
  }
}

void Socket::__readInteger(int *i, Buffer b, int startIndex)
{
  *i = 0;
  char *x_ptr=reinterpret_cast<char *>(i);
  for(int count = sizeof(int)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=b[startIndex + count];
  }
}

void Socket::__coneverToInt(char *c, int *i)
{
  *i = 0;
  char *x_ptr=reinterpret_cast<char *>(i);
  for(int count = sizeof(int)-1; count >= 0; count--)
  {
    *(x_ptr+count)|=c[count];
  }
}

void Socket::__check(const char a, const char b)
{
  if(a == b) return; // everything ok
  stringstream oss;
  oss << "Socket communication error. Awaited ";
  switch(a)
  {
    case __DOUBLE_VALUE:
      oss << "<double value> '"    << __DOUBLE_VALUE   << "'";
      break;
    case __INTEGER_VALUE:
      oss << "<integer value> '"  << __INTEGER_VALUE  << "'";
      break;
    case __STRING_VALUE:
      oss << "<string value> '"   << __STRING_VALUE   << "'";
      break;
    case __DOUBLE_VECTOR:
      oss << "<double vector> '"   << __DOUBLE_VECTOR  << "'";
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
      oss << "<double value> '"    << __DOUBLE_VALUE   << "'";
      break;
    case __INTEGER_VALUE:
      oss << "<integer value> '"  << __INTEGER_VALUE  << "'";
      break;
    case __STRING_VALUE:
      oss << "<string value> '"   << __STRING_VALUE   << "'";
      break;
    case __DOUBLE_VECTOR:
      oss << "<double vector> '"   << __DOUBLE_VALUE   << "'";
      break;
    case __INTEGER_VECTOR:
      oss << "<integer vector> '" << __INTEGER_VECTOR << "'";
      break;
    default:
      oss << "<unknown \"" << b << "\">";
      break;
  }
  cerr << oss.str() << endl;
  close();
}

void Socket::close()
{
  if(_sock != -1)
  {
#ifdef __APPLE__
     ::shutdown(_sock, SHUT_RDWR);
#else
#if defined(_MSC_VER) || defined(_WIN32)
     closesocket(_sock);
#  else
     ::close(_sock);
#  endif
#endif // __APPLE__
  }

  if(_mysock != -1)
  {
#ifdef __APPLE__
     ::shutdown(_sock, SHUT_RDWR);
#else
#if defined(_MSC_VER) || defined(_WIN32)
     closesocket(_sock);
#  else
     ::close(_sock);
#  endif
#endif // __APPLE__
  }

  _sock   = -1;
  _mysock = -1;
}
