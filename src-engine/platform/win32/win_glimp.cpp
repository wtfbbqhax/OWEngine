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
//  File name:   win_glimp.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: This file contains ALL Win32 specific stuff having to do with the
//               OpenGL refresh.  When a port is being made the following functions
//               must be implemented by the port:
//               GLimp_EndFrame
//               GLimp_Init
//               GLimp_LogComment
//               GLimp_Shutdown
//               Note that the GLW_xxx functions are Windows specific GL-subsystem
//               related functions that are relevant ONLY to win_glimp.c
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <GL/glew.h>
#include "../../renderer/r_local.h"
#include "../../qcommon/qcommon.h"
#include "win_local.h"
#include "resource.h"
#include <GL/wglew.h>
#include "glw_win.h"

extern void WG_CheckHardwareGamma( void );
extern void WG_RestoreGamma( void );

static char*	WinGetLastErrorLocal()
{
    static char buf[4096];
    
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                   buf,
                   4096, NULL );
                   
    return buf;
}

typedef enum
{
    RSERR_OK,
    
    RSERR_INVALID_FULLSCREEN,
    RSERR_INVALID_MODE,
    
    RSERR_UNKNOWN
} rserr_t;

#define TRY_PFD_SUCCESS     0
#define TRY_PFD_FAIL_SOFT   1
#define TRY_PFD_FAIL_HARD   2

#define WINDOW_CLASS_NAME   "Wolfenstein"

static void     GLW_InitExtensions( void );
static rserr_t  GLW_SetMode( int mode,
                             int colorbits,
                             bool cdsFullscreen );

static WinVars_t* g_wvPtr = NULL;

// variable declarations
glwstate_t glw_state;

cvar_t*  r_allowSoftwareGL;     // don't abort out if the pixelformat claims software
cvar_t*  r_maskMinidriver;      // allow a different dll name to be treated as if it were opengl32.dll

/*
** GLW_StartDriverAndSetMode
*/
static bool GLW_StartDriverAndSetMode( int mode,
                                       int colorbits,
                                       bool cdsFullscreen )
{
    rserr_t err;
    
    err = GLW_SetMode( r_mode->integer, colorbits, cdsFullscreen );
    
    switch( err )
    {
        case RSERR_INVALID_FULLSCREEN:
            Com_Printf( "...WARNING: fullscreen unavailable in this mode\n" );
            return false;
        case RSERR_INVALID_MODE:
            Com_Printf( "...WARNING: could not set the given mode (%d)\n", mode );
            return false;
        default:
            break;
    }
    return true;
}

/*
** GLW_InitDriver
**
** - get a DC if one doesn't exist
** - create an HGLRC if one doesn't exist
*/
static bool GLW_InitDriver( int colorbits )
{
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof( PIXELFORMATDESCRIPTOR ), // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW |            // support window
        PFD_SUPPORT_OPENGL |            // support OpenGL
        PFD_DOUBLEBUFFER |              // double buffered
        PFD_STEREO |                    // use STEREO by default
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        24,                             // 24-bit z-buffer
        8,                              // 8-bit stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };
    
    Com_Printf( "Initializing OpenGL driver\n" );
    
    if( glw_state.hDC == NULL )
    {
        Com_Printf( "...getting DC: " );
        
        if( ( glw_state.hDC = GetDC( g_wvPtr->hWnd ) ) == NULL )
        {
            Com_Printf( "^3failed^0\n" );
            return false;
        }
        Com_Printf( "succeeded\n" );
    }
    
    if( colorbits == 0 )
    {
        colorbits = glw_state.desktopBitsPixel;
    }
    
    Com_Printf( "...attempting to use stereo\n" );
    glConfig.stereoEnabled = true;
    
    //
    // choose, set, and describe our desired pixel format.  If we're
    // using a minidriver then we need to bypass the GDI functions,
    // otherwise use the GDI functions.
    //
    if( ( glw_state.pixelformat = ChoosePixelFormat( glw_state.hDC, &pfd ) ) == 0 )
    {
        Com_Printf( "...^3GLW_ChoosePFD failed^0\n" );
        return false;
    }
    Com_Printf( "...PIXELFORMAT %d selected\n", glw_state.pixelformat );
    
    DescribePixelFormat( glw_state.hDC, glw_state.pixelformat, sizeof( pfd ), &pfd );
    
    // get the full info
    DescribePixelFormat( glw_state.hDC, glw_state.pixelformat, sizeof( glw_state.pfd ), &glw_state.pfd );
    glConfig.colorBits = glw_state.pfd.cColorBits;
    glConfig.depthBits = glw_state.pfd.cDepthBits;
    glConfig.stencilBits = glw_state.pfd.cStencilBits;
    
    // XP seems to set this incorrectly
    if( !glConfig.stencilBits )
    {
        glConfig.stencilBits = 8;
    }
    
    // the same SetPixelFormat is used either way
    if( SetPixelFormat( glw_state.hDC, glw_state.pixelformat, &glw_state.pfd ) == FALSE )
    {
        Com_Printf( "...^3SetPixelFormat failed^0\n", glw_state.hDC );
        return false;
    }
    
    //
    // startup the OpenGL subsystem by creating a context and making it current
    //
    Com_Printf( "...creating GL context: " );
    if( ( glw_state.hGLRC = wglCreateContext( glw_state.hDC ) ) == 0 )
    {
        Com_Printf( "^3failed^0\n" );
        return false;
    }
    Com_Printf( "succeeded\n" );
    
    Com_Printf( "...making context current: " );
    if( !wglMakeCurrent( glw_state.hDC, glw_state.hGLRC ) )
    {
        wglDeleteContext( glw_state.hGLRC );
        glw_state.hGLRC = NULL;
        Com_Printf( "^3failed^0\n" );
        return false;
    }
    Com_Printf( "succeeded\n" );
    
    return true;
}

