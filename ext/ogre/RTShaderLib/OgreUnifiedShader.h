/*
 * OgreUnifiedShader.h
 * Compatibility header for Ogre RTSS-generated GLSL shaders (GL3+ core profile).
 *
 * Defines unified IN/OUT macros that work across GLSL versions.
 * Ogre injects OGRE_VERTEX_SHADER or OGRE_FRAGMENT_SHADER as compile defines.
 * USE_OGRE_FROM_FUTURE is #defined in the generated shader for GLSL 1.30+.
 */

// -------------------------------------------------------------------------
// Common math helpers (HLSL-style names used in RTSS library shaders)
// -------------------------------------------------------------------------
#define saturate(x)    clamp(x, 0.0, 1.0)
#define mul(a, b)      ((a) * (b))
#define lerp(a, b, t)  mix(a, b, t)

// Scalar-broadcast constructors (BGFX / Ogre shader library conventions)
#define vec2_splat(v)  vec2(v)
#define vec3_splat(v)  vec3(v)
#define vec4_splat(v)  vec4(v)

// -------------------------------------------------------------------------
// Texture sampler declaration macros
// In GLSL core profile, samplers are declared as plain uniforms.
// -------------------------------------------------------------------------
#define SAMPLER2D(name, slot)      uniform sampler2D name
#define SAMPLER3D(name, slot)      uniform sampler3D name
#define SAMPLERCUBE(name, slot)    uniform samplerCube name
#define SAMPLER2DARRAY(name, slot) uniform sampler2DArray name
#define SAMPLER2DMS(name, slot)    uniform sampler2DMS name

// texture2D / texture3D / textureCube were removed in GLSL core 1.30+.
// Map them to the modern 'texture()' built-in.
#define texture2D(s,c)       texture(s,c)
#define texture2DLod(s,c,l)  textureLod(s,c,l)
#define texture3D(s,c)       texture(s,c)
#define textureCube(s,c)     texture(s,c)
#define texture2DProj(s,c)   textureProj(s,c)

// YARS targets OpenGL 3.3+ core profile on both macOS (Ogre 13.x) and Linux
// (Ogre 14.x). GL3+ core profile rejects 'attribute'/'varying'; both stacks
// require 'in'/'out' qualifiers. The legacy branch below is preserved for
// reference but never taken — version detection inside Ogre's GLSL
// preprocessor was unreliable across the two Ogre versions, so we just
// commit to the modern syntax unconditionally.
#if 1 // Modern GLSL (1.30+ / OpenGL 3.3+ core profile) — always
#  define IN(a, b)   in a;
#  define OUT(a, b)  out a;

// In GLSL core profile, gl_FragColor is not a built-in AND names starting
// with 'gl_' cannot be user-declared (GLSL spec section 3.6).
// Declare a user output with a safe name and redirect via macro so that
// RTSS-generated shaders writing 'gl_FragColor = ...' compile correctly.
#  ifdef OGRE_FRAGMENT_SHADER
     layout(location = 0) out vec4 _gl_FragColor_out;
#    define gl_FragColor _gl_FragColor_out
#  endif

#else
// -------------------------------------------------------------------------
// Legacy GLSL 1.10 / 1.20 (attribute/varying syntax, gl_FragColor built-in)
// Preserved for reference; unreachable.
// -------------------------------------------------------------------------
#  define IN(a, b)   attribute a;
#  define OUT(a, b)  varying a;
#endif
