/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).

RTCW SP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW SP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW SP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"

gameImports_t* imports;

//
// dllEntry
//
idGame* dllEntry( gameImports_t* gimports )
{
    imports = gimports;
    
    return game;
}

void    trap_Printf( const char* fmt )
{
    imports->Printf( fmt );
}

void    trap_Error( const char* fmt )
{
    imports->Error( ERR_DROP, fmt );
}

void    trap_Endgame( void )
{
    imports->Endgame();
}

int     trap_Milliseconds( void )
{
    return imports->Milliseconds();
}
int     trap_Argc( void )
{
    return imports->Argc();
}

void    trap_Argv( int n, char* buffer, int bufferLength )
{
    imports->Argv( n, buffer, bufferLength );
}

int     trap_FS_FOpenFile( const char* qpath, fileHandle_t* f, fsMode_t mode )
{
    return imports->FS_FOpenFile( qpath, f, mode );
}

void    trap_FS_Read( void* buffer, int len, fileHandle_t f )
{
    imports->FS_Read( buffer, len, f );
}

int     trap_FS_Write( const void* buffer, int len, fileHandle_t f )
{
    return imports->FS_Write( buffer, len, f );
}

int     trap_FS_Rename( const char* from, const char* to )
{
    imports->FS_Rename( from, to );
    return 0;
}

void    trap_FS_FCloseFile( fileHandle_t f )
{
    imports->FS_FCloseFile( f );
}

void    trap_FS_CopyFile( char* from, char* to )    //DAJ
{
    imports->FS_CopyFile( from, to );
}

int trap_FS_GetFileList( const char* path, const char* extension, char* listbuf, int bufsize )
{
    return imports->FS_GetFileList( path, extension, listbuf, bufsize );
}

void    trap_SendConsoleCommand( int exec_when, const char* text )
{
    imports->SendConsoleCommand( exec_when, text );
}

void    trap_Cvar_Register( vmCvar_t* cvar, const char* var_name, const char* value, int flags )
{
    imports->Cvar_Register( cvar, var_name, value, flags );
}

void    trap_Cvar_Update( vmCvar_t* cvar )
{
    imports->Cvar_Update( cvar );
}

void trap_Cvar_Set( const char* var_name, const char* value )
{
    imports->Cvar_Set( var_name, value );
}

int trap_Cvar_VariableIntegerValue( const char* var_name )
{
    return imports->Cvar_VariableIntegerValue( var_name );
}

void trap_Cvar_VariableStringBuffer( const char* var_name, char* buffer, int bufsize )
{
    imports->Cvar_VariableStringBuffer( var_name, buffer, bufsize );
}


void trap_LocateGameData( gentity_t* gEnts, int numGEntities, int sizeofGEntity_t,
                          playerState_t* clients, int sizeofGClient )
{
    imports->LocateGameData( gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGClient );
}

void trap_DropClient( int clientNum, const char* reason )
{
    imports->DropClient( clientNum, reason );
}

void trap_SendServerCommand( int clientNum, const char* text )
{
    imports->SendServerCommand( clientNum, text );
}

void trap_SetConfigstring( int num, const char* string )
{
    imports->SetConfigstring( num, string );
}

void trap_GetConfigstring( int num, char* buffer, int bufferSize )
{
    imports->GetConfigstring( num, buffer, bufferSize );
}

void trap_GetUserinfo( int num, char* buffer, int bufferSize )
{
    imports->GetUserinfo( num, buffer, bufferSize );
}

void trap_SetUserinfo( int num, const char* buffer )
{
    imports->SetUserinfo( num, buffer );
}

void trap_GetServerinfo( char* buffer, int bufferSize )
{
    imports->GetServerinfo( buffer, bufferSize );
}

void trap_SetBrushModel( gentity_t* ent, const char* name )
{
    imports->SetBrushModel( ent, name );
}

void trap_Trace( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask )
{
    imports->Trace( results, start, mins, maxs, end, passEntityNum, contentmask, false );
}