/*
** GLW_CreateWindow
**
** Responsible for creating the Win32 window and initializing the OpenGL driver.
*/
#define WINDOW_STYLE    ( WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE )
static bool GLW_CreateWindow( int width, int height, int colorbits, bool cdsFullscreen )
{
    RECT r;
    cvar_t*          vid_xpos, *vid_ypos;
    int stylebits;
    int x, y, w, h;
    int exstyle;
    
    //
    // register the window class if necessary
    //
    if( !g_wvPtr->classRegistered )
    {
        WNDCLASS wc;
        
        memset( &wc, 0, sizeof( wc ) );
        
        wc.style         = 0;
        wc.lpfnWndProc   = ( WNDPROC ) glw_state.wndproc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = g_wvPtr->hInstance;
        wc.hIcon         = LoadIcon( g_wvPtr->hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = ( HBRUSH )COLOR_GRAYTEXT;
        wc.lpszMenuName  = 0;
        wc.lpszClassName = WINDOW_CLASS_NAME;
        
        if( !RegisterClass( &wc ) )
        {
            char*			error;
            
            error = WinGetLastErrorLocal();
            Com_Error( ERR_FATAL, "GLW_CreateWindow: could not register window class: '%s'", error );
        }
        g_wvPtr->classRegistered = true;
        Com_Printf( "...registered window class\n" );
    }
    
    //
    // create the HWND if one does not already exist
    //
    if( !g_wvPtr->hWnd )
    {
        //
        // compute width and height
        //
        r.left = 0;
        r.top = 0;
        r.right  = width;
        r.bottom = height;
        
        if( cdsFullscreen )
        {
            exstyle = WS_EX_TOPMOST;
            stylebits = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
        }
        else
        {
            exstyle = 0;
            stylebits = WINDOW_STYLE | WS_SYSMENU;
            AdjustWindowRect( &r, stylebits, FALSE );
        }
        
        w = r.right - r.left;
        h = r.bottom - r.top;
        
        if( cdsFullscreen )
        {
            x = 0;
            y = 0;
        }
        else
        {
            vid_xpos = Cvar_Get( "vid_xpos", "", 0 );
            vid_ypos = Cvar_Get( "vid_ypos", "", 0 );
            x = vid_xpos->integer;
            y = vid_ypos->integer;
            
            // adjust window coordinates if necessary
            // so that the window is completely on screen
            if( x < 0 )
            {
                x = 0;
            }
            if( y < 0 )
            {
                y = 0;
            }
            
            if( w < glw_state.desktopWidth &&
                    h < glw_state.desktopHeight )
            {
                if( x + w > glw_state.desktopWidth )
                {
                    x = ( glw_state.desktopWidth - w );
                }
                if( y + h > glw_state.desktopHeight )
                {
                    y = ( glw_state.desktopHeight - h );
                }
            }
        }
        
        g_wvPtr->hWnd = CreateWindowEx(
                            exstyle,
                            WINDOW_CLASS_NAME,
                            "Wolfenstein",
                            stylebits,
                            x, y, w, h,
                            NULL,
                            NULL,
                            g_wvPtr->hInstance,
                            NULL );
                            
        if( !g_wvPtr->hWnd )
        {
            Com_Error( ERR_FATAL, "GLW_CreateWindow() - Couldn't create window" );
        }
        
        ShowWindow( g_wvPtr->hWnd, SW_SHOW );
        UpdateWindow( g_wvPtr->hWnd );
        Com_Printf( "...created window@%d,%d (%dx%d)\n", x, y, w, h );
    }
    else
    {
        Com_Printf( "...window already present, CreateWindowEx skipped\n" );
    }
    
    if( !GLW_InitDriver( colorbits ) )
    {
        ShowWindow( g_wvPtr->hWnd, SW_HIDE );
        DestroyWindow( g_wvPtr->hWnd );
        g_wvPtr->hWnd = NULL;
        
        return false;
    }
    
    SetForegroundWindow( g_wvPtr->hWnd );
    SetFocus( g_wvPtr->hWnd );
    
    return true;
}

static void PrintCDSError( int value )
{
    switch( value )
    {
        case DISP_CHANGE_RESTART:
            Com_Printf( "restart required\n" );
            break;
        case DISP_CHANGE_BADPARAM:
            Com_Printf( "bad param\n" );
            break;
        case DISP_CHANGE_BADFLAGS:
            Com_Printf( "bad flags\n" );
            break;
        case DISP_CHANGE_FAILED:
            Com_Printf( "DISP_CHANGE_FAILED\n" );
            break;
        case DISP_CHANGE_BADMODE:
            Com_Printf( "bad mode\n" );
            break;
        case DISP_CHANGE_NOTUPDATED:
            Com_Printf( "not updated\n" );
            break;
        default:
            Com_Printf( "unknown error %d\n", value );
            break;
    }
}

/*
** GLW_SetMode
*/
static rserr_t GLW_SetMode( int mode,
                            int colorbits,
                            bool cdsFullscreen )
{
    HDC hDC;
    const char* win_fs[] = { "W", "FS" };
    int cdsRet;
    DEVMODE dm;
    
    //
    // print out informational messages
    //
    Com_Printf( "...setting mode %d:", mode );
    if( !R_GetModeInfo( &glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, mode ) )
    {
        Com_Printf( " invalid mode\n" );
        return RSERR_INVALID_MODE;
    }
    Com_Printf( " %d %d %s\n", glConfig.vidWidth, glConfig.vidHeight, win_fs[cdsFullscreen] );
    
    //
    // check our desktop attributes
    //
    hDC = GetDC( GetDesktopWindow() );
    glw_state.desktopBitsPixel = GetDeviceCaps( hDC, BITSPIXEL );
    glw_state.desktopWidth = GetDeviceCaps( hDC, HORZRES );
    glw_state.desktopHeight = GetDeviceCaps( hDC, VERTRES );
    ReleaseDC( GetDesktopWindow(), hDC );
    
    //
    // verify desktop bit depth
    //
    if( glw_state.desktopBitsPixel < 15 || glw_state.desktopBitsPixel == 24 )
    {
        if( colorbits == 0 || ( !cdsFullscreen && colorbits >= 15 ) )
        {
            if( MessageBox( NULL,
                            "It is highly unlikely that a correct\n"
                            "windowed display can be initialized with\n"
                            "the current desktop display depth.  Select\n"
                            "'OK' to try anyway.  Press 'Cancel' if you\n"
                            "have a 3Dfx Voodoo, Voodoo-2, or Voodoo Rush\n"
                            "3D accelerator installed, or if you otherwise\n"
                            "wish to quit.",
                            "Low Desktop Color Depth",
                            MB_OKCANCEL | MB_ICONEXCLAMATION ) != IDOK )
            {
                return RSERR_INVALID_MODE;
            }
        }
    }
    
    // do a CDS if needed
    if( cdsFullscreen )
    {
        memset( &dm, 0, sizeof( dm ) );
        
        dm.dmSize = sizeof( dm );
        
        dm.dmPelsWidth  = glConfig.vidWidth;
        dm.dmPelsHeight = glConfig.vidHeight;
        dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
        
        if( r_displayRefresh->integer != 0 )
        {
            dm.dmDisplayFrequency = r_displayRefresh->integer;
            dm.dmFields |= DM_DISPLAYFREQUENCY;
        }
        
        // try to change color depth if possible
        if( colorbits != 0 )
        {
            if( glw_state.allowdisplaydepthchange )
            {
                dm.dmBitsPerPel = colorbits;
                dm.dmFields |= DM_BITSPERPEL;
                Com_Printf( "...using colorsbits of %d\n", colorbits );
            }
            else
            {
                Com_Printf( "WARNING:...changing depth not supported on Win95 < pre-OSR 2.x\n" );
            }
        }
        else
        {
            Com_Printf( "...using desktop display depth of %d\n", glw_state.desktopBitsPixel );
        }
        
        //
        // if we're already in fullscreen then just create the window
        //
        if( glw_state.cdsFullscreen )
        {
            Com_Printf( "...already fullscreen, avoiding redundant CDS\n" );
            
            if( !GLW_CreateWindow( glConfig.vidWidth, glConfig.vidHeight, colorbits, true ) )
            {
                Com_Printf( "...restoring display settings\n" );
                ChangeDisplaySettings( 0, 0 );
                return RSERR_INVALID_MODE;
            }
        }
        //
        // need to call CDS
        //
        else
        {
            Com_Printf( "...calling CDS: " );
            
            // try setting the exact mode requested, because some drivers don't report
            // the low res modes in EnumDisplaySettings, but still work
            if( ( cdsRet = ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) ) == DISP_CHANGE_SUCCESSFUL )
            {
                Com_Printf( "ok\n" );
                
                if( !GLW_CreateWindow( glConfig.vidWidth, glConfig.vidHeight, colorbits, true ) )
                {
                    Com_Printf( "...restoring display settings\n" );
                    ChangeDisplaySettings( 0, 0 );
                    return RSERR_INVALID_MODE;
                }
                
                glw_state.cdsFullscreen = true;
            }
            else
            {
                //
                // the exact mode failed, so scan EnumDisplaySettings for the next largest mode
                //
                DEVMODE devmode;
                int modeNum;
                
                Com_Printf( "failed, " );
                
                PrintCDSError( cdsRet );
                
                Com_Printf( "...trying next higher resolution:" );
                
                // we could do a better matching job here...
                for( modeNum = 0 ; ; modeNum++ )
                {
                    if( !EnumDisplaySettings( NULL, modeNum, &devmode ) )
                    {
                        modeNum = -1;
                        break;
                    }
                    if( devmode.dmPelsWidth >= glConfig.vidWidth
                            && devmode.dmPelsHeight >= glConfig.vidHeight
                            && devmode.dmBitsPerPel >= 15 )
                    {
                        break;
                    }
                }
                
                if( modeNum != -1 && ( cdsRet = ChangeDisplaySettings( &devmode, CDS_FULLSCREEN ) ) == DISP_CHANGE_SUCCESSFUL )
                {
                    Com_Printf( " ok\n" );
                    if( !GLW_CreateWindow( glConfig.vidWidth, glConfig.vidHeight, colorbits, true ) )
                    {
                        Com_Printf( "...restoring display settings\n" );
                        ChangeDisplaySettings( 0, 0 );
                        return RSERR_INVALID_MODE;
                    }
                    
                    glw_state.cdsFullscreen = true;
                }
                else
                {
                    Com_Printf( " failed, " );
                    
                    PrintCDSError( cdsRet );
                    
                    Com_Printf( "...restoring display settings\n" );
                    ChangeDisplaySettings( 0, 0 );
                    
                    glw_state.cdsFullscreen = false;
                    glConfig.isFullscreen = false;
                    if( !GLW_CreateWindow( glConfig.vidWidth, glConfig.vidHeight, colorbits, false ) )
                    {
                        return RSERR_INVALID_MODE;
                    }
                    return RSERR_INVALID_FULLSCREEN;
                }
            }
        }
    }
    else
    {
        if( glw_state.cdsFullscreen )
        {
            ChangeDisplaySettings( 0, 0 );
        }
        
        glw_state.cdsFullscreen = false;
        if( !GLW_CreateWindow( glConfig.vidWidth, glConfig.vidHeight, colorbits, false ) )
        {
            return RSERR_INVALID_MODE;
        }
    }
    
    //
    // success, now check display frequency, although this won't be valid on Voodoo(2)
    //
    memset( &dm, 0, sizeof( dm ) );
    dm.dmSize = sizeof( dm );
    if( EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dm ) )
    {
        glConfig.displayFrequency = dm.dmDisplayFrequency;
    }
    
    // NOTE: this is overridden later on standalone 3Dfx drivers
    glConfig.isFullscreen = cdsFullscreen;
    
    return RSERR_OK;
}

