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
//  File name:   sv_game.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: interface to the game dll
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "server.h"

#include "../botlib/botlib.h"

botlib_export_t* botlib_export;

idGame* game;
idGame* ( *gameDllEntry )( gameImports_t* gimports );

static gameImports_t exports;

void SV_GameError( const char* string )
{
    Com_Error( ERR_DROP, "%s", string );
}

void SV_GamePrint( const char* string )
{
    Com_Printf( "%s", string );
}

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int SV_NumForGentity( sharedEntity_t* ent )
{
    int num;
    
    num = ( ( byte* )ent - ( byte* )sv.gentities ) / sv.gentitySize;
    
    return num;
}

sharedEntity_t* SV_GentityNum( int num )
{
    sharedEntity_t* ent;
    
    ent = ( sharedEntity_t* )( ( byte* )sv.gentities + sv.gentitySize * ( num ) );
    
    return ent;
}

playerState_t* SV_GameClientNum( int num )
{
    playerState_t*   ps;
    
    ps = ( playerState_t* )( ( byte* )sv.gameClients + sv.gameClientSize * ( num ) );
    
    return ps;
}

svEntity_t*  SV_SvEntityForGentity( sharedEntity_t* gEnt )
{
    if( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES )
    {
        Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
    }
    return &sv.svEntities[ gEnt->s.number ];
}

sharedEntity_t* SV_GEntityForSvEntity( svEntity_t* svEnt )
{
    int num;
    
    num = svEnt - sv.svEntities;
    return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, const char* text )
{
    if( clientNum == -1 )
    {
        SV_SendServerCommand( NULL, "%s", text );
    }
    else
    {
        if( clientNum < 0 || clientNum >= sv_maxclients->integer )
        {
            return;
        }
        SV_SendServerCommand( svs.clients + clientNum, "%s", text );
    }
}


/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char* reason )
{
    if( clientNum < 0 || clientNum >= sv_maxclients->integer )
    {
        return;
    }
    SV_DropClient( svs.clients + clientNum, reason );
}


/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel( sharedEntity_t* ent, const char* name )
{
    clipHandle_t h;
    vec3_t mins, maxs;
    
    if( !name )
    {
        Com_Error( ERR_DROP, "SV_SetBrushModel: NULL" );
    }
    
    if( name[0] != '*' )
    {
        Com_Error( ERR_DROP, "SV_SetBrushModel: %s isn't a brush model", name );
    }
    
    
    ent->s.modelindex = atoi( name + 1 );
    
    h = collisionModelManager->InlineModel( ent->s.modelindex );
    collisionModelManager->ModelBounds( h, mins, maxs );
    VectorCopy( mins, ent->r.mins );
    VectorCopy( maxs, ent->r.maxs );
    ent->r.bmodel = true;
    
    ent->r.contents = -1;       // we don't know exactly what is in the brushes
    
    SV_LinkEntity( ent );       // FIXME: remove
}



/*
=================
SV_inPVS

Also checks portalareas so that doors block sight
=================
*/
bool SV_inPVS( const vec3_t p1, const vec3_t p2 )
{
    int leafnum;
    int cluster;
    int area1, area2;
    byte*    mask;
    
    leafnum = collisionModelManager->PointLeafnum( p1 );
    cluster = collisionModelManager->LeafCluster( leafnum );
    area1 = collisionModelManager->LeafArea( leafnum );
    mask = collisionModelManager->ClusterPVS( cluster );
    
    leafnum = collisionModelManager->PointLeafnum( p2 );
    cluster = collisionModelManager->LeafCluster( leafnum );
    area2 = collisionModelManager->LeafArea( leafnum );
    if( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) )
    {
        return false;
    }
    if( !collisionModelManager->AreasConnected( area1, area2 ) )
    {
        return false;      // a door blocks sight
    }
    return true;
}


/*
=================
SV_inPVSIgnorePortals

Does NOT check portalareas
=================
*/
bool SV_inPVSIgnorePortals( const vec3_t p1, const vec3_t p2 )
{
    int leafnum;
    int cluster;
    int area1, area2;
    byte*    mask;
    
    leafnum = collisionModelManager->PointLeafnum( p1 );
    cluster = collisionModelManager->LeafCluster( leafnum );
    area1 = collisionModelManager->LeafArea( leafnum );
    mask = collisionModelManager->ClusterPVS( cluster );
    
    leafnum = collisionModelManager->PointLeafnum( p2 );
    cluster = collisionModelManager->LeafCluster( leafnum );
    area2 = collisionModelManager->LeafArea( leafnum );
    
    if( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) )
    {
        return false;
    }
    
    return true;
}


