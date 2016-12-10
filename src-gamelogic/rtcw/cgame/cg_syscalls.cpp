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
//  File name:   cg_syscalls.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: this file is only included when building a dll
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "cg_local.h"

// cg_syscalls.c -- this file is only included when building a dll
// cg_syscalls.asm is included instead when building a qvm
#include "cg_local.h"
#include "../../../src-engine/splines/splines.h"

#define PASSFLOAT(x) x

cgameImports_t* imports;
idCollisionModelManager* collisionModelManager;

idCGame* dllEntry( cgameImports_t* cgimports )
{
    imports = cgimports;
    
    collisionModelManager = imports->collisionModelManager;
    return cgame;
}


void    trap_Print( const char* fmt )
{
    imports->Print( fmt );
}

void    trap_Error( const char* fmt )
{
    imports->Error( ERR_DROP, fmt );
}

int     trap_Milliseconds( void )
{
    return imports->Milliseconds();
}

void    trap_Cvar_Register( vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags )
{
    imports->Cvar_Register( vmCvar, varName, defaultValue, flags );
}

void    trap_Cvar_Update( vmCvar_t* vmCvar )
{
    imports->Cvar_Update( vmCvar );
}

void    trap_Cvar_Set( const char* var_name, const char* value )
{
    imports->Cvar_Set( var_name, value );
}

void trap_Cvar_VariableStringBuffer( const char* var_name, char* buffer, int bufsize )
{
    imports->Cvar_VariableStringBuffer( var_name, buffer, bufsize );
}

int     trap_Argc( void )
{
    return imports->Argc();
}

void    trap_Argv( int n, char* buffer, int bufferLength )
{
    imports->Argv( n, buffer, bufferLength );
}

void    trap_Args( char* buffer, int bufferLength )
{
    imports->Args( buffer, bufferLength );
}

int     trap_FS_FOpenFile( const char* qpath, fileHandle_t* f, fsMode_t mode )
{
    return imports->FS_FOpenFile( qpath, f, mode );
}

void    trap_FS_Read( void* buffer, int len, fileHandle_t f )
{
    imports->FS_Read( buffer, len, f );
}

void    trap_FS_Write( const void* buffer, int len, fileHandle_t f )
{
    imports->FS_Write( buffer, len, f );
}

void    trap_FS_FCloseFile( fileHandle_t f )
{
    imports->FS_FCloseFile( f );
}

void    trap_SendConsoleCommand( const char* text )
{
    imports->SendConsoleCommand( text );
}

void    trap_AddCommand( const char* cmdName )
{
    imports->AddCommand( cmdName );
}

void    trap_SendClientCommand( const char* s )
{
    imports->SendClientCommand( s );
}

void    trap_UpdateScreen( void )
{
    imports->UpdateScreen();
}

void    trap_CM_LoadMap( const char* mapname )
{
    int checksum;
    
    imports->collisionModelManager->LoadMap( mapname, true, &checksum );
}

int     trap_CM_NumInlineModels( void )
{
    return imports->collisionModelManager->NumInlineModels();
}

clipHandle_t trap_CM_InlineModel( int index )
{
    return imports->collisionModelManager->InlineModel( index );
}

clipHandle_t trap_CM_TempBoxModel( const vec3_t mins, const vec3_t maxs )
{
    return imports->collisionModelManager->TempBoxModel( mins, maxs, false );
}

clipHandle_t trap_CM_TempCapsuleModel( const vec3_t mins, const vec3_t maxs )
{
    return imports->collisionModelManager->TempBoxModel( mins, maxs, true );
}

int     trap_CM_PointContents( const vec3_t p, clipHandle_t model )
{
    return imports->collisionModelManager->PointContents( p, model );
}

int     trap_CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles )
{
    return imports->collisionModelManager->TransformedPointContents( p, model, origin, angles );
}

