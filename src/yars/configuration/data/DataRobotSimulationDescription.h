#ifndef __DATA_ROBOT_SIMULATION_DESCRIPTION_H__
#define __DATA_ROBOT_SIMULATION_DESCRIPTION_H__

#include "yars/configuration/data/DataComment.h"
#include "yars/configuration/data/DataSimulator.h"
#include "yars/configuration/data/DataScreens.h"
#include "yars/configuration/data/DataRobots.h"
#include "yars/configuration/data/DataSignals.h"
#include "yars/configuration/data/DataMacros.h"
#include "yars/configuration/data/DataEnvironment.h"
#include "yars/configuration/data/DataNode.h"
#include "yars/configuration/data/DataParseElement.h"
#include "yars/configuration/data/DataConstraints.h"
#include "yars/configuration/data/DataTraces.h"
#include "yars/configuration/data/DataLogging.h"
#include "yars/configuration/data/DataObjects.h"
#include "yars/configuration/data/DataSensors.h"
#include "yars/configuration/data/DataActuators.h"
#include "yars/configuration/data/DataControllers.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"

#include "yars/types/Version.h"
#include "yars/defines/types.h"

#include <string>

# define ROSIML              "rosiml"
# define YARS_STRING_ROOT    (char*)ROSIML
# define YARS_STRING_ROSIML  (char*)ROSIML
# define ROSIML_VERSION      "version"
# define YARS_STRING_VERSION (char*)ROSIML_VERSION

/**
 * @brief This is the entry point to the data representation.
 */
class DataRobotSimulationDescription : public DataNode
{
  public:

    /**
     * @brief Constructor
     *
     * @param parent
     */
    DataRobotSimulationDescription(DataNode *parent = nullptr);
    //DataRobotSimulationDescription(DataRobotSimulationDescription *other);
    //DataRobotSimulationDescription&    operator=(const Matrix &m);

    /**
     * @brief Destructor
     */
    virtual ~DataRobotSimulationDescription();

    /**
     * \brief Version only has major and minor part.
     *
     * Major = Interface change, and hence, incompatibility.
     * Minor = Additional options, and hence higher minor number are not
     *         incompatible to lower minor numbers
     *
     */
    Version version();
    void setVersion(Version version);

    /**
     * @brief Returns the global comment of the xml-description.
     *
     * @return string = comment
     */
    string comment();

    /**
     * @brief Returns the Simulator Description
     *
     * @return description
     * @sa DataSimulator
     */
    DataSimulator* simulator();

    /**
     * @brief Returns the Screens Configuration
     *
     * @return screens configuration
     * @sa DataScreens
     */
    DataScreens* screens();

    /**
     * @brief Return the Macros Configuration
     *
     * @return macros definition
     * @sa DataMacros
     */
    DataMacros* macros();

    /**
     * @brief Returns the robots configuration
     *
     * @return robots configuration
     * @sa DataRobots
     */
    DataRobots* robots();

    /**
     * @brief Returns the environment configuration.
     *
     * @return environment
     * @sa DataEnvironment
     */
    DataEnvironment* environment();

    /**
     * @brief Returns the signals configuration.
     *
     * @return signals
     * @sa DataSignals
     */
    DataSignals* dataSignals();

    /**
     * @brief Returns the traces configuration.
     *
     * @return traces
     * @sa DataTraces
     */
    DataTraces* traces();

    /**
     * @brief Returns the logging configuration.
     *
     * @return logging
     * @sa DataLogging
     */
    DataLogging* logging();

    /**
     * @brief Add a parsed element to the data tree.
     *
     * This is handed to the <em> current </em> element
     *
     * @param element
     * @sa DataNode
     */
    void add(DataParseElement* element);

    static void createXsd(XsdSpecification *spec = NULL);
    void clear();

    DataRobotSimulationDescription* copy();


    void resetTo(const DataRobotSimulationDescription *rsd);

  private:

    void __getChild(DataParseElement *element);
    void __updateFollowables();
    void __updateGeomsContainer();
    void __updateTraces();
    void __checkUniqness();
    void __checkLoggingDefinitions();
    void __updateSensorsContainer();
    void __updateActuatorsContainer();
    void __updateControllersContainer();
    void __applySimulatorFrequencyToControllers();
    void __convertForces();

    Version          _version;
    std::string      _comment;

    DataSimulator   *_simulator;
    DataScreens     *_screens;
    DataMacros      *_macros;
    DataEnvironment *_environment;
    DataRobots      *_robots;
    DataSignals     *_signals;
    // DataConstraints *_constraints;
    DataTraces      *_traces;
    DataLogging     *_logging;

    DataObjects     *_geoms;
    DataSensors     *_sensors;
    DataActuators   *_actuators;
    DataControllers *_controllers;
};

#endif // __DATA_ROBOT_SIMULATION_DESCRIPTION_H__
