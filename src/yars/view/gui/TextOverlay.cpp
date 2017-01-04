#include "TextOverlay.h"

#include <yars/view/gui/ColoredTextAreaOverlayElementFactory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

bool TextOverlay::_added = false;

TextOverlay::TextOverlay(string name)
{
  if(!_added)
  {
    ColoredTextAreaOverlayElementFactory* pColoredTextAreaOverlayElementFactory = new ColoredTextAreaOverlayElementFactory();
    Ogre::OverlayManager::getSingleton().addOverlayElementFactory(pColoredTextAreaOverlayElementFactory);
  }
  _added         = true;
  _robotFontSize = 12;
  _timeFontSize  = 12;
  _statsFontSize = 12;
  _overlayMgr    = Ogre::OverlayManager::getSingletonPtr();

  _overlay = _overlayMgr->create(name.c_str());
  _panel   = static_cast<Ogre::OverlayContainer*>(_overlayMgr->createOverlayElement("Panel", name));
  _panel->setDimensions(1, 1);
  _panel->setPosition(0, 0);
  _overlay->add2D(_panel);
  _overlay->show();
}

void TextOverlay::addTextBox(const std::string& ID,
    const std::string& text,
    Ogre::Real x, Ogre::Real y,
    Ogre::Real width, Ogre::Real height,
    const Ogre::ColourValue& color,
    std::string fontname,
    std::string charheight)
{
  ColoredTextAreaOverlayElement* textBox =
    (ColoredTextAreaOverlayElement*)_overlayMgr->createOverlayElement("ColoredTextArea", ID);
  textBox->setDimensions(width, height);
  textBox->setMetricsMode(Ogre::GMM_PIXELS);
  textBox->setPosition(x, y);
  textBox->setWidth(width);
  textBox->setHeight(height);
  textBox->setParameter("font_name", fontname);
  textBox->setParameter("char_height", charheight);
  textBox->setMainColour(color);
  textBox->setCaption(text);
  _panel->addChild(textBox);
  if(ID == "robot") _robotFontSize = atoi(charheight.c_str());
  if(ID == "stats") _statsFontSize = atoi(charheight.c_str());
  if(ID == "time")  _timeFontSize  = atoi(charheight.c_str());
}

void TextOverlay::removeTextBox(const std::string& ID)
{
  _panel->removeChild(ID);
  _overlayMgr->destroyOverlayElement(ID);
}

void TextOverlay::setText(const std::string& ID, const std::string& Text, int windowHeight)
{
  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  std::string c = "^0" + Text;
  if(ID.find("robot") != std::string::npos)
  {
    int nr_of_lines = 1;
    for(int i=0; i < c.length(); i++)
    {
      if(c[i] == '\n') nr_of_lines++;
    }
    // float height = _robotFontSize * nr_of_lines;
    float height = 1.5 * _robotFontSize * nr_of_lines;
    textBox->setPosition(10, windowHeight-height-25);
    textBox->setHeight(height);
  }

  if(ID.find("time") != std::string::npos)
  {
    textBox->setPosition(10, 10);
    textBox->setHeight((int)_timeFontSize);
  }

  if(ID.find("stats") != std::string::npos)
  {
    textBox->setPosition(10, 10 + 1.2 * _timeFontSize + 5);
    textBox->setHeight((int)_statsFontSize);
  }

  textBox->setCaption(c);
}

// const std::string& TextOverlay::getText(const std::string& ID)
// {
  // Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  // return textBox->getCaption();
// }

void TextOverlay::printf(const std::string& ID,  const char *fmt, /* args*/ ...)
{
  char        text[256];
  va_list        ap;

  if (fmt == NULL)
  {
    *text=0;
  }
  else
  {
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);
  }

  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  textBox->setCaption(text);
}


Ogre::Real TextOverlay::getHeight(const std::string& ID)
{
  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  return textBox->getHeight();
}

Ogre::Real TextOverlay::getWidth(const std::string& ID)
{
  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  return textBox->getWidth();
}

void TextOverlay::setPosition(const std::string& ID, Ogre::Real x, Ogre::Real y)
{
  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  textBox->setPosition(x, y);
}

bool TextOverlay::hasOverlay(const std::string& ID)
{
  Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
  return (textBox != NULL);
}
