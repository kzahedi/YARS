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
#define YARS_STRING_TRACE_LINE_DEFINITION (char*)"trace_line_definition"

class DataTraceLine : public DataTrace, public DataNode
{
  public:
    DataTraceLine(DataNode *parent);

    string target();
    double  lineWidth();
    void   add(DataParseElement *element);

    Colour finalColour();
    double  finalWidth();
    double  length();
    double  time();
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
    double  _lineWidth;
    double  _finalWidth;
    double  _length;
    double  _time;
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
