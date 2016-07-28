#ifndef __DATA_OBJECT_FACTORY_H__
#define __DATA_OBJECT_FACTORY_H__

#include "DataObject.h"
#include "DataBox.h"
#include "DataSphere.h"
#include "DataCylinder.h"
#include "DataCapsule.h"
#include "DataComposite.h"
#include "DataMacro.h"

#include "DataPly.h"

# define YARS_STRING_OBJECT_DEFINITION (char*)"object" DIVIDER DEFINITION

/**
 * @brief Factory for all used basic objects in YARS.
 */
class DataObjectFactory
{
  public:
    /**
     * @brief Returns the object as base class instance DataObject. Ask for type
     * to see what kind of object it is.
     *
     * @param element
     * @param parent
     *
     * @return object
     * @sa DataObject
     * @sa DataObject::type()
     */
    static DataObject* object(DataParseElement *element, DataNode *parent);

    static void createXsd(XsdSpecification *spec);
    static void createXsdWithoutComposite(XsdSpecification *spec);

  private:

    static void        __createXsd(XsdSpecification *spec, bool withComposite);
    static DataObject* __generateBox(DataParseElement *element, DataNode *parent);
    static DataObject* __generateSphere(DataParseElement *element, DataNode *parent);
    static DataObject* __generateCylinder(DataParseElement *element, DataNode *parent);
    static DataObject* __generateCapsule(DataParseElement *element, DataNode *parent);
    static DataObject* __generateComposite(DataParseElement *element, DataNode *parent);
    static DataObject* __generatePly(DataParseElement *element, DataNode *parent);
};

#endif // __DATA_OBJECT_FACTORY_H__
