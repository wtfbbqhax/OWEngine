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
//  File name:   g_public.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: game module information visible to server
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifdef GAMEDLL
#include "../../../src-engine/botlib/botlib.h"
typedef struct gentity_s gentity_t;
#else
#define gentity_t sharedEntity_t
#endif

#define GAME_API_VERSION    8

// entity->svFlags
// the server does not know how to interpret most of the values
// in entityStates (level eType), so the game must explicitly flag
// special server behaviors
#define SVF_NOCLIENT            0x00000001  // don't send entity to clients, even if it has effects
#define SVF_VISDUMMY            0x00000004  // this ent is a "visibility dummy" and needs it's master to be sent to clients that can see it even if they can't see the master ent
#define SVF_BOT                 0x00000008
// Wolfenstein
#define SVF_CASTAI              0x00000010
// done.
#define SVF_BROADCAST           0x00000020  // send to all connected clients
#define SVF_PORTAL              0x00000040  // merge a second pvs at origin2 into snapshots
#define SVF_USE_CURRENT_ORIGIN  0x00000080  // entity->r.currentOrigin instead of entity->s.origin
// for link position (missiles and movers)
// Ridah
#define SVF_NOFOOTSTEPS         0x00000100
// done.
// MrE:
#define SVF_CAPSULE             0x00000200  // use capsule for collision detection

#define SVF_VISDUMMY_MULTIPLE   0x00000400  // so that one vis dummy can add to snapshot multiple speakers

// recent id changes
#define SVF_SINGLECLIENT        0x00000800  // only send to a single client (entityShared_t->singleClient)
#define SVF_NOSERVERINFO        0x00001000  // don't send CS_SERVERINFO updates to this client
// so that it can be updated for ping tools without
// lagging clients
#define SVF_NOTSINGLECLIENT     0x00002000  // send entity to everyone but one client
// (entityShared_t->singleClient)

//===============================================================


typedef struct
{
    entityState_t s;                // communicated by server to clients
    
    bool linked;                // false if not in any good cluster
    int linkcount;
    
    int svFlags;                    // SVF_NOCLIENT, SVF_BROADCAST, etc
    int singleClient;               // only send to this client when SVF_SINGLECLIENT is set
    
    bool bmodel;                // if false, assume an explicit mins / maxs bounding box
    // only set by trap_SetBrushModel
    vec3_t mins, maxs;
    int contents;                   // CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
    // a non-solid entity should set to 0
    
    vec3_t absmin, absmax;          // derived from mins/maxs and origin + rotation
    
    // currentOrigin will be used for all collision detection and world linking.
    // it will not necessarily be the same as the trajectory evaluation for the current
    // time, because each entity must be moved one at a time after time is advanced
    // to avoid simultanious collision issues
    vec3_t currentOrigin;
    vec3_t currentAngles;
    
    // when a trace call is made and passEntityNum != ENTITYNUM_NONE,
    // an ent will be excluded from testing if:
    // ent->s.number == passEntityNum	(don't interact with self)
    // ent->s.ownerNum = passEntityNum	(don't interact with your own missiles)
    // entity[ent->s.ownerNum].ownerNum = passEntityNum	(don't interact with other missiles from owner)
    int ownerNum;
    int eventTime;
    idTraceModel* traceModel;
} entityShared_t;

// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
typedef struct
{
    entityState_t s;                // communicated by server to clients
    entityShared_t r;               // shared by both the server system and game
} sharedEntity_t;

//===============================================================

//
// system functions provided by the main engine
//
struct gameImports_t
{
    void( *Printf )( const char* fmt, ... );
    void( *Error )( int level, const char* fmt, ... );
    
    void( *Endgame )( void );
    
    int( *Milliseconds )( void );
    int( *Argc )( void );
    
    void( *Argv )( int n, char* buffer, int bufferLength );
    
    int( *FS_FOpenFile )( const char* qpath, fileHandle_t* f, fsMode_t mode );
    int( *FS_Read )( void* buffer, int len, fileHandle_t f );
    int( *FS_Write )( const void* buffer, int len, fileHandle_t f );
    void( *FS_Rename )( const char* from, const char* to );
    void( *FS_FCloseFile )( fileHandle_t f );
    void( *FS_CopyFile )( char* from, char* to );
    int( *FS_GetFileList )( const char* path, const char* extension, char* listbuf, int bufsize );
    
