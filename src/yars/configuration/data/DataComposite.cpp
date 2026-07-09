#include "DataComposite.h"
#include "DataBinding.h"

#include "DataObjectFactory.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

#define YARS_STRING_POSE (char *)"pose"
#define YARS_STRING_OBJECT (char *)"object"
#define YARS_STRING_NAME (char *)"name"
#define YARS_STRING_GEOMETRY (char *)"geometry"
#define YARS_STRING_BODY (char *)"body"
#define YARS_STRING_VISUALISE (char *)"visualise"
#define YARS_STRING_COMPOSITE_BODY_DEFINTION (char *)"composite_body_definition"
// # define YARS_STRING_TRUE_FALSE_DEFINITION                    (char*)"true_false_definition"
#define YARS_STRING_RADIUS_DIMENSION_DEFINITION (char *)"radius_dimension_definition"
#define YARS_STRING_COMPOSITE_GEOM_DEFINITION (char *)"composite_geom_geom_definition"
#define YARS_STRING_BOX (char *)"box"
#define YARS_STRING_COMPOSITE_GEOM_BOX_DEFINTION (char *)"composite_box_geom_definition"
#define YARS_STRING_COMPOSITE_BODY_BOX_DEFINTION (char *)"composite_box_body_definition"
#define YARS_STRING_CYLINDER (char *)"cylinder"
#define YARS_STRING_COMPOSITE_GEOM_CYLINDER_DEFINTION (char *)"composite_cylinder_geom_definition"
#define YARS_STRING_COMPOSITE_BODY_CYLINDER_DEFINTION (char *)"composite_cylinder_body_definition"

#define YARS_STRING_CAPPED_CYLINDER (char *)"capsule"
#define YARS_STRING_COMPOSITE_GEOM_CAPPED_CYLINDER_DEFINTION (char *)"composite_capsule_geom_definition"
#define YARS_STRING_COMPOSITE_BODY_CAPPED_CYLINDER_DEFINTION (char *)"composite_capsule_body_definition"

#define YARS_STRING_SPHERE (char *)"sphere"
#define YARS_STRING_COMPOSITE_BODY_SPHERE_DEFINTION (char *)"composite_sphere_body_definition"
#define YARS_STRING_COMPOSITE_GEOM_SPHERE_DEFINTION (char *)"composite_sphere_geom_definition"

#define YARS_STRING_POSE_DEFINITION (char *)"pose_definition"
#define YARS_STRING_DIMENSION (char *)"dimension"

// # define YARS_STRING_MASS                                     (char*)"mass"
// # define YARS_STRING_MASS_DEFINITION                          (char*)"mass_definition"

#define YARS_STRING_COMPOSITE_BODY_PHYSICS (char *)"composite_body_physics"

#define YARS_STRING_C_PHYSICS_DEFINITION (char *)"composite_physics_definition"


namespace
{
// Attribute binding table for the composite's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &compositeAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataComposite *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataComposite::DataComposite(DataNode *parent)
    : DataObject(parent, DATA_OBJECT_COMPOSITE)
{
  _physics = new DataPhysicsParameter(this);
}

void DataComposite::add(DataParseElement *element)
{
  if (element->closing(YARS_STRING_OBJECT_COMPOSITE))
  {
    current = parent;
    return;
  }
  if (element->opening(YARS_STRING_OBJECT_COMPOSITE))
  {
    yars::applyAttributes(this, element, compositeAttributeBindings());
    return;
  }
  if (element->opening(YARS_STRING_PHYSICS))
  {
    current = _physics;
    _physics->add(element);
  }

  DataObject *object = DataObjectFactory::object(element, this);
  if (object != NULL)
  {
    current = object;
    object->add(element);
    _geoms.push_back(object);
  }
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

DataObject *DataComposite::geom(int index)
{
  return _geoms[index];
}

void DataComposite::add(DataObject *object)
{
  _geoms.push_back(object);
}

void DataComposite::applyOffset(Pose pose)
{
  for (DataObjects::iterator i = _geoms.begin(); i != _geoms.end(); i++)
  {
    (*i)->applyOffset(pose);
  }
}

DataComposite *DataComposite::_copy()
{
  DataComposite *copy = new DataComposite(NULL); // no parent required -> no parsing occurs from here on
  for (DataObjects::iterator i = g_begin(); i != g_end(); i++)
  {
    copy->add((*i)->copy());
  }
  copy->setPhysics(_physics->copy());
  return copy;
}

void DataComposite::_resetTo(const DataObject *other)
{
  DataComposite *composite = (DataComposite *)other;
  for (int i = 0; i < g_size(); i++)
  {
    _geoms[i]->resetTo(composite->geom(i));
  }
}
