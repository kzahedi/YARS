package org.yars.communication;

import java.util.Vector;
import java.util.StringTokenizer;

public class YarsClientCom
{
  private LowLevelSocketCommunication _socket       = null;
  private Vector <Sensor>             _sensors      = null;
  private Vector <Motor>              _motors       = null;
  private String                      _name         = null;
  private int                         _sizeOfInt    = -1;
  private int                         _sizeOfDouble = -1;
  private boolean                     _ok           = true;
  private boolean                     _reset        = true;
  private boolean                     _end          = true;

  public YarsClientCom()
  { }

  public void connect() throws CommunicationException
  {
    connect("localhost", 4500);
  }

  public void connect(int port) throws CommunicationException
  {
    connect("localhost", port);
  }

  public void connect(String hostname, int port) throws CommunicationException
  {
    _socket = new LowLevelSocketCommunication();
    _socket.connect(hostname, port);
    __configuration();
    __printData();
  }

  public double getSensorValue(int sensorIndex, int valueIndex)
  {
    return _sensors.elementAt(sensorIndex).values.elementAt(valueIndex);
  }

  public void setMotorValue(int motorIndex, int valueIndex, double value)
  {
    _motors.elementAt(motorIndex).values.set(valueIndex, new Double(value));
  }

  public String getSensorName(int sensorIndex)
  {
    return _sensors.elementAt(sensorIndex).name;
  }

  public String getMotorName(int motorIndex)
  {
    return _motors.elementAt(motorIndex).name;
  }

  public Domain getSensorRawDomain(int sensorIndex, int valueIndex)
  {
    return _sensors.elementAt(sensorIndex).rawDomains.elementAt(valueIndex);
  }

  public Domain getSensorMappedDomain(int sensorIndex, int valueIndex)
  {
    return _sensors.elementAt(sensorIndex).mappedDomains.elementAt(valueIndex);
  }

  public void update() throws CommunicationException
  {
    __sendMotorCommands();
    // __checkStatus();
    __receiveSensorValues();
  }

  public boolean reset()
  {
    return _reset;
  }

  public boolean ok()
  {
    return _ok;
  }

  public boolean quit()
  {
    return _end;
  }

  public void sendReset()
  {
    _socket.sendString("RESET");
  }

  public void sendQuit()
  {
    _socket.sendString("QUIT");
  }

  public int numberOfSensors()
  {
    return _sensors.size();
  }

  public int numberOfMotors()
  {
    return _motors.size();
  }

  public String robotName()
  {
    return _name;
  }

  public Sensor sensor(int index)
  {
    return _sensors.elementAt(index);
  }

  public Motor motor(int index)
  {
    return _motors.elementAt(index);
  }


  //**********************************************************************
  //**********************************************************************
  // PRIVATE FUNCTIONS
  //**********************************************************************
  //**********************************************************************

  //**********************************************************************
  // COMMUNICATION FUNCTIONS
  //**********************************************************************

  private void __sendMotorCommands() throws CommunicationException
  {
    _socket.sendString("ACTUATORS");
    Vector<Double> values = new Vector<Double>();
    for(int i = 0; i < _motors.size(); i++)
    {
      Motor motor = _motors.elementAt(i);
      for(int j = 0; j < motor.dimension; j++)
      {
        values.add(motor.values.elementAt(j));
      }
    }
    _socket.sendDoubleVector(values);
  }

  private void __checkStatus() throws CommunicationException
  {
    _ok    = false;
    _reset = false;
    _end   = false;
    _socket.sendString("STATUS");
    String status  = _socket.receiveString();
    if(status.equals("QUIT"))
    {
      _end = true;
    }
    if(status.equals("RESET"))
    {
      _reset = true;
    }
    if(status.equals("OK"))
    {
      _ok = true;
    }
  }

  /**
   * \todo throw exception
   */
  void __receiveSensorValues() throws CommunicationException
  {
    _socket.sendString("SENSORS");
    Vector<Double> values = _socket.receiveDoubleVector();
    int index = 0;
    for(int i = 0; i < _sensors.size(); i++)
    {
      Sensor s = _sensors.elementAt(i);
      for(int j = 0; j < s.values.size(); j++)
      {
        if(index < values.size())
        {
          s.values.set(j, values.elementAt(index));
          index++;
        }
      }
    }
  }

  //**********************************************************************
  // DEBUG FUNCTION
  //**********************************************************************

