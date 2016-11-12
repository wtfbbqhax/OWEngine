//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2009  Nokia Corporation.  All rights reserved.
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
//  File name:   android_glimp.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __ANDROID_GLIMP_H__
#define __ANDROID_GLIMP_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include <GLES/gl.h>

#include "android_input.h"

#ifndef GLAPI
#define GLAPI extern
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#ifndef APIENTRY
#define APIENTRY GLAPIENTRY
#endif

/* "P" suffix to be used for a pointer to a function */
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRYP
#define GLAPIENTRYP GLAPIENTRY *
#endif

void GLimp_Init( void );
void GLimp_LogComment( char* comment );
void GLimp_EndFrame( void );
void GLimp_Shutdown( void );
void qglArrayElement( GLint i );
void qglCallList( GLuint list );
void qglDrawBuffer( GLenum mode );
void qglLockArrays( GLint i, GLsizei size );
void qglUnlockArrays( void );
void GLimp_SetGamma( unsigned char red[256], unsigned char green[256],
                     unsigned char blue[256] );
qboolean GLimp_SpawnRenderThread( void ( *function )( void ) );
void GLimp_FrontEndSleep( void );
void* GLimp_RendererSleep( void );
void* GLimp_RenderThreadWrapper( void* stub );
void GLimp_WakeRenderer( void* data );

#endif

