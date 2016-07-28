#ifndef _MAPPING_FUNCTION_H_
#define _MAPPING_FUNCTION_H_

#include <vector>

#include <yars/defines/types.h>

using namespace std;

/** @deprecated due to complexity */
class MappingFunction
{
  public:
    /* Constructor
     * you have to set the range of the input values and the range of the
     * output values
     * start hasn't to be smaller than end
     * e.g. for input range: 255.0 (start) -255.0 (end)
     *      and output range: -1.0 (start) 1.0 (end)
     *      you will get an inverse linear mapping*/
    MappingFunction(yReal inStartVal, yReal inEndVal, yReal outStartVal,
        yReal outEndVal);

    /* Destructor */
    ~MappingFunction();

    /* map to a specific transfer function type, linear
     * returns a value in the range of outStartVal and outEndVal */
    yReal map(yReal value);
    yReal map(int mapNum, yReal value);
    void   map(std::vector<yReal> *values);

    void addMap(yReal inStartVal, yReal inEndVal, yReal outStartVal,
        yReal outEndVal);

    /* you have to set the range of the input values and the range of the
     * output values
     * start hasn't to be smaller than end
     * e.g. for input range: 255.0 (start) -255.0 (end)
     *      and output range: -1.0 (start) 1.0 (end)
     *      you will get an inverse linear mapping*/
    void setInputOutputRange(yReal inStartVal, yReal inEndVal, yReal outStartVal,
        yReal outEndVal);
    void setInputOutputRange(int mapNum, yReal inStartVal, yReal inEndVal,
        yReal outStartVal, yReal outEndVal);

    /* returns the start value of input range */
    yReal getInStartVal();
    yReal getInStartVal(int mapNum);

    /* returns the end value of input range */
    yReal getInEndVal();
    yReal getInEndVal(int mapNum);

    /* returns the start value of output range */
    yReal getOutStartVal();
    yReal getOutStartVal(int mapNum);
    /* returns the end value of output range */
    yReal getOutEndVal();
    yReal getOutEndVal(int mapNum);

    /* checks if the value which has to be mapped is within the input range
     * if not, than the value is adjusted to the correct limit respectivly */
    void checkValueRange(yReal *value);
    void checkValueRange(int mapNum, yReal *value);

    int getNumberOfMaps();

  private:
    /* calculates a and b */
    void calculateConstants();
    void calculateConstants(int mapNum);

    bool doesMapNumExist(int mapNum);

    struct MapStruct
    {
      yReal inStartVal;
      yReal inEndVal;
      yReal outStartVal;
      yReal outEndVal;

      yReal inMin;       // TODO: we should be able to do this more efficiently
      yReal inMax;       //       i.e. without these 4 extra variables
      yReal outMin;
      yReal outMax;

      yReal a;           // mapping constants
      yReal b;
    };

    std::vector<struct MapStruct> mapData_;
};

#endif
