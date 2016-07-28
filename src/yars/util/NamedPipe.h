#ifndef __NAMED_PIPE_COMMUNICATION_H__
#define __NAMED_PIPE_COMMUNICATION_H__

#define __YARS_STANDARD_PORT 4500

#include <yars/defines/types.h>

#include <sys/types.h>
#include <string>
#include <vector>


#if defined(_MSC_VER) || defined(_WIN32)
#  include <winsock2.h>
#  include <Ws2tcpip.h>
#  include <stdio.h>
#else // _MSC_VER
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#endif // _MSC_VER

#define NP_WRITE 876543
#define NP_READ  876544


using namespace std;

class Buffer : public std::vector<char>
{
  public:
    char label;
    // d = yReal
    // i = int
    // s = string
    // D = vector of yReal
    // I = vector of int
    //
    // in case of D, I, s the first sizeof(int) bytes give the size of the
    // vector or the length of the string
};


/** \brief TCP/IP NamedPipe communication.
 *
 * This class implements the basic TCP/IP socket communication for clients and
 * for the yars server in C++.
 *
 * This is the low-level communication, i.e. data-types are communicated but not
 * interpreted. These data types are:
 *
 * \li Integer
 * \li Double
 * \li String
 * \li std::vector<int>
 * \li std::vector<yReal>
 *
 * The interface is the same for all the data types above. Example for a server
 * is:
 *
 \verbatim
 NamedPipe *socket = new NamedPipe();
 socket << 1.2;
 delete socket; \endverbatim
 *
 * Communication is coded in little endian, i.e. for integers and floating
 * points number, the low byte is send first.
 **/
class NamedPipe
{
  public:
    /** \brief Standard constructor.
     *
     *  Sets the default values.
     **/
    NamedPipe();

    /** \brief Standard destructor.
     *
     *  Closes sockets, if they are still open.
     **/
    ~NamedPipe();

    /** \brief Client function to open a communication.
     *
     * Open a TCP/IP communication to a server. Most common, the communication
     * with be locally to yars, i.e. on a single machine. Therefore, the
     * standard host is the localhost (127.0.0.1) and the standard port is set
     * to yars' standard port of 4500.
     * \param[in] std::string host
     * \param[in] integer port
     */
    void init(const std::string name, bool create);


    /** \brief Send a string.
     *
     * This functions sends a string of any length over the communication port.
     * Strings are automatically partitioned by the TCP/IP stack, if necessary.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <int size = number of chars to follow>
     <char 1>
     <char 2>
     <char 3>\endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'s'>
     <int low byte 0>
     <int byte 1>
     <int byte 2>
     <int high byte 3>
     <string byte 1>
     <string byte 2>
     ...\endverbatim
     *
     * \param[in] std::string s, the string to be send
     */
    const NamedPipe& operator<<(const std::string&) const;

    /** \brief Receive a string.
     *
     * This function receives a string of any length from the communication
     * port.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <int size = number of chars to follow>
     <char 1>
     <char 2>
     <char 3>
     ...\endverbatim
     *
     * which, in details, is:
     *
     *
     \verbatim
     <'s'>
     <int low byte 0>
     <int byte 1>
     <int byte 2>
     <int high byte 3>
     <string byte 1>
     <string byte 2>
     ... \endverbatim
     * \param[out] std::string s, the received string
     */
    const NamedPipe& operator>>(std::string&) const;

    /** \brief Send a yReal.
     *
     * This functions sends a yReal over the communication port. A yReal is
     * given by 8 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <yReal bytes> \endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'d'>
     <low byte = byte 0>
     <byte 1>
     <byte 2>
     <byte 3>
     <byte 4>
     <byte 5>
     <byte 6>
     <high byte = byte 7>\endverbatim
     *
     * \param[in] yReal d, the yReal to be send
     */
    const NamedPipe& operator<<(const yReal&) const;


    /** \brief Receives a yReal.
     *
     * This functions receives a yReal over the communication port. A yReal is
     * given by 8 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <yReal bytes> \endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'d'>
     <low byte = byte 0>
     <byte 1>
     <byte 2>
     <byte 3>
     <byte 4>
     <byte 5>
     <byte 6>
     <high byte = byte 7>\endverbatim
     *
     * \param[out] yReal d, the yReal to be send
     */
    const NamedPipe& operator>>(yReal&) const;

    /** \brief Send an int.
     *
     * This functions sends an int over the communication port. A int is
     * given by 4 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <int bytes> \endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'i'>
     <low byte = byte 0>
     <byte 1>
     <byte 2>
     <high byte = byte 3>\endverbatim
     *
     * \param[in] int i, the integer to be send
     */
    const NamedPipe& operator<<(const int&) const;

