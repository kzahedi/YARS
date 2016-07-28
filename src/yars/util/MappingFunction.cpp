#include <yars/util/MappingFunction.h>
#include <yars/util/macros.h>

#include <sstream>

MappingFunction::MappingFunction(yReal inStartVal, yReal inEndVal, yReal
    outStartVal, yReal outEndVal)
{
  addMap(inStartVal, inEndVal, outStartVal, outEndVal);
}

MappingFunction::~MappingFunction()
{
}

yReal MappingFunction::map(yReal value)
{
  // checkValueRange(&value);

  if(value < mapData_[0].inMin)
    value = mapData_[0].inMin;
  else if(value > mapData_[0].inMax)
    value = mapData_[0].inMax;

  value = (mapData_[0].a * value) + mapData_[0].b;

  if(value < mapData_[0].outMin)
    value = mapData_[0].outMin;
  else if(value > mapData_[0].outMax)
    value = mapData_[0].outMax;

  return value;
}

yReal MappingFunction::map(int mapNum, yReal value)
{
  if(doesMapNumExist(mapNum))
  {
    // checkValueRange(mapNum, &value);

    if(value < mapData_[mapNum].inMin)
      value = mapData_[mapNum].inMin;
    else if(value > mapData_[mapNum].inMax)
      value = mapData_[mapNum].inMax;

    value = (mapData_[mapNum].a * value) + mapData_[mapNum].b;

    if(value < mapData_[mapNum].outMin)
      value = mapData_[mapNum].outMin;
    else if(value > mapData_[mapNum].outMax)
      value = mapData_[mapNum].outMax;

    return value;
  }
  else
  {
    std::stringstream oss;
    oss << "MappingFunction::map(int, yReal) mapNum " << mapNum << " does not exist.";
    throw(oss.str().c_str());
  }
}

void MappingFunction::map(std::vector<yReal> *values)
{
  if(mapData_.size() < (*values).size())
  {
    std::stringstream oss;
    oss << "MappingFunction::map(std::vector<yReal>) mapData.size(" << mapData_.size() << ") < values.size(" << (*values).size() << ")";
    throw(oss.str().c_str());
  }
  else
  {
    for(unsigned int i=0; i < (*values).size(); i++)
    {
      //checkValueRange(i, &((*values)[i]));

      if((*values)[i] < mapData_[i].inMin)
        (*values)[i] = mapData_[i].inMin;
      else if((*values)[i] > mapData_[i].inMax)
        (*values)[i] = mapData_[i].inMax;

      (*values)[i] = (mapData_[i].a * (*values)[i]) + mapData_[i].b;

      if((*values)[i] < mapData_[i].outMin)
        (*values)[i] = mapData_[i].outMin;
      else if((*values)[i] > mapData_[i].outMax)
        (*values)[i] = mapData_[i].outMax;
    }
  }
}

void MappingFunction::addMap(yReal inStartVal, yReal inEndVal,
    yReal outStartVal, yReal outEndVal)
{
  int newMapNum = mapData_.size();

  MapStruct map;
  mapData_.push_back(map);

  setInputOutputRange(newMapNum, inStartVal, inEndVal, outStartVal,
      outEndVal);
}

void MappingFunction::setInputOutputRange(yReal inStartVal, yReal inEndVal,
    yReal outStartVal, yReal outEndVal)
{
  setInputOutputRange(0, inStartVal, inEndVal, outStartVal, outEndVal);
}

void MappingFunction::setInputOutputRange(int mapNum, yReal inStartVal,
    yReal inEndVal, yReal outStartVal, yReal outEndVal)
{
  if(doesMapNumExist(mapNum))
  {
    mapData_[mapNum].inStartVal  = inStartVal;
    mapData_[mapNum].inEndVal    = inEndVal;
    mapData_[mapNum].outStartVal = outStartVal;
    mapData_[mapNum].outEndVal   = outEndVal;

    mapData_[mapNum].inMin       = MIN(inStartVal, inEndVal);
    mapData_[mapNum].inMax       = MAX(inStartVal, inEndVal);
    mapData_[mapNum].outMin      = MIN(outStartVal, outEndVal);
    mapData_[mapNum].outMax      = MAX(outStartVal, outEndVal);

    calculateConstants(mapNum);
  }
  else
  {
    // TODO: error handling
  }
}

