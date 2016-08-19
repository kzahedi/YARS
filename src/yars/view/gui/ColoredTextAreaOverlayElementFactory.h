#ifndef __COLORED_TEXT_AREA_OVERLAY_ELEMENT_FACTORY_H__
#define __COLORED_TEXT_AREA_OVERLAY_ELEMENT_FACTORY_H__


#include <OgreOverlay/OgreOverlayElementFactory.h>
#include "view/gui/ColoredTextAreaOverlayElement.h"

/** Factory for creating TextAreaOverlayElement instances. */
class ColoredTextAreaOverlayElementFactory : public Ogre::OverlayElementFactory
{
  public:
    /** See OverlayElementFactory */
    Ogre::OverlayElement* createOverlayElement(const Ogre::String& instanceName)
    {
      return new ColoredTextAreaOverlayElement(instanceName);
    }
    /** See OverlayElementFactory */
    const Ogre::String& getTypeName() const
    {
      static Ogre::String name = "ColoredTextArea";
      return name;
    }
};

#endif // __COLORED_TEXT_AREA_OVERLAY_ELEMENT_FACTORY_H__
