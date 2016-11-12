//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
//  Copyright (C) 2010 Roderick Colenbrander
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
//  File name:   android_input.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "android_glimp.h"
#include "../../client/client.h"
#include "../../renderer/tr_local.h"
#include "../../game/q_shared.h"
#include <android/log.h>

/* These variables are for confining menu input to 640x480 */
static float scale_ratio;
static int offset_x;

/* Shared cursor position between touch and trackball code */
static float cursor_x = 0, cursor_y = 0;

static qboolean motion_event = qfalse;
static float motion_dx = 0;
static float motion_dy = 0;

static qboolean trackball_event = qfalse;
static float trackball_dx = 0;
static float trackball_dy = 0;

void ( *setState )( int shown );

void setInputCallbacks( void* set_state )
{
    setState = set_state;
}

void Com_PushEvent( sysEvent_t* event );

void Com_QueueEvent( int evTime,
                     sysEventType_t evType,
                     int evValue, int evValue2,
                     int evPtrLength,
                     void* evPtr )
{
    sysEvent_t ev;
    memset( &ev, 0, sizeof( ev ) );
    ev.evTime = evTime;
    ev.evType = evType;
    ev.evValue = evValue;
    ev.evValue2 = evValue2;
    ev.evPtrLength = evPtrLength;
    ev.evPtr = evPtr;
    Com_PushEvent( &ev );
}

void OWE_KeyEvent( int state, int key, int character )
{
    int t = Sys_Milliseconds();
    if( key != 0 )
    {
        Com_QueueEvent( t, SE_KEY, key, state, 0, NULL );
        Com_DPrintf( "SE_KEY key=%d state=%d\n", key, state );
    }
    if( ( state == 1 ) && ( character != 0 ) )
    {
        Com_DPrintf( "SE_CHAR key=%d state=%d\n", character, state );
        Com_QueueEvent( t, SE_CHAR, character, 0, 0, NULL );
    }
}

enum Action
{
    ACTION_DOWN = 0,
    ACTION_UP = 1,
    ACTION_MOVE = 2
};


void OWE_MotionEvent( float dx, float dy )
{
    motion_event = qtrue;
    motion_dx += dx;
    motion_dy += dy;
}

/* The quake3 menu is 640x480, make sure the coordinates originating from this area are scaled.
 * The UI code already contains clamping to 640x480, so don't perform it here.
 */
inline int scale_x_input( float x )
{
    return ( int )( ( x - offset_x ) * scale_ratio );
}

inline int scale_y_input( float y )
{
    return ( int )( y * scale_ratio );
}

static void processMotionEvents( void )
{
    if( motion_event )
    {
        int t = Sys_Milliseconds();
        Com_QueueEvent( t, SE_MOUSE, ( int )( motion_dx ), ( int )( motion_dy ), 0, NULL );
        motion_event = qfalse;
        motion_dx = 0;
        motion_dy = 0;
    }
}

void queueTrackballEvent( int action, float dx, float dy )
{
    int t = Sys_Milliseconds();
    static int keyPress = 0;
    
    switch( action )
    {
        case ACTION_DOWN:
            Com_QueueEvent( t, SE_KEY, K_MOUSE1, 1, 0, NULL );
            keyPress = 1;
            break;
        case ACTION_UP:
            if( keyPress )
                Com_QueueEvent( t, SE_KEY, K_MOUSE1, 0, 0, NULL );
            keyPress = 0;
    }
    
    trackball_dx += dx;
    trackball_dy += dy;
    trackball_event = qtrue;
}

inline float clamp_to_screen_width( float x )
{
    if( x > SCREEN_WIDTH )
        return SCREEN_WIDTH;
    else if( x < 0 )
        return 0;
    return x;
}

inline float clamp_to_screen_height( float y )
{
    if( y > SCREEN_HEIGHT )
        return SCREEN_HEIGHT;
    else if( y < 0 )
        return 0;
    return y;
}

static void processTrackballEvents( void )
{
    if( trackball_event )
    {
        int t = Sys_Milliseconds(); //what time should we use?
        
        /* Trackball dx/dy are <1.0, so make them a bit bigger to prevent kilometers of scrolling */
        trackball_dx *= 50.0;
        trackball_dy *= 50.0;
        cursor_x += trackball_dx;
        cursor_y += trackball_dy;
        
        cursor_x = clamp_to_screen_width( cursor_x );
        cursor_y = clamp_to_screen_height( cursor_y );
        
        Com_QueueEvent( t, SE_MOUSE, ( int )trackball_dx, ( int )trackball_dy, 0, NULL );
        
        trackball_event = qfalse;
        trackball_dx = 0;
        trackball_dy = 0;
    }
}

void IN_Frame( void )
{
    static int prev_state = -1;
    int state = -1;
    processMotionEvents();
    processTrackballEvents();
    
    /* We are in game and neither console/ui is active */
    //if (cls.state == CA_ACTIVE && Key_GetCatcher() == 0)
    
    state = ( ( ( cl.snap.ps.serverCursorHint == HINT_DOOR_ROTATING ) || ( cl.snap.ps.serverCursorHint == HINT_DOOR )
                || ( cl.snap.ps.serverCursorHint == HINT_BUTTON ) || ( cl.snap.ps.serverCursorHint == HINT_ACTIVATE ) ) << 0 ) | ( ( cls.state == CA_ACTIVE && Key_GetCatcher() == 0 ) << 1 ) | ( ( cl.snap.ps.serverCursorHint == HINT_BREAKABLE ) << 2 );
//cl.snap.ps.pm_flags & PMF_DUCKED;
//    else
//        state = 0;

    if( state != prev_state )
    {
        setState( state );
        prev_state = state;
    }
}

void IN_Init( void )
{
    scale_ratio = ( float )SCREEN_HEIGHT / glConfig.vidHeight;
    offset_x = ( glConfig.vidWidth - ( ( float )SCREEN_WIDTH / scale_ratio ) ) / 2;
}

qboolean IN_MotionPressed( void )
{
    return qfalse;
}

void IN_Shutdown( void )
{
}

void Sys_SendKeyEvents( void )
{
}

void IN_Restart( void )
{
    IN_Init();
}

