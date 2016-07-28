#ifndef __DATA_GENERIC_FEEDBACK_SENSOR_H__
#define __DATA_GENERIC_FEEDBACK_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>

# define GENERIC_FEEDBACK_SENSOR                "feedback"
# define YARS_STRING_GENERIC_FEEDBACK_SENSOR     (char*)GENERIC_FEEDBACK_SENSOR
# define YARS_STRING_GENERIC_FEEDBACK_SENSOR_DEFINITION  (char*)GENERIC_FEEDBACK_SENSOR DIVIDER DEFINITION


#include <string>

using namespace std;

class DataGenericFeedbackSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericFeedbackSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericFeedbackSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataGenericFeedbackSensor *_copy();

    int dimension() { return 1; }

    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

  private:
    void     __setMapping();
    Domain   _domain;
    yReal _internalValue;
    yReal _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;

};

#endif // __DATA_GENERIC_FEEDBACK_SENSOR_H__
