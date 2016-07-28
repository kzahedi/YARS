package org.yars.communication;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.BindException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Vector;


/** \brief Implements the YARS socket communication for Java.
 * \todo Unit test
 */
public class LowLevelSocketCommunication
{

  private String       _host         = "localhost";
  private String       _addrString   = null;
  private InetAddress  _inetAddr     = null;
  private InputStream  _in           = null;
  private OutputStream _out          = null;
  private ServerSocket _serverSocket = null;
  private Socket       _clientSocket = null;

  /** \brief The function to open a port to YARS 
   * \todo Throw Exception
   */
  public void connect(String host, int port) // throws YarsSocketException
  {
    try
    {
      _inetAddr = InetAddress.getByName(_host);
    }
    catch (UnknownHostException e)
    {
      System.out.println("CLIENT: host " + _host + " unknown");
    }
    System.out.println("CLIENT: Trying to connect to " + _inetAddr + ":" + port);
    try
    {
      _clientSocket = new Socket(_host, port);
      System.out.println("CLIENT: clientSocket: " + _clientSocket.toString());
      _in  = _clientSocket.getInputStream();
      _out = _clientSocket.getOutputStream();
    }
    catch (IOException e)
    {
      _in = null;
      _out = null;
      System.out.println("CLIENT: couldnt open socket");
      return;
    }
  }

  public void sendString(String s)
  {
    Buffer b = new Buffer(s);
    __send(b);
  }

  public String receiveString() throws CommunicationException
  {
    Buffer b = __receive();
    return b.stringValue();
  }

  public void sendInteger(Integer i)
  {
    Buffer b = new Buffer(i);
    __send(b);
  }

  /**
   * \todo throw exception
   */
  public int receiveInteger() throws CommunicationException
  {
    Buffer b = __receive();
    return b.intValue();
  }

  public void sendDouble(Double d)
  {
    Buffer b = new Buffer(d);
    __send(b);
  }

  public double receiveDouble() throws CommunicationException
  {
    Buffer b = __receive();
    return b.doubleValue();
  }

  public void sendIntVector(Vector<Integer> vi)
  {
    Buffer b = new Buffer(vi, (int)0);
    __send(b);
  }

  public Vector<Integer> receiveIntVector() throws CommunicationException
  {
    Buffer b = __receive();
    return b.intVector();
  }

  public void sendDoubleVector(Vector<Double> vd)
  {
    Buffer b = new Buffer(vd, (double)0.0);
    __send(b);
  }

  public Vector<Double> receiveDoubleVector() throws CommunicationException
  {
    Buffer b = __receive();
    return b.doubleVector();
  }
  
  private void __send(Buffer b)
  {
    try
    {
      _out.write(b.bytes(), 0, b.size() + 1);
    }
    catch(IOException e)
    {
      e.printStackTrace();
    }
  }

  private void __printBytes(byte c)
  {
    for(int i = 0; i < 8; i++)
    {
      if((((byte)(c >> i)) & (byte)(0x01))  == 1)
      {
        System.out.print("1");
      }
      else
      {
        System.out.print("0");
      }
    }
    System.out.println();
  }


  private Buffer __receive()
  {
    int size = -1;
    Buffer b = null;
    try
    {
      byte[] bb = new byte[1];
      _in.read(bb, 0, 1);
      byte label = bb[0];
      b = new Buffer((char)label);
      switch(label)
      {
        case Buffer.INTEGER:
          __read(b, 4);
          break;
        case Buffer.DOUBLE:
          System.out.println("Receiving double");
          __read(b, 8);
          for(int i = 0; i < b.size(); i++)
          {
            __printBytes(b.elementAt(i));
          }
         break;
        case Buffer.STRING:
          size = __readInt();
          __read(b, size);
          break;
        case Buffer.INTEGER_VECTOR:
          size = __readInt();
          __read(b, size * 4); /** \todo get 4 from handshake? */
          break;
        case Buffer.DOUBLE_VECTOR:
          size = __readInt();
          __read(b, size * 8); /** \todo get 8 from handshake? */
          break;
      }
    }
    catch(IOException e)
    {
      e.printStackTrace();
    }
    return b;
  }


  private void __read(Buffer b, int numberOfBytes)
  {
    byte[] bytes = new byte[numberOfBytes];
    try
    {
      _in.read(bytes, 0, numberOfBytes);
    }
    catch(IOException e)
    {
      e.printStackTrace();
    }
    b.add(bytes);
  }

  private int __readInt()
  {
    byte[] b = new byte[4];
    try
    {
      _in.read(b, 0, 4);
    }
    catch (IOException e)
    {
      e.printStackTrace();
    }
    return __bytesToInt(b);
  }

  private int __bytesToInt(byte[] b)
  {
    int i = 0;
    for(int n = 0; n < 4; n++)
    {
      i |= (b[n] << (n * 8));
    }
    return i;
  }



}

