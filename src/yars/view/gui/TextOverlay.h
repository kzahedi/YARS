#ifndef __TEXT_OVERLAY_H__
#define __TEXT_OVERLAY_H__

#include <yars/view/gui/ColoredTextAreaOverlayElementFactory.h>

#include <OGRE/Ogre.h>
#if __APPLE__
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#else
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#endif
#include <string>
#include <vector>

using namespace std;

class TextOverlay
{
public:
  TextOverlay(string name);
  ~TextOverlay();

  void printf(const std::string &ID, const char *fmt, /* args*/...);

  void addTextBox(
      const std::string &ID,
      const std::string &text,
      Ogre::Real x, Ogre::Real y,
      Ogre::Real width, Ogre::Real height,
      const Ogre::ColourValue &color = Ogre::ColourValue(1.0, 1.0, 1.0, 1.0),
      std::string fontname = "MyFont",
      std::string charheight = "");

  bool hasOverlay(const std::string &ID);
  void removeTextBox(const std::string &ID);
  void setCaption(const std::string &ID, const Ogre::DisplayString &text);
  void setPosition(const std::string &ID, Ogre::Real x, Ogre::Real y);
  Ogre::ColourValue getColour(unsigned char ID, float Value);

  Ogre::Real getWidth(const std::string &ID);
  Ogre::Real getHeight(const std::string &ID);

  Ogre::DisplayString stripColours(const Ogre::DisplayString &text);

  void setText(const std::string &ID, const std::string &Text, int windowHeight = 0);
  // const std::string& getText(const std::string& ID);
  static TextOverlay *_me;
  std::vector<unsigned char> _colors;

private:
  void updateColours(void);

  Ogre::OverlayManager *_overlayMgr;
  Ogre::Overlay *_overlay;
  Ogre::OverlayContainer *_panel;
  Ogre::Real _valueTop;
  Ogre::Real _valueBottom;
  Ogre::RenderOperation _renderOp;
  static bool _added;
  int _robotFontSize;
  int _timeFontSize;
  int _statsFontSize;
};

#endif // __TEXT_OVERLAY_H__
