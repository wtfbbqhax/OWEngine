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
//  File name:   ui_public.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __UI_PUBLIC_H__
#define __UI_PUBLIC_H__

#include "../../../src-engine/qcommon/q_shared.h"

#define UI_API_VERSION  4

typedef struct
{
    connstate_t connState;
    int connectPacketCount;
    int clientNum;
    char servername[MAX_STRING_CHARS];
    char updateInfoString[MAX_STRING_CHARS];
    char messageString[MAX_STRING_CHARS];
} uiClientState_t;

typedef enum
{
    UIMENU_NONE,
    UIMENU_MAIN,
    UIMENU_INGAME,
    UIMENU_NEED_CD,
    UIMENU_ENDGAME, //----(SA)	added
    UIMENU_BAD_CD_KEY,
    UIMENU_TEAM,
    UIMENU_PREGAME, //----(SA)	added
    UIMENU_POSTGAME,
    UIMENU_NOTEBOOK,
    UIMENU_CLIPBOARD,
    UIMENU_HELP,
    UIMENU_BOOK1,           //----(SA)	added
    UIMENU_BOOK2,           //----(SA)	added
    UIMENU_BOOK3,           //----(SA)	added
    UIMENU_WM_PICKTEAM,     // NERVE - SMF - for multiplayer only
    UIMENU_WM_PICKPLAYER,   // NERVE - SMF - for multiplayer only
    UIMENU_WM_QUICKMESSAGE, // NERVE - SMF
    UIMENU_WM_LIMBO,        // NERVE - SMF
    UIMENU_BRIEFING         //----(SA)	added
} uiMenuCommand_t;

#define SORT_HOST           0
#define SORT_MAP            1
#define SORT_CLIENTS        2
#define SORT_GAME           3
#define SORT_PING           4

#define SORT_SAVENAME       0
#define SORT_SAVETIME       1

//
// idUserInterfaceManager
//
class idUserInterfaceManager
{
public:
    virtual void	Init( bool inGameLoad ) = 0;
    virtual void	Shutdown( void ) = 0;
    
    virtual	void	KeyEvent( int key, bool down ) = 0;
    virtual void	MouseEvent( int dx, int dy ) = 0;
    virtual void	Refresh( int time ) = 0;
    
    virtual bool IsFullscreen( void ) = 0;
    
    virtual void SetActiveMenu( uiMenuCommand_t menu ) = 0;
    
    virtual uiMenuCommand_t GetActiveMenu( void ) = 0;
    virtual bool ConsoleCommand( int realTime ) = 0;
    
    // if !overlay, the background will be drawn, otherwise it will be
    // overlayed over whatever the cgame has drawn.
    // a GetClientState syscall will be made to get the current strings
    virtual void DrawConnectScreen( bool overlay ) = 0;
};

extern idUserInterfaceManager* uiManager;

#endif