void trap_TraceCapsule( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask )
{
    imports->Trace( results, start, mins, maxs, end, passEntityNum, contentmask, true );
}

int trap_PointContents( const vec3_t point, int passEntityNum )
{
    return imports->PointContents( point, passEntityNum );
}

bool trap_InPVS( const vec3_t p1, const vec3_t p2 )
{
    return imports->InPVS( p1, p2 );
}

bool trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 )
{
    return imports->InPVSIgnorePortals( p1, p2 );
}

void trap_AdjustAreaPortalState( gentity_t* ent, bool open )
{
    imports->AdjustAreaPortalState( ent, open );
}

bool trap_AreasConnected( int area1, int area2 )
{
    return imports->collisionModelManager->AreasConnected( area1, area2 );
}

void trap_LinkEntity( gentity_t* ent )
{
    imports->LinkEntity( ent );
}

void trap_UnlinkEntity( gentity_t* ent )
{
    imports->UnlinkEntity( ent );
}


int trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int* list, int maxcount )
{
    return imports->EntitiesInBox( mins, maxs, list, maxcount );
}

bool trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t* ent )
{
    return imports->EntityContact( mins, maxs, ent, false );
}

bool trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t* ent )
{
    return imports->EntityContact( mins, maxs, ent, true );
}

int trap_BotAllocateClient( void )
{
    return imports->BotAllocateClient();
}

void trap_BotFreeClient( int clientNum )
{
    imports->BotFreeClient( clientNum );
}

void trap_GetUsercmd( int clientNum, usercmd_t* cmd )
{
    imports->GetUsercmd( clientNum, cmd );
}

bool trap_GetEntityToken( char* buffer, int bufferSize )
{
    return imports->GetEntityToken( buffer, bufferSize );
}

int trap_DebugPolygonCreate( int color, int numPoints, vec3_t* points )
{
    return imports->DebugPolygonCreate( color, numPoints, points );
}

void trap_DebugPolygonDelete( int id )
{
    imports->DebugPolygonDelete( id );
}

int trap_RealTime( qtime_t* qtime )
{
    return imports->RealTime( qtime );
}

void trap_SnapVector( float* v )
{
    SnapVector( v );
    return;
}

bool trap_GetTag( int clientNum, char* tagName, orientation_t* or )
{
    return imports->GetTag( clientNum, tagName, or );
}

// BotLib traps start here
int trap_BotLibSetup( void )
{
    return imports->botlib->BotLibSetup();
}

int trap_BotLibShutdown( void )
{
    return imports->botlib->BotLibShutdown();
}

int trap_BotLibVarSet( char* var_name, char* value )
{
    return imports->botlib->BotLibVarSet( var_name, value );
}

int trap_BotLibVarGet( char* var_name, char* value, int size )
{
    return imports->botlib->BotLibVarGet( var_name, value, size );
}

int trap_BotLibDefine( char* string )
{
    return imports->botlib->PC_AddGlobalDefine( string );
}

int trap_BotLibStartFrame( float time )
{
    return imports->botlib->BotLibStartFrame( time );
}

int trap_BotLibLoadMap( const char* mapname )
{
    return imports->botlib->BotLibLoadMap( mapname );
}

int trap_BotLibUpdateEntity( int ent, void /* struct bot_updateentity_s */ *bue )
{
    return imports->botlib->BotLibUpdateEntity( ent, ( bot_entitystate_t* )bue );
}

int trap_BotGetSnapshotEntity( int clientNum, int sequence )
{
    return imports->BotGetSnapshotEntity( clientNum, sequence );
}

int trap_BotGetServerCommand( int clientNum, char* message, int size )
{
    return imports->BotGetConsoleMessage( clientNum, message, size );
}

void trap_BotUserCommand( int clientNum, usercmd_t* ucmd )
{
    imports->BotGetUserCommand( clientNum, ucmd );
}

