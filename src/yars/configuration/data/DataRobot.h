#ifndef __DATA_ROBOT_H__
#define __DATA_ROBOT_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataActuators.h>
#include <yars/configuration/data/DataSensors.h>
#include <yars/configuration/data/DataObjects.h>
#include <yars/configuration/data/DataObjectFactory.h>
#include <yars/configuration/data/DataSensorFactory.h>
#include <yars/configuration/data/DataActuatorFactory.h>
#include <yars/configuration/data/DataController.h>
#include <yars/configuration/data/DataMacroInstance.h>
#include <yars/configuration/data/DataMacros.h>
#include <yars/configuration/data/DataController.h>
#include <yars/configuration/data/DataConstraints.h>
#include <yars/configuration/data/MacroUserInterface.h>

#include <yars/types/Pose.h>

#include <vector>
#include <string>

# define YARS_STRING_BODY             (char*)"body"
# define YARS_STRING_ROBOT            (char*)"robot"
# define YARS_STRING_ROBOT_DEFINITION (char*)"robot_definition"

using namespace std;

class DataRobot : public DataNode, public MacroUserInterface
{
  public:
    /**
     * @brief Constructor
     *
     * @param parent
     */
    DataRobot(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataRobot();

    /**
     * @brief Returns the name.
     *
     * @return name
     */
    string name();

    /**
     * @brief Returns the pose.
     *
     * @return pose
     * @sa Pose
     */
    Pose pose();

    /**
     * @brief Returns the iterator pointing to first object of the vector of
     * DataObject.
     *
     * @return iterator
     * @sa DataObject
     */
    DataObjects::iterator o_begin();

    /**
     * @brief Returns the iterator pointing to last object of the vector of
     * DataObject.
     *
     * @return iterator
     * @sa DataObject
     */
    DataObjects::iterator o_end();

    /**
     * @brief Returns the number of objects.
     *
     * @return number of objects
     */
    int o_size();

    /**
     * @brief Returns the iterator pointing to first geom of the vector of
     * DataObject.
     *
     * @return iterator
     * @sa DataObject
     */
    DataObjects::iterator g_begin();

    /**
     * @brief Returns the iterator pointing to last geom of the vector of
     * DataObject.
     *
     * @return iterator
     * @sa DataObject
     */
    DataObjects::iterator g_end();

    /**
     * @brief Returns the number of geoms.
     *
     * @return number of geoms
     */
    int g_size();

    /**
     * @brief Returns the iterator pointing to first object of the vector of
     * DataSensor.
     *
     * @return iterator
     * @sa DataObject
     */
    DataSensors::iterator s_begin();

    /**
     * @brief Returns the iterator pointing to last object of the vector of
     * DataSensor.
     *
     * @return iterator
     * @sa DataObject
     */
    DataSensors::iterator s_end();

    /**
     * @brief Returns the number of sensors.
     *
     * @return number of sensors.
     */
    int s_size();

    DataSensor* sensor(int index);

    /**
     * @brief Returns the iterator pointing to first object of the vector of
     * DataActuator.
     *
     * @return iteartor
     * @sa DataObject
     */
    DataActuators::iterator a_begin();

    /**
     * @brief Returns the iterator pointing to last object of the vector of
     * DataActuator.
     *
     * @return iterator
     * @sa DataObject
     */
    std::vector<DataActuator*>::iterator a_end();

    /**
     * @brief Returns the number actuators.
     *
     * @return number of actuators.
     */
    int a_size();

    DataActuator* actuator(int index);

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    void setMacros(DataMacros* macros);

    static void createXsd(XsdSpecification *spec);

    DataController* controller();
    bool selfCollide();

    DataMacros* macros();
    DataRobot* copy();

    std::vector<double>::iterator av_begin();
    std::vector<double>::iterator av_end();
    int                          av_size();

    std::vector<double>::iterator sv_begin();
    std::vector<double>::iterator sv_end();
    int                          sv_size();

    void updateSensorValues();
    void updateActuatorValues();

    void resetTo(const DataRobot *robot);

    DataObject* findObject(string name);

  protected:
    void __assignSensorsToObjects();
    void __gatherGeoms();
    void __collectActuatorObjects();
  private:
    void __applyPose();
    void __applyMacros();
    void __setMotorVector();
    void __setSensorVector();
    void __setActuatorsInController();
    void __getActuatorObjects();

    Pose                            _pose;
    string                          _name;
    DataObjects                     _objects;
    DataObjects                     _geoms;
    std::vector<DataSensor*>        _sensors;
    std::vector<DataActuator*>      _actuators;
    std::vector<DataMacroInstance*> _macros;
    DataController*                 _controller;
    bool                            _selfCollide;
    DataMacros                      *_macrosDefinitions;
    bool                            _processingSensors;
    bool                            _processingActuators;

    std::vector<double>             _actuatorValues;
    std::vector<double>             _sensorValues;
};

#endif // __DATA_ROBOT_H__
