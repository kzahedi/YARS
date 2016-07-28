#include "DataObjectFactory.h"
#include "DataBox.h"
#include "DataSphere.h"
#include "DataCylinder.h"
#include "DataCapsule.h"
#include "DataComposite.h"
#include "DataPly.h"
#include "DataMacro.h"


#include <iostream>
using namespace std;


DataObject* DataObjectFactory::object(DataParseElement *element, DataNode *parent)
{
  if(element->opening(YARS_STRING_OBJECT_BOX))
  {
    return __generateBox(element, parent);
  }
  if(element->opening(YARS_STRING_OBJECT_SPHERE))
  {
    return __generateSphere(element, parent);
  }
  if(element->opening(YARS_STRING_OBJECT_CYLINDER))
  {
    return __generateCylinder(element, parent);
  }
  if(element->opening(YARS_STRING_OBJECT_CAPPED_CYLINDER))
  {
    return __generateCapsule(element, parent);
  }
  if(element->opening(YARS_STRING_OBJECT_COMPOSITE))
  {
    return __generateComposite(element, parent);
  }
  if(element->opening(YARS_STRING_OBJECT_PLY))
  {
    return __generatePly(element, parent);
  }
  return NULL;
}

DataObject* DataObjectFactory::__generateBox(DataParseElement *element, DataNode *parent)
{
  DataBox *box = new DataBox(parent);
  box->add(element);
  return box;
}

DataObject* DataObjectFactory::__generateSphere(DataParseElement *element, DataNode *parent)
{
  DataSphere *sphere = new DataSphere(parent);
  sphere->add(element);
  return sphere;
}

DataObject* DataObjectFactory::__generateCylinder(DataParseElement *element, DataNode *parent)
{
  DataCylinder *cylinder = new DataCylinder(parent);
  cylinder->add(element);
  return cylinder;
}

DataObject* DataObjectFactory::__generateCapsule(DataParseElement *element, DataNode *parent)
{
  DataCapsule *cappedCylinder = new DataCapsule(parent);
  cappedCylinder->add(element);
  return cappedCylinder;
}

DataObject* DataObjectFactory::__generateComposite(DataParseElement *element, DataNode *parent)
{
  DataComposite *composite = new DataComposite(parent);
  composite->add(element);
  return composite;
}

DataObject* DataObjectFactory::__generatePly(DataParseElement *element, DataNode *parent)
{
  DataPly *ply = new DataPly(parent);
  ply->add(element);
  return ply;
}

void DataObjectFactory::createXsd(XsdSpecification *spec)
{
  __createXsd(spec, true);
}

void DataObjectFactory::createXsdWithoutComposite(XsdSpecification *spec)
{
  __createXsd(spec, false);
}

void DataObjectFactory::__createXsd(XsdSpecification *spec, bool withComposite)
{
  XsdChoice *objectDefinition = new XsdChoice(YARS_STRING_OBJECT_DEFINITION, "0", YARS_STRING_XSD_UNBOUNDED);
  objectDefinition->add(NE(YARS_STRING_OBJECT_BOX,
                           YARS_STRING_OBJECT_BOX_DEFINTION,            "0", YARS_STRING_XSD_UNBOUNDED));

  objectDefinition->add(NE(YARS_STRING_OBJECT_SPHERE,
                           YARS_STRING_OBJECT_SPHERE_DEFINTION,         "0", YARS_STRING_XSD_UNBOUNDED));

  objectDefinition->add(NE(YARS_STRING_OBJECT_PLY,
                           YARS_STRING_OBJECT_PLY_DEFINTION,            "0", YARS_STRING_XSD_UNBOUNDED));

  objectDefinition->add(NE(YARS_STRING_OBJECT_CYLINDER,
                           YARS_STRING_OBJECT_CYLINDER_DEFINTION,        "0", YARS_STRING_XSD_UNBOUNDED));

  objectDefinition->add(NE(YARS_STRING_OBJECT_CAPPED_CYLINDER,
                           YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION, "0", YARS_STRING_XSD_UNBOUNDED));

  if(withComposite)
  {
    objectDefinition->add(NE(YARS_STRING_OBJECT_COMPOSITE,
          YARS_STRING_OBJECT_COMPOSITE_DEFINITION,      "0", YARS_STRING_XSD_UNBOUNDED));
  }

  objectDefinition->add(NE(YARS_STRING_OBJECT_MACRO,
                           YARS_STRING_OBJECT_MACRO_DEFINTION,           "0", YARS_STRING_XSD_UNBOUNDED));
  spec->add(objectDefinition);

  DataBox::createXsd(spec);
  DataSphere::createXsd(spec);
  DataCylinder::createXsd(spec);
  DataCapsule::createXsd(spec);
  DataPly::createXsd(spec);

  if(withComposite) DataComposite::createXsd(spec);
}
