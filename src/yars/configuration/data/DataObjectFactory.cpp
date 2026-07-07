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