void trap_AAS_EntityInfo( int entnum, void /* struct aas_entityinfo_s */ *info )
{
    imports->botlib->aas.AAS_EntityInfo( entnum, ( aas_entityinfo_s* )info );
}

int trap_AAS_Initialized( void )
{
    return imports->botlib->aas.AAS_Initialized();
}

void trap_AAS_PresenceTypeBoundingBox( int presencetype, vec3_t mins, vec3_t maxs )
{
    imports->botlib->aas.AAS_PresenceTypeBoundingBox( presencetype, mins, maxs );
}

float trap_AAS_Time( void )
{
    return imports->botlib->aas.AAS_Time();
}

// Ridah, multiple AAS files
void trap_AAS_SetCurrentWorld( int index )
{
    imports->botlib->aas.AAS_SetCurrentWorld( index );
}
// done.

int trap_AAS_PointAreaNum( vec3_t point )
{
    return imports->botlib->aas.AAS_PointAreaNum( point );
}

int trap_AAS_TraceAreas( vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas )
{
    return imports->botlib->aas.AAS_TraceAreas( start, end, areas, points, maxareas );
}

int trap_AAS_PointContents( vec3_t point )
{
    return imports->botlib->aas.AAS_PointContents( point );
}

int trap_AAS_NextBSPEntity( int ent )
{
    return imports->botlib->aas.AAS_NextBSPEntity( ent );
}

int trap_AAS_ValueForBSPEpairKey( int ent, char* key, char* value, int size )
{
    return imports->botlib->aas.AAS_ValueForBSPEpairKey( ent, key, value, size );
}

int trap_AAS_VectorForBSPEpairKey( int ent, char* key, vec3_t v )
{
    return imports->botlib->aas.AAS_VectorForBSPEpairKey( ent, key, v );
}

int trap_AAS_FloatForBSPEpairKey( int ent, char* key, float* value )
{
    return imports->botlib->aas.AAS_FloatForBSPEpairKey( ent, key, value );
}

int trap_AAS_IntForBSPEpairKey( int ent, char* key, int* value )
{
    return imports->botlib->aas.AAS_IntForBSPEpairKey( ent, key, value );
}

int trap_AAS_AreaReachability( int areanum )
{
    return imports->botlib->aas.AAS_AreaReachability( areanum );
}

int trap_AAS_AreaTravelTimeToGoalArea( int areanum, vec3_t origin, int goalareanum, int travelflags )
{
    return imports->botlib->aas.AAS_AreaTravelTimeToGoalArea( areanum, origin, goalareanum, travelflags );
}

int trap_AAS_Swimming( vec3_t origin )
{
    return imports->botlib->aas.AAS_Swimming( origin );
}

