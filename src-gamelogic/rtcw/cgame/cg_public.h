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
//  File name:   cg_public.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_PUBLIC_H__
#define __CG_PUBLIC_H__

#define CMD_BACKUP          64
#define CMD_MASK            ( CMD_BACKUP - 1 )
// allow a lot of command backups for very fast systems
// multiple commands may be combined into a single packet, so this
// needs to be larger than PACKET_BACKUP

#define MAX_ENTITIES_IN_SNAPSHOT    256

// snapshots are a view of the server at a given time

// Snapshots are generated at regular time intervals by the server,
// but they may not be sent if a client's rate level is exceeded, or
// they may be dropped by the network.
typedef struct
{
    int snapFlags;                      // SNAPFLAG_RATE_DELAYED, etc
    int ping;
    
    int serverTime;                 // server time the message is valid for (in msec)
    
    byte areamask[MAX_MAP_AREA_BYTES];                  // portalarea visibility bits
    
    playerState_t ps;                       // complete information about the current player at this time
    
    int numEntities;                        // all of the entities that need to be presented
    entityState_t entities[MAX_ENTITIES_IN_SNAPSHOT];   // at the time of this snapshot
    
    int numServerCommands;                  // text based server commands to execute when this
    int serverCommandSequence;              // snapshot becomes current
} snapshot_t;

enum
{
    CGAME_EVENT_NONE,
    CGAME_EVENT_TEAMMENU,
    CGAME_EVENT_SCOREBOARD,
    CGAME_EVENT_EDITHUD
};

#define CGAME_IMPORT_API_VERSION    4

struct cgameImports_t
{
    void( *Print )( const char* fmt, ... );
    
    void( *Error )( int level, const char* fmt, ... );
    int( *Milliseconds )( void );
    
    void( *Cvar_Register )( vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags );
    void( *Cvar_Update )( vmCvar_t* vmCvar );
    void( *Cvar_Set )( const char* var_name, const char* value );
    
    void( *Cvar_VariableStringBuffer )( const char* var_name, char* buffer, int bufsize );
    
    int( *Argc )( void );
    
    void( *Argv )( int n, char* buffer, int bufferLength );
    
    void( *Args )( char* buffer, int bufferLength );
    
    int( *FS_FOpenFile )( const char* qpath, fileHandle_t* f, fsMode_t mode );
    int( *FS_Read )( void* buffer, int len, fileHandle_t f );
    
    int( *FS_Write )( const void* buffer, int len, fileHandle_t f );
    
    void( *FS_FCloseFile )( fileHandle_t f );
    
    void( *SendConsoleCommand )( const char* text );
    
    void( *AddCommand )( const char* cmdName );
    
    void( *SendClientCommand )( const char* s );
    
    void( *UpdateScreen )( void );
    
    void( *GetGlconfig )( glconfig_t* glconfig );
    
    void( *GetGameState )( gameState_t* gamestate );
    
    void( *GetCurrentSnapshotNumber )( int* snapshotNumber, int* serverTime );
    
    bool( *GetSnapshot )( int snapshotNumber, snapshot_t* snapshot );
    
    bool( *GetServerCommand )( int serverCommandNumber );
    
    int( *GetCurrentCmdNumber )( void );
    
    bool( *GetUserCmd )( int cmdNumber, usercmd_t* ucmd );
    
    void( *SetUserCmdValue )( int stateValue, int holdableValue, float sensitivityScale, int cld );
    int( *MemoryRemaining )( void );
    
    bool( *loadCamera )( int camNum, const char* name );
    void( *startCamera )( int camNum, int time );
    void( *stopCamera )( int camNum );
    
    bool( *getCameraInfo )( int camNum, int time, vec3_t* origin, vec3_t* angles, float* fov );
    
    
    bool( *Key_IsDown )( int keynum );
    int( *Key_GetCatcher )( void );
    void( *Key_SetCatcher )( int catcher );
    int( *Key_GetKey )( const char* binding );
    
    int( *PC_AddGlobalDefine )( char* define );
    
    int( *PC_LoadSource )( const char* filename );
    
    int( *PC_FreeSource )( int handle );
    
    int( *PC_ReadToken )( int handle, pc_token_t* pc_token );
    
    int( *PC_SourceFileAndLine )( int handle, char* filename, int* line );
    
    int( *RealTime )( qtime_t* qtime );
    
    void( *SendMoveSpeedsToGame )( int entnum, char* movespeeds );
    
    // this returns a handle.  arg0 is the name in the format "idlogo.roq", set arg1 to NULL, alteredstates to qfalse (do not alter gamestate)
    int( *CIN_PlayCinematic )( const char* arg0, int xpos, int ypos, int width, int height, int bits );
    
    // stops playing the cinematic and ends it.  should always return FMV_EOF
    // cinematics must be stopped in reverse order of when they are started
    e_status( *CIN_StopCinematic )( int handle );
    
    // will run a frame of the cinematic but will not draw it.  Will return FMV_EOF if the end of the cinematic has been reached.
    e_status( *CIN_RunCinematic )( int handle );
    
    // draws the current frame
    void( *CIN_DrawCinematic )( int handle );
    
    
    // allows you to resize the animation dynamically
    void( *CIN_SetExtents )( int handle, int x, int y, int w, int h );
    
    void( *UI_Popup )( const char* arg0 );
    void( *UI_LimboChat )( const char* arg0 );
    void( *UI_ClosePopup )( const char* arg0 );
    bool( *GetModelInfo )( int clientNum, char* modelName, animModelInfo_t** modelInfo );
    
    idRenderSystem* renderSystem;
    idCollisionModelManager* collisionModelManager;
    idSoundSystem* soundSystem;
};


/*
==================================================================

functions exported to the main executable

==================================================================
*/

class idCGame
{
public:
    virtual void Init( int serverMessageNum, int serverCommandSequence ) = 0;
    // called when the level loads or when the renderer is restarted
    // all media should be registered at this time
    // cgame will display loading status by calling SCR_Update, which
    // will call CG_DrawInformation during the loading process
    // reliableCommandSequence will be 0 on fresh loads, but higher for
    // demos, tourney restarts, or vid_restarts
    
    virtual void Shutdown() = 0;
    // oportunity to flush and close any open files
    
    virtual bool ConsoleCommand() = 0;
    // a console command has been issued locally that is not recognized by the
    // main game system.
    // use Cmd_Argc() / Cmd_Argv() to read the command, return qfalse if the
    // command is not known to the game
    
    virtual void DrawActiveFrame( int serverTime, stereoFrame_t stereoView, bool demoPlayback ) = 0;
    // Generates and draws a game scene and status information at the given time.
    // If demoPlayback is set, local movement prediction will not be enabled
    
    virtual int CrosshairPlayer( void ) = 0;
    
    virtual int LastAttacker( void ) = 0;
    virtual void KeyEvent( int key, bool down ) = 0;
    
    virtual void MouseEvent( int dx, int dy ) = 0;
    
    virtual void EventHandling( int type ) = 0;
    
    virtual bool GetTag( int clientNum, char* tagname, orientation_t* or ) = 0;
};

extern idCGame* cgame;

//----------------------------------------------

#endif // !__CG_PUBLIC_H__