    /** \brief Receives an int.
     *
     * This functions receives an int over the communication port. A int is
     * given by 4 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <int bytes> \endverbatim
     *
     * which, in details, is:
     *
     * \verbatim <'i'> <low byte = byte 0> <byte 1> <byte 2> <high byte = byte 3>\endverbatim
     *
     * \param[out] int i, the integer to be send
     */
    const NamedPipe& operator>>(int&) const;

    /** \brief Send a vector of integers.
     *
     * This functions sends several integers of the communication port, which
     * are given to the function in form of a std::vector<int>. Each int is
     * given by 4 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <vector size bytes>
     <first integer bytes>
     <second integer bytes> ...\endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'I'>
     <size low byte = byte 0>
     <byte 1>
     <byte 2>
     <size high byte = byte 3>
     <first integer low byte = byte 0>
     <first integer byte 1>
     <first integer byte 2>
     <first integer high byte = byte 3>
     <second integer bytes> ... \endverbatim
     *
     * \param[in] std::vector<int> vi, the vector of integers to be send
     */
    const NamedPipe& operator<<(const std::vector<int>&) const;

    /** \brief Receives a vector of integers.
     *
     * This functions receives several integers of the communication port, which
     * are stored by the function in form of a std::vector<int>. Each int is
     * given by 4 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <vector size bytes>
     <first integer bytes>
     <second integer bytes> ...\endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'I'>
     <size low byte = byte 0>
     <byte 1>
     <byte 2>
     <size high byte = byte 3>
     <first integer low byte = byte 0>
     <first integer byte 1>
     <first integer byte 2>
     <first integer high byte = byte 3>
     <second integer bytes> ... \endverbatim
     *
     * \param[out] std::vector<int> vi, the vector of integers to be receives
     */
    const NamedPipe& operator>>(std::vector<int>&) const;

    /** \brief Send a vector of yReals.
     *
     * This functions sends several yReals of the communication port, which
     * are given to the function in form of a std::vector<yReal>. Each yReal is
     * given by 8 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <vector size bytes>
     <first yReal bytes>
     <second yReal bytes> ...\endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'D'>
     <size low byte = byte 0>
     <byte 1>
     <byte 2>
     <size high byte = byte 3>
     <first yReal low byte = byte 0>
     <first yReal byte 1>
     <first yReal byte 2>
     <first yReal byte 3>
     <first yReal byte 4>
     <first yReal byte 5>
     <first yReal byte 6>
     <first yReal high byte = byte 7>
     <second yReals bytes> ... \endverbatim
     *
     * \param[in] std::vector<yReal> vd, the vector of yReals to be send
     */
    const NamedPipe& operator<<(const std::vector<yReal>&) const;

    /** \brief Receives a vector of yReals.
     *
     * This functions receives several yReals of the communication port, which
     * are then stored by the function in form of a std::vector<yReal>. Each yReal is
     * given by 8 bytes.
     *
     * Packing and unpacking of the data is done by the NamedPipe-class, in the
     * following way:
     *
     * \verbatim
     <type char>
     <vector size bytes>
     <first yReal bytes>
     <second yReal bytes> ...\endverbatim
     *
     * which, in details, is:
     *
     * \verbatim
     <'D'>
     <size low byte = byte 0>
     <byte 1>
     <byte 2>
     <size high byte = byte 3>
     <first yReal low byte = byte 0>
     <first yReal byte 1>
     <first yReal byte 2>
     <first yReal byte 3>
     <first yReal byte 4>
     <first yReal byte 5>
     <first yReal byte 6>
     <first yReal high byte = byte 7>
     <second yReals bytes> ... \endverbatim
     *
     * \param[out] std::vector<yReals> vd, the vector of integers to be received
     */
    const NamedPipe& operator>>(std::vector<yReal>&) const;

    void closePipe();

  private:
    const NamedPipe& operator<<(const Buffer&) const;
    const NamedPipe& operator>>(Buffer&) const;

    void __check(const char a, const char b);
    void __readDouble(yReal *d, Buffer b, int startIndex);
    void __readInteger(int *i, Buffer b, int startIndex);
    void __writeDouble(Buffer *b, yReal d);
    void __writeInteger(Buffer *b, int i);
    void __coneverToInt(char *c, int *i);

    int  _fdIn;
    int  _fdOut;
    bool _created;
    string _inName;
    string _outName;
};

#endif // __NAMED_PIPE_COMMUNICATION_H__
