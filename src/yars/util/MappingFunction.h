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
    MappingFunction(double inStartVal, double inEndVal, double outStartVal,
        double outEndVal);

    /* Destructor */
    ~MappingFunction();

    /* map to a specific transfer function type, linear
     * returns a value in the range of outStartVal and outEndVal */
    double map(double value);
    double map(int mapNum, double value);
    void   map(std::vector<double> *values);

    void addMap(double inStartVal, double inEndVal, double outStartVal,
        double outEndVal);

    /* you have to set the range of the input values and the range of the
     * output values
     * start hasn't to be smaller than end
     * e.g. for input range: 255.0 (start) -255.0 (end)
     *      and output range: -1.0 (start) 1.0 (end)
     *      you will get an inverse linear mapping*/
    void setInputOutputRange(double inStartVal, double inEndVal, double outStartVal,
        double outEndVal);
    void setInputOutputRange(int mapNum, double inStartVal, double inEndVal,
        double outStartVal, double outEndVal);

    /* returns the start value of input range */
    double getInStartVal();
    double getInStartVal(int mapNum);

    /* returns the end value of input range */
    double getInEndVal();
    double getInEndVal(int mapNum);

    /* returns the start value of output range */
    double getOutStartVal();
    double getOutStartVal(int mapNum);
    /* returns the end value of output range */
    double getOutEndVal();
    double getOutEndVal(int mapNum);

    /* checks if the value which has to be mapped is within the input range
     * if not, than the value is adjusted to the correct limit respectivly */
    void checkValueRange(double *value);
    void checkValueRange(int mapNum, double *value);

    int getNumberOfMaps();

  private:
    /* calculates a and b */
    void calculateConstants();
    void calculateConstants(int mapNum);

    bool doesMapNumExist(int mapNum);

    struct MapStruct
    {
      double inStartVal;
      double inEndVal;
      double outStartVal;
      double outEndVal;

      double inMin;       // TODO: we should be able to do this more efficiently
      double inMax;       //       i.e. without these 4 extra variables
      double outMin;
      double outMax;

      double a;           // mapping constants
      double b;
    };

    std::vector<struct MapStruct> mapData_;
};

#endif
