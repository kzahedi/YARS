#ifndef __DATA_SIMULATOR_H__
#define __DATA_SIMULATOR_H__

#include "DataNode.h"

# define ITERATIONS                       "iterations"
# define SIMULATOR                        "simulator"
# define YARS_STRING_ITERATIONS           (char*)ITERATIONS
# define YARS_STRING_SIMULATOR            (char*)SIMULATOR
# define YARS_STRING_SIMULATOR_DEFINITION (char*)SIMULATOR DIVIDER DEFINITION

# define CONTROL_TYPE_SIMULATOR 110001
# define CONTROL_TYPE_BEHAVIOUR 110002

// # define BEHAVIOUR                        "behaviour"
// # define YARS_STRING_BEHAVIOUR            (char*)BEHAVIOUR

/**
 * @brief Contains control information of the YARS simulator.
 *
 * These parameters include:
 * <ul>
 *   <li> Update Frequency of the behaviour </li>
 *   <li> Update Frequency of the physics engine </li>
 *   <li> [OPTIONAL] Maximal number of iterations (physics) </li>
 *   <li> [OPTIONAL] Number of iterations (physics) after which the simulator is
 *        automatically reset. </li>
 * <ul>
 *
 */
class DataSimulator : public DataNode
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataSimulator(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataSimulator();

    void add(DataParseElement *element);

    /**
     * @brief The frequency with which the simulator is updated in Hz.
     *
     * A simulator of 100 results in a ODE stepwidth of 0.01
     *
     * @return -1 in case of an error
     * @return >0 otherwise
     *
     * @sa http://www.ode.org
     */
    int simulatorFrequency();

    /**
     * @brief The frequency with which the behaviour of the robots are updated.
     * Must be a divisor of the simulator frequency.
     *
     * @return -1 in case of an error
     * @return >0 otherwise
     *
     * @sa simulatorFrequency()
     */
    int behaviourFrequency();

    /**
     * @brief Determines after how many iterations YARS is quit automatically.
     *
     * It is set to 0, if the parameter was not given in the XML description.
     */
    int iterations();
    void setIterations(int iterations);

    /**
     * @brief Determines after how many iterations YARS is automatically reset.
     *
     * It is set to 0, if the parameter was not given in the XML description.
     */
    int autoReset();

    yReal stepSize();

    int count();

    void setSimulatorFrequency(int frequency);
    void setStepSize(yReal stepSize);
    bool useRandomSeed();
    void setRandomSeed(int seed);
    int  getRandomSeed();
    int  getSolverIterations();
    DataSimulator* copy();

    static void createXsd(XsdSpecification *spec);

  private:
    /**
     * @brief  Process update frequencies, and set values
     *
     * @param element
     * @sa    simulatorFrequency()
     * @sa    DataParseElement
     */
    void __processUpdateFrequencies(DataParseElement *element);

    /**
     * @brief Process control parameters
     *
     * @param element
     * @sa    DataParseElement
     */
    void __processControlParameters(DataParseElement *element);

    int      _simulatorFrequency;
    int      _iterations;
    int      _autoReset;
    int      _controlType;
    int      _randomSeed;
    int      _solverIterations;
    bool     _useRandomSeed;

    yReal _stepSize;

};

#endif // __DATA_SIMULATOR_H__