  private void __printData()
  {
    System.out.println("Data received form YARS:\n");
    System.out.println("  Robot name: " + _name);
    System.out.println("  Number of Sensors: " + _sensors.size());
    System.out.println("  Number of Motors: "  + _motors.size());
    for(int i = 0; i < _sensors.size(); i++)
    {
      Sensor s = _sensors.elementAt(i);
      System.out.println("    Sensor " + i);
      System.out.println("      Name " + s.name);
      System.out.println("      Dimension " + s.dimension);
      for(int j = 0; j < _sensors.elementAt(j).rawDomains.size(); j++)
      {
        System.out.println("        Raw Domain  ["
            + _sensors.elementAt(j).rawDomains.elementAt(j).min
            + ", "
            + _sensors.elementAt(j).rawDomains.elementAt(j).max
            + "]");
      }
      for(int j = 0; j < _sensors.elementAt(j).mappedDomains.size(); j++)
      {
        System.out.println("        Mapped Domain  ["
            + _sensors.elementAt(j).mappedDomains.elementAt(j).min
            + ", "
            + _sensors.elementAt(j).mappedDomains.elementAt(j).max
            + "]");
      }
    }
    for(int i = 0; i < _motors.size(); i++)
    {
      Motor motor = _motors.elementAt(i);
      System.out.println("    Motor " + i);
      System.out.println("      Name " + motor.name);
      System.out.println("      Dimension " + motor.dimension);
      for(int j = 0; j < _motors.elementAt(j).rawDomains.size(); j++)
      {
        System.out.println("        Raw Domain  ["
            + _motors.elementAt(j).rawDomains.elementAt(j).min
            + ", "
            + _motors.elementAt(j).rawDomains.elementAt(j).max
            + "]");
      }
      for(int j = 0; j < _motors.elementAt(j).mappedDomains.size(); j++)
      {
        System.out.println("        Mapped Domain  ["
            + _motors.elementAt(j).mappedDomains.elementAt(j).min
            + ", "
            + _motors.elementAt(j).mappedDomains.elementAt(j).max
            + "]");
      }
    }
  }

  //**********************************************************************
  // PARSE CONFIGURATION FUNCTIONS
  //**********************************************************************

  private void __configuration() throws CommunicationException
  {
    _sensors = new Vector<Sensor>();
    _motors  = new Vector<Motor>();
    String s;
    _socket.sendString("CONFIGURATION");
    s = _socket.receiveString();
    if(!s.equals("BEGIN CONFIGURATION"))
    {
      throw new CommunicationException("No configuration!");
    }
    while(!s.equals("END CONFIGURATION"))
    {
      s = _socket.receiveString();
      if(s.equals("BEGIN DATA TYPES"))
      {
        __readDataTypeSpecs();
      }
      if(s.equals("BEGIN ROBOT DATA"))
      {
        __readRobotInformation();
      }
    }
  }

  private void __readDataTypeSpecs() throws CommunicationException
  {
    String s = "";
    while(!s.equals("END DATA TYPES"))
    {
      s = _socket.receiveString();
      if(s.contains("INTEGER"))
      {
        StringTokenizer st = new StringTokenizer(s, " ");
        st.nextToken();
        _sizeOfInt = Integer.parseInt(st.nextToken());
      }
      if(s.contains("DOUBLE"))
      {
        StringTokenizer st = new StringTokenizer(s, " ");
        st.nextToken();
        _sizeOfDouble = Integer.parseInt(st.nextToken());
      }
    }
  }

  private void __readRobotInformation() throws CommunicationException
  {
    String s = "";
    while(!s.equals("END ROBOT DATA"))
    {
      s = _socket.receiveString();
      if(s.startsWith("NAME "))
      {
        StringTokenizer st = new StringTokenizer(s, " ");
        String t = st.nextToken();
        _name = s.substring(t.length(), s.length());
      }
      if(s.equals("BEGIN SENSOR"))
      {
        __readSensor();
      }
      if(s.equals("BEGIN ACTUATOR"))
      {
        __readMotor();
      }
    }
  }

  private void __readMotor() throws CommunicationException
  {
    String s = "";
    Motor motor = new Motor();
    while(!s.equals("END ACTUATOR"))
    {
      s = _socket.receiveString();
      if(s.startsWith("DIMENSION"))
      {
        __setDimension(s, motor);
      }
      if(s.startsWith("RAW DOMAIN"))
      {
        Domain d = new Domain();
        __addDomain(s, d);
        motor.rawDomains.add(d);
      }
      if(s.startsWith("MAPPED DOMAIN"))
      {
        Domain d = new Domain();
        __addDomain(s, d);
        motor.mappedDomains.add(d);
      }
      if(s.startsWith("NAME"))
      {
        __addEntityName(s, motor);
      }
    }
    _motors.add(motor);
  }

  private void __readSensor() throws CommunicationException
  {
    String s = "";
    Sensor sensor = new Sensor();
    while(!s.equals("END SENSOR"))
    {
      s = _socket.receiveString();
      if(s.startsWith("DIMENSION"))
      {
        __setDimension(s, sensor);
      }
      if(s.startsWith("RAW DOMAIN"))
      {
        Domain d = new Domain();
        __addDomain(s, d);
        sensor.rawDomains.add(d);
      }
      if(s.startsWith("MAPPED DOMAIN"))
      {
        Domain d = new Domain();
        __addDomain(s, d);
        sensor.mappedDomains.add(d);
      }
      if(s.startsWith("NAME"))
      {
        __addEntityName(s, sensor);
      }
    }
    _sensors.add(sensor);
  }

  private void __addEntityName(String s, Entity e)
  {
    StringTokenizer st = new StringTokenizer(s, " ");
    String prefix = st.nextToken();
    e.name = s.substring(prefix.length(), s.length());
  }

  private void __setDimension(String s, Entity e)
  {
    StringTokenizer st = new StringTokenizer(s, " ");
    st.nextToken();
    e.dimension = Integer.parseInt(st.nextToken().trim());
    for(int i = 0; i < e.dimension; i++)
    {
      e.values.add(new Double(0.0));
    }
  }

  private void __addDomain(String s, Domain d)
  {
    StringTokenizer st = new StringTokenizer(s, " ");
    st.nextToken();
    st.nextToken();
    d.min = Double.parseDouble(st.nextToken().trim());
    d.max = Double.parseDouble(st.nextToken().trim());
  }
}
