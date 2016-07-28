#ifndef __OSX_WRAP_H__
#define __OSX_WRAP_H__

// C++ header so we keep the Obj-C stuff contained

#include <SDL2/SDL_syswm.h>

Ogre::String OSX_cocoa_view( SDL_SysWMinfo &info );

void OSX_GL_set_current( Ogre::RenderWindow * ogre_render_window );
void OSX_GL_clear_current( Ogre::RenderWindow * ogre_render_window );

#endif // __OSX_WRAP_H__