void    trap_CM_BoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
                          const vec3_t mins, const vec3_t maxs,
                          clipHandle_t model, int brushmask )
{
    imports->collisionModelManager->BoxTrace( results, start, end, mins, maxs, model, brushmask, false );
}

void    trap_CM_TransformedBoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
                                     const vec3_t mins, const vec3_t maxs,
                                     clipHandle_t model, int brushmask,
                                     const vec3_t origin, const vec3_t angles )
{
    imports->collisionModelManager->TransformedBoxTrace( results, start, end, mins, maxs, model, brushmask, origin, angles, false );
}

void    trap_CM_CapsuleTrace( trace_t* results, const vec3_t start, const vec3_t end,
                              const vec3_t mins, const vec3_t maxs,
                              clipHandle_t model, int brushmask )
{
    imports->collisionModelManager->BoxTrace( results, start, end, mins, maxs, model, brushmask, true );
}

void    trap_CM_TransformedCapsuleTrace( trace_t* results, const vec3_t start, const vec3_t end,
        const vec3_t mins, const vec3_t maxs,
        clipHandle_t model, int brushmask,
        const vec3_t origin, const vec3_t angles )
{
    imports->collisionModelManager->TransformedBoxTrace( results, start, end, mins, maxs, model, brushmask, origin, angles, true );
}

int     trap_CM_MarkFragments( int numPoints, const vec3_t* points,
                               const vec3_t projection,
                               int maxPoints, vec3_t pointBuffer,
                               int maxFragments, markFragment_t* fragmentBuffer )
{
    return imports->renderSystem->MarkFragments( numPoints, points, projection, maxPoints, pointBuffer, maxFragments, fragmentBuffer );
}

void    trap_S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx )
{
    imports->soundSystem->StartSound( origin, entityNum, entchannel, sfx );
}

//----(SA)	added
void    trap_S_StartSoundEx( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int flags )
{
    imports->soundSystem->StartSoundEx( origin, entityNum, entchannel, sfx, flags );
}
//----(SA)	end

void    trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum )
{
    imports->soundSystem->StartLocalSound( sfx, channelNum );
}

void    trap_S_ClearLoopingSounds( bool killall )
{
    imports->soundSystem->ClearLoopingSounds(); // (SA) modified so no_pvs sounds can function
    // RF, if killall, then stop all sounds
    if( killall )
    {
        imports->soundSystem->ClearSounds( true, false );
    }
    //else if ( args[1] == 2 ) { // wtf???
    //imports->soundSystem->ClearSounds( true,true );
    //}
}

void    trap_S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int volume )
{
    imports->soundSystem->AddLoopingSound( entityNum, origin, velocity, 1250, sfx, volume );   // volume was previously removed from CG_S_ADDLOOPINGSOUND.  I added 'range'
}

//----(SA)	added
void    trap_S_AddRangedLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int range )
{
    imports->soundSystem->AddLoopingSound( entityNum, origin, velocity, range, sfx, 255 ); // RF, assume full volume, since thats how it worked before
}
//----(SA)	end

void    trap_S_AddRealLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx )
{
    // not in use
    //	imports->soundSystem->ADDREALLOOPINGSOUND, entityNum, origin, velocity, 1250, sfx, 255 );	//----(SA)	modified
}

void    trap_S_StopLoopingSound( int entityNum )
{
    //	imports->soundSystem->StopLoopingSound( entityNum );
}

//----(SA)	added
void    trap_S_StopStreamingSound( int entityNum )
{
    imports->soundSystem->StopEntStreamingSound( entityNum );
}
//----(SA)	end

void    trap_S_UpdateEntityPosition( int entityNum, const vec3_t origin )
{
    imports->soundSystem->UpdateEntityPosition( entityNum, origin );
}

// Ridah, talking animations
int     trap_S_GetVoiceAmplitude( int entityNum )
{
    return imports->soundSystem->GetVoiceAmplitude( entityNum );
}
// done.

