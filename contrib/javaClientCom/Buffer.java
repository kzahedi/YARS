package org.yars.communication;

import java.util.Vector;

class Buffer extends Vector<Byte>
{
  public final static char STRING         = 's';
  public final static char INTEGER        = 'i';
  public final static char DOUBLE         = 'd';
  public final static char INTEGER_VECTOR = 'I';
  public final static char DOUBLE_VECTOR  = 'D';
  private char _label                     =  0;

  public Buffer(char label)
  {
    _label = label;
  }

  public Buffer(int i)
  {
    __setLabel(INTEGER);
    __intToBuffer(i);
  }

  public Buffer(double d)
  {
    __setLabel(DOUBLE);
    __doubleToBuffer(d);
  }

  public Buffer(Vector<Integer> vi, int i)
  {
    __setLabel(INTEGER_VECTOR);
    __intVectorToBuffer(vi);
  }

  public Buffer(Vector<Double> vd, double d)
  {
    __setLabel(DOUBLE_VECTOR);
    __doubleVectorToBuffer(vd);
  }

  public Buffer(String s)
  {
    __setLabel(STRING);
    __stringToBuffer(s);
  }

  public char label()
  {
    return _label;
  }

  public byte[] bytes()
  {
    byte[] bytes = new byte[1 + size()];
    bytes[0] = (byte)_label;
    for(int i = 0; i < size(); i++)
    {
      bytes[1 + i] = elementAt(i);
    }
    return bytes;
  }

  public void add(byte[] bytes)
  {
    for(int i = 0; i < bytes.length; i++)
    {
      add(bytes[i]);
    }
  }

  public int intValue() throws CommunicationException
  {
    if(!__isInteger())
    {
      throw new CommunicationException("Integer requested but not received");
    }
    return __bufferToInt(0);
  }

  public double doubleValue() throws CommunicationException
  {
    if(!__isDouble())
    {
      throw new CommunicationException("Double requested but not received");
    }
    return __bufferToDouble(0);
  }

  public String stringValue() throws CommunicationException
  {
    if(!__isString())
    {
      throw new CommunicationException("String requested but not received");
    }
    return __bufferToString();
  }

  public Vector<Integer> intVector() throws CommunicationException
  {
    if(!__isIntegerVector())
    {
      throw new CommunicationException("Vector<Integer> requested but not received");
    }
    return __bufferToIntegerValue();
  }

  public Vector<Double> doubleVector() throws CommunicationException
  {
    if(!__isDoubleVector())
    {
      throw new CommunicationException("Vector<Double> requested but not received");
    }
    return __bufferToDoubleValue();
  }

  private Vector<Double> __bufferToDoubleValue()
  {
    Vector<Double> vd = new Vector<Double>();
    for(int i = 0; i < size() / 8; i++) // 8 = int size
    {
      double value = __bufferToDouble(i * 8);
      vd.add(value);
    }
    return vd;
  }

  private Vector<Integer> __bufferToIntegerValue()
  {
    Vector<Integer> vi = new Vector<Integer>();
    for(int i = 0; i < size() / 4; i++) // 4 = int size
    {
      int value = __bufferToInt(i * 4);
      vi.add(value);
    }
    return vi;
  }

  private String __bufferToString()
  {
    String s = "";
    char[] c = new char[size()];
    for(int i = 0; i < size(); i++)
    {
      c[i] = (char)((byte)elementAt(i));
    }
    return new String(c);
  }

  private int __bufferToInt(int offset)
  {
    int i = 0;
    for(int n = 0 + offset; n < 4 + offset; n++)
    {
      byte b = (byte)elementAt(n);
      i |= (b & 0xff) << (n * 8);
    }
    return i;
  }

  private double __bufferToDouble(int offset)
  {
    long d = 0;
    for(int n = 0 + offset; n < 8 + offset; n++)
    {
      byte b = (byte)elementAt(n);
      d |= ((long)(b & 0xff)) << (n * 8);
    }
    return Double.longBitsToDouble( d );
  }

  private void __intToBuffer(int i)
  {
    for(int n = 0; n < 4; n++)
    {
      add((byte) (i >> (n * 8)));
    }
  }

  private void __doubleToBuffer(double d)
  {
    long l = Double.doubleToLongBits(d);
    for(int n = 0; n < 8; n++)
    {
      add( (byte)(l >> (n * 8)));
    }
  }

  private void __intVectorToBuffer(Vector<Integer> vi)
  {
    int s = vi.size();
    __intToBuffer(s);
    for(int n = 0; n < vi.size(); n++)
    {
      int i = vi.elementAt(n);
      __intToBuffer(i);
    }
  }

  private void __doubleVectorToBuffer(Vector<Double> vd)
  {
    int i = vd.size();
    __intToBuffer(i);
    for(int n = 0; n < vd.size(); n++)
    {
      double d = vd.elementAt(n);
      __doubleToBuffer(d);
    }
  }

  private void __stringToBuffer(String s)
  {
    int i = s.length();
    __intToBuffer(i);
    for(int n = 0; n < s.length(); n++)
    {
      add( (byte)s.charAt(n));
    }
  }

  private void __setLabel(char label)
  {
    _label = label;
  }

  private boolean __isInteger()
  {
    return (_label == INTEGER);
  }

  private boolean __isDouble()
  {
    return (_label == DOUBLE);
  }

  private boolean __isDoubleVector()
  {
    return (_label == DOUBLE_VECTOR);
  }

  private boolean __isIntegerVector()
  {
    return (_label == INTEGER_VECTOR);
  }

  private boolean __isString()
  {
    return (_label == STRING);
  }
}
