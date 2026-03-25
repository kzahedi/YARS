#ifndef __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__
#define __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__

#include "ColoredTextAreaOverlayElement.h"
#include <OgreRoot.h>
#define POS_TEX_BINDING 0
#define COLOUR_BINDING 1

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
  // mColoursChanged = true; // Removed - private member
}

void ColoredTextAreaOverlayElement::setValueTop(float Value)
{
  m_ValueBottom = Value;
  // mColoursChanged = true; // Removed - private member
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
  bool noColor = true;
  int i, iNumColorCodes = 0, iNumSpaces = 0;
  for (i = 0; i < (int)text.size() - 1; ++i)
  {
    if (text[i] == ' ' || text[i] == '\n')
    {
      // Spaces and newlines are skipped when rendering and as such can't have a color
      ++iNumSpaces;
    }
    else if (text[i] == '^' &&
             text[i + 1] >= '0' && text[i + 1] <= '9') // This is a color code
    {
      // Fill the color array starting from this point to the end with the new color code
      // adjustments need to made because color codes will be removed and spaces are not counted
      fill(m_Colors.begin() + i - (2 * iNumColorCodes) - iNumSpaces, m_Colors.end(), text[i + 1] - '0');
      ++i;
      ++iNumColorCodes;
      noColor = false;
    }
  }
  // Always set the caption on the base class (strips color codes)
  TextAreaOverlayElement::setCaption(StripColors(text));
}

void ColoredTextAreaOverlayElement::updateColours(void)
{
  // The base class updateColours() is private so we can't call it directly.
  // Replicate its behaviour: fill the COLOUR_BINDING vertex buffer (binding 1)
  // with the top/bottom colours set via setColour() / setMainColour().
  RenderOperation rop;
  getRenderOperation(rop);
  if (!rop.vertexData) return;

  auto &vbb = rop.vertexData->vertexBufferBinding;
  if (!vbb->isBufferBound(COLOUR_BINDING))
    return;

  HardwareVertexBufferSharedPtr vbuf = vbb->getBuffer(COLOUR_BINDING);
  if (!vbuf || vbuf->getSizeInBytes() == 0)
    return;

  // Number of character quads allocated in the VBO (4 vertices each)
  size_t numChars = vbuf->getSizeInBytes() / (4 * sizeof(RGBA));

  const ColourValue &colTop    = getColourTop();
  const ColourValue &colBottom = getColourBottom();
  RGBA top    = colTop.getAsBYTE();
  RGBA bottom = colBottom.getAsBYTE();

  RGBA *pDest = static_cast<RGBA *>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
  for (size_t i = 0; i < numChars; ++i)
  {
    *pDest++ = top;
    *pDest++ = top;
    *pDest++ = bottom;
    *pDest++ = bottom;
  }
  vbuf->unlock();
}

void ColoredTextAreaOverlayElement::setMainColour(ColourValue c)
{
  _color = c;
  setColour(c);
}

#endif // __COLORED_TEXT_AREA_OVERLAY_ELEMENT_H__