void    trap_S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater )
{
    imports->soundSystem->Respatialize( entityNum, origin, axis, inwater );
}

sfxHandle_t trap_S_RegisterSound( const char* sample )
{
    CG_DrawInformation();
    return imports->soundSystem->RegisterSound( sample );
}

void    trap_S_StartBackgroundTrack( const char* intro, const char* loop, int fadeupTime )
{
    imports->soundSystem->StartBackgroundTrack( intro, loop, fadeupTime );
}

void    trap_S_FadeBackgroundTrack( float targetvol, int time, int num )   // yes, i know.  fadebackground coming in, fadestreaming going out.  will have to see where functionality leads...
{
    imports->soundSystem->FadeStreamingSound( PASSFLOAT( targetvol ), time, num ); // 'num' is '0' if it's music, '1' if it's "all streaming sounds"
}

void    trap_S_FadeAllSound( float targetvol, int time )
{
    imports->soundSystem->FadeAllSounds( PASSFLOAT( targetvol ), time );
}

//----(SA)	end

void    trap_S_StartStreamingSound( const char* intro, const char* loop, int entnum, int channel, int attenuation )
{
    imports->soundSystem->StartStreamingSound( intro, loop, entnum, channel, attenuation );
}

void    trap_R_LoadWorldMap( const char* mapname )
{
    imports->renderSystem->LoadWorld( mapname );
}

qhandle_t trap_R_RegisterModel( const char* name )
{
    CG_DrawInformation();
    return imports->renderSystem->RegisterModel( name );
}

//----(SA)	added
bool trap_R_GetSkinModel( qhandle_t skinid, const char* type, char* name )
{
    return imports->renderSystem->GetSkinModel( skinid, type, name );
}

qhandle_t trap_R_GetShaderFromModel( qhandle_t modelid, int surfnum, int withlightmap )
{
    return imports->renderSystem->GetShaderFromModel( modelid, surfnum, withlightmap );
}
//----(SA)	end

qhandle_t trap_R_RegisterSkin( const char* name )
{
    CG_DrawInformation();
    return imports->renderSystem->RegisterSkin( name );
}

qhandle_t trap_R_RegisterShader( const char* name )
{
    CG_DrawInformation();
    return imports->renderSystem->RegisterShader( name );
}

qhandle_t trap_R_RegisterShaderNoMip( const char* name )
{
    CG_DrawInformation();
    return imports->renderSystem->RegisterShaderNoMip( name );
}

void trap_R_RegisterFont( const char* fontName, int pointSize, fontInfo_t* font )
{
    imports->renderSystem->RegisterFont( fontName, pointSize, font );
}

void    trap_R_ClearScene( void )
{
    imports->renderSystem->ClearScene();
}

void    trap_R_AddRefEntityToScene( const refEntity_t* re )
{
    imports->renderSystem->AddRefEntityToScene( re );
}

void    trap_R_AddPolyToScene( qhandle_t hShader, int numVerts, const polyVert_t* verts )
{
    imports->renderSystem->AddPolyToScene( hShader, numVerts, verts );
}

// Ridah
void    trap_R_AddPolysToScene( qhandle_t hShader, int numVerts, const polyVert_t* verts, int numPolys )
{
    imports->renderSystem->AddPolysToScene( hShader, numVerts, verts, numPolys );
}

void    trap_RB_ZombieFXAddNewHit( int entityNum, const vec3_t hitPos, const vec3_t hitDir )
{
    imports->renderSystem->ZombieFXAddNewHit( entityNum, hitPos, hitDir );
}
// done.

void    trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b, int overdraw )
{
    imports->renderSystem->AddLightToScene( org, PASSFLOAT( intensity ), PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), overdraw );
}

qhandle_t trap_R_LoadAnim( qhandle_t modelIndex, const char* name )
{
    return imports->renderSystem->LoadAnim( modelIndex, name );
}

