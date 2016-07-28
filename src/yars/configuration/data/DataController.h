#ifndef __DATA_CONTROLLER_H__
#define __DATA_CONTROLLER_H__

# define YARS_STRING_CONTROLLER            (char*)"controller"
# define YARS_STRING_CONTROLLER_DEFINITION (char*)"controller" DIVIDER DEFINITION

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataParameter.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/types/Matrix.h>

#include <map>
#include <vector>
#include <string>

using namespace std;

class DataController : public DataNode, public std::vector<DataParameter*>
{
  public:
    DataController(DataNode *parent);
    virtual ~DataController() { };

    static void createXsd(XsdSpecification *spec);
    void add(DataParseElement *element);

    string module();
    string name();
    void setModule(string module);
    DataController* copy();

    void addLoggable(string name, yReal              *value);
    void addLoggable(string name, int                *value);
    void addLoggable(string name, std::vector<yReal> *vector);
    void addLoggable(string name, ::Matrix           *matrix);

    bool getLoggable(string name, yReal              &real);
    bool getLoggable(string name, int                &real);
    bool getLoggable(string name, std::vector<yReal> &vector);
    bool getLoggable(string name, ::Matrix           &matrix);

    void resetTo(const DataController *other);

    void clearOsd();
    std::vector<string>::const_iterator s_begin();
    std::vector<string>::const_iterator s_end();
    void addLine(string s);

    void addActuator(DataActuator *actuator);
    DataActuator* actuator(string name);

    void lockOSD();
    void unlockOSD();

    int frequency();
    void applySimulatorFrequency(int simFreq);

  protected:
    string                            _module;
    string                            _name;

    map<string, yReal*>               _reals;
    map<string, int*>                 _ints;
    map<string, ::Matrix*>            _matrices;
    map<string, std::vector<yReal>* > _vectors;

    std::vector<string>               _osd;
    std::vector<DataActuator*>        _actuators;
    pthread_mutex_t                   _osdMutex;
    int                               _frequency;
};

#endif // __DATA_CONTROLLER_H__