/*
========================
SV_AdjustAreaPortalState
========================
*/
void SV_AdjustAreaPortalState( sharedEntity_t* ent, bool open )
{
    svEntity_t*  svEnt;
    
    svEnt = SV_SvEntityForGentity( ent );
    if( svEnt->areanum2 == -1 )
    {
        return;
    }
    collisionModelManager->AdjustAreaPortalState( svEnt->areanum, svEnt->areanum2, open );
}


/*
==================
SV_GameAreaEntities
==================
*/
bool    SV_EntityContact( const vec3_t mins, const vec3_t maxs, const sharedEntity_t* gEnt, const int capsule )
{
    const float* origin, *angles;
    clipHandle_t ch;
    trace_t trace;
    
    // check for exact collision
    origin = gEnt->r.currentOrigin;
    angles = gEnt->r.currentAngles;
    
    ch = SV_ClipHandleForEntity( gEnt );
    collisionModelManager->TransformedBoxTrace( &trace, vec3_origin, vec3_origin, mins, maxs,
            ch, -1, origin, angles, capsule );
            
    return trace.startsolid;
}


/*
===============
SV_GetServerinfo

===============
*/
void SV_GetServerinfo( char* buffer, int bufferSize )
{
    if( bufferSize < 1 )
    {
        Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
    }
    Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

/*
===============
SV_LocateGameData

===============
*/
void SV_LocateGameData( sharedEntity_t* gEnts, int numGEntities, int sizeofGEntity_t,
                        playerState_t* clients, int sizeofGameClient )
{
    sv.gentities = gEnts;
    sv.gentitySize = sizeofGEntity_t;
    sv.num_entities = numGEntities;
    
    sv.gameClients = clients;
    sv.gameClientSize = sizeofGameClient;
}


/*
===============
SV_GetUsercmd

===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t* cmd )
{
    if( clientNum < 0 || clientNum >= sv_maxclients->integer )
    {
        Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
    }
    *cmd = svs.clients[clientNum].lastUsercmd;
}

//==============================================

//
// SV_GetEntityToken
//
bool SV_GetEntityToken( char* buffer, int bufferSize )
{
    const char*  s;
    
    s = COM_Parse( &sv.entityParsePoint );
    Q_strncpyz( buffer, s, bufferSize );
    if( !sv.entityParsePoint && !s[0] )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs( void )
{
    if( !gvm || game == NULL )
    {
        return;
    }
    game->Shutdown( false );
    game = NULL;
    
    Sys_UnloadDll( gvm );
    gvm = NULL;
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
static void SV_InitGameVM( bool restart )
{
    int i;
    
    // start the entity parsing at the beginning
    sv.entityParsePoint = collisionModelManager->EntityString();
    
    // use the current msec count for a random seed
    // init for this gamestate
    game->Init( svs.time, Com_Milliseconds(), restart );
    
    // clear all gentity pointers that might still be set from
    // a previous level
    for( i = 0 ; i < sv_maxclients->integer ; i++ )
    {
        svs.clients[i].gentity = NULL;
    }
}



/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs( void )
{
    if( !gvm )
    {
        return;
    }
    game->Shutdown( true );
    
    SV_InitGameVM( true );
}

//
// SV_EndGame
//
void SV_EndGame( void )
{
    Com_Error( ERR_ENDGAME, "endgame" ); // no message, no error print
}

//
// SV_BotGetUserCommand
//
void SV_BotGetUserCommand( int clientNum, usercmd_t* ucmd )
{
    SV_ClientThink( &svs.clients[clientNum], ucmd );
}

//
// SV_PhysicsSetGravity
//
void SV_PhysicsSetGravity( const idVec3& gravity )
{
    physicsManager->SetGravity( gravity );
}

//
// SV_AllocTraceModel
//
idTraceModel* SV_AllocTraceModel( void )
{
    return physicsManager->AllocTraceModel();
}

//
// SV_ResetPhysics
//
void SV_ResetPhysics( void )
{
    physicsManager->Reset();
}

//
// SV_InitExportTable
//
void SV_InitExportTable( void )
{
    exports.Printf = Com_Printf;
    
    exports.Error = Com_Error;
    exports.Endgame = SV_EndGame;
    
    exports.Milliseconds = Sys_Milliseconds;
    exports.Argc = Cmd_Argc;
    
    exports.Argv = Cmd_ArgvBuffer;
    
    exports.FS_FOpenFile = FS_FOpenFileByMode;
    exports.FS_Read = FS_Read;
    exports.FS_Write = FS_Write;
    exports.FS_Rename = FS_Rename;
    exports.FS_FCloseFile = FS_FCloseFile;
    exports.FS_CopyFile = FS_CopyFileOS;
    exports.FS_GetFileList = FS_GetFileList;
    
    exports.SendConsoleCommand = Cbuf_ExecuteText;
    
    exports.Cvar_Register = Cvar_Register;
    exports.Cvar_Update = Cvar_Update;
    
    exports.Cvar_Set = Cvar_Set;
    
    exports.Cvar_VariableIntegerValue = Cvar_VariableIntegerValue;
    
    exports.Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;
    
    exports.LocateGameData = SV_LocateGameData;
    
    exports.DropClient = SV_GameDropClient;
    exports.SendServerCommand = SV_GameSendServerCommand;
    exports.SetConfigstring = SV_SetConfigstring;
    exports.GetConfigstring = SV_GetConfigstring;
    exports.GetUserinfo = SV_GetUserinfo;
    exports.SetUserinfo = SV_SetUserinfo;
    exports.GetServerinfo = SV_GetServerinfo;
    exports.SetBrushModel = SV_SetBrushModel;
    
    exports.Trace = SV_Trace;
    
    exports.PointContents = SV_PointContents;
    exports.InPVS = SV_inPVS;
    
    exports.InPVSIgnorePortals = SV_inPVSIgnorePortals;
    exports.AdjustAreaPortalState = SV_AdjustAreaPortalState;
    
    exports.LinkEntity = SV_LinkEntity;
    exports.UnlinkEntity = SV_UnlinkEntity;
    
    exports.EntitiesInBox = SV_AreaEntities;
    
    // Had to cast cause the real function is expecting a int for the last parem -- stupid.
    exports.EntityContact = ( bool( __cdecl* )( const vec_t[], const vec_t[], const sharedEntity_t*, bool ) )SV_EntityContact;
    
    exports.BotAllocateClient = SV_BotAllocateClient;
    exports.BotFreeClient = SV_BotFreeClient;
    
    exports.GetUsercmd = SV_GetUsercmd;
    exports.GetEntityToken = SV_GetEntityToken;
    
    exports.DebugPolygonCreate = BotImport_DebugPolygonCreate;
    exports.DebugPolygonDelete = BotImport_DebugPolygonDelete;
    
    exports.RealTime = Com_RealTime;
    
    exports.GetTag = SV_GetTag;
    
    exports.BotGetSnapshotEntity = SV_BotGetSnapshotEntity;
    exports.BotGetConsoleMessage = SV_BotGetConsoleMessage;
    
    exports.BotGetUserCommand = SV_BotGetUserCommand;
    
    exports.PhysicsSetGravity = SV_PhysicsSetGravity;
    exports.AllocTraceModel = SV_AllocTraceModel;
    
    exports.botlib = botlib_export;
    exports.collisionModelManager = collisionModelManager;
}


/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs( void )
{
    cvar_t*  var;
    //FIXME these are temp while I make bots run in vm
    extern int bot_enable;
    
    var = Cvar_Get( "bot_enable", "1", CVAR_LATCH );
    if( var )
    {
        bot_enable = var->integer;
    }
    else
    {
        bot_enable = 0;
    }
    
    // load the dll or bytecode
    gvm = Sys_LoadDll( "qagame" );
    if( !gvm )
    {
        Com_Error( ERR_FATAL, "VM_Create on game failed" );
    }
    
    // Get the entry point.
    gameDllEntry = ( idGame * ( __cdecl* )( gameImports_t* ) )Sys_GetProcAddress( gvm, "dllEntry" );
    if( !gameDllEntry )
    {
        Com_Error( ERR_FATAL, "VM_GetEntryPoint on game failed.\n" );
    }
    
    // Init the export table.
    SV_InitExportTable();
    
    game = gameDllEntry( &exports );
    
    SV_InitGameVM( false );
}


/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
bool SV_GameCommand( void )
{
    if( sv.state != SS_GAME )
    {
        return false;
    }
    
    return game->ConsoleCommand();
}


/*
====================
SV_SendMoveSpeedsToGame
====================
*/
void SV_SendMoveSpeedsToGame( int entnum, char* text )
{
    if( !gvm )
    {
        return;
    }
    game->RetrieveMoveSpeedsFromClient( entnum, text );
}

/*
====================
SV_GetTag

  return false if unable to retrieve tag information for this client
====================
*/
extern bool CL_GetTag( int clientNum, char* tagname, orientation_t* or );

bool SV_GetTag( int clientNum, char* tagname, orientation_t* or )
{
    if( com_dedicated->integer )
    {
        return false;
    }
    
    return CL_GetTag( clientNum, tagname, or );
}

/*
===================
SV_GetModelInfo

  request this modelinfo from the game
===================
*/
bool SV_GetModelInfo( int clientNum, char* modelName, animModelInfo_t** modelInfo )
{
    return game->GetModelInfo( clientNum, modelName, modelInfo );
}