int trap_AAS_PredictClientMovement( void /* struct aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize )
{
    return imports->botlib->aas.AAS_PredictClientMovement( ( aas_clientmove_s* )move, entnum, origin, presencetype, onground, velocity, cmdmove, cmdframes, maxframes, frametime, stopevent, stopareanum, visualize );
}

// Ridah, route-tables
void trap_AAS_RT_ShowRoute( vec3_t srcpos, int srcnum, int destnum )
{
    imports->botlib->aas.AAS_RT_ShowRoute( srcpos, srcnum, destnum );
}

bool trap_AAS_RT_GetHidePos( vec3_t srcpos, int srcnum, int srcarea, vec3_t destpos, int destnum, int destarea, vec3_t returnPos )
{
    return imports->botlib->aas.AAS_RT_GetHidePos( srcpos, srcnum, srcarea, destpos, destnum, destarea, returnPos );
}

int trap_AAS_FindAttackSpotWithinRange( int srcnum, int rangenum, int enemynum, float rangedist, int travelflags, float* outpos )
{
    return imports->botlib->aas.AAS_FindAttackSpotWithinRange( srcnum, rangenum, enemynum, rangedist, travelflags, outpos );
}

bool trap_AAS_GetRouteFirstVisPos( vec3_t srcpos, vec3_t destpos, int travelflags, vec3_t retpos )
{
    return imports->botlib->aas.AAS_GetRouteFirstVisPos( srcpos, destpos, travelflags, retpos );
}

void trap_AAS_SetAASBlockingEntity( vec3_t absmin, vec3_t absmax, bool blocking )
{
    imports->botlib->aas.AAS_SetAASBlockingEntity( absmin, absmax, blocking );
}
// done.

void trap_EA_Say( int client, char* str )
{
    imports->botlib->ea.EA_Say( client, str );
}

void trap_EA_SayTeam( int client, char* str )
{
    imports->botlib->ea.EA_SayTeam( client, str );
}

void trap_EA_UseItem( int client, char* it )
{
    imports->botlib->ea.EA_UseItem( client, it );
}

void trap_EA_DropItem( int client, char* it )
{
    imports->botlib->ea.EA_DropItem( client, it );
}

void trap_EA_UseInv( int client, char* inv )
{
    imports->botlib->ea.EA_UseInv( client, inv );
}

void trap_EA_DropInv( int client, char* inv )
{
    imports->botlib->ea.EA_DropInv( client, inv );
}

void trap_EA_Gesture( int client )
{
    imports->botlib->ea.EA_Gesture( client );
}

void trap_EA_Command( int client, char* command )
{
    imports->botlib->ea.EA_Command( client, command );
}

void trap_EA_SelectWeapon( int client, int weapon )
{
    imports->botlib->ea.EA_SelectWeapon( client, weapon );
}

void trap_EA_Talk( int client )
{
    imports->botlib->ea.EA_Talk( client );
}

void trap_EA_Attack( int client )
{
    imports->botlib->ea.EA_Attack( client );
}

void trap_EA_Reload( int client )
{
    imports->botlib->ea.EA_Reload( client );
}

void trap_EA_Use( int client )
{
    imports->botlib->ea.EA_Use( client );
}

void trap_EA_Respawn( int client )
{
    imports->botlib->ea.EA_Respawn( client );
}

void trap_EA_Jump( int client )
{
    imports->botlib->ea.EA_Jump( client );
}

void trap_EA_DelayedJump( int client )
{
    imports->botlib->ea.EA_DelayedJump( client );
}

void trap_EA_Crouch( int client )
{
    imports->botlib->ea.EA_Crouch( client );
}

void trap_EA_MoveUp( int client )
{
    imports->botlib->ea.EA_MoveUp( client );
}

void trap_EA_MoveDown( int client )
{
    imports->botlib->ea.EA_MoveDown( client );
}

void trap_EA_MoveForward( int client )
{
    imports->botlib->ea.EA_MoveForward( client );
}

void trap_EA_MoveBack( int client )
{
    imports->botlib->ea.EA_MoveBack( client );
}

void trap_EA_MoveLeft( int client )
{
    imports->botlib->ea.EA_MoveLeft( client );
}

void trap_EA_MoveRight( int client )
{
    imports->botlib->ea.EA_MoveRight( client );
}

void trap_EA_Move( int client, vec3_t dir, float speed )
{
    imports->botlib->ea.EA_Move( client, dir, speed );
}

void trap_EA_View( int client, vec3_t viewangles )
{
    imports->botlib->ea.EA_View( client, viewangles );
}

void trap_EA_EndRegular( int client, float thinktime )
{
    imports->botlib->ea.EA_EndRegular( client, thinktime );
}

void trap_EA_GetInput( int client, float thinktime, void /* struct bot_input_s */ *input )
{
    imports->botlib->ea.EA_GetInput( client, thinktime, ( bot_input_t* )input );
}

void trap_EA_ResetInput( int client, void* init )
{
    imports->botlib->ea.EA_ResetInput( client, ( bot_input_t* )init );
}

