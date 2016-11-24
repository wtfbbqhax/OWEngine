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
//  File name:   keys.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __KEYS_H__
#define __KEYS_H__

#include "../../src-gamelogic/rtcw/ui/keycodes.h"

#define MAX_KEYS        256

typedef struct
{
    bool down;
    int repeats;                // if > 1, it is autorepeating
    char*        binding;
} qkey_t;

extern bool key_overstrikeMode;
extern qkey_t keys[MAX_KEYS];

// NOTE TTimo the declaration of field_t and Field_Clear is now in qcommon/qcommon.h

void Field_KeyDownEvent( field_t* edit, int key );
void Field_CharEvent( field_t* edit, int ch );
void Field_Draw( field_t* edit, int x, int y, int width, bool showCursor );
void Field_BigDraw( field_t* edit, int x, int y, int width, bool showCursor );

#define     COMMAND_HISTORY     32
extern field_t historyEditLines[COMMAND_HISTORY];

extern field_t g_consoleField;
extern field_t chatField;
extern int anykeydown;
extern bool chat_team;
extern bool chat_limbo;             // NERVE - SMF
extern int chat_playerNum;

void Key_WriteBindings( fileHandle_t f );
void Key_SetBinding( int keynum, const char* binding );
char* Key_GetBinding( int keynum );
bool Key_IsDown( int keynum );
bool Key_GetOverstrikeMode( void );
void Key_SetOverstrikeMode( bool state );
void Key_ClearStates( void );
int Key_GetKey( const char* binding );

#endif // !__KEYS_H__