/*
** GLW_InitExtensions
*/
static void GLW_InitExtensions( void )
{
    if( !r_allowExtensions->integer )
    {
        Com_Printf( "* IGNORING OPENGL EXTENSIONS *\n" );
        return;
    }
    
    Com_Printf( "Initializing OpenGL extensions\n" );
    
    glConfig.textureCompression = TC_NONE;
    
    // GL_EXT_texture_compression_s3tc
    if( GLEW_ARB_texture_compression &&
            GLEW_EXT_texture_compression_s3tc )
    {
        if( r_ext_compressed_textures->value )
        {
            glConfig.textureCompression = TC_EXT_COMP_S3TC;
            Com_Printf( "...found OpenGL extension - GL_EXT_texture_compression_s3tc\n" );
        }
        else
        {
            Com_Printf( "...ignoring GL_EXT_texture_compression_s3tc\n" );
        }
    }
    else
    {
        Com_Printf( "...GL_EXT_texture_compression_s3tc not found\n" );
    }
    
    // GL_S3_s3tc ... legacy extension before GL_EXT_texture_compression_s3tc.
    if( glConfig.textureCompression == TC_NONE )
    {
        if( GLEW_S3_s3tc )
        {
            if( r_ext_compressed_textures->value )
            {
                glConfig.textureCompression = TC_S3TC;
                Com_Printf( "...found OpenGL extension - GL_S3_s3tc\n" );
            }
            else
            {
                Com_Printf( "...ignoring GL_S3_s3tc\n" );
            }
        }
        else
        {
            Com_Printf( "...GL_S3_s3tc not found\n" );
        }
    }
    
    
    // GL_EXT_texture_env_add
    glConfig.textureEnvAddAvailable = false;
    if( GLEW_EXT_texture_env_add )
    {
        if( r_ext_texture_env_add->integer )
        {
            glConfig.textureEnvAddAvailable = true;
            Com_Printf( "...found OpenGL extension - GL_EXT_texture_env_add\n" );
        }
        else
        {
            glConfig.textureEnvAddAvailable = false;
            Com_Printf( "...ignoring GL_EXT_texture_env_add\n" );
        }
    }
    else
    {
        Com_Printf( "...GL_EXT_texture_env_add not found\n" );
    }
    
    // GL_ARB_multitexture
    if( GLEW_ARB_multitexture )
    {
        if( r_ext_multitexture->value )
        {
            glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &glConfig.maxTextureUnits );
            
            if( glConfig.maxTextureUnits > 1 )
            {
                Com_Printf( "...found OpenGL extension - GL_ARB_multitexture\n" );
            }
            else
            {
                Com_Error( ERR_FATAL, "...not using GL_ARB_multitexture, < 2 texture units\n" );
            }
        }
        else
        {
            Com_Printf( "...GL_ARB_multitexture not found\n" );
        }
    }
    else
    {
        Com_Printf( "...GL_ARB_multitexture not found\n" );
    }
    
    // GL_ARB_transpose_matrix
    if( GLEW_ARB_transpose_matrix )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_transpose_matrix\n" );
    }
    else
    {
        Com_Printf( "...GL_ARB_transpose_matrix not found\n" );
    }
    
    // GL_ARB_vertex_buffer_object
    if( GLEW_ARB_vertex_buffer_object )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_vertex_buffer_object\n" );
    }
    else
    {
        Com_Error( PRINT_ALL, "...GL_ARB_vertex_buffer_object not found\n" );
    }
    
    // GL_EXT_framebuffer_object
    if( GLEW_EXT_framebuffer_object )
    {
        Com_Printf( "...found OpenGL extension - GL_EXT_framebuffer_object\n" );
    }
    else
    {
        Com_Printf( "...GL_EXT_framebuffer_object not found\n" );
    }
    
    // GL_ARB_shader_objects
    if( GLEW_ARB_shader_objects )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_shader_objects\n" );
    }
    else
    {
        Com_Error( PRINT_ALL, "...GL_ARB_shader_objects not found\n" );
    }
    
    // GL_ARB_vertex_program
    if( GLEW_ARB_vertex_program )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_vertex_program\n" );
    }
    else
    {
        Com_Error( ERR_FATAL, "...GL_ARB_vertex_program not found\n" );
    }
    
    // GL_ARB_vertex_shader
    if( GLEW_ARB_vertex_shader )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_vertex_shader\n" );
    }
    else
    {
        Com_Error( PRINT_ALL, "...GL_ARB_vertex_shader not found\n" );
    }
    
    // GL_ARB_fragment_shader
    if( GLEW_ARB_fragment_shader )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_fragment_shader\n" );
    }
    else
    {
        Com_Error( PRINT_ALL, "...GL_ARB_fragment_shader not found\n" );
    }
    
    // GL_ARB_geometry_shader4
    if( GLEW_ARB_geometry_shader4 )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_geometry_shader4\n" );
    }
    else
    {
        Com_Error( PRINT_ALL, "...GL_ARB_geometry_shader4 not found\n" );
    }
    
    // GL_ARB_shading_language_100
    if( GLEW_ARB_shading_language_100 )
    {
        Com_Printf( "...found OpenGL extension - GL_ARB_shading_language_100\n" );
    }
    else
    {
        Com_Printf( "...GL_ARB_shading_language_100 not found\n" );
    }
}

