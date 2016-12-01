//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2016 Dusan Jocic <dusanjocic@msn.com>
//
//  This file is part of the OWEngine single player GPL Source Code.
//
//  OWengine Source Code is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OWEngine Source Code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with OWEngine Source Code. If not, see <http://www.gnu.org/licenses/>.
//
//  In addition, the OWEngine Source Code is also subject to certain additional terms.
//  You should have received a copy of these additional terms immediately following
//  the terms and conditions of the GNU General Public License which accompanied the
//  OWEngine Source Code. If not, please request a copy in writing from id Software
//  at the address below.
//
//  If you have questions concerning this license or the applicable additional terms,
//  you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
//  Suite 120, Rockville, Maryland 20850 USA.
//
// -------------------------------------------------------------------------
//  File name:   qgl.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __QGL_H__
#define __QGL_H__

#if defined( __LINT__ )

#include <GL/gl.h>

#elif defined( _WIN32 )

#if !defined( __GNUC__ )
#pragma warning (disable: 4201)
#pragma warning (disable: 4214)
#pragma warning (disable: 4514)
#pragma warning (disable: 4032)
#pragma warning (disable: 4201)
#pragma warning (disable: 4214)
#endif /* __GNUC__ */

#include <windows.h>
#include <GL/glew.h>
#elif defined( __MACOS__ )

#include <OpenGL/gl.h>

#elif defined( __linux__ )

// using our local glext.h
// http://oss.sgi.com/projects/ogl-sample/ABI/
#define GL_GLEXT_LEGACY
#define GLX_GLXEXT_LEGACY
// some GL headers define that, but only partially
// define and undefine so GL doesn't attempt anything
#define GL_ARB_multitexture
#include <GL/gl.h>
#include <GL/glx.h>
#undef GL_ARB_multitexture
#include "glext.h"

#elif defined( __FreeBSD__ )

#include <GL/gl.h>
#include <GL/glx.h>
#if defined( __FX__ )
#include <GL/fxmesa.h>
#endif

#elif defined (__ANDROID__)
#include <GLES/gl.h>
#include "../unix/android_glimp.h"

#else
#include <gl.h>

#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef WINAPI
#define WINAPI
#endif

//===========================================================================

/*
** multitexture extension definitions
*/
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_ACTIVE_TEXTURES_ARB          0x84E2

#define GL_RGB_S3TC	0x83A0
#define GL_RGB4_S3TC	0x83A1
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3

//#undef NDEBUG
//#define QGL_LOG_GL_CALLS

#ifdef QGL_LOG_GL_CALLS
extern unsigned int QGLLogGLCalls;

extern FILE* QGLDebugFile( void );
#endif


extern void QGLCheckError( const char* message );
extern unsigned int QGLBeginStarted;

//===========================================================================

// non-windows systems will just redefine qgl* to gl*
#if !defined( _WIN32 ) && !defined( MACOS_X ) && !defined( __linux__ ) && !defined( __FreeBSD__ ) // rb010123

#include "qgl_linked.h"

#elif defined( MACOS_X )
// This includes #ifdefs for optional logging and GL error checking after every GL call as well as #defines to prevent incorrect usage of the non-'qgl' versions of the GL API.
#include "macosx_qgl.h"

#else

// windows systems use a function pointer for each call so we can load minidrivers

