#include "ColoredTextAreaOverlayElement.h"

#include "ColoredTextAreaOverlayElement.h"

#define POS_TEX_BINDING 0
#define COLOUR_BINDING 1

#include <iostream>

using namespace std;

ColoredTextAreaOverlayElement::ColoredTextAreaOverlayElement(const Ogre::String& name)
  : TextAreaOverlayElement(name)
  , m_ValueTop(1.0f)
  , m_ValueBottom(0.8f)
{
  cout << "hier 0" << endl;
}

ColoredTextAreaOverlayElement::~ColoredTextAreaOverlayElement(void)
{
}

void ColoredTextAreaOverlayElement::setValueBottom(float Value)
{
  m_ValueTop = Value;
  mColoursChanged = true;
}

void ColoredTextAreaOverlayElement::setValueTop(float Value)
{
  m_ValueBottom = Value;
  mColoursChanged = true;
}

Ogre::ColourValue ColoredTextAreaOverlayElement::GetColor(unsigned char ID, float Value)
{
  switch (ID)
  {
    case 0:
      return Ogre::ColourValue(0, 0, 0);    // Black
    case 1:
      return Ogre::ColourValue(Value, 0, 0);    // Red
    case 2:
      return Ogre::ColourValue(0, Value, 0);    // Green
    case 3:
      return Ogre::ColourValue(Value, Value, 0);    // Yellow
    case 4:
      return Ogre::ColourValue(0, 0, Value);    // Blue
    case 5:
      return Ogre::ColourValue(0, Value, Value);    // Cyan
    case 6:
      return Ogre::ColourValue(Value, 0, Value);    // Magenta
    case 7:
      return Ogre::ColourValue(Value, Value, Value);    // White
  }
  return Ogre::ColourValue::Black;
}

Ogre::DisplayString ColoredTextAreaOverlayElement::StripColors(const Ogre::DisplayString& text)
{
  Ogre::DisplayString StrippedText;
  int i;
  for (i = 0; i < (int)text.size()-1; ++i)
  {
    if (text[i] == '^' &&
        text[i+1] >= '0' && text[i+1] <= '7')    // This is a color code, ignore it
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

void ColoredTextAreaOverlayElement::setCaption(const Ogre::DisplayString& text)
{
  m_Colors.resize(text.size(), 7);
  int i, iNumColorCodes = 0, iNumSpaces = 0;
  for (i = 0; i < (int)text.size()-1; ++i)
  {
    if (text[i] == ' ' || text[i] == '\n')
    {
      // Spaces and newlines are skipped when rendering and as such can't have a color
      ++iNumSpaces;
    }
    else if (text[i] == '^' &&
             text[i+1] >= '0' && text[i+1] <= '7')    // This is a color code
    {
      // Fill the color array starting from this point to the end with the new color code
      // adjustments need to made because color codes will be removed and spaces are not counted
      fill(m_Colors.begin()+i-(2*iNumColorCodes)-iNumSpaces, m_Colors.end(), text[i+1]-'0');
      ++i;
      ++iNumColorCodes;
    }
  }
  // Set the caption using the base class, but strip the color codes from it first
  TextAreaOverlayElement::setCaption(StripColors(text));
}

void ColoredTextAreaOverlayElement::updateColours(void)
{
  // Convert to system-specific
  Ogre::RGBA topColour, bottomColour;
  // Set default to white
  Ogre::Root::getSingleton().convertColourValue(Ogre::ColourValue::White, &topColour);
  Ogre::Root::getSingleton().convertColourValue(Ogre::ColourValue::White, &bottomColour);

  Ogre::HardwareVertexBufferSharedPtr vbuf = 
    mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);

  Ogre::RGBA* pDest = static_cast<Ogre::RGBA*>(
                                   vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD) );

  for (size_t i = 0; i < mAllocSize; ++i)
  {
    if (i < m_Colors.size())
    {
      Ogre::Root::getSingleton().convertColourValue(GetColor(m_Colors[i], m_ValueTop), &topColour);
      Ogre::Root::getSingleton().convertColourValue(GetColor(m_Colors[i], m_ValueBottom), &bottomColour);
    }

    // First tri (top, bottom, top)
    *pDest++ = topColour;
    *pDest++ = bottomColour;
    *pDest++ = topColour;
    // Second tri (top, bottom, bottom)
    *pDest++ = topColour;
    *pDest++ = bottomColour;
    *pDest++ = bottomColour;
  }
  vbuf->unlock();
}
