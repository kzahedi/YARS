#ifndef __DATA_OBJECT_H__
#define __DATA_OBJECT_H__

# define DATA_OBJECT_BOX             1001
# define DATA_OBJECT_SPHERE          1002
# define DATA_OBJECT_CYLINDER        1003
# define DATA_OBJECT_CAPPED_CYLINDER 1004
# define DATA_OBJECT_PLY             1005
# define DATA_OBJECT_COMPOSITE       1006
# define DATA_OBJECT_MACRO           1007

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataPhysicsParameter.h>
#include <yars/configuration/data/DataMeshVisualisation.h>
#include <yars/configuration/data/DataSensor.h>
#include <yars/types/Pose.h>
#include <yars/types/Quaternion.h>

#include <pthread.h>

class DataObject : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataObject(DataNode *parent, int type);

    virtual ~DataObject();

    /**
     * @brief Returns the type of the object
     *
     * @return DATA_OBJECT_BOX
     * @return DATA_OBJECT_SPHERE
     * @return DATA_OBJECT_CYLINDER
     * @return DATA_OBJECT_CAPPED_CYLINDER
     * @return DATA_OBJECT_PLY
     * @return DATA_OBJECT_COMPOSITE
     * @return DATA_OBJECT_MACRO
     */
    int type();

    /**
     * @brief Return physics properties of this box.
     *
     * @return physics
     * @sa DataPhysics
     */
    DataPhysicsParameter* physics();

    Pose pose();
    ::Quaternion quaternion();
    void setPosition(P3D p);

    void applyOffset(Pose p);
    bool useApplyOffset();
    void setUseApplyOffset(bool b); 

    bool visualise();

    /**
     * @brief Returns the name of the object.
     *
     * @return name
     */
    string  name();

    DataObject* copy()
    {
      return _copy();
    };

    virtual DataObject* _copy()                    = 0;
    virtual void _resetTo(const DataObject *other) = 0;

    void setPose(Pose pose);


    void setCurrentForce(double x, double y, double z);
    P3D  getCurrentForce();

    void setCurrentVelocity(double x, double y, double z);
    P3D  getCurrentVelocity();

    void setCurrentTorque(double x, double y, double z);
    P3D  getCurrentTorque();

    void setCurrentAngularVelocity(double x, double y, double z);
    P3D  getCurrentAngularVelocity();

    void setPose(::Quaternion q, P3D p);
    void setPhysics(DataPhysicsParameter *physics);
    void setVisualise(bool visualise);
    void setName(string name);

    void addSensor(DataSensor* sensor);
    std::vector<DataSensor*>::iterator s_begin();
    std::vector<DataSensor*>::iterator s_end();
    int                                s_size();

    vector<DataMeshVisualisation*>::iterator m_begin();
    vector<DataMeshVisualisation*>::iterator m_end();
    int                                      m_size();

    void resetTo(const DataObject *other);
    bool isSoft();

  protected:
    DataPhysicsParameter               *_physics;
    bool                                _visualise;
    Pose                                _pose;
    string                              _name;
    std::vector<DataMeshVisualisation*> _meshes;
    bool                                _isSoft;

  private:
    int                      _type;
    std::vector<DataSensor*> _sensors;
    pthread_mutex_t          _mutex;
    P3D                      _force;
    P3D                      _torque;
    P3D                      _velocity;
    P3D                      _angularVelocity;
    bool                     _applyOffset; 
};

#endif // __DATA_OBJECT_H__
