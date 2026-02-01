#ifndef __COLORED_TEXT_AREA_OVERLAY_ELEMENT_CPP__
#define __COLORED_TEXT_AREA_OVERLAY_ELEMENT_CPP__

#include "ColoredTextAreaOverlayElement.h"
#include <OgreRoot.h>

using namespace Ogre;
using namespace std;

ColoredTextAreaOverlayElement::ColoredTextAreaOverlayElement(const String &name)
    : TextAreaOverlayElement(name), m_ValueTop(1.0f), m_ValueBottom(0.8f)
{
}

ColoredTextAreaOverlayElement::~ColoredTextAreaOverlayElement(void)
{
}

void ColoredTextAreaOverlayElement::setValueBottom(float Value)
{
  m_ValueTop = Value;
  // OGRE 14: mColoursChanged is private, use base class method
  setColour(getColour());  // Force refresh
}

void ColoredTextAreaOverlayElement::setValueTop(float Value)
{
  m_ValueBottom = Value;
  // OGRE 14: mColoursChanged is private, use base class method
  setColour(getColour());  // Force refresh
}

ColourValue ColoredTextAreaOverlayElement::GetColor(unsigned char ID, float Value, ColourValue def)
{
  switch (ID)
  {
  case 0:
    return def;
  case 1:
    return ColourValue(Value, 0, 0); // Red
  case 2:
    return ColourValue(0, Value, 0); // Green
  case 3:
    return ColourValue(Value, Value, 0); // Yellow
  case 4:
    return ColourValue(0, 0, Value); // Blue
  case 5:
    return ColourValue(0, Value, Value); // Cyan
  case 6:
    return ColourValue(Value, 0, Value); // Magenta
  case 7:
    return ColourValue(Value, Value, Value); // White
  case 8:
    return ColourValue(Value * 0.9, Value * 0.9, Value * 0.9); // Gray
  case 9:
    return ColourValue(0.5, 0.5, Value * 0.9); // dark blue
  }
  return def;
}

DisplayString ColoredTextAreaOverlayElement::StripColors(const Ogre::String &otext)
{
  try
  {
    DisplayString text = DisplayString(otext.c_str());
    DisplayString StrippedText;
    int i;
    for (i = 0; i < (int)text.size() - 1; ++i)
    {
      if (text[i] == '^' &&
          text[i + 1] >= '0' && text[i + 1] <= '9') // This is a color code, ignore it
      {
        ++i;
      }
      else
      {
        StrippedText.append(1, text[i]);
      }
    }
    // One last character to add because loop went to size()-1
    if (i < (int)text.size())
      StrippedText.append(1, text[i]);
    return StrippedText;
  }
  catch (...)
  {
  }
  return String("UTF8 error (String cannot be disaplayed with current font set)");
}

void ColoredTextAreaOverlayElement::setCaption(const DisplayString &text)
{
  m_Colors.clear();
  m_Colors.resize(text.size(), 9);
  int i, iNumColorCodes = 0, iNumSpaces = 0;
  for (i = 0; i < (int)text.size() - 1; ++i)
  {
    if (text[i] == ' ' || text[i] == '\n')
    {
      ++iNumSpaces;
    }
    else if (text[i] == '^' &&
             text[i + 1] >= '0' && text[i + 1] <= '9')
    {
      fill(m_Colors.begin() + i - (2 * iNumColorCodes) - iNumSpaces, m_Colors.end(), text[i + 1] - '0');
      ++i;
      ++iNumColorCodes;
    }
  }
  // Set the caption using the base class, but strip the color codes from it first
  TextAreaOverlayElement::setCaption(StripColors(text));
}

void ColoredTextAreaOverlayElement::updateColours(void)
{
  // OGRE 14: updateColours is now private in base class
  // Color handling is done through setColour() in other methods
}

void ColoredTextAreaOverlayElement::setMainColour(ColourValue c)
{
  _color = c;
  setColour(c);  // Use base class method
}

#endif // __COLORED_TEXT_AREA_OVERLAY_ELEMENT_CPP__
