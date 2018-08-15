#include "DataComposite.h"

#include "DataObjectFactory.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

# define YARS_STRING_POSE                                     (char*)"pose"
# define YARS_STRING_OBJECT                                   (char*)"object"
# define YARS_STRING_NAME                                     (char*)"name"
# define YARS_STRING_GEOMETRY                                 (char*)"geometry"
# define YARS_STRING_BODY                                     (char*)"body"
# define YARS_STRING_VISUALISE                                (char*)"visualise"
# define YARS_STRING_COMPOSITE_BODY_DEFINTION                 (char*)"composite_body_divider_definition"
# define YARS_STRING_TRUE_FALSE_DEFINITION                    (char*)"true_false_divider_definition"
# define YARS_STRING_RADIUS_DIMENSION_DEFINITION              (char*)"radius_dimension_divider_definition"
# define YARS_STRING_COMPOSITE_GEOM_DEFINITION                (char*)"composite_geom_geom_divider_definition"
# define YARS_STRING_BOX                                      (char*)"box"
# define YARS_STRING_COMPOSITE_GEOM_BOX_DEFINTION             (char*)"composite_box_geom_divider_definition"
# define YARS_STRING_COMPOSITE_BODY_BOX_DEFINTION             (char*)"composite_box_body_divider_definition"
# define YARS_STRING_CYLINDER                                 (char*)"cylinder"
# define YARS_STRING_COMPOSITE_GEOM_CYLINDER_DEFINTION        (char*)"composite_cylinder_geom_divider_definition"
# define YARS_STRING_COMPOSITE_BODY_CYLINDER_DEFINTION        (char*)"composite_cylinder_body_divider_definition"

# define YARS_STRING_CAPPED_CYLINDER                          (char*)"capsule"
# define YARS_STRING_COMPOSITE_GEOM_CAPPED_CYLINDER_DEFINTION (char*)"composite_capsule_geom_divider_definition"
# define YARS_STRING_COMPOSITE_BODY_CAPPED_CYLINDER_DEFINTION (char*)"composite_capsule_body_divider_definition"

# define YARS_STRING_SPHERE                                   (char*)"sphere"
# define YARS_STRING_COMPOSITE_BODY_SPHERE_DEFINTION          (char*)"composite_sphere_body_divider_definition"
# define YARS_STRING_COMPOSITE_GEOM_SPHERE_DEFINTION          (char*)"composite_sphere_geom_divider_definition"

# define YARS_STRING_POSE_DEFINITION                          (char*)"pose_divider_definition"
# define YARS_STRING_DIMENSION                                (char*)"dimension"

# define YARS_STRING_MASS                                     (char*)"mass"
# define YARS_STRING_MASS_DEFINITION                          (char*)"mass_divider_definition"

# define YARS_STRING_COMPOSITE_BODY_PHYSICS                   (char*)"composite_body_physics"

# define YARS_STRING_C_PHYSICS_DEFINITION                     (char*)"composite_physics_divider_definition"

DataComposite::DataComposite(DataNode *parent)
  : DataObject(parent, DATA_OBJECT_COMPOSITE)
{
  _physics = new DataPhysicsParameter(this);
}

void DataComposite::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_COMPOSITE))
  {
    current = parent;
    return;
  }
  if(element->opening(YARS_STRING_OBJECT_COMPOSITE))
  {
    element->set(YARS_STRING_NAME, _name);
    return;
  }
  if(element->opening(YARS_STRING_PHYSICS))
  {
    current = _physics;
    _physics->add(element);
  }

  DataObject *object = DataObjectFactory::object(element, this);
  if(object != NULL)
  {
    current = object;
    object->add(element);
    _geoms.push_back(object);
  }
}

void DataComposite::createXsd(XsdSpecification *spec)
{
  XsdSequence *composite = new XsdSequence(YARS_STRING_OBJECT_COMPOSITE_DEFINITION);
  composite->add(NA(YARS_STRING_NAME,     YARS_STRING_XSD_STRING,                false));
  composite->add(NE(YARS_STRING_GEOMETRY, YARS_STRING_COMPOSITE_GEOM_DEFINITION, 1, 1));
  composite->add(NE(YARS_STRING_POSE,     YARS_STRING_POSE_DEFINITION,           0, 1));
  composite->add(NE(YARS_STRING_PHYSICS,  YARS_STRING_C_PHYSICS_DEFINITION,      1, 1));
  spec->add(composite);

  XsdChoice *geom = new XsdChoice(YARS_STRING_COMPOSITE_GEOM_DEFINITION, "0", YARS_STRING_XSD_UNBOUNDED);
  geom->add(NE(YARS_STRING_BOX,             YARS_STRING_OBJECT_BOX_DEFINTION,             0, 1));
  geom->add(NE(YARS_STRING_SPHERE,          YARS_STRING_OBJECT_SPHERE_DEFINTION,          0, 1));
  geom->add(NE(YARS_STRING_CYLINDER,        YARS_STRING_OBJECT_CYLINDER_DEFINTION,        0, 1));
  geom->add(NE(YARS_STRING_CAPPED_CYLINDER, YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION, 0, 1));
  geom->add(NE(YARS_STRING_OBJECT_PLY,      YARS_STRING_OBJECT_PLY_DEFINTION, 0, 1));
  spec->add(geom);

  DataObjectFactory::createXsdWithoutComposite(spec);
}

DataObjects::iterator DataComposite::g_begin()
{
  return _geoms.begin();
}

DataObjects::iterator DataComposite::g_end()
{
  return _geoms.end();
}

int DataComposite::g_size()
{
  return (int)_geoms.size();
}

DataObject* DataComposite::geom(int index)
{
  return _geoms[index];
}

void DataComposite::add(DataObject* object)
{
  _geoms.push_back(object);
}

void DataComposite::applyOffset(Pose pose)
{
  for(DataObjects::iterator i = _geoms.begin(); i != _geoms.end(); i++)
  {
    (*i)->applyOffset(pose);
  }
}

DataComposite* DataComposite::_copy()
{
  DataComposite *copy = new DataComposite(NULL); // no parent required -> no parsing occurs from here on
  for(DataObjects::iterator i = g_begin(); i != g_end(); i++)
  {
    copy->add((*i)->copy());
  }
  copy->setPhysics(_physics->copy());
  return copy;
}

void DataComposite::_resetTo(const DataObject* other)
{
  DataComposite *composite = (DataComposite*)other;
  for(int i = 0; i < g_size(); i++)
  {
    _geoms[i]->resetTo(composite->geom(i));
  }
}
