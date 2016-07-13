//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
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
//  File name:   android_glimp.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <stdbool.h>
#include <EGL/egl.h>

#include "android_glimp.h"
#include "../../client/client.h"
#include "../../renderer/tr_local.h"

static int screen_width = 640;
static int screen_height = 480;

static const char* Q_stristr( const char* s, const char* find )
{
    register char c, sc;
    register size_t len;
    
    if( ( c = *find++ ) != 0 )
    {
        if( c >= 'a' && c <= 'z' )
        {
            c -= ( 'a' - 'A' );
        }
        len = strlen( find );
        do
        {
            do
            {
                if( ( sc = *s++ ) == 0 )
                    return NULL;
                if( sc >= 'a' && sc <= 'z' )
                {
                    sc -= ( 'a' - 'A' );
                }
            }
            while( sc != c );
        }
        while( Q_stricmpn( s, find, len ) != 0 );
        s--;
    }
    return s;
}

static qboolean GLimp_HaveExtension( const char* ext )
{
    const char* ptr = Q_stristr( glConfig.extensions_string, ext );
    if( ptr == NULL )
        return qfalse;
    ptr += strlen( ext );
    return ( ( *ptr == ' ' ) || ( *ptr == '\0' ) ); // verify it's complete string.
}

static void qglMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t )
{
    qglMultiTexCoord4f( target, s, t, 1, 1 );
}

/*
===============
GLimp_InitExtensions
===============
*/
static void GLimp_InitExtensions( void )
{
    if( !r_allowExtensions->integer )
    {
        ri.Printf( PRINT_ALL, "* IGNORING OPENGL EXTENSIONS *\n" );
        return;
    }
    
    ri.Printf( PRINT_ALL, "Initializing OpenGL extensions\n" );
    
    glConfig.textureCompression = TC_NONE;
    
    // GL_EXT_texture_compression_s3tc
//Not supported on Android
#if 0
    if( GLimp_HaveExtension( "GL_EXT_texture_compression_s3tc" ) )
    {
        if( r_ext_compressed_textures->value )
        {
            glConfig.textureCompression = TC_EXT_COMP_S3TC;
            ri.Printf( PRINT_ALL, "...using GL_EXT_texture_compression_s3tc\n" );
        }
        else
        {
            ri.Printf( PRINT_ALL, "...ignoring GL_EXT_texture_compression_s3tc\n" );
        }
    }
    else
    {
        ri.Printf( PRINT_ALL, "...GL_EXT_texture_compression_s3tc not found\n" );
    }
    
    // GL_S3_s3tc ... legacy extension before GL_EXT_texture_compression_s3tc.
    if( glConfig.textureCompression == TC_NONE )
    {
        if( GLimp_HaveExtension( "GL_S3_s3tc" ) )
        {
            if( r_ext_compressed_textures->value )
            {
                glConfig.textureCompression = TC_S3TC;
                ri.Printf( PRINT_ALL, "...using GL_S3_s3tc\n" );
            }
            else
            {
                ri.Printf( PRINT_ALL, "...ignoring GL_S3_s3tc\n" );
            }
        }
        else
        {
            ri.Printf( PRINT_ALL, "...GL_S3_s3tc not found\n" );
        }
    }
#endif
    
    // GL_EXT_texture_env_add
    glConfig.textureEnvAddAvailable = qtrue;
    {
        if( r_ext_multitexture->value )
        {
            qglMultiTexCoord2fARB = qglMultiTexCoord2f;
            qglActiveTextureARB = qglActiveTexture;
            qglClientActiveTextureARB = qglClientActiveTexture;
            
            if( qglActiveTextureARB )
            {
                GLint glint = 0;
                qglGetIntegerv( GL_MAX_TEXTURE_UNITS, &glint );
                glConfig.maxActiveTextures = ( int ) glint;
                if( glConfig.maxActiveTextures > 1 )
                {
                    ri.Printf( PRINT_ALL, "...using GL_ARB_multitexture\n" );
                }
                else
                {
                    qglMultiTexCoord2fARB = NULL;
                    qglActiveTextureARB = NULL;
                    qglClientActiveTextureARB = NULL;
                    ri.Printf( PRINT_ALL, "...not using GL_ARB_multitexture, < 2 texture units\n" );
                }
            }
        }
        else
        {
            ri.Printf( PRINT_ALL, "...ignoring GL_ARB_multitexture\n" );
        }
    }
    
    glConfig.textureFilterAnisotropicAvailable = qfalse;
}

int malimode = 0;

