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
//  File name:   unix_joystick.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>  // bk001204

#include "../../client/client.h"
#include "unix_local.h"

// TTimo moved joystick.h include here, conflicts in client.h with wkey_t
#include <linux/joystick.h>

/* We translate axes movement into keypresses. */
int joy_keys[16] =
{
    K_LEFTARROW, K_RIGHTARROW,
    K_UPARROW, K_DOWNARROW,
    K_JOY16, K_JOY17,
    K_JOY18, K_JOY19,
    K_JOY20, K_JOY21,
    K_JOY22, K_JOY23,
    
    K_JOY24, K_JOY25,
    K_JOY26, K_JOY27
};

/* Our file descriptor for the joystick device. */
static int joy_fd = -1;


// bk001130 - from linux_glimp.c
extern cvar_t*   in_joystick;
extern cvar_t*   in_joystickDebug;
extern cvar_t*   joy_threshold;

/**********************************************/
/* Joystick routines.                         */
/**********************************************/
void IN_StartupJoystick( void )
{
    int i = 0;
    
    joy_fd = -1;
    
    if( !in_joystick->integer )
    {
        Com_DPrintf( "Joystick is not active.\n" );
        return;
    }
    
    for( i = 0; i < 4; i++ )
    {
        char filename[PATH_MAX];
        
        //Dushan
        snprintf( filename, PATH_MAX, "/dev/input/js%d", i );
        
        joy_fd = open( filename, O_RDONLY | O_NONBLOCK );
        
        if( joy_fd != -1 )
        {
            struct js_event event;
            char axes = 0;
            char buttons = 0;
            char name[128];
            int n = -1;
            
            Com_Printf( "Joystick %s found\n", filename );
            
            // Get rid of initialization messages.
            do
            {
                n = read( joy_fd, &event, sizeof( event ) );
                
                if( n == -1 )
                {
                    break;
                }
                
            }
            while( ( event.type & JS_EVENT_INIT ) );
            
            //Get joystick statistics.
            ioctl( joy_fd, JSIOCGAXES, &axes );
            ioctl( joy_fd, JSIOCGBUTTONS, &buttons );
            
            if( ioctl( joy_fd, JSIOCGNAME( sizeof( name ) ), name ) < 0 )
            {
                strncpy( name, "Unknown", sizeof( name ) );
            }
            
            Com_DPrintf( "Name:    %s\n", name );
            Com_DPrintf( "Axes:    %d\n", axes );
            Com_DPrintf( "Buttons: %d\n", buttons );
            
            // Our work here is done.
            return;
        }
        
        
        
        /* No soup for you. */
        if( joy_fd == -1 )
        {
            Com_Printf( "No joystick found.\n" );
            return;
        }
        
    }
    
    void IN_JoyMove( void )
    {
        /* Store instantaneous joystick state. Hack to get around
         * event model used in Linux joystick driver.
           */
        static int axes_state[16];
        /* Old bits for Quake-style input compares. */
        static unsigned int old_axes = 0;
        /* Our current goodies. */
        unsigned int axes = 0;
        int i = 0;
        
        if( joy_fd == -1 )
        {
            return;
        }
        
        /* Empty the queue, dispatching button presses immediately
           * and updating the instantaneous state for the axes.
           */
        do
        {
            int n = -1;
            struct js_event event;
            
            n = read( joy_fd, &event, sizeof( event ) );
            
            if( n == -1 )
            {
                /* No error, we're non-blocking. */
                break;
            }
            
            if( event.type & JS_EVENT_BUTTON )
            {
                Sys_QueEvent( 0, SE_KEY, K_JOY1 + event.number, event.value, 0, NULL );
            }
            else if( event.type & JS_EVENT_AXIS )
            {
            
                if( event.number >= 16 )
                {
                    continue;
                }
                
                axes_state[event.number] = event.value;
            }
            else
            {
                Com_Printf( "Unknown joystick event type\n" );
            }
            
        }
        while( 1 );
        
        
        /* Translate our instantaneous state to bits. */
        for( i = 0; i < 16; i++ )
        {
            float f = ( ( float ) axes_state[i] ) / 32767.0f;
            
            if( f < -joy_threshold->value )
            {
                axes |= ( 1 << ( i * 2 ) );
            }
            else if( f > joy_threshold->value )
            {
                axes |= ( 1 << ( ( i * 2 ) + 1 ) );
            }
            
        }
        
        /* Time to update axes state based on old vs. new. */
        for( i = 0; i < 16; i++ )
        {
        
            if( ( axes & ( 1 << i ) ) && !( old_axes & ( 1 << i ) ) )
            {
                Sys_QueEvent( 0, SE_KEY, joy_keys[i], qtrue, 0, NULL );
            }
            
            if( !( axes & ( 1 << i ) ) && ( old_axes & ( 1 << i ) ) )
            {
                Sys_QueEvent( 0, SE_KEY, joy_keys[i], qfalse, 0, NULL );
            }
        }
        
        /* Save for future generations. */
        old_axes = axes;
    }
    
    
    