/*
** GLimp_EndFrame
*/
void GLimp_EndFrame( void )
{
    //
    // swapinterval stuff
    //
    if( r_swapInterval->modified )
    {
        r_swapInterval->modified = false;
        
        if( !glConfig.stereoEnabled )       // why?
        {
            if( WGLEW_EXT_swap_control )
            {
                wglSwapIntervalEXT( r_swapInterval->integer );
            }
        }
    }
    
    
    // don't flip if drawing to front buffer
    if( Q_stricmp( r_drawBuffer->string, "GL_FRONT" ) != 0 )
    {
        if( glConfig.driverType > GLDRV_ICD )
        {
            if( !SwapBuffers( glw_state.hDC ) )
            {
                Com_Error( ERR_FATAL, "GLimp_EndFrame() - SwapBuffers() failed!\n" );
            }
        }
        else
        {
            SwapBuffers( glw_state.hDC );
        }
    }
    
    // check logging
}

#if 0
//Dushan - not used
static void GetGLVersion( int* major, int* minor )
{
    // for all versions
    char* ver = ( char* )glGetString( GL_VERSION );
    
    *major = ver[0] - '0';
    if( *major >= 3 )
    {
        // for GL 3.x
        glGetIntegerv( GL_MAJOR_VERSION, major );
        glGetIntegerv( GL_MINOR_VERSION, minor );
    }
    else
    {
        *minor = ver[2] - '0';
    }
    
    // GLSL
    ver = ( char* )glGetString( GL_SHADING_LANGUAGE_VERSION );
}
#endif

