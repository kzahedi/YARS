#ifndef __DATA_SHARP_DM2Y3A003K0F_SENSOR_H__
#define __DATA_SHARP_DM2Y3A003K0F_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>

#include <yars/types/Domain.h>
#include <yars/types/Pose.h>

#include <yars/util/Mapping.h>

# define YARS_STRING_SHARP_IR_DM2Y3A003K0F            (char*)"sharpDM2Y3A003K0F"
# define YARS_STRING_SHARP_IR_DM2Y3A003K0F_DEFINITION (char*)"sharpDM2Y3A003K0F_definition"

#include <string>

using namespace std;

class DataSharpDM2Y3A003K0FSensor : public DataSensor
{
  public:
    DataSharpDM2Y3A003K0FSensor(DataNode* parent);
    ~DataSharpDM2Y3A003K0FSensor();

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

    DataSharpDM2Y3A003K0FSensor *_copy();

    int dimension() { return 1; }

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    Domain getInternalDomain(int index) { return Domain(0,0); };
    Domain getExternalDomain(int index) { return Domain(0,0); };

    void _resetTo(const DataSensor *sensor);

  private:
    void     __setMapping();
    Pose     _pose;
    double _internalValue;
    double _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;

};

#endif // __DATA_SHARP_DM2Y3A003K0F_SENSOR_H__