void MappingFunction::calculateConstants()
{
  for(unsigned int i=0; i < mapData_.size(); i++)
  {
    calculateConstants(i);
  }
}

void MappingFunction::calculateConstants(int mapNum)
{
  if(! (doesMapNumExist(mapNum)))
  {
    // TODO: error message /handling
    return;
  }

  // TODO: error handling if NaN or infinity occurs ...
  mapData_[mapNum].a = (mapData_[mapNum].outStartVal -
                        mapData_[mapNum].outEndVal)
                      /(mapData_[mapNum].inStartVal  -
                        mapData_[mapNum].inEndVal);
  mapData_[mapNum].b = mapData_[mapNum].outStartVal -
                      ((mapData_[mapNum].inStartVal
                        /(mapData_[mapNum].inStartVal -
                          mapData_[mapNum].inEndVal)
                       )
                       * (mapData_[mapNum].outStartVal -
                          mapData_[mapNum].outEndVal)
                      );
}

yReal MappingFunction::getInStartVal()
{
  return mapData_[0].inStartVal;
}

yReal MappingFunction::getInStartVal(int mapNum)
{
  if(! (doesMapNumExist(mapNum)))
  {
    // TODO: error message /handling
    return 0.0;
  }

  return mapData_[mapNum].inStartVal;
}

yReal MappingFunction::getInEndVal()
{
  return mapData_[0].inEndVal;
}

yReal MappingFunction::getInEndVal(int mapNum)
{
  if(! (doesMapNumExist(mapNum)))
  {
    // TODO: error message /handling
    return 0.0;
  }

  return mapData_[mapNum].inEndVal;
}

yReal MappingFunction::getOutStartVal()
{
  return mapData_[0].outStartVal;
}

yReal MappingFunction::getOutStartVal(int mapNum)
{
  if(! (doesMapNumExist(mapNum)))
  {
    // TODO: error message /handling
    return 0.0;
  }

  return mapData_[mapNum].outStartVal;
}

yReal MappingFunction::getOutEndVal()
{
  return mapData_[0].outEndVal;
}

yReal MappingFunction::getOutEndVal(int mapNum)
{
  if(! (doesMapNumExist(mapNum)))
  {
    // TODO: error message /handling
    return 0.0;
  }

  return mapData_[mapNum].outEndVal;
}

// depreceated
void MappingFunction::checkValueRange(yReal *value)
{
  if(mapData_[0].inStartVal < mapData_[0].inEndVal)
  {
    if(*value < mapData_[0].inStartVal)
    {
      *value = mapData_[0].inStartVal;
    }
    else if(*value > mapData_[0].inEndVal)
    {
      *value = mapData_[0].inEndVal;
    }
  }
  else
  {
    if(*value > mapData_[0].inStartVal)
    {
      *value = mapData_[0].inStartVal;
    }
    else if(*value < mapData_[0].inEndVal)
    {
      *value = mapData_[0].inEndVal;
    }
  }
}

// depreceated
void MappingFunction::checkValueRange(int mapNum, yReal *value)
{
  if(mapData_[mapNum].inStartVal < mapData_[mapNum].inEndVal)
  {
    if(*value < mapData_[mapNum].inStartVal)
    {
      *value = mapData_[mapNum].inStartVal;
    }
    else if(*value > mapData_[mapNum].inEndVal)
    {
      *value = mapData_[mapNum].inEndVal;
    }
  }
  else
  {
    if(*value > mapData_[mapNum].inStartVal)
    {
      *value = mapData_[mapNum].inStartVal;
    }
    else if(*value < mapData_[mapNum].inEndVal)
    {
      *value = mapData_[mapNum].inEndVal;
    }
  }
}

bool MappingFunction::doesMapNumExist(int mapNum)
{
  if(mapNum < (int)mapData_.size())
  {
    return true;
  }
  else
  {
    return false;
  }
}

int MappingFunction::getNumberOfMaps()
{
  return mapData_.size();
}
