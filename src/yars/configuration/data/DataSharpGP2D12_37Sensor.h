#ifndef __DATA_SHARP_GP2D12_37_SENSOR_H__
#define __DATA_SHARP_GP2D12_37_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>

#include <yars/types/Domain.h>
#include <yars/types/Pose.h>

#include <yars/util/Mapping.h>

# define SHARP_IR_GP2D12_37 "sharpGP2D12_37"
# define YARS_STRING_SHARP_IR_GP2D12_37  (char*)SHARP_IR_GP2D12_37
# define YARS_STRING_SHARP_IR_GP2D12_37_DEFINITION  (char*)SHARP_IR_GP2D12_37 DIVIDER DEFINITION

#include <string>

using namespace std;

class DataSharpGP2D12_37Sensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataSharpGP2D12_37Sensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataSharpGP2D12_37Sensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the pose with respect to the origin of the attaching object.
     *
     * @return
     */
    Pose pose();

    static void createXsd(XsdSpecification *spec);

    DataSharpGP2D12_37Sensor *_copy();

    int dimension() { return 1; }

    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);
    Domain getInternalDomain(int index) { return Domain(0,0); };
    Domain getExternalDomain(int index) { return Domain(0,0); };

    void _resetTo(const DataSensor *sensor);

  private:
    void     __setMapping();
    Pose     _pose;
    yReal _internalValue;
    yReal _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;
};

#endif // __DATA_SHARP_GP2D12_37_SENSOR_H__
