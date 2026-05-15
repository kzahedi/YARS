
#ifndef _OgreGL3PlusExport_H
#define _OgreGL3PlusExport_H

#ifdef RENDERSYSTEM_GL3PLUS_STATIC_DEFINE
#  define _OgreGL3PlusExport
#  define RENDERSYSTEM_GL3PLUS_NO_EXPORT
#else
#  ifndef _OgreGL3PlusExport
#    ifdef RenderSystem_GL3Plus_EXPORTS
        /* We are building this library */
#      define _OgreGL3PlusExport __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define _OgreGL3PlusExport __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef RENDERSYSTEM_GL3PLUS_NO_EXPORT
#    define RENDERSYSTEM_GL3PLUS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef RENDERSYSTEM_GL3PLUS_DEPRECATED
#  define RENDERSYSTEM_GL3PLUS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef RENDERSYSTEM_GL3PLUS_DEPRECATED_EXPORT
#  define RENDERSYSTEM_GL3PLUS_DEPRECATED_EXPORT _OgreGL3PlusExport RENDERSYSTEM_GL3PLUS_DEPRECATED
#endif

#ifndef RENDERSYSTEM_GL3PLUS_DEPRECATED_NO_EXPORT
#  define RENDERSYSTEM_GL3PLUS_DEPRECATED_NO_EXPORT RENDERSYSTEM_GL3PLUS_NO_EXPORT RENDERSYSTEM_GL3PLUS_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RENDERSYSTEM_GL3PLUS_NO_DEPRECATED
#    define RENDERSYSTEM_GL3PLUS_NO_DEPRECATED
#  endif
#endif

#endif /* _OgreGL3PlusExport_H */
