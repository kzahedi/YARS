#ifndef __DATA_PARAMETER_H__
#define __DATA_PARAMETER_H__

#include <yars/defines/types.h>
#include <yars/configuration/data/DataNode.h>

#include <string>

using namespace std;

# define YARS_STRING_PARAMETER            (char*)"parameter"
# define YARS_STRING_PARAMETER_DEFINITION (char*)"parameter_definition"
# define YARS_STRING_VALUE                (char*)"value"

class DataParameter : public DataNode
{
  public:
    DataParameter(DataNode *parent);
    virtual ~DataParameter() { };

    void add(DataParseElement *element);

    string name();
    double realValue();
    int intValue();
    string value();
    bool boolValue();

    void createXsd();

    DataParameter *copy();

    void resetTo(const DataParameter *other);

  private:
    string _name;
    string _value;
};

#endif // __DATA_PARAMETER_H__