int trap_BotLoadCharacter( char* charfile, int skill )
{
    return imports->botlib->ai.BotLoadCharacter( charfile, skill );
}

void trap_BotFreeCharacter( int character )
{
    imports->botlib->ai.BotFreeCharacter( character );
}

float trap_Characteristic_Float( int character, int index )
{
    return imports->botlib->ai.Characteristic_Float( character, index );
}

float trap_Characteristic_BFloat( int character, int index, float min, float max )
{
    return imports->botlib->ai.Characteristic_BFloat( character, index, min, max );
}

int trap_Characteristic_Integer( int character, int index )
{
    return imports->botlib->ai.Characteristic_Integer( character, index );
}

int trap_Characteristic_BInteger( int character, int index, int min, int max )
{
    return imports->botlib->ai.Characteristic_BInteger( character, index, min, max );
}

void trap_Characteristic_String( int character, int index, char* buf, int size )
{
    imports->botlib->ai.Characteristic_String( character, index, buf, size );
}

int trap_BotAllocChatState( void )
{
    return imports->botlib->ai.BotAllocChatState( );
}

void trap_BotFreeChatState( int handle )
{
    imports->botlib->ai.BotFreeChatState( handle );
}

void trap_BotQueueConsoleMessage( int chatstate, int type, char* message )
{
    imports->botlib->ai.BotQueueConsoleMessage( chatstate, type, message );
}

void trap_BotRemoveConsoleMessage( int chatstate, int handle )
{
    imports->botlib->ai.BotRemoveConsoleMessage( chatstate, handle );
}

int trap_BotNextConsoleMessage( int chatstate, void /* struct bot_consolemessage_s */ *cm )
{
    return imports->botlib->ai.BotNextConsoleMessage( chatstate, ( bot_consolemessage_s* )cm );
}

int trap_BotNumConsoleMessages( int chatstate )
{
    return imports->botlib->ai.BotNumConsoleMessages( chatstate );
}

void trap_BotInitialChat( int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7 )
{
    imports->botlib->ai.BotInitialChat( chatstate, type, mcontext, var0, var1, var2, var3, var4, var5, var6, var7 );
}

int trap_BotNumInitialChats( int chatstate, char* type )
{
    return imports->botlib->ai.BotNumInitialChats( chatstate, type );
}

int trap_BotReplyChat( int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7 )
{
    return imports->botlib->ai.BotReplyChat( chatstate, message, mcontext, vcontext, var0, var1, var2, var3, var4, var5, var6, var7 );
}

int trap_BotChatLength( int chatstate )
{
    return imports->botlib->ai.BotChatLength( chatstate );
}

void trap_BotEnterChat( int chatstate, int client, int sendto )
{
    imports->botlib->ai.BotEnterChat( chatstate, client, sendto );
}

void trap_BotGetChatMessage( int chatstate, char* buf, int size )
{
    imports->botlib->ai.BotGetChatMessage( chatstate, buf, size );
}

int trap_StringContains( char* str1, char* str2, int casesensitive )
{
    return imports->botlib->ai.StringContains( str1, str2, casesensitive );
}

int trap_BotFindMatch( char* str, void /* struct bot_match_s */ *match, unsigned long int context )
{
    return imports->botlib->ai.BotFindMatch( str, ( struct bot_match_s* )match, context );
}

void trap_BotMatchVariable( void /* struct bot_match_s */ *match, int variable, char* buf, int size )
{
    imports->botlib->ai.BotMatchVariable( ( struct bot_match_s* )match, variable, buf, size );
}

void trap_UnifyWhiteSpaces( char* string )
{
    imports->botlib->ai.UnifyWhiteSpaces( string );
}

void trap_BotReplaceSynonyms( char* string, unsigned long int context )
{
    imports->botlib->ai.BotReplaceSynonyms( string, context );
}

int trap_BotLoadChatFile( int chatstate, char* chatfile, char* chatname )
{
    return imports->botlib->ai.BotLoadChatFile( chatstate, chatfile, chatname );
}