static void GLW_InitOpenGLContext()
{
    int retVal;
    const char* success[] = { "failed", "success" };
    
    if( WGLEW_ARB_create_context || wglewIsSupported( "WGL_ARB_create_context" ) )
    {
        if( wglCreateContextAttribsARB )
        {
            int attributes[11];
            int attribIndex = 0;
            attributes[attribIndex++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            attributes[attribIndex++] = 3;
            attributes[attribIndex++] = WGL_CONTEXT_MINOR_VERSION_ARB;
            attributes[attribIndex++] = 3;
            attributes[attribIndex++] = WGL_CONTEXT_PROFILE_MASK_ARB;
            attributes[attribIndex++] = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            attributes[attribIndex++] = 0;
            
            // set current context to NULL
            retVal = wglMakeCurrent( glw_state.hDC, NULL ) != 0;
            Com_Printf( "...wglMakeCurrent( %p, %p ): %s\n", glw_state.hDC, NULL, success[retVal] );
            
            // delete HGLRC
            if( glw_state.hGLRC )
            {
                retVal = wglDeleteContext( glw_state.hGLRC ) != 0;
                Com_Printf( "...deleting standard GL context: %s\n", success[retVal] );
                glw_state.hGLRC = NULL;
            }
            
            Com_Printf( "...initializing OpenGL %i.%i context ", 3, 3 );
            glw_state.hGLRC = wglCreateContextAttribsARB( glw_state.hDC, 0, attributes );
            
            if( wglMakeCurrent( glw_state.hDC, glw_state.hGLRC ) )
            {
                Com_Printf( " done\n" );
                glConfig.driverType = GLDRV_OPENGL;
            }
            else
            {
                Com_Error( ERR_FATAL, "Could not initialize OpenGL 3.3 context\n" );
            }
        }
    }
    else
    {
        Com_Error( ERR_FATAL, "Cannot create A GL Rendering Context" );
    }
}


static void GLW_StartOpenGL()
{
    GLenum glewResult;
    
    glConfig.driverType = GLDRV_OPENGL;
    
    // create the window and set up the context
    if( !GLW_StartDriverAndSetMode( r_mode->integer, r_colorbits->integer, r_fullscreen->integer ) )
    {
        // if we're on a 24/32-bit desktop and we're going fullscreen on an ICD,
        // try it again but with a 16-bit desktop
        if( r_colorbits->integer != 16 || r_fullscreen->integer != true || r_mode->integer != 3 )
        {
            if( !GLW_StartDriverAndSetMode( 3, 16, true ) )
            {
                Com_Error( ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem" );
            }
        }
    }
    
    glewResult = glewInit();
    if( GLEW_OK != glewResult )
    {
        // glewInit failed, something is seriously wrong
        Com_Error( ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem: %s", glewGetErrorString( glewResult ) );
    }
    else
    {
        Com_Printf( "Using GLEW %s\n", glewGetString( GLEW_VERSION ) );
    }
    
    GLW_InitOpenGLContext();
}
/*
** GLimp_Init
**
** This is the platform specific OpenGL initialization function.  It
** is responsible for loading OpenGL, initializing it, setting
** extensions, creating a window of the appropriate size, doing
** fullscreen manipulations, etc.  Its overall responsibility is
** to make sure that a functional OpenGL subsystem is operating
** when it returns to the ref.
*/
void GLimp_Init( void )
{
    char buf[1024];
    cvar_t* lastValidRenderer = Cvar_Get( "r_lastValidRenderer", "(uninitialized)", CVAR_ARCHIVE );
    cvar_t* cv;
    int i = 0, exts = 0;
    
    Com_Printf( "Initializing OpenGL subsystem\n" );
    
    g_wvPtr = ( WinVars_t* )Sys_GetSystemHandles();
    if( !g_wvPtr )
    {
        Com_Error( ERR_FATAL, "GLimp_Init() - could not receive WinVars_t g_wv\n" );
    }
    
    // save off hInstance and wndproc
    cv = Cvar_Get( "win_hinstance", "", 0 );
    sscanf( cv->string, "%i", ( int* )&g_wvPtr->hInstance );
    
    cv = Cvar_Get( "win_wndproc", "", 0 );
    sscanf( cv->string, "%i", ( int* )&glw_state.wndproc );
    
    r_allowSoftwareGL = Cvar_Get( "r_allowSoftwareGL", "0", CVAR_LATCH );
    r_maskMinidriver = Cvar_Get( "r_maskMinidriver", "0", CVAR_LATCH );
    
    // load appropriate DLL and initialize subsystem
    GLW_StartOpenGL();
    
    // get our config strings
    Q_strncpyz( glConfig.vendor_string, ( const char* )glGetString( GL_VENDOR ), sizeof( glConfig.vendor_string ) );
    Q_strncpyz( glConfig.renderer_string, ( const char* )glGetString( GL_RENDERER ), sizeof( glConfig.renderer_string ) );
    Q_strncpyz( glConfig.version_string, ( const char* )glGetString( GL_VERSION ), sizeof( glConfig.version_string ) );
    glGetIntegerv( GL_NUM_EXTENSIONS, &exts );
    glConfig.extensions_string[0] = 0;
    for( i = 0; i < exts; i++ )
    {
        if( strlen( glConfig.extensions_string ) + 100 >= sizeof( glConfig.extensions_string ) )
        {
            //Just so we wont error out when there are really a lot of extensions
            break;
        }
        Q_strcat( glConfig.extensions_string, sizeof( glConfig.extensions_string ), va( "%s ", glGetStringi( GL_EXTENSIONS, i ) ) );
    }
    
    // chipset specific configuration
    Q_strncpyz( buf, glConfig.renderer_string, sizeof( buf ) );
    Q_strlwr( buf );
    
    //
    // NOTE: if changing cvars, do it within this block.  This allows them
    // to be overridden when testing driver fixes, etc. but only sets
    // them to their default state when the hardware is first installed/run.
    //
    if( Q_stricmp( lastValidRenderer->string, glConfig.renderer_string ) )
    {
        glConfig.hardwareType = GLHW_GENERIC;
        
        Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
        
        // VOODOO GRAPHICS w/ 2MB
        if( strstr( buf, "voodoo graphics/1 tmu/2 mb" ) )
        {
            Cvar_Set( "r_picmip", "2" );
            Cvar_Get( "r_picmip", "1", CVAR_ARCHIVE | CVAR_LATCH );
        }
        else if( strstr( buf, "matrox" ) )
        {
            Cvar_Set( "r_allowExtensions", "0" );
        }
        else
        {
            if( strstr( buf, "rage 128" ) || strstr( buf, "rage128" ) )
            {
                Cvar_Set( "r_finish", "0" );
            }
            // Savage3D and Savage4 should always have trilinear enabled
            else if( strstr( buf, "savage3d" ) || strstr( buf, "s3 savage4" ) )
            {
                Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
            }
        }
    }
    
    //
    // this is where hardware specific workarounds that should be
    // detected/initialized every startup should go.
    //
    if( strstr( buf, "banshee" ) || strstr( buf, "voodoo3" ) )
    {
        glConfig.hardwareType = GLHW_3DFX_2D3D;
    }
    // VOODOO GRAPHICS w/ 2MB
    else if( strstr( buf, "voodoo graphics/1 tmu/2 mb" ) )
    {
    }
    else if( strstr( buf, "glzicd" ) )
    {
    }
    else if( strstr( buf, "rage pro" ) || strstr( buf, "Rage Pro" ) || strstr( buf, "ragepro" ) )
    {
        glConfig.hardwareType = GLHW_RAGEPRO;
    }
    else if( strstr( buf, "rage 128" ) )
    {
    }
    else if( strstr( buf, "permedia2" ) )
    {
        glConfig.hardwareType = GLHW_PERMEDIA2;
    }
    else if( strstr( buf, "riva 128" ) )
    {
        glConfig.hardwareType = GLHW_RIVA128;
    }
    else if( strstr( buf, "riva tnt " ) )
    {
    }
    
    if( strstr( buf, "geforce" ) || strstr( buf, "ge-force" ) || strstr( buf, "radeon" ) || strstr( buf, "nv20" ) || strstr( buf, "nv30" )
            || strstr( buf, "quadro" ) )
    {
        Cvar_Set( "r_highQualityVideo", "1" );
    }
    else
    {
        Cvar_Set( "r_highQualityVideo", "0" );
    }
    
    Cvar_Set( "r_lastValidRenderer", glConfig.renderer_string );
    
    GLW_InitExtensions();
    WG_CheckHardwareGamma();
}

/*
** GLimp_Shutdown
**
** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.
*/
void GLimp_Shutdown( void )
{
//	const char *strings[] = { "soft", "hard" };
    const char* success[] = { "failed", "success" };
    int retVal;
    
    Com_Printf( "Shutting down OpenGL subsystem\n" );
    
    // restore gamma.  We do this first because 3Dfx's extension needs a valid OGL subsystem
    WG_RestoreGamma();
    
    // release DC
    if( glw_state.hDC )
    {
        // delete HGLRC
        if( glw_state.hGLRC )
        {
            // set current context to NULL
            retVal = wglMakeCurrent( glw_state.hDC, NULL ) != 0;
            Com_Printf( "...wglMakeCurrent( %p, %p ): %s\n", glw_state.hDC, NULL, success[retVal] );
            
            retVal = wglDeleteContext( glw_state.hGLRC ) != 0;
            Com_Printf( "...deleting GL context: %s\n", success[retVal] );
            glw_state.hGLRC = NULL;
        }
        
        retVal = ReleaseDC( g_wvPtr->hWnd, glw_state.hDC ) != 0;
        Com_Printf( "...releasing DC: %s\n", success[retVal] );
        glw_state.hDC = NULL;
    }
    
    // destroy window
    if( g_wvPtr->hWnd )
    {
        Com_Printf( "...destroying window\n" );
        ShowWindow( g_wvPtr->hWnd, SW_HIDE );
        DestroyWindow( g_wvPtr->hWnd );
        g_wvPtr->hWnd = NULL;
        glw_state.pixelFormatSet = false;
    }
    
    // close the r_logFile
    if( glw_state.log_fp )
    {
        fclose( glw_state.log_fp );
        glw_state.log_fp = 0;
    }
    
    // reset display settings
    if( glw_state.cdsFullscreen )
    {
        Com_Printf( "...resetting display\n" );
        ChangeDisplaySettings( 0, 0 );
        glw_state.cdsFullscreen = false;
    }
    
    
    memset( &glConfig, 0, sizeof( glConfig ) );
    memset( &glState, 0, sizeof( glState ) );
    
    g_wvPtr = NULL;
}

/*
** GLimp_LogComment
*/
void GLimp_LogComment( char* comment )
{
    if( glw_state.log_fp )
    {
        fprintf( glw_state.log_fp, "%s", comment );
    }
}


/*
===========================================================

SMP acceleration

===========================================================
*/

HANDLE renderCommandsEvent;
HANDLE renderCompletedEvent;
HANDLE renderActiveEvent;

void ( *glimpRenderThread )( void );

void GLimp_RenderThreadWrapper( void )
{
    glimpRenderThread();
    
    // unbind the context before we die
    wglMakeCurrent( glw_state.hDC, NULL );
}

/*
=======================
GLimp_SpawnRenderThread
=======================
*/
HANDLE renderThreadHandle;
int renderThreadId;
bool GLimp_SpawnRenderThread( void ( *function )( void ) )
{

    renderCommandsEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    renderCompletedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    renderActiveEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    
    glimpRenderThread = function;
    
    renderThreadHandle = CreateThread(
                             NULL,   // LPSECURITY_ATTRIBUTES lpsa,
                             0,      // DWORD cbStack,
                             ( LPTHREAD_START_ROUTINE )GLimp_RenderThreadWrapper, // LPTHREAD_START_ROUTINE lpStartAddr,
                             0,          // LPVOID lpvThreadParm,
                             0,          //   DWORD fdwCreate,
                             ( LPDWORD )&renderThreadId );
                             
    if( !renderThreadHandle )
    {
        return false;
    }
    
    return true;
}

static void*    smpData;
static int wglErrors;

void* GLimp_RendererSleep( void )
{
    void*    data;
    
    if( !wglMakeCurrent( glw_state.hDC, NULL ) )
    {
        wglErrors++;
    }
    
    ResetEvent( renderActiveEvent );
    
    // after this, the front end can exit GLimp_FrontEndSleep
    SetEvent( renderCompletedEvent );
    
    WaitForSingleObject( renderCommandsEvent, INFINITE );
    
    if( !wglMakeCurrent( glw_state.hDC, glw_state.hGLRC ) )
    {
        wglErrors++;
    }
    
    ResetEvent( renderCompletedEvent );
    ResetEvent( renderCommandsEvent );
    
    data = smpData;
    
    // after this, the main thread can exit GLimp_WakeRenderer
    SetEvent( renderActiveEvent );
    
    return data;
}


void GLimp_FrontEndSleep( void )
{
    WaitForSingleObject( renderCompletedEvent, INFINITE );
    
    if( !wglMakeCurrent( glw_state.hDC, glw_state.hGLRC ) )
    {
        wglErrors++;
    }
}


void GLimp_WakeRenderer( void* data )
{
    smpData = data;
    
    if( !wglMakeCurrent( glw_state.hDC, NULL ) )
    {
        wglErrors++;
    }
    
    // after this, the renderer can continue through GLimp_RendererSleep
    SetEvent( renderCommandsEvent );
    
    WaitForSingleObject( renderActiveEvent, INFINITE );
}

