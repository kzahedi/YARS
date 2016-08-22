#ifndef __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__
#define __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__

// taken from
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=ColoredTextAreaOverlayElement&structure=Cookbook

#include <OgreOverlay/OgreTextAreaOverlayElement.h>
#include <Ogre/Ogre.h>

#include <vector>

using namespace std;

class ColoredTextAreaOverlayElement : public Ogre::TextAreaOverlayElement
{
  public:
    ColoredTextAreaOverlayElement(const Ogre::String& name);
    ~ColoredTextAreaOverlayElement(void);

    void setValueBottom(float Value);
    void setValueTop(float Value);
    void setCaption(const Ogre::DisplayString& text);
    static Ogre::DisplayString StripColors(const Ogre::DisplayString& text);
    static Ogre::ColourValue GetColor(unsigned char ID, float Value = 1.0f);

  protected:
    void updateColours(void);

    std::vector<unsigned char> m_Colors;
    float m_ValueTop;
    float m_ValueBottom;
};

#endif // __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__