void trap_BotSetChatGender( int chatstate, int gender )
{
    imports->botlib->ai.BotSetChatGender( chatstate, gender );
}

void trap_BotSetChatName( int chatstate, char* name )
{
    imports->botlib->ai.BotSetChatName( chatstate, name );
}

void trap_BotResetGoalState( int goalstate )
{
    imports->botlib->ai.BotResetGoalState( goalstate );
}

void trap_BotResetAvoidGoals( int goalstate )
{
    imports->botlib->ai.BotResetAvoidGoals( goalstate );
}

void trap_BotRemoveFromAvoidGoals( int goalstate, int number )
{
    imports->botlib->ai.BotRemoveFromAvoidGoals( goalstate, number );
}

void trap_BotPushGoal( int goalstate, void /* struct bot_goal_s */ *goal )
{
    imports->botlib->ai.BotPushGoal( goalstate, ( bot_goal_s* )goal );
}

void trap_BotPopGoal( int goalstate )
{
    imports->botlib->ai.BotPopGoal( goalstate );
}

void trap_BotEmptyGoalStack( int goalstate )
{
    imports->botlib->ai.BotEmptyGoalStack( goalstate );
}

void trap_BotDumpAvoidGoals( int goalstate )
{
    imports->botlib->ai.BotDumpAvoidGoals( goalstate );
}

void trap_BotDumpGoalStack( int goalstate )
{
    imports->botlib->ai.BotDumpGoalStack( goalstate );
}

void trap_BotGoalName( int number, char* name, int size )
{
    imports->botlib->ai.BotGoalName( number, name, size );
}

int trap_BotGetTopGoal( int goalstate, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotGetTopGoal( goalstate, ( bot_goal_s* )goal );
}

int trap_BotGetSecondGoal( int goalstate, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotGetSecondGoal( goalstate, ( bot_goal_s* )goal );
}

int trap_BotChooseLTGItem( int goalstate, vec3_t origin, int* inventory, int travelflags )
{
    return imports->botlib->ai.BotChooseLTGItem( goalstate, origin, inventory, travelflags );
}

int trap_BotChooseNBGItem( int goalstate, vec3_t origin, int* inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime )
{
    return imports->botlib->ai.BotChooseNBGItem( goalstate, origin, inventory, travelflags, ( bot_goal_s* )ltg, maxtime );
}

int trap_BotTouchingGoal( vec3_t origin, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotTouchingGoal( origin, ( struct bot_goal_s* )goal );
}

int trap_BotItemGoalInVisButNotVisible( int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotItemGoalInVisButNotVisible( viewer, eye, viewangles, ( struct bot_goal_s* )goal );
}

int trap_BotGetLevelItemGoal( int index, char* classname, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotGetLevelItemGoal( index, classname, ( struct bot_goal_s* )goal );
}

int trap_BotGetNextCampSpotGoal( int num, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotGetNextCampSpotGoal( num, ( struct bot_goal_s* )goal );
}

int trap_BotGetMapLocationGoal( char* name, void /* struct bot_goal_s */ *goal )
{
    return imports->botlib->ai.BotGetMapLocationGoal( ( char* )name, ( struct bot_goal_s* )goal );
}

float trap_BotAvoidGoalTime( int goalstate, int number )
{
    return imports->botlib->ai.BotAvoidGoalTime( goalstate, number );
}

void trap_BotInitLevelItems( void )
{
    imports->botlib->ai.BotInitLevelItems( );
}

void trap_BotUpdateEntityItems( void )
{
    imports->botlib->ai.BotUpdateEntityItems( );
}

int trap_BotLoadItemWeights( int goalstate, char* filename )
{
    return imports->botlib->ai.BotLoadItemWeights( goalstate, filename );
}

void trap_BotFreeItemWeights( int goalstate )
{
    imports->botlib->ai.BotFreeItemWeights( goalstate );
}