void GLimp_Init( void )
{
    ri.Printf( PRINT_ALL, "Initializing OpenGL subsystem\n" );
    
    bzero( &glConfig, sizeof( glConfig ) );
    
    glConfig.isFullscreen = r_fullscreen->integer;
    glConfig.vidWidth = screen_width;
    glConfig.vidHeight = screen_height;
    glConfig.windowAspect = ( float )glConfig.vidWidth / glConfig.vidHeight;
    // FIXME
    glConfig.colorBits = 32;
    glConfig.stencilBits = 8;
    glConfig.depthBits = 16;
    glConfig.textureCompression = TC_NONE;
    
    // This values force the UI to disable driver selection
    glConfig.driverType = GLDRV_ICD;
    glConfig.hardwareType = GLHW_GENERIC;
    
    Q_strncpyz( glConfig.vendor_string,
                ( const char* )qglGetString( GL_VENDOR ),
                sizeof( glConfig.vendor_string ) );
    Q_strncpyz( glConfig.renderer_string,
                ( const char* )qglGetString( GL_RENDERER ),
                sizeof( glConfig.renderer_string ) );
                
    const char* ptr = Q_stristr( glConfig.renderer_string, "Mali-" );
    if( ptr != NULL )
        malimode = 1;
        
    Q_strncpyz( glConfig.version_string,
                ( const char* )qglGetString( GL_VERSION ),
                sizeof( glConfig.version_string ) );
    Q_strncpyz( glConfig.extensions_string,
                ( const char* )qglGetString( GL_EXTENSIONS ),
                sizeof( glConfig.extensions_string ) );
                
    qglLockArraysEXT = qglLockArrays;
    qglUnlockArraysEXT = qglUnlockArrays;
    
    GLimp_InitExtensions();
    
    IN_Init( );
    
    ri.Printf( PRINT_ALL, "------------------\n" );
}

void GLimp_LogComment( char* comment )
{
    //fprintf(stderr, "%s: %s\n", __func__, comment);
//      ri.Printf(PRINT_ALL, "%s: %s\n", __func__, comment);
}

bool scndswp = 0;
void GLimp_EndFrame( void )
{
    if( scndswp ) eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );
    scndswp = 1;
}

void GLimp_Shutdown( void )
{
}

void qglArrayElement( GLint i )
{
}

void qglCallList( GLuint list )
{
}

void qglDrawBuffer( GLenum mode )
{
}

void qglLockArrays( GLint i, GLsizei size )
{
}

void qglUnlockArrays( void )
{
}

void GLimp_SetGamma( unsigned char red[256], unsigned char green[256],
                     unsigned char blue[256] )
{
}



/*
===========================================================

SMP acceleration

===========================================================
*/
#include <semaphore.h>
#include <pthread.h>
sem_t renderCommandsEvent;
sem_t renderCompletedEvent;
sem_t renderActiveEvent;

void ( *glimpRenderThread )( void );

void* GLimp_RenderThreadWrapper( void* stub )
{
    glimpRenderThread();
    return NULL;
}


/*
=======================
GLimp_SpawnRenderThread
=======================
*/
pthread_t renderThreadHandle;
qboolean GLimp_SpawnRenderThread( void ( *function )( void ) )
{


    return qfalse;//Android doesn't support it =(
    sem_init( &renderCommandsEvent, 0, 0 );
    sem_init( &renderCompletedEvent, 0, 0 );
    sem_init( &renderActiveEvent, 0, 0 );
    
    glimpRenderThread = function;
    
    if( pthread_create( &renderThreadHandle, NULL,
                        GLimp_RenderThreadWrapper, NULL ) )
    {
        return qfalse;
    }
    
    return qtrue;
}

static void*  smpData;
//static	int		glXErrors; // bk001204 - unused

void* GLimp_RendererSleep( void )
{
    void*  data;
    
    // after this, the front end can exit GLimp_FrontEndSleep
    sem_post( &renderCompletedEvent );
    
    sem_wait( &renderCommandsEvent );
    
    data = smpData;
    
    // after this, the main thread can exit GLimp_WakeRenderer
    sem_post( &renderActiveEvent );
    
    return data;
}


void GLimp_FrontEndSleep( void )
{
    sem_wait( &renderCompletedEvent );
}


void GLimp_WakeRenderer( void* data )
{
    smpData = data;
    
    // after this, the renderer can continue through GLimp_RendererSleep
    sem_post( &renderCommandsEvent );
    
    sem_wait( &renderActiveEvent );
}



void OWE_SetResolution( int width, int height )
{
    screen_width = width;
    screen_height = height;
}

void OWE_OGLRestart()
{
    CL_Vid_Restart_f();
}

