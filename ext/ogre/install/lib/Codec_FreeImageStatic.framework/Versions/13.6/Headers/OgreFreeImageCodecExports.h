
#ifndef _OgreFreeImageCodecExport_H
#define _OgreFreeImageCodecExport_H

#ifdef CODEC_FREEIMAGE_STATIC_DEFINE
#  define _OgreFreeImageCodecExport
#  define CODEC_FREEIMAGE_NO_EXPORT
#else
#  ifndef _OgreFreeImageCodecExport
#    ifdef Codec_FreeImage_EXPORTS
        /* We are building this library */
#      define _OgreFreeImageCodecExport 
#    else
        /* We are using this library */
#      define _OgreFreeImageCodecExport 
#    endif
#  endif

#  ifndef CODEC_FREEIMAGE_NO_EXPORT
#    define CODEC_FREEIMAGE_NO_EXPORT 
#  endif
#endif

#ifndef CODEC_FREEIMAGE_DEPRECATED
#  define CODEC_FREEIMAGE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CODEC_FREEIMAGE_DEPRECATED_EXPORT
#  define CODEC_FREEIMAGE_DEPRECATED_EXPORT _OgreFreeImageCodecExport CODEC_FREEIMAGE_DEPRECATED
#endif

#ifndef CODEC_FREEIMAGE_DEPRECATED_NO_EXPORT
#  define CODEC_FREEIMAGE_DEPRECATED_NO_EXPORT CODEC_FREEIMAGE_NO_EXPORT CODEC_FREEIMAGE_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CODEC_FREEIMAGE_NO_DEPRECATED
#    define CODEC_FREEIMAGE_NO_DEPRECATED
#  endif
#endif

#endif /* _OgreFreeImageCodecExport_H */
