#ifndef __DATA_RECORDING_H__
#define __DATA_RECORDING_H__

# define YARS_STRING_RECORDING            (char*)"recording"
# define YARS_STRING_RECORDING_DEFINITION (char*)"recording" DIVIDER DEFINITION

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataParameter.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/types/Matrix.h>
#include <yars/types/Domain.h>

#include <map>
#include <vector>
#include <string>

typedef __Domain<unsigned long> RecordingInterval;

using namespace std;

class DataRecording : public DataNode, public std::vector<RecordingInterval>
{
  public:
    DataRecording(DataNode *parent);
    virtual ~DataRecording() { };

    static void createXsd(XsdSpecification *spec);
    void add(DataParseElement *element);
    void resetTo(const DataRecording*);
    DataRecording* copy();

    // returns true if current time step is within a recoding interval
    bool record();
};

#endif // __DATA_RECORDING_H__

