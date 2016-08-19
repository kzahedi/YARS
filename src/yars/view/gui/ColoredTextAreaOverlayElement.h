#include <vector>
#include <OgreOverlay/OgreTextAreaOverlayElement.h>



class ColoredTextAreaOverlayElement : public Ogre::TextAreaOverlayElement
{
  public:
    ColoredTextAreaOverlayElement(const Ogre::String& name);
    ~ColoredTextAreaOverlayElement(void);

    void setValueBottom(float Value);
    void setValueTop(float Value);
    void setCaption(const Ogre::DisplayString& text);
    static Ogre::DisplayString StripColors(const Ogre::String& text);
    static Ogre::ColourValue GetColor(unsigned char ID, float Value = 1.0f, Ogre::ColourValue def = Ogre::ColourValue::Black);

    void updateColours(void);
    void setMainColour(Ogre::ColourValue c);

  protected:
    std::vector<unsigned char> m_Colors;
    float m_ValueTop;
    float m_ValueBottom;
    Ogre::ColourValue _color;
};