//----(SA)
void    trap_R_AddCoronaToScene( const vec3_t org, float r, float g, float b, float scale, int id, int flags )
{
    imports->renderSystem->AddCoronaToScene( org, PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), PASSFLOAT( scale ), id, flags );
}
//----(SA)

//----(SA)
void    trap_R_SetFog( int fogvar, int var1, int var2, float r, float g, float b, float density )
{
    imports->renderSystem->SetFog( fogvar, var1, var2, PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), PASSFLOAT( density ) );
}
//----(SA)
void    trap_R_RenderScene( const refdef_t* fd )
{
    imports->renderSystem->RenderScene( fd );
}

void    trap_R_SetColor( const float* rgba )
{
    imports->renderSystem->SetColor( rgba );
}

void    trap_R_DrawStretchPic( float x, float y, float w, float h,
                               float s1, float t1, float s2, float t2, qhandle_t hShader )
{
    imports->renderSystem->DrawStretchPic( PASSFLOAT( x ), PASSFLOAT( y ), PASSFLOAT( w ), PASSFLOAT( h ), PASSFLOAT( s1 ), PASSFLOAT( t1 ), PASSFLOAT( s2 ), PASSFLOAT( t2 ), hShader );
}

void    trap_R_DrawStretchPicGradient( float x, float y, float w, float h,
                                       float s1, float t1, float s2, float t2, qhandle_t hShader,
                                       const float* gradientColor, int gradientType )
{
    imports->renderSystem->DrawStretchPicGradient( PASSFLOAT( x ), PASSFLOAT( y ), PASSFLOAT( w ), PASSFLOAT( h ), PASSFLOAT( s1 ), PASSFLOAT( t1 ), PASSFLOAT( s2 ), PASSFLOAT( t2 ), hShader, gradientColor, gradientType );
}

void    trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs )
{
    imports->renderSystem->ModelBounds( model, mins, maxs );
}

int     trap_R_LerpTag( orientation_t* tag, const refEntity_t* refent, const char* tagName, int startIndex )
{
    return imports->renderSystem->LerpTag( tag, refent, tagName, startIndex );
}

void    trap_R_RemapShader( const char* oldShader, const char* newShader, const char* timeOffset )
{
    imports->renderSystem->RemapShader( oldShader, newShader, timeOffset );
}

void        trap_GetGlconfig( glconfig_t* glconfig )
{
    imports->GetGlconfig( glconfig );
}

void        trap_GetGameState( gameState_t* gamestate )
{
    imports->GetGameState( gamestate );
}

void        trap_GetCurrentSnapshotNumber( int* snapshotNumber, int* serverTime )
{
    imports->GetCurrentSnapshotNumber( snapshotNumber, serverTime );
}

bool    trap_GetSnapshot( int snapshotNumber, snapshot_t* snapshot )
{
    return imports->GetSnapshot( snapshotNumber, snapshot );
}

bool    trap_GetServerCommand( int serverCommandNumber )
{
    return imports->GetServerCommand( serverCommandNumber );
}

int         trap_GetCurrentCmdNumber( void )
{
    return imports->GetCurrentCmdNumber();
}

bool    trap_GetUserCmd( int cmdNumber, usercmd_t* ucmd )
{
    return imports->GetUserCmd( cmdNumber, ucmd );
}

void        trap_SetUserCmdValue( int stateValue, int holdableValue, float sensitivityScale, int mpSetup, int mpIdentClient )   //----(SA)	// NERVE - SMF - added cld
{
    imports->SetUserCmdValue( stateValue, holdableValue, PASSFLOAT( sensitivityScale ), mpSetup, mpIdentClient );
}

int trap_MemoryRemaining( void )
{
    return imports->MemoryRemaining();
}

bool trap_loadCamera( int camNum, const char* name )
{
    return imports->loadCamera( camNum, name );
}