    void( *SendConsoleCommand )( int exec_when, const char* text );
    
    void( *Cvar_Register )( vmCvar_t* cvar, const char* var_name, const char* value, int flags );
    void( *Cvar_Update )( vmCvar_t* cvar );
    
    void( *Cvar_Set )( const char* var_name, const char* value );
    
    int( *Cvar_VariableIntegerValue )( const char* var_name );
    
    void( *Cvar_VariableStringBuffer )( const char* var_name, char* buffer, int bufsize );
    
    
    void( *LocateGameData )( gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* clients, int sizeofGClient );
    
    void( *DropClient )( int clientNum, const char* reason );
    void( *SendServerCommand )( int clientNum, const char* text );
    void( *SetConfigstring )( int num, const char* string );
    void( *GetConfigstring )( int num, char* buffer, int bufferSize );
    void( *GetUserinfo )( int num, char* buffer, int bufferSize );
    void( *SetUserinfo )( int num, const char* buffer );
    void( *GetServerinfo )( char* buffer, int bufferSize );
    void( *SetBrushModel )( gentity_t* ent, const char* name );
    
    void( *Trace )( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, int capsule );
    
    int( *PointContents )( const vec3_t point, int passEntityNum );
    bool( *InPVS )( const vec3_t p1, const vec3_t p2 );
    
    bool( *InPVSIgnorePortals )( const vec3_t p1, const vec3_t p2 );
    void( *AdjustAreaPortalState )( gentity_t* ent, bool open );
    
    void( *LinkEntity )( gentity_t* ent );
    void( *UnlinkEntity )( gentity_t* ent );
    
    int( *EntitiesInBox )( const vec3_t mins, const vec3_t maxs, int* list, int maxcount );
    
    bool( *EntityContact )( const vec3_t mins, const vec3_t maxs, const gentity_t* ent, bool capsule );
    
    int( *BotAllocateClient )( void );
    void( *BotFreeClient )( int clientNum );
    
    void( *GetUsercmd )( int clientNum, usercmd_t* cmd );
    bool( *GetEntityToken )( char* buffer, int bufferSize );
    
    int( *DebugPolygonCreate )( int color, int numPoints, vec3_t* points );
    void( *DebugPolygonDelete )( int id );
    
    int( *RealTime )( qtime_t* qtime );
    
    bool( *GetTag )( int clientNum, char* tagName, orientation_t* or );
    
    int( *BotGetSnapshotEntity )( int client, int ent );
    int( *BotGetConsoleMessage )( int client, char* buf, int size );
    
    void( *BotGetUserCommand )( int clientNum, usercmd_t* ucmd );
    
    void( *PhysicsSetGravity )( const idVec3& gravity );
    idTraceModel* ( *AllocTraceModel )( void );
    void( *ResetPhysics )( void );
    
    botlib_export_t*	botlib;
    idCollisionModelManager* collisionModelManager;
};

//
// idGame
//
class idGame
{
public:
    virtual void Init( int levelTime, int randomSeed, int restart ) = 0;
    virtual void Shutdown( int restart ) = 0;
    virtual void ClientBegin( int clientNum ) = 0;
    virtual char* ClientConnect( int clientNum, bool firstTime, bool isBot ) = 0;
    virtual void ClientThink( int clientNum ) = 0;
    virtual void ClientUserinfoChanged( int clientNum ) = 0;
    virtual void ClientDisconnect( int clientNum ) = 0;
    virtual void ClientCommand( int clientNum ) = 0;
    virtual void RunFrame( int levelTime ) = 0;
    virtual bool ConsoleCommand( void ) = 0;
    virtual bool AICastVisibleFromPos( vec3_t srcpos, int srcnum, vec3_t destpos, int destnum, bool updateVisPos ) = 0;
    virtual bool AICastCheckAttackAtPos( int entnum, int enemy, vec3_t pos, bool ducking, bool allowHitWorld ) = 0;
    virtual void RetrieveMoveSpeedsFromClient( int entnum, char* text ) = 0;
    virtual bool GetModelInfo( int clientNum, char* modelName, animModelInfo_t** modelInfo ) = 0;
    virtual int	BotAIStartFrame( int time ) = 0;
};

extern idGame* game;
