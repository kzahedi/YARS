#ifndef __DATA_TRACE_LINE_H__
#define __DATA_TRACE_LINE_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataTrace.h>

#include <yars/types/Colour.h>
#include <yars/types/P3D.h>

#include <string>

using namespace std;

#define YARS_STRING_TRACE                 (char*)"trace"
#define YARS_STRING_TRACE_LINE_DEFINITION (char*)"trace" DIVIDER "line" DIVIDER DEFINITION

class DataTraceLine : public DataTrace, public DataNode
{
  public:
    DataTraceLine(DataNode *parent);

    string target();
    yReal  lineWidth();
    void   add(DataParseElement *element);

    Colour finalColour();
    yReal  finalWidth();
    yReal  length();
    yReal  time();
    string texture();
    string particles();
    string billboard();
    bool   useLine();
    bool   useParticles();
    bool   useBillboard();
    int    nrOfElements();
    bool   useProjection();
    int    projectionPlane();

    static void createXsd(XsdSpecification *spec);

    DataTraceLine* copy();

    P3D position();

  private:
    string _target;
    yReal  _lineWidth;
    yReal  _finalWidth;
    yReal  _length;
    yReal  _time;
    string _texture;
    string _particles;
    string _billboard;
    Colour _finalColour;
    P3D    _position;
    int    _nrOfElements;
    bool   _useLine;
    bool   _useParticles;
    bool   _useBillboard;
};

#endif // __DATA_TRACE_LINE_H__
