#include <yars/util/Mapping.h>
#include <yars/defines/types.h>

Mapping::Mapping()
{
  _in.min  = __Y_REAL_MIN;
  _in.max  = __Y_REAL_MAX;
  _out.min = __Y_REAL_MIN;
  _out.max = __Y_REAL_MAX;
}

void Mapping::setInputDomain(Domain in)
{
  _in = in;
}

void Mapping::setOutputDomain(Domain out)
{
  _out = out;
}

void Mapping::setInputDomain(yReal min, yReal max)
{
  _in.min = min;
  _in.max = max;
}

void Mapping::setOutputDomain(yReal min, yReal max)
{
  _out.min = min;
  _out.max = max;
}

yReal Mapping::map(yReal value)
{
  return _out.remap(_in.map(value));
}

yReal Mapping::invMap(yReal value)
{
  return _in.remap(_out.map(value));
}

