#ifndef __ROBOTCONTROLLER_HPP__
#define __ROBOTCONTROLLER_HPP__


#include <yars/defines/types.h>
#include <yars/rc/RobotControllerParameter.hpp>
#include <yars/types/NameDimensionDomain.hpp>
#include <yars/configuration/data/DataController.h>

#include <vector>
#include <sstream>

using namespace std;

class Status
{
  public:
    Status()
    {
      reset = false;
      quit  = false;
    }
    bool reset;
    bool quit;
};


/**
  Abstract definition of the RobotControllers. Implementations of this class
  might be used for defining "controlled" robots. A controlled robot uses an
  exported network (designed or evolved in an external neural network or
  artificial evolution program or an otherwise defined controller. The advantage
  is that for moving the robot no connection to a client is necessary.
 **/
class RobotController
{

  public:
    RobotController()
    {
      status.reset = false;
      status.quit  = false;
      seed         = -1; // -1 means it will not be set
    }

    virtual ~RobotController() {};

    // Method for defining the calculation of the motor values.
    virtual void update() = 0;
    // initialisation of the sensors and motors vectors and additional local-Variables
    virtual void init()   = 0;

    virtual void close()  = 0;

    virtual void reset()  = 0;

    /**
     * \todo check this function
     */
    bool isReset()
    {
      return status.reset;
    }
    /**
     * \todo check this function
     */
    bool isQuit()
    {
      return status.quit;
    }

    int getSeed()
    {
      return seed;
    }

    /**
     * \todo check this function
     */
    void setReset()
    {
      status.reset = true;
    }

    /**
     * \todo check this function
     */
    void setQuit()
    {
      status.quit = true;
    }

    void unsetStatus()
    {
      status.reset = false;
      status.quit  = false;
    }

    // gives access to the vector motors
    void getMotorValues(std::vector<double> *values)
    {
      *values = motors;
    }

    std::vector<double>::const_iterator m_begin()
    {
      return motors.begin();
    }

    std::vector<double>::const_iterator m_end()
    {
      return motors.end();
    }

    std::vector<double> getMotorValues()
    {
      return motors;
    }

    // change the value of sensors, e.g. in case of updated/changed sensor-values
    void setSensorValues(std::vector<double> values)
    {
      sensors = values;
    }

    void setSensorValues(std::vector<double>::const_iterator begin, std::vector<double>::const_iterator end)
    {
      int index = 0;
      for(std::vector<double>::const_iterator s = begin; s != end; s++) sensors[index++] = (*s);
    }

    void setMotorValues(std::vector<double>::iterator begin, std::vector<double>::iterator end)
    {
      int index = 0;
      for(std::vector<double>::iterator v = begin; v != end; v++) (*v) = motors[index++];
    }

    // define the number of Output values that are necessary for the given robot
    void setNumberOfMotorsValues(int number)
    {
      motors.resize(number);
    }

    void setNumberOfSensorValues(int number)
    {
      sensors.resize(number);
    }

    void setParameter(RobotControllerParameter param)
    {
      parameter = param;
    }

    void setSensors(std::vector<NameDimensionDomain> sensorConf)
    {
      sensorConfiguration = sensorConf;
    }

    void setMotors(std::vector<NameDimensionDomain> motorConf)
    {
      motorConfiguration = motorConf;
    }

    void getSensorConfiguration(string *s)
    {
      int index = 0;
      stringstream oss;
      for(unsigned int i = 0; i < sensorConfiguration.size(); i++)
      {
        if(sensorConfiguration[i].dimension == 0)
        {
          oss << (index++) << " = " << sensorConfiguration[i].name << endl;
        }
        for(int j = 0; j < sensorConfiguration[i].dimension; j++)
        {
          oss << (index++) << " = " << sensorConfiguration[i].name;
          if(sensorConfiguration[i].dimension > 0)
          {
            oss << " " << j;
          }
          oss << endl;
        }
      }
      *s = oss.str();
    }

    void getMotorConfiguration(string *s)
    {
      int index = 0;
      stringstream oss;
      for(unsigned int i = 0; i < motorConfiguration.size(); i++)
      {
        if(motorConfiguration[i].dimension == 0)
        {
          oss << i << " = " << motorConfiguration[i].name << endl;
        }
        for(int j = 0; j < motorConfiguration[i].dimension; j++)
        {
          oss << (index++) << " = " << motorConfiguration[i].name;
          if(motorConfiguration[i].dimension > 0)
          {
            oss << " " << j;
          }
          oss << endl;
        }
      }
      *s = oss.str();
    }

    void getSensorConfiguration(std::vector<string> *v)
    {
      v->clear();
      for(unsigned int i = 0; i < sensorConfiguration.size(); i++)
      {
        if(sensorConfiguration[i].dimension == 0)
        {
          v->push_back(sensorConfiguration[i].name);
        }
        for(int j = 0; j < sensorConfiguration[i].dimension; j++)
        {
          stringstream oss;
          oss << sensorConfiguration[i].name;
          if(sensorConfiguration[i].dimension > 0)
          {
            oss << " " << j;
          }
          v->push_back(oss.str());
        }
      }
    }

    void getMotorConfiguration(std::vector<string> *v)
    {
      v->clear();
      for(unsigned int i = 0; i < motorConfiguration.size(); i++)
      {
        if(motorConfiguration[i].dimension == 0)
        {
          v->push_back(motorConfiguration[i].name);
        }
        for(int j = 0; j < motorConfiguration[i].dimension; j++)
        {
          stringstream oss;
          oss << motorConfiguration[i].name;
          if(motorConfiguration[i].dimension > 0)
          {
            oss << " " << j;
          }
          v->push_back(oss.str());
        }
      }
    }

    void printSensorMotorConfiguration()
    {
      string s;
      getSensorConfiguration(&s);
      cout << "Sensor Configuration:" << endl;
      cout << s << endl;
      getMotorConfiguration(&s);
      cout << "Motor Configuration:" << endl;
      cout << s << endl;
    }

    void setData(DataController *controller)
    {
      _data = controller;
    }

    void addLoggable(string name, double *value)
    {
      _data->addLoggable(name, value);
    }

    void addLoggable(string name, std::vector<double> *vector)
    {
      _data->addLoggable(name, vector);
    }

    void addLoggable(string name, ::Matrix *matrix)
    {
      _data->addLoggable(name, matrix);
    }

    void setArguments(int argc_, const char **argv_)
    {
      argc = argc_;
      argv = argv_;
    }

    void addLine(string line)
    {
      _data->lockOSD();
      _data->addLine(line);
      _data->unlockOSD();
    }
    
    void clearOsd()
    {
      _data->lockOSD();
      _data->clearOsd();
      _data->unlockOSD();
    }

    DataActuator* getActuator(string name)
    {
      return _data->actuator(name);
    }

    void setFrequency(int f)
    {
      _frequency = f;
    }

    int frequency()
    {
      return _frequency;
    }



  protected:

    // field of motor values. e.g. for controlling all motors
    std::vector<double> motors;

    // field of sensor values e.g. using all/parts of the sensors
    std::vector<double> sensors;

    // configurations
    std::vector<NameDimensionDomain> motorConfiguration;
    std::vector<NameDimensionDomain> sensorConfiguration;

    // parameter
    RobotControllerParameter parameter;
    int          argc;
    const char **argv;

    int          seed;

    std::vector<string> _osd;

  private:
    DataController *_data;
    Status           status;
    int             _frequency; // Just for read out. Setting it has no effect
};

// the types of the class factory for the concrete implementations of RobotController's
typedef RobotController* create_c();

#endif // __ROBOTCONTROLLER_HPP__