void trap_BotInterbreedGoalFuzzyLogic( int parent1, int parent2, int child )
{
    imports->botlib->ai.BotInterbreedGoalFuzzyLogic( parent1, parent2, child );
}

void trap_BotSaveGoalFuzzyLogic( int goalstate, char* filename )
{
    imports->botlib->ai.BotSaveGoalFuzzyLogic( goalstate, filename );
}

void trap_BotMutateGoalFuzzyLogic( int goalstate, float range )
{
    imports->botlib->ai.BotMutateGoalFuzzyLogic( goalstate, range );
}

int trap_BotAllocGoalState( int state )
{
    return imports->botlib->ai.BotAllocGoalState( state );
}

void trap_BotFreeGoalState( int handle )
{
    imports->botlib->ai.BotFreeGoalState( handle );
}

void trap_BotResetMoveState( int movestate )
{
    imports->botlib->ai.BotResetMoveState( movestate );
}

void trap_BotMoveToGoal( void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags )
{
    imports->botlib->ai.BotMoveToGoal( ( bot_moveresult_s* )result, movestate, ( bot_goal_s* )goal, travelflags );
}

int trap_BotMoveInDirection( int movestate, vec3_t dir, float speed, int type )
{
    return imports->botlib->ai.BotMoveInDirection( movestate, dir, speed, type );
}

void trap_BotResetAvoidReach( int movestate )
{
    imports->botlib->ai.BotResetAvoidReach( movestate );
}

void trap_BotResetLastAvoidReach( int movestate )
{
    imports->botlib->ai.BotResetLastAvoidReach( movestate );
}

int trap_BotReachabilityArea( vec3_t origin, int testground )
{
    return imports->botlib->ai.BotReachabilityArea( origin, testground );
}

int trap_BotMovementViewTarget( int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target )
{
    return imports->botlib->ai.BotMovementViewTarget( movestate, ( bot_goal_s* )goal, travelflags, lookahead, target );
}

int trap_BotPredictVisiblePosition( vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target )
{
    return imports->botlib->ai.BotPredictVisiblePosition( origin, areanum, ( bot_goal_s* )goal, travelflags, target );
}

int trap_BotAllocMoveState( void )
{
    return imports->botlib->ai.BotAllocMoveState();
}

void trap_BotFreeMoveState( int handle )
{
    imports->botlib->ai.BotFreeMoveState( handle );
}

void trap_BotInitMoveState( int handle, void /* struct bot_initmove_s */ *initmove )
{
    imports->botlib->ai.BotInitMoveState( handle, ( bot_initmove_s* )initmove );
}

// Ridah
void trap_BotInitAvoidReach( int handle )
{
    imports->botlib->ai.BotInitAvoidReach( handle );
}
// Done.

int trap_BotChooseBestFightWeapon( int weaponstate, int* inventory )
{
    return imports->botlib->ai.BotChooseBestFightWeapon( weaponstate, inventory );
}

void trap_BotGetWeaponInfo( int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo )
{
    imports->botlib->ai.BotGetWeaponInfo( weaponstate, weapon, ( weaponinfo_s* )weaponinfo );
}

int trap_BotLoadWeaponWeights( int weaponstate, char* filename )
{
    return imports->botlib->ai.BotLoadWeaponWeights( weaponstate, filename );
}

int trap_BotAllocWeaponState( void )
{
    return imports->botlib->ai.BotAllocWeaponState();
}

void trap_BotFreeWeaponState( int weaponstate )
{
    imports->botlib->ai.BotFreeWeaponState( weaponstate );
}

void trap_BotResetWeaponState( int weaponstate )
{
    imports->botlib->ai.BotResetWeaponState( weaponstate );
}

int trap_GeneticParentsAndChildSelection( int numranks, float* ranks, int* parent1, int* parent2, int* child )
{
    return imports->botlib->ai.GeneticParentsAndChildSelection( numranks, ranks, parent1, parent2, child );
}
