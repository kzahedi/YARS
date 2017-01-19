#ifndef __DATA_TRACE_H__
#define __DATA_TRACE_H__

#include <yars/configuration/data/DataObject.h>
#include <yars/types/Pose.h>
#include <yars/types/P3D.h>
#include <yars/types/Colour.h>
#include <yars/defines/types.h>

#include <boost/circular_buffer.hpp>

# define DATA_TRACE_PROJECT_XY 123456
# define DATA_TRACE_PROJECT_YZ 123457
# define DATA_TRACE_PROJECT_XZ 123458

/** super class for DataTraceLine and DataTracePoint */
class DataTrace : public boost::circular_buffer<P3D>
{
  public:
    DataTrace();

    void   update();
    void   setTarget(DataObject* t);
    void   useLocalCoordinateSystem(bool useLocal);
    int    step();
    void   reset();
    Colour color();
    bool   fade();
    int    maximum();
    P3D    offset();


  protected:
    Colour      _color;
    int         _step;
    DataObject* _target;
    bool        _useLocal;
    P3D         _offset;
    bool        _fade;
    bool        _local;
    int         _max;
    bool        _project;
    int         _projectionPlane;

  private:

};

#endif // __DATA_TRACE_H__
