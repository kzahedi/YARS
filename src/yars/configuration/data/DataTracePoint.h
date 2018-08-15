#ifndef __DATA_TRACE_POINT_H__
#define __DATA_TRACE_POINT_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataTrace.h>

#include <yars/types/Colour.h>
#include <yars/types/P3D.h>

#include <string>

using namespace std;

#define YARS_STRING_POINT                  (char*)"point"
#define YARS_STRING_TRACE_POINT_DEFINITION (char*)"trace_point_definition"

class DataTracePoint : public DataTrace, public DataNode
{
  public:
    DataTracePoint(DataNode *parent);

    string   target();
    double pointSize();
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataTracePoint* copy();

  private:
    string   _target;
    double _pointSize;
};

#endif // __DATA_TRACE_POINT_H__