extern void ( APIENTRY* qglAccum )( GLenum op, GLfloat value );
extern void ( APIENTRY* qglAlphaFunc )( GLenum func, GLclampf ref );
extern GLboolean( APIENTRY* qglAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
extern void ( APIENTRY* qglArrayElement )( GLint i );
extern void ( APIENTRY* qglBegin )( GLenum mode );
extern void ( APIENTRY* qglBindTexture )( GLenum target, GLuint texture );
extern void ( APIENTRY* qglBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
extern void ( APIENTRY* qglBlendFunc )( GLenum sfactor, GLenum dfactor );
extern void ( APIENTRY* qglCallList )( GLuint list );
extern void ( APIENTRY* qglCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
extern void ( APIENTRY* qglClear )( GLbitfield mask );
extern void ( APIENTRY* qglClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern void ( APIENTRY* qglClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
extern void ( APIENTRY* qglClearDepth )( GLclampd depth );
extern void ( APIENTRY* qglClearIndex )( GLfloat c );
extern void ( APIENTRY* qglClearStencil )( GLint s );
extern void ( APIENTRY* qglClipPlane )( GLenum plane, const GLdouble* equation );
extern void ( APIENTRY* qglColor3b )( GLbyte red, GLbyte green, GLbyte blue );
extern void ( APIENTRY* qglColor3bv )( const GLbyte* v );
extern void ( APIENTRY* qglColor3d )( GLdouble red, GLdouble green, GLdouble blue );
extern void ( APIENTRY* qglColor3dv )( const GLdouble* v );
extern void ( APIENTRY* qglColor3f )( GLfloat red, GLfloat green, GLfloat blue );
extern void ( APIENTRY* qglColor3fv )( const GLfloat* v );
extern void ( APIENTRY* qglColor3i )( GLint red, GLint green, GLint blue );
extern void ( APIENTRY* qglColor3iv )( const GLint* v );
extern void ( APIENTRY* qglColor3s )( GLshort red, GLshort green, GLshort blue );
extern void ( APIENTRY* qglColor3sv )( const GLshort* v );
extern void ( APIENTRY* qglColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
extern void ( APIENTRY* qglColor3ubv )( const GLubyte* v );
extern void ( APIENTRY* qglColor3ui )( GLuint red, GLuint green, GLuint blue );
extern void ( APIENTRY* qglColor3uiv )( const GLuint* v );
extern void ( APIENTRY* qglColor3us )( GLushort red, GLushort green, GLushort blue );
extern void ( APIENTRY* qglColor3usv )( const GLushort* v );
extern void ( APIENTRY* qglColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
extern void ( APIENTRY* qglColor4bv )( const GLbyte* v );
extern void ( APIENTRY* qglColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
extern void ( APIENTRY* qglColor4dv )( const GLdouble* v );
extern void ( APIENTRY* qglColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern void ( APIENTRY* qglColor4fv )( const GLfloat* v );
extern void ( APIENTRY* qglColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
extern void ( APIENTRY* qglColor4iv )( const GLint* v );
extern void ( APIENTRY* qglColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
extern void ( APIENTRY* qglColor4sv )( const GLshort* v );
extern void ( APIENTRY* qglColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
extern void ( APIENTRY* qglColor4ubv )( const GLubyte* v );
extern void ( APIENTRY* qglColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
extern void ( APIENTRY* qglColor4uiv )( const GLuint* v );
extern void ( APIENTRY* qglColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
extern void ( APIENTRY* qglColor4usv )( const GLushort* v );
extern void ( APIENTRY* qglColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
extern void ( APIENTRY* qglColorMaterial )( GLenum face, GLenum mode );
extern void ( APIENTRY* qglColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
extern void ( APIENTRY* qglCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
extern void ( APIENTRY* qglCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
extern void ( APIENTRY* qglCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
extern void ( APIENTRY* qglCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
extern void ( APIENTRY* qglCullFace )( GLenum mode );
extern void ( APIENTRY* qglDeleteLists )( GLuint list, GLsizei range );
extern void ( APIENTRY* qglDeleteTextures )( GLsizei n, const GLuint* textures );
extern void ( APIENTRY* qglDepthFunc )( GLenum func );
extern void ( APIENTRY* qglDepthMask )( GLboolean flag );
extern void ( APIENTRY* qglDepthRange )( GLclampd zNear, GLclampd zFar );
extern void ( APIENTRY* qglDisable )( GLenum cap );
extern void ( APIENTRY* qglDisableClientState )( GLenum array );
extern void ( APIENTRY* qglDrawArrays )( GLenum mode, GLint first, GLsizei count );
extern void ( APIENTRY* qglDrawBuffer )( GLenum mode );
extern void ( APIENTRY* qglDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
extern void ( APIENTRY* qglDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern void ( APIENTRY* qglEdgeFlag )( GLboolean flag );
extern void ( APIENTRY* qglEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglEdgeFlagv )( const GLboolean* flag );
extern void ( APIENTRY* qglEnable )( GLenum cap );
extern void ( APIENTRY* qglEnableClientState )( GLenum array );
extern void ( APIENTRY* qglEnd )( void );
extern void ( APIENTRY* qglEndList )( void );
extern void ( APIENTRY* qglEvalCoord1d )( GLdouble u );
extern void ( APIENTRY* qglEvalCoord1dv )( const GLdouble* u );
extern void ( APIENTRY* qglEvalCoord1f )( GLfloat u );
extern void ( APIENTRY* qglEvalCoord1fv )( const GLfloat* u );
extern void ( APIENTRY* qglEvalCoord2d )( GLdouble u, GLdouble v );
extern void ( APIENTRY* qglEvalCoord2dv )( const GLdouble* u );
extern void ( APIENTRY* qglEvalCoord2f )( GLfloat u, GLfloat v );
extern void ( APIENTRY* qglEvalCoord2fv )( const GLfloat* u );
extern void ( APIENTRY* qglEvalMesh1 )( GLenum mode, GLint i1, GLint i2 );
extern void ( APIENTRY* qglEvalMesh2 )( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
extern void ( APIENTRY* qglEvalPoint1 )( GLint i );
extern void ( APIENTRY* qglEvalPoint2 )( GLint i, GLint j );
extern void ( APIENTRY* qglFeedbackBuffer )( GLsizei size, GLenum type, GLfloat* buffer );
extern void ( APIENTRY* qglFinish )( void );
extern void ( APIENTRY* qglFlush )( void );
extern void ( APIENTRY* qglFogf )( GLenum pname, GLfloat param );
extern void ( APIENTRY* qglFogfv )( GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglFogi )( GLenum pname, GLint param );
extern void ( APIENTRY* qglFogiv )( GLenum pname, const GLint* params );
extern void ( APIENTRY* qglFrontFace )( GLenum mode );
extern void ( APIENTRY* qglFrustum )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern GLuint( APIENTRY* qglGenLists )( GLsizei range );
extern void ( APIENTRY* qglGenTextures )( GLsizei n, GLuint* textures );
extern void ( APIENTRY* qglGetBooleanv )( GLenum pname, GLboolean* params );
extern void ( APIENTRY* qglGetClipPlane )( GLenum plane, GLdouble* equation );
extern void ( APIENTRY* qglGetDoublev )( GLenum pname, GLdouble* params );
extern GLenum( APIENTRY* qglGetError )( void );
extern void ( APIENTRY* qglGetFloatv )( GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetIntegerv )( GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetLightfv )( GLenum light, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetLightiv )( GLenum light, GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetMapdv )( GLenum target, GLenum query, GLdouble* v );
extern void ( APIENTRY* qglGetMapfv )( GLenum target, GLenum query, GLfloat* v );
extern void ( APIENTRY* qglGetMapiv )( GLenum target, GLenum query, GLint* v );
extern void ( APIENTRY* qglGetMaterialfv )( GLenum face, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetMaterialiv )( GLenum face, GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetPixelMapfv )( GLenum map, GLfloat* values );
extern void ( APIENTRY* qglGetPixelMapuiv )( GLenum map, GLuint* values );
extern void ( APIENTRY* qglGetPixelMapusv )( GLenum map, GLushort* values );
extern void ( APIENTRY* qglGetPointerv )( GLenum pname, GLvoid * *params );
extern void ( APIENTRY* qglGetPolygonStipple )( GLubyte* mask );
extern const GLubyte* ( APIENTRY* qglGetString )( GLenum name );
extern void ( APIENTRY* qglGetTexEnvfv )( GLenum target, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetTexEnviv )( GLenum target, GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetTexGendv )( GLenum coord, GLenum pname, GLdouble* params );
extern void ( APIENTRY* qglGetTexGenfv )( GLenum coord, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetTexGeniv )( GLenum coord, GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetTexImage )( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels );
extern void ( APIENTRY* qglGetTexLevelParameterfv )( GLenum target, GLint level, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetTexLevelParameteriv )( GLenum target, GLint level, GLenum pname, GLint* params );
extern void ( APIENTRY* qglGetTexParameterfv )( GLenum target, GLenum pname, GLfloat* params );
extern void ( APIENTRY* qglGetTexParameteriv )( GLenum target, GLenum pname, GLint* params );
extern void ( APIENTRY* qglHint )( GLenum target, GLenum mode );
extern void ( APIENTRY* qglIndexMask )( GLuint mask );
extern void ( APIENTRY* qglIndexPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglIndexd )( GLdouble c );
extern void ( APIENTRY* qglIndexdv )( const GLdouble* c );
extern void ( APIENTRY* qglIndexf )( GLfloat c );
extern void ( APIENTRY* qglIndexfv )( const GLfloat* c );
extern void ( APIENTRY* qglIndexi )( GLint c );
extern void ( APIENTRY* qglIndexiv )( const GLint* c );
extern void ( APIENTRY* qglIndexs )( GLshort c );
extern void ( APIENTRY* qglIndexsv )( const GLshort* c );
extern void ( APIENTRY* qglIndexub )( GLubyte c );
extern void ( APIENTRY* qglIndexubv )( const GLubyte* c );
extern void ( APIENTRY* qglInitNames )( void );
extern void ( APIENTRY* qglInterleavedArrays )( GLenum format, GLsizei stride, const GLvoid* pointer );
extern GLboolean( APIENTRY* qglIsEnabled )( GLenum cap );
extern GLboolean( APIENTRY* qglIsList )( GLuint list );
extern GLboolean( APIENTRY* qglIsTexture )( GLuint texture );
extern void ( APIENTRY* qglLightModelf )( GLenum pname, GLfloat param );
extern void ( APIENTRY* qglLightModelfv )( GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglLightModeli )( GLenum pname, GLint param );
extern void ( APIENTRY* qglLightModeliv )( GLenum pname, const GLint* params );
extern void ( APIENTRY* qglLightf )( GLenum light, GLenum pname, GLfloat param );
extern void ( APIENTRY* qglLightfv )( GLenum light, GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglLighti )( GLenum light, GLenum pname, GLint param );
extern void ( APIENTRY* qglLightiv )( GLenum light, GLenum pname, const GLint* params );
extern void ( APIENTRY* qglLineStipple )( GLint factor, GLushort pattern );
extern void ( APIENTRY* qglLineWidth )( GLfloat width );
extern void ( APIENTRY* qglListBase )( GLuint base );
extern void ( APIENTRY* qglLoadIdentity )( void );
extern void ( APIENTRY* qglLoadMatrixd )( const GLdouble* m );
extern void ( APIENTRY* qglLoadMatrixf )( const GLfloat* m );
extern void ( APIENTRY* qglLoadName )( GLuint name );
extern void ( APIENTRY* qglLogicOp )( GLenum opcode );
extern void ( APIENTRY* qglMap1d )( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points );
extern void ( APIENTRY* qglMap1f )( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points );
extern void ( APIENTRY* qglMap2d )( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points );
extern void ( APIENTRY* qglMap2f )( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points );
extern void ( APIENTRY* qglMapGrid1d )( GLint un, GLdouble u1, GLdouble u2 );
extern void ( APIENTRY* qglMapGrid1f )( GLint un, GLfloat u1, GLfloat u2 );
extern void ( APIENTRY* qglMapGrid2d )( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
extern void ( APIENTRY* qglMapGrid2f )( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
extern void ( APIENTRY* qglMaterialf )( GLenum face, GLenum pname, GLfloat param );
extern void ( APIENTRY* qglMaterialfv )( GLenum face, GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglMateriali )( GLenum face, GLenum pname, GLint param );
extern void ( APIENTRY* qglMaterialiv )( GLenum face, GLenum pname, const GLint* params );
extern void ( APIENTRY* qglMatrixMode )( GLenum mode );
extern void ( APIENTRY* qglMultMatrixd )( const GLdouble* m );
extern void ( APIENTRY* qglMultMatrixf )( const GLfloat* m );
extern void ( APIENTRY* qglNewList )( GLuint list, GLenum mode );
extern void ( APIENTRY* qglNormal3b )( GLbyte nx, GLbyte ny, GLbyte nz );
extern void ( APIENTRY* qglNormal3bv )( const GLbyte* v );
extern void ( APIENTRY* qglNormal3d )( GLdouble nx, GLdouble ny, GLdouble nz );
extern void ( APIENTRY* qglNormal3dv )( const GLdouble* v );
extern void ( APIENTRY* qglNormal3f )( GLfloat nx, GLfloat ny, GLfloat nz );
extern void ( APIENTRY* qglNormal3fv )( const GLfloat* v );
extern void ( APIENTRY* qglNormal3i )( GLint nx, GLint ny, GLint nz );
extern void ( APIENTRY* qglNormal3iv )( const GLint* v );
extern void ( APIENTRY* qglNormal3s )( GLshort nx, GLshort ny, GLshort nz );
extern void ( APIENTRY* qglNormal3sv )( const GLshort* v );
extern void ( APIENTRY* qglNormalPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglOrtho )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern void ( APIENTRY* qglPassThrough )( GLfloat token );
extern void ( APIENTRY* qglPixelMapfv )( GLenum map, GLsizei mapsize, const GLfloat* values );
extern void ( APIENTRY* qglPixelMapuiv )( GLenum map, GLsizei mapsize, const GLuint* values );
extern void ( APIENTRY* qglPixelMapusv )( GLenum map, GLsizei mapsize, const GLushort* values );
extern void ( APIENTRY* qglPixelStoref )( GLenum pname, GLfloat param );
extern void ( APIENTRY* qglPixelStorei )( GLenum pname, GLint param );
extern void ( APIENTRY* qglPixelTransferf )( GLenum pname, GLfloat param );
extern void ( APIENTRY* qglPixelTransferi )( GLenum pname, GLint param );
extern void ( APIENTRY* qglPixelZoom )( GLfloat xfactor, GLfloat yfactor );
extern void ( APIENTRY* qglPointSize )( GLfloat size );
extern void ( APIENTRY* qglPolygonMode )( GLenum face, GLenum mode );
extern void ( APIENTRY* qglPolygonOffset )( GLfloat factor, GLfloat units );
extern void ( APIENTRY* qglPolygonStipple )( const GLubyte* mask );
extern void ( APIENTRY* qglPopAttrib )( void );
extern void ( APIENTRY* qglPopClientAttrib )( void );
extern void ( APIENTRY* qglPopMatrix )( void );
extern void ( APIENTRY* qglPopName )( void );
extern void ( APIENTRY* qglPrioritizeTextures )( GLsizei n, const GLuint* textures, const GLclampf* priorities );
extern void ( APIENTRY* qglPushAttrib )( GLbitfield mask );
extern void ( APIENTRY* qglPushClientAttrib )( GLbitfield mask );
extern void ( APIENTRY* qglPushMatrix )( void );
extern void ( APIENTRY* qglPushName )( GLuint name );
extern void ( APIENTRY* qglRasterPos2d )( GLdouble x, GLdouble y );
extern void ( APIENTRY* qglRasterPos2dv )( const GLdouble* v );
extern void ( APIENTRY* qglRasterPos2f )( GLfloat x, GLfloat y );
extern void ( APIENTRY* qglRasterPos2fv )( const GLfloat* v );
extern void ( APIENTRY* qglRasterPos2i )( GLint x, GLint y );
extern void ( APIENTRY* qglRasterPos2iv )( const GLint* v );
extern void ( APIENTRY* qglRasterPos2s )( GLshort x, GLshort y );
extern void ( APIENTRY* qglRasterPos2sv )( const GLshort* v );
extern void ( APIENTRY* qglRasterPos3d )( GLdouble x, GLdouble y, GLdouble z );
extern void ( APIENTRY* qglRasterPos3dv )( const GLdouble* v );
extern void ( APIENTRY* qglRasterPos3f )( GLfloat x, GLfloat y, GLfloat z );
extern void ( APIENTRY* qglRasterPos3fv )( const GLfloat* v );
extern void ( APIENTRY* qglRasterPos3i )( GLint x, GLint y, GLint z );
extern void ( APIENTRY* qglRasterPos3iv )( const GLint* v );
extern void ( APIENTRY* qglRasterPos3s )( GLshort x, GLshort y, GLshort z );
extern void ( APIENTRY* qglRasterPos3sv )( const GLshort* v );
extern void ( APIENTRY* qglRasterPos4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void ( APIENTRY* qglRasterPos4dv )( const GLdouble* v );
extern void ( APIENTRY* qglRasterPos4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void ( APIENTRY* qglRasterPos4fv )( const GLfloat* v );
extern void ( APIENTRY* qglRasterPos4i )( GLint x, GLint y, GLint z, GLint w );
extern void ( APIENTRY* qglRasterPos4iv )( const GLint* v );
extern void ( APIENTRY* qglRasterPos4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern void ( APIENTRY* qglRasterPos4sv )( const GLshort* v );
extern void ( APIENTRY* qglReadBuffer )( GLenum mode );
extern void ( APIENTRY* qglReadPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels );
extern void ( APIENTRY* qglRectd )( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
extern void ( APIENTRY* qglRectdv )( const GLdouble* v1, const GLdouble* v2 );
extern void ( APIENTRY* qglRectf )( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
extern void ( APIENTRY* qglRectfv )( const GLfloat* v1, const GLfloat* v2 );
extern void ( APIENTRY* qglRecti )( GLint x1, GLint y1, GLint x2, GLint y2 );
extern void ( APIENTRY* qglRectiv )( const GLint* v1, const GLint* v2 );
extern void ( APIENTRY* qglRects )( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
extern void ( APIENTRY* qglRectsv )( const GLshort* v1, const GLshort* v2 );
extern GLint( APIENTRY* qglRenderMode )( GLenum mode );
extern void ( APIENTRY* qglRotated )( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
extern void ( APIENTRY* qglRotatef )( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern void ( APIENTRY* qglScaled )( GLdouble x, GLdouble y, GLdouble z );
extern void ( APIENTRY* qglScalef )( GLfloat x, GLfloat y, GLfloat z );
extern void ( APIENTRY* qglScissor )( GLint x, GLint y, GLsizei width, GLsizei height );
extern void ( APIENTRY* qglSelectBuffer )( GLsizei size, GLuint* buffer );
extern void ( APIENTRY* qglShadeModel )( GLenum mode );
extern void ( APIENTRY* qglStencilFunc )( GLenum func, GLint ref, GLuint mask );
extern void ( APIENTRY* qglStencilMask )( GLuint mask );
extern void ( APIENTRY* qglStencilOp )( GLenum fail, GLenum zfail, GLenum zpass );
extern void ( APIENTRY* qglTexCoord1d )( GLdouble s );
extern void ( APIENTRY* qglTexCoord1dv )( const GLdouble* v );
extern void ( APIENTRY* qglTexCoord1f )( GLfloat s );
extern void ( APIENTRY* qglTexCoord1fv )( const GLfloat* v );
extern void ( APIENTRY* qglTexCoord1i )( GLint s );
extern void ( APIENTRY* qglTexCoord1iv )( const GLint* v );
extern void ( APIENTRY* qglTexCoord1s )( GLshort s );
extern void ( APIENTRY* qglTexCoord1sv )( const GLshort* v );
extern void ( APIENTRY* qglTexCoord2d )( GLdouble s, GLdouble t );
extern void ( APIENTRY* qglTexCoord2dv )( const GLdouble* v );
extern void ( APIENTRY* qglTexCoord2f )( GLfloat s, GLfloat t );
extern void ( APIENTRY* qglTexCoord2fv )( const GLfloat* v );
extern void ( APIENTRY* qglTexCoord2i )( GLint s, GLint t );
extern void ( APIENTRY* qglTexCoord2iv )( const GLint* v );
extern void ( APIENTRY* qglTexCoord2s )( GLshort s, GLshort t );
extern void ( APIENTRY* qglTexCoord2sv )( const GLshort* v );
extern void ( APIENTRY* qglTexCoord3d )( GLdouble s, GLdouble t, GLdouble r );
extern void ( APIENTRY* qglTexCoord3dv )( const GLdouble* v );
extern void ( APIENTRY* qglTexCoord3f )( GLfloat s, GLfloat t, GLfloat r );
extern void ( APIENTRY* qglTexCoord3fv )( const GLfloat* v );
extern void ( APIENTRY* qglTexCoord3i )( GLint s, GLint t, GLint r );
extern void ( APIENTRY* qglTexCoord3iv )( const GLint* v );
extern void ( APIENTRY* qglTexCoord3s )( GLshort s, GLshort t, GLshort r );
extern void ( APIENTRY* qglTexCoord3sv )( const GLshort* v );
extern void ( APIENTRY* qglTexCoord4d )( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
extern void ( APIENTRY* qglTexCoord4dv )( const GLdouble* v );
extern void ( APIENTRY* qglTexCoord4f )( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
extern void ( APIENTRY* qglTexCoord4fv )( const GLfloat* v );
extern void ( APIENTRY* qglTexCoord4i )( GLint s, GLint t, GLint r, GLint q );
extern void ( APIENTRY* qglTexCoord4iv )( const GLint* v );
extern void ( APIENTRY* qglTexCoord4s )( GLshort s, GLshort t, GLshort r, GLshort q );
extern void ( APIENTRY* qglTexCoord4sv )( const GLshort* v );
extern void ( APIENTRY* qglTexCoordPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglTexEnvf )( GLenum target, GLenum pname, GLfloat param );
extern void ( APIENTRY* qglTexEnvfv )( GLenum target, GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglTexEnvi )( GLenum target, GLenum pname, GLint param );
extern void ( APIENTRY* qglTexEnviv )( GLenum target, GLenum pname, const GLint* params );
extern void ( APIENTRY* qglTexGend )( GLenum coord, GLenum pname, GLdouble param );
extern void ( APIENTRY* qglTexGendv )( GLenum coord, GLenum pname, const GLdouble* params );
extern void ( APIENTRY* qglTexGenf )( GLenum coord, GLenum pname, GLfloat param );
extern void ( APIENTRY* qglTexGenfv )( GLenum coord, GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglTexGeni )( GLenum coord, GLenum pname, GLint param );
extern void ( APIENTRY* qglTexGeniv )( GLenum coord, GLenum pname, const GLint* params );
extern void ( APIENTRY* qglTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern void ( APIENTRY* qglTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern void ( APIENTRY* qglTexParameterf )( GLenum target, GLenum pname, GLfloat param );
extern void ( APIENTRY* qglTexParameterfv )( GLenum target, GLenum pname, const GLfloat* params );
extern void ( APIENTRY* qglTexParameteri )( GLenum target, GLenum pname, GLint param );
extern void ( APIENTRY* qglTexParameteriv )( GLenum target, GLenum pname, const GLint* params );
extern void ( APIENTRY* qglTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels );
extern void ( APIENTRY* qglTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern void ( APIENTRY* qglTranslated )( GLdouble x, GLdouble y, GLdouble z );
extern void ( APIENTRY* qglTranslatef )( GLfloat x, GLfloat y, GLfloat z );
extern void ( APIENTRY* qglVertex2d )( GLdouble x, GLdouble y );
extern void ( APIENTRY* qglVertex2dv )( const GLdouble* v );
extern void ( APIENTRY* qglVertex2f )( GLfloat x, GLfloat y );
extern void ( APIENTRY* qglVertex2fv )( const GLfloat* v );
extern void ( APIENTRY* qglVertex2i )( GLint x, GLint y );
extern void ( APIENTRY* qglVertex2iv )( const GLint* v );
extern void ( APIENTRY* qglVertex2s )( GLshort x, GLshort y );
extern void ( APIENTRY* qglVertex2sv )( const GLshort* v );
extern void ( APIENTRY* qglVertex3d )( GLdouble x, GLdouble y, GLdouble z );
extern void ( APIENTRY* qglVertex3dv )( const GLdouble* v );
extern void ( APIENTRY* qglVertex3f )( GLfloat x, GLfloat y, GLfloat z );
extern void ( APIENTRY* qglVertex3fv )( const GLfloat* v );
extern void ( APIENTRY* qglVertex3i )( GLint x, GLint y, GLint z );
extern void ( APIENTRY* qglVertex3iv )( const GLint* v );
extern void ( APIENTRY* qglVertex3s )( GLshort x, GLshort y, GLshort z );
extern void ( APIENTRY* qglVertex3sv )( const GLshort* v );
extern void ( APIENTRY* qglVertex4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void ( APIENTRY* qglVertex4dv )( const GLdouble* v );
extern void ( APIENTRY* qglVertex4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void ( APIENTRY* qglVertex4fv )( const GLfloat* v );
extern void ( APIENTRY* qglVertex4i )( GLint x, GLint y, GLint z, GLint w );
extern void ( APIENTRY* qglVertex4iv )( const GLint* v );
extern void ( APIENTRY* qglVertex4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern void ( APIENTRY* qglVertex4sv )( const GLshort* v );
extern void ( APIENTRY* qglVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern void ( APIENTRY* qglViewport )( GLint x, GLint y, GLsizei width, GLsizei height );

#if defined( _WIN32 )

extern int ( WINAPI* qwglChoosePixelFormat )( HDC, CONST PIXELFORMATDESCRIPTOR* );
extern int ( WINAPI* qwglDescribePixelFormat )( HDC, int, UINT, LPPIXELFORMATDESCRIPTOR );
extern int ( WINAPI* qwglGetPixelFormat )( HDC );
extern BOOL( WINAPI* qwglSetPixelFormat )( HDC, int, CONST PIXELFORMATDESCRIPTOR* );
extern BOOL( WINAPI* qwglSwapBuffers )( HDC );

extern BOOL( WINAPI* qwglGetDeviceGammaRamp3DFX )( HDC, LPVOID );
extern BOOL( WINAPI* qwglSetDeviceGammaRamp3DFX )( HDC, LPVOID );

extern BOOL( WINAPI* qwglCopyContext )( HGLRC, HGLRC, UINT );
extern HGLRC( WINAPI* qwglCreateContext )( HDC );
extern HGLRC( WINAPI* qwglCreateLayerContext )( HDC, int );
extern BOOL( WINAPI* qwglDeleteContext )( HGLRC );
extern HGLRC( WINAPI* qwglGetCurrentContext )( VOID );
extern HDC( WINAPI* qwglGetCurrentDC )( VOID );
extern PROC( WINAPI* qwglGetProcAddress )( LPCSTR );
extern BOOL( WINAPI* qwglMakeCurrent )( HDC, HGLRC );
extern BOOL( WINAPI* qwglShareLists )( HGLRC, HGLRC );
extern BOOL( WINAPI* qwglUseFontBitmaps )( HDC, DWORD, DWORD, DWORD );

extern BOOL( WINAPI* qwglUseFontOutlines )( HDC, DWORD, DWORD, DWORD, FLOAT,
        FLOAT, int, LPGLYPHMETRICSFLOAT );

extern BOOL( WINAPI* qwglDescribeLayerPlane )( HDC, int, int, UINT,
        LPLAYERPLANEDESCRIPTOR );
extern int ( WINAPI* qwglSetLayerPaletteEntries )( HDC, int, int, int,
        CONST COLORREF* );
extern int ( WINAPI* qwglGetLayerPaletteEntries )( HDC, int, int, int,
        COLORREF* );
extern BOOL( WINAPI* qwglRealizeLayerPalette )( HDC, int, BOOL );
extern BOOL( WINAPI* qwglSwapLayerBuffers )( HDC, UINT );

extern BOOL( WINAPI* qwglSwapIntervalEXT )( int interval );

#endif  // _WIN32

#if ( ( defined __linux__ )  || ( defined __FreeBSD__ ) ) // rb010123

//FX Mesa Functions
// bk001129 - from cvs1.17 (mkv)
#if defined ( __FX__ )
extern fxMesaContext( *qfxMesaCreateContext )( GLuint win, GrScreenResolution_t, GrScreenRefresh_t, const GLint attribList[] );
extern fxMesaContext( *qfxMesaCreateBestContext )( GLuint win, GLint width, GLint height, const GLint attribList[] );
extern void( *qfxMesaDestroyContext )( fxMesaContext ctx );
extern void( *qfxMesaMakeCurrent )( fxMesaContext ctx );
extern fxMesaContext( *qfxMesaGetCurrentContext )( void );
extern void( *qfxMesaSwapBuffers )( void );
#endif

//GLX Functions
extern void* ( *qwglGetProcAddress )( const char* symbol );
extern XVisualInfo* ( *qglXChooseVisual )( Display* dpy, int screen, int* attribList );
extern GLXContext( *qglXCreateContext )( Display* dpy, XVisualInfo* vis, GLXContext shareList, Bool direct );
extern void( *qglXDestroyContext )( Display* dpy, GLXContext ctx );
extern Bool( *qglXMakeCurrent )( Display* dpy, GLXDrawable drawable, GLXContext ctx );
extern void( *qglXCopyContext )( Display* dpy, GLXContext src, GLXContext dst, GLuint mask );
extern void( *qglXSwapBuffers )( Display* dpy, GLXDrawable drawable );

extern int ( *qglXSwapIntervalSGI )( int interval );
extern int ( *qglXGetVideoSyncSGI )( unsigned int* count );
extern int ( *qglXWaitVideoSyncSGI )( int divisor, int remainder, unsigned int* count );
extern const char* ( *qglXQueryExtensionsString )( Display* dpy, int screen );

#endif // __linux__ || __FreeBSD__

#endif  // _WIN32 && __linux__

#else

// This has to be done to avoid infinite recursion between our glGetError wrapper and QGLCheckError()
static inline GLenum _glGetError( void )
{
    return glGetError();
}

// void glAlphaFunc (GLenum func, GLclampf ref);
static inline void qglAlphaFunc( GLenum func, GLclampf ref )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glAlphaFunc(func=%lu, ref=%f)\n", func, ref );
#endif
    glAlphaFunc( func, ref );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glAlphaFunc" );
#endif
}


// void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static inline void qglClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearColor(red=%f, green=%f, blue=%f, alpha=%f)\n", red, green, blue, alpha );
#endif
    glClearColor( red, green, blue, alpha );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearColor" );
#endif
}


// void glClearDepthf (GLclampf depth);
static inline void qglClearDepthf( GLclampf depth )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearDepthf(depth=%f)\n", depth );
#endif
    glClearDepthf( depth );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearDepthf" );
#endif
}

static inline void qglClearDepth( GLclampf depth )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearDepthf(depth=%f)\n", depth );
#endif
    glClearDepthf( depth );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearDepthf" );
#endif
}


// void glClipPlanef (GLenum plane, const GLfloat *equation);
static inline void qglClipPlanef( GLenum plane, const GLfloat* equation )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClipPlanef(plane=%lu, equation=%p)\n", plane, equation );
#endif
    glClipPlanef( plane, equation );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClipPlanef" );
#endif
}

static inline void qglClipPlane( GLenum plane, const GLfloat* equation )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClipPlanef(plane=%lu, equation=%p)\n", plane, equation );
#endif
    glClipPlanef( plane, equation );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClipPlanef" );
#endif
}


// void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
// void glDepthRangef (GLclampf zNear, GLclampf zFar);
static inline void qglDepthRangef( GLclampf zNear, GLclampf zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDepthRangef(zNear=%f, zFar=%f)\n", zNear, zFar );
#endif
    glDepthRangef( zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDepthRangef" );
#endif
}

static inline void qglDepthRange( GLclampf zNear, GLclampf zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDepthRangef(zNear=%f, zFar=%f)\n", zNear, zFar );
#endif
    glDepthRangef( zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDepthRangef" );
#endif
}


// void glFogf (GLenum pname, GLfloat param);
static inline void qglFogf( GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFogf(pname=%lu, param=%f)\n", pname, param );
#endif
    glFogf( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFogf" );
#endif
}


// void glFogfv (GLenum pname, const GLfloat *params);
static inline void qglFogfv( GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFogfv(pname=%lu, params=%p)\n", pname, params );
#endif
    glFogfv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFogfv" );
#endif
}


// void glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
static inline void qglFrustumf( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFrustumf(left=%f, right=%f, bottom=%f, top=%f, zNear=%f, zFar=%f)\n", left, right, bottom, top, zNear, zFar );
#endif
    glFrustumf( left, right, bottom, top, zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFrustumf" );
#endif
}


// void glGetClipPlanef (GLenum pname, GLfloat eqn[4]);
static inline void qglGetClipPlanef( GLenum pname, GLfloat eqn[4] )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetClipPlanef(pname=%lu, eqn=%f)\n", pname, eqn );
#endif
    glGetClipPlanef( pname, eqn );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetClipPlanef" );
#endif
}


// void glGetFloatv (GLenum pname, GLfloat *params);
static inline void qglGetFloatv( GLenum pname, GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetFloatv(pname=%lu, params=%p)\n", pname, params );
#endif
    glGetFloatv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetFloatv" );
#endif
}


// void glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
static inline void qglGetLightfv( GLenum light, GLenum pname, GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetLightfv(light=%lu, pname=%lu, params=%p)\n", light, pname, params );
#endif
    glGetLightfv( light, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetLightfv" );
#endif
}


// void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
static inline void qglGetMaterialfv( GLenum face, GLenum pname, GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetMaterialfv(face=%lu, pname=%lu, params=%p)\n", face, pname, params );
#endif
    glGetMaterialfv( face, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetMaterialfv" );
#endif
}


// void glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params);
static inline void qglGetTexEnvfv( GLenum env, GLenum pname, GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexEnvfv(env=%lu, pname=%lu, params=%p)\n", env, pname, params );
#endif
    glGetTexEnvfv( env, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexEnvfv" );
#endif
}


// void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
static inline void qglGetTexParameterfv( GLenum target, GLenum pname, GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexParameterfv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glGetTexParameterfv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexParameterfv" );
#endif
}


// void glLightModelf (GLenum pname, GLfloat param);
static inline void qglLightModelf( GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightModelf(pname=%lu, param=%f)\n", pname, param );
#endif
    glLightModelf( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightModelf" );
#endif
}


// void glLightModelfv (GLenum pname, const GLfloat *params);
static inline void qglLightModelfv( GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightModelfv(pname=%lu, params=%p)\n", pname, params );
#endif
    glLightModelfv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightModelfv" );
#endif
}


// void glLightf (GLenum light, GLenum pname, GLfloat param);
static inline void qglLightf( GLenum light, GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightf(light=%lu, pname=%lu, param=%f)\n", light, pname, param );
#endif
    glLightf( light, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightf" );
#endif
}


// void glLightfv (GLenum light, GLenum pname, const GLfloat *params);
static inline void qglLightfv( GLenum light, GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightfv(light=%lu, pname=%lu, params=%p)\n", light, pname, params );
#endif
    glLightfv( light, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightfv" );
#endif
}


// void glLineWidth (GLfloat width);
static inline void qglLineWidth( GLfloat width )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLineWidth(width=%f)\n", width );
#endif
    glLineWidth( width );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLineWidth" );
#endif
}


// void glLoadMatrixf (const GLfloat *m);
static inline void qglLoadMatrixf( const GLfloat* m )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLoadMatrixf(m=%p)\n", m );
#endif
    glLoadMatrixf( m );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLoadMatrixf" );
#endif
}


// void glMaterialf (GLenum face, GLenum pname, GLfloat param);
static inline void qglMaterialf( GLenum face, GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMaterialf(face=%lu, pname=%lu, param=%f)\n", face, pname, param );
#endif
    glMaterialf( face, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMaterialf" );
#endif
}


// void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
static inline void qglMaterialfv( GLenum face, GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMaterialfv(face=%lu, pname=%lu, params=%p)\n", face, pname, params );
#endif
    glMaterialfv( face, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMaterialfv" );
#endif
}


// void glMultMatrixf (const GLfloat *m);
static inline void qglMultMatrixf( const GLfloat* m )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMultMatrixf(m=%p)\n", m );
#endif
    glMultMatrixf( m );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMultMatrixf" );
#endif
}


// void glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
static inline void qglMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMultiTexCoord4f(target=%lu, s=%f, t=%f, r=%f, q=%f)\n", target, s, t, r, q );
#endif
    glMultiTexCoord4f( target, s, t, r, q );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMultiTexCoord4f" );
#endif
}


// void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz);
static inline void qglNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glNormal3f(nx=%f, ny=%f, nz=%f)\n", nx, ny, nz );
#endif
    glNormal3f( nx, ny, nz );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glNormal3f" );
#endif
}


// void glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
static inline void qglOrthof( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glOrthof(left=%f, right=%f, bottom=%f, top=%f, zNear=%f, zFar=%f)\n", left, right, bottom, top, zNear, zFar );
#endif
    glOrthof( left, right, bottom, top, zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glOrthof" );
#endif
}

static inline void qglOrtho( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glOrthof(left=%f, right=%f, bottom=%f, top=%f, zNear=%f, zFar=%f)\n", left, right, bottom, top, zNear, zFar );
#endif
    glOrthof( left, right, bottom, top, zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glOrthof" );
#endif
}


// void glPointParameterf (GLenum pname, GLfloat param);
static inline void qglPointParameterf( GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointParameterf(pname=%lu, param=%f)\n", pname, param );
#endif
    glPointParameterf( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointParameterf" );
#endif
}


// void glPointParameterfv (GLenum pname, const GLfloat *params);
static inline void qglPointParameterfv( GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointParameterfv(pname=%lu, params=%p)\n", pname, params );
#endif
    glPointParameterfv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointParameterfv" );
#endif
}


// void glPointSize (GLfloat size);
static inline void qglPointSize( GLfloat size )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointSize(size=%f)\n", size );
#endif
    glPointSize( size );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointSize" );
#endif
}


// void glPolygonOffset (GLfloat factor, GLfloat units);
static inline void qglPolygonOffset( GLfloat factor, GLfloat units )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPolygonOffset(factor=%f, units=%f)\n", factor, units );
#endif
    glPolygonOffset( factor, units );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPolygonOffset" );
#endif
}


// void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
static inline void qglRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glRotatef(angle=%f, x=%f, y=%f, z=%f)\n", angle, x, y, z );
#endif
    glRotatef( angle, x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glRotatef" );
#endif
}


// void glScalef (GLfloat x, GLfloat y, GLfloat z);
static inline void qglScalef( GLfloat x, GLfloat y, GLfloat z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glScalef(x=%f, y=%f, z=%f)\n", x, y, z );
#endif
    glScalef( x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glScalef" );
#endif
}


// void glTexEnvf (GLenum target, GLenum pname, GLfloat param);
static inline void qglTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnvf(target=%lu, pname=%lu, param=%f)\n", target, pname, param );
#endif
    glTexEnvf( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnvf" );
#endif
}


// void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
static inline void qglTexEnvfv( GLenum target, GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnvfv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexEnvfv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnvfv" );
#endif
}


// void glTexParameterf (GLenum target, GLenum pname, GLfloat param);
static inline void qglTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameterf(target=%lu, pname=%lu, param=%f)\n", target, pname, param );
#endif
    glTexParameterf( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameterf" );
#endif
}


// void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
static inline void qglTexParameterfv( GLenum target, GLenum pname, const GLfloat* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameterfv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexParameterfv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameterfv" );
#endif
}


// void glTranslatef (GLfloat x, GLfloat y, GLfloat z);
static inline void qglTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTranslatef(x=%f, y=%f, z=%f)\n", x, y, z );
#endif
    glTranslatef( x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTranslatef" );
#endif
}


// void glActiveTexture (GLenum texture);
static inline void qglActiveTexture( GLenum texture )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glActiveTexture(texture=%lu)\n", texture );
#endif
    glActiveTexture( texture );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glActiveTexture" );
#endif
}


// void glAlphaFuncx (GLenum func, GLclampx ref);
static inline void qglAlphaFuncx( GLenum func, GLclampx ref )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glAlphaFuncx(func=%lu, ref=%ld)\n", func, ref );
#endif
    glAlphaFuncx( func, ref );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glAlphaFuncx" );
#endif
}


// void glBindBuffer (GLenum target, GLuint buffer);
static inline void qglBindBuffer( GLenum target, GLuint buffer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glBindBuffer(target=%lu, buffer=%lu)\n", target, buffer );
#endif
    glBindBuffer( target, buffer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glBindBuffer" );
#endif
}


// void glBindTexture (GLenum target, GLuint texture);
static inline void qglBindTexture( GLenum target, GLuint texture )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glBindTexture(target=%lu, texture=%lu)\n", target, texture );
#endif
    glBindTexture( target, texture );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glBindTexture" );
#endif
}


// void glBlendFunc (GLenum sfactor, GLenum dfactor);
static inline void qglBlendFunc( GLenum sfactor, GLenum dfactor )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glBlendFunc(sfactor=%lu, dfactor=%lu)\n", sfactor, dfactor );
#endif
    glBlendFunc( sfactor, dfactor );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glBlendFunc" );
#endif
}


// void glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
static inline void qglBufferData( GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glBufferData(target=%lu, size=%ld, data=%p, usage=%lu)\n", target, size, data, usage );
#endif
    glBufferData( target, size, data, usage );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glBufferData" );
#endif
}


// void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
static inline void qglBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glBufferSubData(target=%lu, offset=%ld, size=%ld, data=%p)\n", target, offset, size, data );
#endif
    glBufferSubData( target, offset, size, data );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glBufferSubData" );
#endif
}


// void glClear (GLbitfield mask);
static inline void qglClear( GLbitfield mask )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClear(mask=%lu)\n", mask );
#endif
    glClear( mask );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClear" );
#endif
}


// void glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
static inline void qglClearColorx( GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearColorx(red=%ld, green=%ld, blue=%ld, alpha=%ld)\n", red, green, blue, alpha );
#endif
    glClearColorx( red, green, blue, alpha );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearColorx" );
#endif
}


// void glClearDepthx (GLclampx depth);
static inline void qglClearDepthx( GLclampx depth )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearDepthx(depth=%ld)\n", depth );
#endif
    glClearDepthx( depth );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearDepthx" );
#endif
}


// void glClearStencil (GLint s);
static inline void qglClearStencil( GLint s )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClearStencil(s=%ld)\n", s );
#endif
    glClearStencil( s );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClearStencil" );
#endif
}


// void glClientActiveTexture (GLenum texture);
static inline void qglClientActiveTexture( GLenum texture )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClientActiveTexture(texture=%lu)\n", texture );
#endif
    glClientActiveTexture( texture );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClientActiveTexture" );
#endif
}


// void glClipPlanex (GLenum plane, const GLfixed *equation);
static inline void qglClipPlanex( GLenum plane, const GLfixed* equation )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glClipPlanex(plane=%lu, equation=%p)\n", plane, equation );
#endif
    glClipPlanex( plane, equation );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glClipPlanex" );
#endif
}


// void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
static inline void qglColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glColor4ub(red=%u, green=%u, blue=%u, alpha=%u)\n", red, green, blue, alpha );
#endif
    glColor4ub( red, green, blue, alpha );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glColor4ub" );
#endif
}


// void glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
static inline void qglColor4x( GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glColor4x(red=%ld, green=%ld, blue=%ld, alpha=%ld)\n", red, green, blue, alpha );
#endif
    glColor4x( red, green, blue, alpha );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glColor4x" );
#endif
}


// void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
static inline void qglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glColorMask(red=%u, green=%u, blue=%u, alpha=%u)\n", red, green, blue, alpha );
#endif
    glColorMask( red, green, blue, alpha );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glColorMask" );
#endif
}


// void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static inline void qglColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glColorPointer(size=%ld, type=%lu, stride=%ld, pointer=%p)\n", size, type, stride, pointer );
#endif
    glColorPointer( size, type, stride, pointer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glColorPointer" );
#endif
}


// void glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
static inline void qglCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glCompressedTexImage2D(target=%lu, level=%ld, internalformat=%lu, width=%ld, height=%ld, border=%ld, imageSize=%ld, data=%p)\n", target, level, internalformat, width, height, border, imageSize, data );
#endif
    glCompressedTexImage2D( target, level, internalformat, width, height, border, imageSize, data );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glCompressedTexImage2D" );
#endif
}


// void glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
static inline void qglCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glCompressedTexSubImage2D(target=%lu, level=%ld, xoffset=%ld, yoffset=%ld, width=%ld, height=%ld, format=%lu, imageSize=%ld, data=%p)\n", target, level, xoffset, yoffset, width, height, format, imageSize, data );
#endif
    glCompressedTexSubImage2D( target, level, xoffset, yoffset, width, height, format, imageSize, data );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glCompressedTexSubImage2D" );
#endif
}


// void glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
static inline void qglCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glCopyTexImage2D(target=%lu, level=%ld, internalformat=%lu, x=%ld, y=%ld, width=%ld, height=%ld, border=%ld)\n", target, level, internalformat, x, y, width, height, border );
#endif
    glCopyTexImage2D( target, level, internalformat, x, y, width, height, border );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glCopyTexImage2D" );
#endif
}


// void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static inline void qglCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glCopyTexSubImage2D(target=%lu, level=%ld, xoffset=%ld, yoffset=%ld, x=%ld, y=%ld, width=%ld, height=%ld)\n", target, level, xoffset, yoffset, x, y, width, height );
#endif
    glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glCopyTexSubImage2D" );
#endif
}


// void glCullFace (GLenum mode);
static inline void qglCullFace( GLenum mode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glCullFace(mode=%lu)\n", mode );
#endif
    glCullFace( mode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glCullFace" );
#endif
}


// void glDeleteBuffers (GLsizei n, const GLuint *buffers);
static inline void qglDeleteBuffers( GLsizei n, const GLuint* buffers )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDeleteBuffers(n=%ld, buffers=%p)\n", n, buffers );
#endif
    glDeleteBuffers( n, buffers );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDeleteBuffers" );
#endif
}


// void glDeleteTextures (GLsizei n, const GLuint *textures);
static inline void qglDeleteTextures( GLsizei n, const GLuint* textures )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDeleteTextures(n=%ld, textures=%p)\n", n, textures );
#endif
    glDeleteTextures( n, textures );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDeleteTextures" );
#endif
}


// void glDepthFunc (GLenum func);
static inline void qglDepthFunc( GLenum func )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDepthFunc(func=%lu)\n", func );
#endif
    glDepthFunc( func );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDepthFunc" );
#endif
}


// void glDepthMask (GLboolean flag);
static inline void qglDepthMask( GLboolean flag )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDepthMask(flag=%u)\n", flag );
#endif
    glDepthMask( flag );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDepthMask" );
#endif
}


// void glDepthRangex (GLclampx zNear, GLclampx zFar);
static inline void qglDepthRangex( GLclampx zNear, GLclampx zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDepthRangex(zNear=%ld, zFar=%ld)\n", zNear, zFar );
#endif
    glDepthRangex( zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDepthRangex" );
#endif
}


// void glDisable (GLenum cap);
static inline void qglDisable( GLenum cap )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDisable(cap=%lu)\n", cap );
#endif
    glDisable( cap );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDisable" );
#endif
}


// void glDisableClientState (GLenum array);
static inline void qglDisableClientState( GLenum array )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDisableClientState(array=%lu)\n", array );
#endif
    glDisableClientState( array );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDisableClientState" );
#endif
}


// void glDrawArrays (GLenum mode, GLint first, GLsizei count);
static inline void qglDrawArrays( GLenum mode, GLint first, GLsizei count )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDrawArrays(mode=%lu, first=%ld, count=%ld)\n", mode, first, count );
#endif
    glDrawArrays( mode, first, count );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDrawArrays" );
#endif
}


// void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
static inline void qglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glDrawElements(mode=%lu, count=%ld, type=%lu, indices=%p)\n", mode, count, type, indices );
#endif
    glDrawElements( mode, count, type, indices );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glDrawElements" );
#endif
}


// void glEnable (GLenum cap);
static inline void qglEnable( GLenum cap )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glEnable(cap=%lu)\n", cap );
#endif
    glEnable( cap );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glEnable" );
#endif
}


// void glEnableClientState (GLenum array);
static inline void qglEnableClientState( GLenum array )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glEnableClientState(array=%lu)\n", array );
#endif
    glEnableClientState( array );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glEnableClientState" );
#endif
}


// void glFinish (void);
static inline void qglFinish( void )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFlush = glFinish(void)\n" );
#endif
    glFinish();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFinish" );
#endif
}


// void glFlush (void);
static inline void qglFlush( void )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFlush(void)\n" );
#endif
    glFlush();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFlush" );
#endif
}


// void glFogx (GLenum pname, GLfixed param);
static inline void qglFogx( GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFogx(pname=%lu, param=%ld)\n", pname, param );
#endif
    glFogx( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFogx" );
#endif
}


// void glFogxv (GLenum pname, const GLfixed *params);
static inline void qglFogxv( GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFogxv(pname=%lu, params=%p)\n", pname, params );
#endif
    glFogxv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFogxv" );
#endif
}


// void glFrontFace (GLenum mode);
static inline void qglFrontFace( GLenum mode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFrontFace(mode=%lu)\n", mode );
#endif
    glFrontFace( mode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFrontFace" );
#endif
}


// void glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
static inline void qglFrustumx( GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glFrustumx(left=%ld, right=%ld, bottom=%ld, top=%ld, zNear=%ld, zFar=%ld)\n", left, right, bottom, top, zNear, zFar );
#endif
    glFrustumx( left, right, bottom, top, zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glFrustumx" );
#endif
}


// void glGetBooleanv (GLenum pname, GLboolean *params);
static inline void qglGetBooleanv( GLenum pname, GLboolean* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetBooleanv(pname=%lu, params=%p)\n", pname, params );
#endif
    glGetBooleanv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetBooleanv" );
#endif
}


// void glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);
static inline void qglGetBufferParameteriv( GLenum target, GLenum pname, GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetBufferParameteriv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glGetBufferParameteriv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetBufferParameteriv" );
#endif
}


// void glGetClipPlanex (GLenum pname, GLfixed eqn[4]);
static inline void qglGetClipPlanex( GLenum pname, GLfixed eqn[4] )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetClipPlanex(pname=%lu, eqn=%ld)\n", pname, eqn );
#endif
    glGetClipPlanex( pname, eqn );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetClipPlanex" );
#endif
}


// void glGenBuffers (GLsizei n, GLuint *buffers);
static inline void qglGenBuffers( GLsizei n, GLuint* buffers )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGenBuffers(n=%ld, buffers=%p)\n", n, buffers );
#endif
    glGenBuffers( n, buffers );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGenBuffers" );
#endif
}


// void glGenTextures (GLsizei n, GLuint *textures);
static inline void qglGenTextures( GLsizei n, GLuint* textures )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGenTextures(n=%ld, textures=%p)\n", n, textures );
#endif
    glGenTextures( n, textures );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGenTextures" );
#endif
}


// GLenum glGetError (void);
static inline GLenum qglGetError( void )
{
    GLenum returnValue;
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetError(void)\n" );
#endif
    returnValue = glGetError();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetError" );
#endif
    return returnValue;
}


// void glGetFixedv (GLenum pname, GLfixed *params);
static inline void qglGetFixedv( GLenum pname, GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetFixedv(pname=%lu, params=%p)\n", pname, params );
#endif
    glGetFixedv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetFixedv" );
#endif
}


// void glGetIntegerv (GLenum pname, GLint *params);
static inline void qglGetIntegerv( GLenum pname, GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetIntegerv(pname=%lu, params=%p)\n", pname, params );
#endif
    glGetIntegerv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetIntegerv" );
#endif
}


// void glGetLightxv (GLenum light, GLenum pname, GLfixed *params);
static inline void qglGetLightxv( GLenum light, GLenum pname, GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetLightxv(light=%lu, pname=%lu, params=%p)\n", light, pname, params );
#endif
    glGetLightxv( light, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetLightxv" );
#endif
}


// void glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params);
static inline void qglGetMaterialxv( GLenum face, GLenum pname, GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetMaterialxv(face=%lu, pname=%lu, params=%p)\n", face, pname, params );
#endif
    glGetMaterialxv( face, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetMaterialxv" );
#endif
}


// void glGetPointerv (GLenum pname, void **params);
static inline void qglGetPointerv( GLenum pname, void** params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetPointerv(pname=%lu, params=%p)\n", pname, params );
#endif
    glGetPointerv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetPointerv" );
#endif
}


// const GLubyte * glGetString (GLenum name);
static inline const GLubyte* qglGetString( GLenum name )
{
    const GLubyte* returnValue;
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetString(name=%lu)\n", name );
#endif
    returnValue = glGetString( name );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetString" );
#endif
    return returnValue;
}


// void glGetTexEnviv (GLenum env, GLenum pname, GLint *params);
static inline void qglGetTexEnviv( GLenum env, GLenum pname, GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexEnviv(env=%lu, pname=%lu, params=%p)\n", env, pname, params );
#endif
    glGetTexEnviv( env, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexEnviv" );
#endif
}


// void glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params);
static inline void qglGetTexEnvxv( GLenum env, GLenum pname, GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexEnvxv(env=%lu, pname=%lu, params=%p)\n", env, pname, params );
#endif
    glGetTexEnvxv( env, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexEnvxv" );
#endif
}


// void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
static inline void qglGetTexParameteriv( GLenum target, GLenum pname, GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexParameteriv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glGetTexParameteriv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexParameteriv" );
#endif
}


// void glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params);
static inline void qglGetTexParameterxv( GLenum target, GLenum pname, GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glGetTexParameterxv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glGetTexParameterxv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glGetTexParameterxv" );
#endif
}


// void glHint (GLenum target, GLenum mode);
static inline void qglHint( GLenum target, GLenum mode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glHint(target=%lu, mode=%lu)\n", target, mode );
#endif
    glHint( target, mode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glHint" );
#endif
}


// GLboolean glIsBuffer (GLuint buffer);
static inline GLboolean qglIsBuffer( GLuint buffer )
{
    GLboolean returnValue;
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glIsBuffer(buffer=%lu)\n", buffer );
#endif
    returnValue = glIsBuffer( buffer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glIsBuffer" );
#endif
    return returnValue;
}


// GLboolean glIsEnabled (GLenum cap);
static inline GLboolean qglIsEnabled( GLenum cap )
{
    GLboolean returnValue;
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glIsEnabled(cap=%lu)\n", cap );
#endif
    returnValue = glIsEnabled( cap );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glIsEnabled" );
#endif
    return returnValue;
}


// GLboolean glIsTexture (GLuint texture);
static inline GLboolean qglIsTexture( GLuint texture )
{
    GLboolean returnValue;
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glIsTexture(texture=%lu)\n", texture );
#endif
    returnValue = glIsTexture( texture );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glIsTexture" );
#endif
    return returnValue;
}


// void glLightModelx (GLenum pname, GLfixed param);
static inline void qglLightModelx( GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightModelx(pname=%lu, param=%ld)\n", pname, param );
#endif
    glLightModelx( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightModelx" );
#endif
}


// void glLightModelxv (GLenum pname, const GLfixed *params);
static inline void qglLightModelxv( GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightModelxv(pname=%lu, params=%p)\n", pname, params );
#endif
    glLightModelxv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightModelxv" );
#endif
}


// void glLightx (GLenum light, GLenum pname, GLfixed param);
static inline void qglLightx( GLenum light, GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightx(light=%lu, pname=%lu, param=%ld)\n", light, pname, param );
#endif
    glLightx( light, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightx" );
#endif
}


// void glLightxv (GLenum light, GLenum pname, const GLfixed *params);
static inline void qglLightxv( GLenum light, GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLightxv(light=%lu, pname=%lu, params=%p)\n", light, pname, params );
#endif
    glLightxv( light, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLightxv" );
#endif
}


// void glLineWidthx (GLfixed width);
static inline void qglLineWidthx( GLfixed width )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLineWidthx(width=%ld)\n", width );
#endif
    glLineWidthx( width );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLineWidthx" );
#endif
}


// void glLoadIdentity (void);
static inline void qglLoadIdentity( void )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLoadIdentity(void)\n" );
#endif
    glLoadIdentity();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLoadIdentity" );
#endif
}


// void glLoadMatrixx (const GLfixed *m);
static inline void qglLoadMatrixx( const GLfixed* m )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLoadMatrixx(m=%p)\n", m );
#endif
    glLoadMatrixx( m );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLoadMatrixx" );
#endif
}


// void glLogicOp (GLenum opcode);
static inline void qglLogicOp( GLenum opcode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glLogicOp(opcode=%lu)\n", opcode );
#endif
    glLogicOp( opcode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glLogicOp" );
#endif
}


// void glMaterialx (GLenum face, GLenum pname, GLfixed param);
static inline void qglMaterialx( GLenum face, GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMaterialx(face=%lu, pname=%lu, param=%ld)\n", face, pname, param );
#endif
    glMaterialx( face, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMaterialx" );
#endif
}


// void glMaterialxv (GLenum face, GLenum pname, const GLfixed *params);
static inline void qglMaterialxv( GLenum face, GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMaterialxv(face=%lu, pname=%lu, params=%p)\n", face, pname, params );
#endif
    glMaterialxv( face, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMaterialxv" );
#endif
}


// void glMatrixMode (GLenum mode);
static inline void qglMatrixMode( GLenum mode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMatrixMode(mode=%lu)\n", mode );
#endif
    glMatrixMode( mode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMatrixMode" );
#endif
}


// void glMultMatrixx (const GLfixed *m);
static inline void qglMultMatrixx( const GLfixed* m )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMultMatrixx(m=%p)\n", m );
#endif
    glMultMatrixx( m );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMultMatrixx" );
#endif
}


// void glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
static inline void qglMultiTexCoord4x( GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glMultiTexCoord4x(target=%lu, s=%ld, t=%ld, r=%ld, q=%ld)\n", target, s, t, r, q );
#endif
    glMultiTexCoord4x( target, s, t, r, q );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glMultiTexCoord4x" );
#endif
}


// void glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz);
static inline void qglNormal3x( GLfixed nx, GLfixed ny, GLfixed nz )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glNormal3x(nx=%ld, ny=%ld, nz=%ld)\n", nx, ny, nz );
#endif
    glNormal3x( nx, ny, nz );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glNormal3x" );
#endif
}


// void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
static inline void qglNormalPointer( GLenum type, GLsizei stride, const GLvoid* pointer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glNormalPointer(type=%lu, stride=%ld, pointer=%p)\n", type, stride, pointer );
#endif
    glNormalPointer( type, stride, pointer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glNormalPointer" );
#endif
}


// void glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
static inline void qglOrthox( GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glOrthox(left=%ld, right=%ld, bottom=%ld, top=%ld, zNear=%ld, zFar=%ld)\n", left, right, bottom, top, zNear, zFar );
#endif
    glOrthox( left, right, bottom, top, zNear, zFar );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glOrthox" );
#endif
}


// void glPixelStorei (GLenum pname, GLint param);
static inline void qglPixelStorei( GLenum pname, GLint param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPixelStorei(pname=%lu, param=%ld)\n", pname, param );
#endif
    glPixelStorei( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPixelStorei" );
#endif
}


// void glPointParameterx (GLenum pname, GLfixed param);
static inline void qglPointParameterx( GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointParameterx(pname=%lu, param=%ld)\n", pname, param );
#endif
    glPointParameterx( pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointParameterx" );
#endif
}


// void glPointParameterxv (GLenum pname, const GLfixed *params);
static inline void qglPointParameterxv( GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointParameterxv(pname=%lu, params=%p)\n", pname, params );
#endif
    glPointParameterxv( pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointParameterxv" );
#endif
}


// void glPointSizex (GLfixed size);
static inline void qglPointSizex( GLfixed size )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointSizex(size=%ld)\n", size );
#endif
    glPointSizex( size );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointSizex" );
#endif
}


// void glPolygonOffsetx (GLfixed factor, GLfixed units);
static inline void qglPolygonOffsetx( GLfixed factor, GLfixed units )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPolygonOffsetx(factor=%ld, units=%ld)\n", factor, units );
#endif
    glPolygonOffsetx( factor, units );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPolygonOffsetx" );
#endif
}


// void glPopMatrix (void);
static inline void qglPopMatrix( void )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPopMatrix(void)\n" );
#endif
    glPopMatrix();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPopMatrix" );
#endif
}


// void glPushMatrix (void);
static inline void qglPushMatrix( void )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPushMatrix(void)\n" );
#endif
    glPushMatrix();
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPushMatrix" );
#endif
}


// void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
static inline void qglReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glReadPixels(x=%ld, y=%ld, width=%ld, height=%ld, format=%lu, type=%lu, pixels=%p)\n", x, y, width, height, format, type, pixels );
#endif
    glReadPixels( x, y, width, height, format, type, pixels );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glReadPixels" );
#endif
}


// void glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
static inline void qglRotatex( GLfixed angle, GLfixed x, GLfixed y, GLfixed z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glRotatex(angle=%ld, x=%ld, y=%ld, z=%ld)\n", angle, x, y, z );
#endif
    glRotatex( angle, x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glRotatex" );
#endif
}


// void glSampleCoverage (GLclampf value, GLboolean invert);
static inline void qglSampleCoverage( GLclampf value, GLboolean invert )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glSampleCoverage(value=%f, invert=%u)\n", value, invert );
#endif
    glSampleCoverage( value, invert );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glSampleCoverage" );
#endif
}


// void glSampleCoveragex (GLclampx value, GLboolean invert);
static inline void qglSampleCoveragex( GLclampx value, GLboolean invert )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glSampleCoveragex(value=%ld, invert=%u)\n", value, invert );
#endif
    glSampleCoveragex( value, invert );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glSampleCoveragex" );
#endif
}


// void glScalex (GLfixed x, GLfixed y, GLfixed z);
static inline void qglScalex( GLfixed x, GLfixed y, GLfixed z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glScalex(x=%ld, y=%ld, z=%ld)\n", x, y, z );
#endif
    glScalex( x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glScalex" );
#endif
}


// void glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
static inline void qglScissor( GLint x, GLint y, GLsizei width, GLsizei height )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glScissor(x=%ld, y=%ld, width=%ld, height=%ld)\n", x, y, width, height );
#endif
    glScissor( x, y, width, height );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glScissor" );
#endif
}


// void glShadeModel (GLenum mode);
static inline void qglShadeModel( GLenum mode )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glShadeModel(mode=%lu)\n", mode );
#endif
    glShadeModel( mode );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glShadeModel" );
#endif
}


// void glStencilFunc (GLenum func, GLint ref, GLuint mask);
static inline void qglStencilFunc( GLenum func, GLint ref, GLuint mask )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glStencilFunc(func=%lu, ref=%ld, mask=%lu)\n", func, ref, mask );
#endif
    glStencilFunc( func, ref, mask );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glStencilFunc" );
#endif
}


// void glStencilMask (GLuint mask);
static inline void qglStencilMask( GLuint mask )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glStencilMask(mask=%lu)\n", mask );
#endif
    glStencilMask( mask );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glStencilMask" );
#endif
}


// void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
static inline void qglStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glStencilOp(fail=%lu, zfail=%lu, zpass=%lu)\n", fail, zfail, zpass );
#endif
    glStencilOp( fail, zfail, zpass );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glStencilOp" );
#endif
}


// void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static inline void qglTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexCoordPointer(size=%ld, type=%lu, stride=%ld, pointer=%p)\n", size, type, stride, pointer );
#endif
    glTexCoordPointer( size, type, stride, pointer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexCoordPointer" );
#endif
}


// void glTexEnvi (GLenum target, GLenum pname, GLint param);
static inline void qglTexEnvi( GLenum target, GLenum pname, GLint param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnvi(target=%lu, pname=%lu, param=%ld)\n", target, pname, param );
#endif
    glTexEnvi( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnvi" );
#endif
}


// void glTexEnvx (GLenum target, GLenum pname, GLfixed param);
static inline void qglTexEnvx( GLenum target, GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnvx(target=%lu, pname=%lu, param=%ld)\n", target, pname, param );
#endif
    glTexEnvx( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnvx" );
#endif
}


// void glTexEnviv (GLenum target, GLenum pname, const GLint *params);
static inline void qglTexEnviv( GLenum target, GLenum pname, const GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnviv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexEnviv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnviv" );
#endif
}


// void glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params);
static inline void qglTexEnvxv( GLenum target, GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexEnvxv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexEnvxv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexEnvxv" );
#endif
}

//We aren't using Android version of glTexImage2D, because we don't have any performance gain using it.
static inline void qglTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexImage2D(target=%lu, level=%ld, internalformat=%ld, width=%ld, height=%ld, border=%ld, format=%lu, type=%lu, pixels=%p)\n", target, level, internalformat, width, height, border, format, type, pixels );
#endif
    glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexImage2D" );
#endif
}


// void glTexParameteri (GLenum target, GLenum pname, GLint param);
static inline void qglTexParameteri( GLenum target, GLenum pname, GLint param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameteri(target=%lu, pname=%lu, param=%ld)\n", target, pname, param );
#endif
    glTexParameteri( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameteri" );
#endif
}


// void glTexParameterx (GLenum target, GLenum pname, GLfixed param);
static inline void qglTexParameterx( GLenum target, GLenum pname, GLfixed param )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameterx(target=%lu, pname=%lu, param=%ld)\n", target, pname, param );
#endif
    glTexParameterx( target, pname, param );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameterx" );
#endif
}


// void glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
static inline void qglTexParameteriv( GLenum target, GLenum pname, const GLint* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameteriv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexParameteriv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameteriv" );
#endif
}


// void glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params);
static inline void qglTexParameterxv( GLenum target, GLenum pname, const GLfixed* params )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexParameterxv(target=%lu, pname=%lu, params=%p)\n", target, pname, params );
#endif
    glTexParameterxv( target, pname, params );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexParameterxv" );
#endif
}


// void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
static inline void qglTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTexSubImage2D(target=%lu, level=%ld, xoffset=%ld, yoffset=%ld, width=%ld, height=%ld, format=%lu, type=%lu, pixels=%p)\n", target, level, xoffset, yoffset, width, height, format, type, pixels );
#endif
    glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTexSubImage2D" );
#endif
}


// void glTranslatex (GLfixed x, GLfixed y, GLfixed z);
static inline void qglTranslatex( GLfixed x, GLfixed y, GLfixed z )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glTranslatex(x=%ld, y=%ld, z=%ld)\n", x, y, z );
#endif
    glTranslatex( x, y, z );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glTranslatex" );
#endif
}

// void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static inline void qglVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glVertexPointer(size=%ld, type=%lu, stride=%ld, pointer=%p)\n", size, type, stride, pointer );
#endif
    glVertexPointer( size, type, stride, pointer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glVertexPointer" );
#endif
}


// void glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
static inline void qglViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glViewport(x=%ld, y=%ld, width=%ld, height=%ld)\n", x, y, width, height );
#endif
    glViewport( x, y, width, height );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glViewport" );
#endif
}


// void glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer);
static inline void qglPointSizePointerOES( GLenum type, GLsizei stride, const GLvoid* pointer )
{
#if !defined(NDEBUG) && defined(QGL_LOG_GL_CALLS)
    if( QGLLogGLCalls )
        fprintf( QGLDebugFile(), "glPointSizePointerOES(type=%lu, stride=%ld, pointer=%p)\n", type, stride, pointer );
#endif
    glPointSizePointerOES( type, stride, pointer );
#if !defined(NDEBUG) && defined(QGL_CHECK_GL_ERRORS)
    if( !QGLBeginStarted )
        QGLCheckError( "glPointSizePointerOES" );
#endif
}

// Prevent calls to the 'normal' GL functions
#define glAlphaFunc CALL_THE_QGL_VERSION_OF_glAlphaFunc
#define glClearColor CALL_THE_QGL_VERSION_OF_glClearColor
#define glClearDepthf CALL_THE_QGL_VERSION_OF_glClearDepthf
#define glClipPlanef CALL_THE_QGL_VERSION_OF_glClipPlanef
#define glDepthRangef CALL_THE_QGL_VERSION_OF_glDepthRangef
#define glFogf CALL_THE_QGL_VERSION_OF_glFogf
#define glFogfv CALL_THE_QGL_VERSION_OF_glFogfv
#define glFrustumf CALL_THE_QGL_VERSION_OF_glFrustumf
#define glGetClipPlanef CALL_THE_QGL_VERSION_OF_glGetClipPlanef
#define glGetFloatv CALL_THE_QGL_VERSION_OF_glGetFloatv
#define glGetLightfv CALL_THE_QGL_VERSION_OF_glGetLightfv
#define glGetMaterialfv CALL_THE_QGL_VERSION_OF_glGetMaterialfv
#define glGetTexEnvfv CALL_THE_QGL_VERSION_OF_glGetTexEnvfv
#define glGetTexParameterfv CALL_THE_QGL_VERSION_OF_glGetTexParameterfv
#define glLightModelf CALL_THE_QGL_VERSION_OF_glLightModelf
#define glLightModelfv CALL_THE_QGL_VERSION_OF_glLightModelfv
#define glLightf CALL_THE_QGL_VERSION_OF_glLightf
#define glLightfv CALL_THE_QGL_VERSION_OF_glLightfv
#define glLineWidth CALL_THE_QGL_VERSION_OF_glLineWidth
#define glLoadMatrixf CALL_THE_QGL_VERSION_OF_glLoadMatrixf
#define glMaterialf CALL_THE_QGL_VERSION_OF_glMaterialf
#define glMaterialfv CALL_THE_QGL_VERSION_OF_glMaterialfv
#define glMultMatrixf CALL_THE_QGL_VERSION_OF_glMultMatrixf
#define glMultiTexCoord4f CALL_THE_QGL_VERSION_OF_glMultiTexCoord4f
#define glNormal3f CALL_THE_QGL_VERSION_OF_glNormal3f
#define glOrthof CALL_THE_QGL_VERSION_OF_glOrthof
#define glPointParameterf CALL_THE_QGL_VERSION_OF_glPointParameterf
#define glPointParameterfv CALL_THE_QGL_VERSION_OF_glPointParameterfv
#define glPointSize CALL_THE_QGL_VERSION_OF_glPointSize
#define glPolygonOffset CALL_THE_QGL_VERSION_OF_glPolygonOffset
#define glRotatef CALL_THE_QGL_VERSION_OF_glRotatef
#define glScalef CALL_THE_QGL_VERSION_OF_glScalef
#define glTexEnvf CALL_THE_QGL_VERSION_OF_glTexEnvf
#define glTexEnvfv CALL_THE_QGL_VERSION_OF_glTexEnvfv
#define glTexParameterf CALL_THE_QGL_VERSION_OF_glTexParameterf
#define glTexParameterfv CALL_THE_QGL_VERSION_OF_glTexParameterfv
#define glTranslatef CALL_THE_QGL_VERSION_OF_glTranslatef
#define glActiveTexture CALL_THE_QGL_VERSION_OF_glActiveTexture
#define glAlphaFuncx CALL_THE_QGL_VERSION_OF_glAlphaFuncx
#define glBindBuffer CALL_THE_QGL_VERSION_OF_glBindBuffer
#define glBindTexture CALL_THE_QGL_VERSION_OF_glBindTexture
#define glBlendFunc CALL_THE_QGL_VERSION_OF_glBlendFunc
#define glBufferData CALL_THE_QGL_VERSION_OF_glBufferData
#define glBufferSubData CALL_THE_QGL_VERSION_OF_glBufferSubData
#define glClear CALL_THE_QGL_VERSION_OF_glClear
#define glClearColorx CALL_THE_QGL_VERSION_OF_glClearColorx
#define glClearDepthx CALL_THE_QGL_VERSION_OF_glClearDepthx
#define glClearStencil CALL_THE_QGL_VERSION_OF_glClearStencil
#define glClientActiveTexture CALL_THE_QGL_VERSION_OF_glClientActiveTexture
#define glClipPlanex CALL_THE_QGL_VERSION_OF_glClipPlanex
#define glColor4ub CALL_THE_QGL_VERSION_OF_glColor4ub
#define glColor4x CALL_THE_QGL_VERSION_OF_glColor4x
#define glColorMask CALL_THE_QGL_VERSION_OF_glColorMask
#define glColorPointer CALL_THE_QGL_VERSION_OF_glColorPointer
#define glCompressedTexImage2D CALL_THE_QGL_VERSION_OF_glCompressedTexImage2D
#define glCompressedTexSubImage2D CALL_THE_QGL_VERSION_OF_glCompressedTexSubImage2D
#define glCopyTexImage2D CALL_THE_QGL_VERSION_OF_glCopyTexImage2D
#define glCopyTexSubImage2D CALL_THE_QGL_VERSION_OF_glCopyTexSubImage2D
#define glCullFace CALL_THE_QGL_VERSION_OF_glCullFace
#define glDeleteBuffers CALL_THE_QGL_VERSION_OF_glDeleteBuffers
#define glDeleteTextures CALL_THE_QGL_VERSION_OF_glDeleteTextures
#define glDepthFunc CALL_THE_QGL_VERSION_OF_glDepthFunc
#define glDepthMask CALL_THE_QGL_VERSION_OF_glDepthMask
#define glDepthRangex CALL_THE_QGL_VERSION_OF_glDepthRangex
#define glDisable CALL_THE_QGL_VERSION_OF_glDisable
#define glDisableClientState CALL_THE_QGL_VERSION_OF_glDisableClientState
#define glDrawArrays CALL_THE_QGL_VERSION_OF_glDrawArrays
#define glDrawElements CALL_THE_QGL_VERSION_OF_glDrawElements
#define glEnable CALL_THE_QGL_VERSION_OF_glEnable
#define glEnableClientState CALL_THE_QGL_VERSION_OF_glEnableClientState
#define glFinish CALL_THE_QGL_VERSION_OF_glFinish
#define glFlush CALL_THE_QGL_VERSION_OF_glFlush
#define glFogx CALL_THE_QGL_VERSION_OF_glFogx
#define glFogxv CALL_THE_QGL_VERSION_OF_glFogxv
#define glFrontFace CALL_THE_QGL_VERSION_OF_glFrontFace
#define glFrustumx CALL_THE_QGL_VERSION_OF_glFrustumx
#define glGetBooleanv CALL_THE_QGL_VERSION_OF_glGetBooleanv
#define glGetBufferParameteriv CALL_THE_QGL_VERSION_OF_glGetBufferParameteriv
#define glGetClipPlanex CALL_THE_QGL_VERSION_OF_glGetClipPlanex
#define glGenBuffers CALL_THE_QGL_VERSION_OF_glGenBuffers
#define glGenTextures CALL_THE_QGL_VERSION_OF_glGenTextures
#define glGetError CALL_THE_QGL_VERSION_OF_glGetError
#define glGetFixedv CALL_THE_QGL_VERSION_OF_glGetFixedv
#define glGetIntegerv CALL_THE_QGL_VERSION_OF_glGetIntegerv
#define glGetLightxv CALL_THE_QGL_VERSION_OF_glGetLightxv
#define glGetMaterialxv CALL_THE_QGL_VERSION_OF_glGetMaterialxv
#define glGetPointerv CALL_THE_QGL_VERSION_OF_glGetPointerv
#define glGetString CALL_THE_QGL_VERSION_OF_glGetString
#define glGetTexEnviv CALL_THE_QGL_VERSION_OF_glGetTexEnviv
#define glGetTexEnvxv CALL_THE_QGL_VERSION_OF_glGetTexEnvxv
#define glGetTexParameteriv CALL_THE_QGL_VERSION_OF_glGetTexParameteriv
#define glGetTexParameterxv CALL_THE_QGL_VERSION_OF_glGetTexParameterxv
#define glHint CALL_THE_QGL_VERSION_OF_glHint
#define glIsBuffer CALL_THE_QGL_VERSION_OF_glIsBuffer
#define glIsEnabled CALL_THE_QGL_VERSION_OF_glIsEnabled
#define glIsTexture CALL_THE_QGL_VERSION_OF_glIsTexture
#define glLightModelx CALL_THE_QGL_VERSION_OF_glLightModelx
#define glLightModelxv CALL_THE_QGL_VERSION_OF_glLightModelxv
#define glLightx CALL_THE_QGL_VERSION_OF_glLightx
#define glLightxv CALL_THE_QGL_VERSION_OF_glLightxv
#define glLineWidthx CALL_THE_QGL_VERSION_OF_glLineWidthx
#define glLoadIdentity CALL_THE_QGL_VERSION_OF_glLoadIdentity
#define glLoadMatrixx CALL_THE_QGL_VERSION_OF_glLoadMatrixx
#define glLogicOp CALL_THE_QGL_VERSION_OF_glLogicOp
#define glMaterialx CALL_THE_QGL_VERSION_OF_glMaterialx
#define glMaterialxv CALL_THE_QGL_VERSION_OF_glMaterialxv
#define glMatrixMode CALL_THE_QGL_VERSION_OF_glMatrixMode
#define glMultMatrixx CALL_THE_QGL_VERSION_OF_glMultMatrixx
#define glMultiTexCoord4x CALL_THE_QGL_VERSION_OF_glMultiTexCoord4x
#define glNormal3x CALL_THE_QGL_VERSION_OF_glNormal3x
#define glNormalPointer CALL_THE_QGL_VERSION_OF_glNormalPointer
#define glOrthox CALL_THE_QGL_VERSION_OF_glOrthox
#define glPixelStorei CALL_THE_QGL_VERSION_OF_glPixelStorei
#define glPointParameterx CALL_THE_QGL_VERSION_OF_glPointParameterx
#define glPointParameterxv CALL_THE_QGL_VERSION_OF_glPointParameterxv
#define glPointSizex CALL_THE_QGL_VERSION_OF_glPointSizex
#define glPolygonOffsetx CALL_THE_QGL_VERSION_OF_glPolygonOffsetx
#define glPopMatrix CALL_THE_QGL_VERSION_OF_glPopMatrix
#define glPushMatrix CALL_THE_QGL_VERSION_OF_glPushMatrix
#define glReadPixels CALL_THE_QGL_VERSION_OF_glReadPixels
#define glRotatex CALL_THE_QGL_VERSION_OF_glRotatex
#define glSampleCoverage CALL_THE_QGL_VERSION_OF_glSampleCoverage
#define glSampleCoveragex CALL_THE_QGL_VERSION_OF_glSampleCoveragex
#define glScalex CALL_THE_QGL_VERSION_OF_glScalex
#define glScissor CALL_THE_QGL_VERSION_OF_glScissor
#define glShadeModel CALL_THE_QGL_VERSION_OF_glShadeModel
#define glStencilFunc CALL_THE_QGL_VERSION_OF_glStencilFunc
#define glStencilMask CALL_THE_QGL_VERSION_OF_glStencilMask
#define glStencilOp CALL_THE_QGL_VERSION_OF_glStencilOp
#define glTexCoordPointer CALL_THE_QGL_VERSION_OF_glTexCoordPointer
#define glTexEnvi CALL_THE_QGL_VERSION_OF_glTexEnvi
#define glTexEnvx CALL_THE_QGL_VERSION_OF_glTexEnvx
#define glTexEnviv CALL_THE_QGL_VERSION_OF_glTexEnviv
#define glTexEnvxv CALL_THE_QGL_VERSION_OF_glTexEnvxv
#define glTexImage2D CALL_THE_QGL_VERSION_OF_glTexImage2D
#define glTexParameteri CALL_THE_QGL_VERSION_OF_glTexParameteri
#define glTexParameterx CALL_THE_QGL_VERSION_OF_glTexParameterx
#define glTexParameteriv CALL_THE_QGL_VERSION_OF_glTexParameteriv
#define glTexParameterxv CALL_THE_QGL_VERSION_OF_glTexParameterxv
#define glTexSubImage2D CALL_THE_QGL_VERSION_OF_glTexSubImage2D
#define glTranslatex CALL_THE_QGL_VERSION_OF_glTranslatex
#define glVertexPointer CALL_THE_QGL_VERSION_OF_glVertexPointer
#define glViewport CALL_THE_QGL_VERSION_OF_glViewport
#define glPointSizePointerOES CALL_THE_QGL_VERSION_OF_glPointSizePointerOES

#endif //!__QGL_H__