void trap_startCamera( int camNum, int time )
{
    imports->startCamera( camNum, time );
}

//----(SA)	added
void trap_stopCamera( int camNum )
{
    imports->stopCamera( camNum );
}
//----(SA)	end

bool trap_getCameraInfo( int camNum, int time, vec3_t* origin, vec3_t* angles, float* fov )
{
    return imports->getCameraInfo( camNum, time, origin, angles, fov );
}


bool trap_Key_IsDown( int keynum )
{
    return imports->Key_IsDown( keynum );
}

int trap_Key_GetCatcher( void )
{
    return imports->Key_GetCatcher();
}

void trap_Key_SetCatcher( int catcher )
{
    imports->Key_SetCatcher( catcher );
}

int trap_Key_GetKey( const char* binding )
{
    return imports->Key_GetKey( binding );
}


int trap_PC_AddGlobalDefine( char* define )
{
    return imports->PC_AddGlobalDefine( define );
}

int trap_PC_LoadSource( const char* filename )
{
    return imports->PC_LoadSource( filename );
}

int trap_PC_FreeSource( int handle )
{
    return imports->PC_FreeSource( handle );
}

int trap_PC_ReadToken( int handle, pc_token_t* pc_token )
{
    return imports->PC_ReadToken( handle, pc_token );
}


int trap_PC_SourceFileAndLine( int handle, char* filename, int* line )
{
    return imports->PC_SourceFileAndLine( handle, filename, line );
}

void    trap_S_StopBackgroundTrack( void )
{
    imports->soundSystem->StopBackgroundTrack();
}

int trap_RealTime( qtime_t* qtime )
{
    return imports->RealTime( qtime );
}

void trap_SnapVector( float* v )
{
    SnapVector( v );
}

void trap_SendMoveSpeedsToGame( int entnum, char* movespeeds )
{
    imports->SendMoveSpeedsToGame( entnum, movespeeds );
}

// this returns a handle.  arg0 is the name in the format "idlogo.roq", set arg1 to NULL, alteredstates to false (do not alter gamestate)
int trap_CIN_PlayCinematic( const char* arg0, int xpos, int ypos, int width, int height, int bits )
{
    return imports->CIN_PlayCinematic( arg0, xpos, ypos, width, height, bits );
}

// stops playing the cinematic and ends it.  should always return FMV_EOF
// cinematics must be stopped in reverse order of when they are started
e_status trap_CIN_StopCinematic( int handle )
{
    return ( e_status )imports->CIN_StopCinematic( handle );
}


// will run a frame of the cinematic but will not draw it.  Will return FMV_EOF if the end of the cinematic has been reached.
e_status trap_CIN_RunCinematic( int handle )
{
    return ( e_status )imports->CIN_RunCinematic( handle );
}


// draws the current frame
void trap_CIN_DrawCinematic( int handle )
{
    imports->CIN_DrawCinematic( handle );
}


// allows you to resize the animation dynamically
void trap_CIN_SetExtents( int handle, int x, int y, int w, int h )
{
    imports->CIN_SetExtents( handle, x, y, w, h );
}

bool trap_GetEntityToken( char* buffer, int bufferSize )
{
    return imports->renderSystem->GetEntityToken( buffer, bufferSize );
}

void trap_UI_Popup( const char* arg0 )
{
    imports->UI_Popup( arg0 );
}

// NERVE - SMF
void trap_UI_LimboChat( const char* arg0 )
{
    imports->UI_LimboChat( arg0 );
}

void trap_UI_ClosePopup( const char* arg0 )
{
    imports->UI_ClosePopup( arg0 );
}
// -NERVE - SMF

bool trap_GetModelInfo( int clientNum, char* modelName, animModelInfo_t** modelInfo )
{
    return imports->GetModelInfo( clientNum, modelName, modelInfo );
}

void trap_PhysicsDrawDebug( void )
{
    imports->PhysicsDrawDebug();
}
