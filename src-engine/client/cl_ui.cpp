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
//  File name:   cl_ui.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "client.h"

#include "../botlib/botlib.h"

extern botlib_export_t* botlib_export;

vm_t* uivm;

extern char cl_cdkey[34];


/*
====================
GetClientState
====================
*/
static void GetClientState( uiClientState_t* state )
{
    state->connectPacketCount = clc.connectPacketCount;
    state->connState = cls.state;
    Q_strncpyz( state->servername, cls.servername, sizeof( state->servername ) );
    Q_strncpyz( state->updateInfoString, cls.updateInfoString, sizeof( state->updateInfoString ) );
    Q_strncpyz( state->messageString, clc.serverMessage, sizeof( state->messageString ) );
    state->clientNum = cl.snap.ps.clientNum;
}

/*
====================
LAN_LoadCachedServers
====================
*/
void LAN_LoadCachedServers()
{
    // TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
    // show_bug.cgi?id=445
    /*
      int size;
      fileHandle_t fileIn;
      cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
      cls.numGlobalServerAddresses = 0;
      if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
    	  FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
    	  FS_Read(&cls.nummplayerservers, sizeof(int), fileIn);
    	  FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
    	  FS_Read(&size, sizeof(int), fileIn);
    	  if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers)) {
    		  FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
    		  FS_Read(&cls.mplayerServers, sizeof(cls.mplayerServers), fileIn);
    		  FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
    	  } else {
    		  cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
    		  cls.numGlobalServerAddresses = 0;
    	  }
    	  FS_FCloseFile(fileIn);
      }
    */
}

/*
====================
LAN_SaveServersToCache
====================
*/
void LAN_SaveServersToCache()
{
    // TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
    // show_bug.cgi?id=445
    /*
      int size;
      fileHandle_t fileOut;
    #ifdef __MACOS__	//DAJ MacOS file typing
      {
    	  extern _MSL_IMP_EXP_C long _fcreator, _ftype;
    	  _ftype = 'WlfB';
    	  _fcreator = 'WlfS';
      }
    #endif
      fileOut = FS_SV_FOpenFileWrite("servercache.dat");
      FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
      FS_Write(&cls.nummplayerservers, sizeof(int), fileOut);
      FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
      size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers);
      FS_Write(&size, sizeof(int), fileOut);
      FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
      FS_Write(&cls.mplayerServers, sizeof(cls.mplayerServers), fileOut);
      FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
      FS_FCloseFile(fileOut);
    */
}


/*
====================
LAN_ResetPings
====================
*/
static void LAN_ResetPings( int source )
{
    int count, i;
    serverInfo_t* servers = NULL;
    count = 0;
    
    switch( source )
    {
        case AS_LOCAL:
            servers = &cls.localServers[0];
            count = MAX_OTHER_SERVERS;
            break;
        case AS_MPLAYER:
            servers = &cls.mplayerServers[0];
            count = MAX_OTHER_SERVERS;
            break;
        case AS_GLOBAL:
            servers = &cls.globalServers[0];
            count = MAX_GLOBAL_SERVERS;
            break;
        case AS_FAVORITES:
            servers = &cls.favoriteServers[0];
            count = MAX_OTHER_SERVERS;
            break;
    }
    if( servers )
    {
        for( i = 0; i < count; i++ )
        {
            servers[i].ping = -1;
        }
    }
}

/*
====================
LAN_AddServer
====================
*/
static int LAN_AddServer( int source, const char* name, const char* address )
{
    int max, *count, i;
    netadr_t adr;
    serverInfo_t* servers = NULL;
    max = MAX_OTHER_SERVERS;
    count = 0;
    
    switch( source )
    {
        case AS_LOCAL:
            count = &cls.numlocalservers;
            servers = &cls.localServers[0];
            break;
        case AS_MPLAYER:
            count = &cls.nummplayerservers;
            servers = &cls.mplayerServers[0];
            break;
        case AS_GLOBAL:
            max = MAX_GLOBAL_SERVERS;
            count = &cls.numglobalservers;
            servers = &cls.globalServers[0];
            break;
        case AS_FAVORITES:
            count = &cls.numfavoriteservers;
            servers = &cls.favoriteServers[0];
            break;
    }
    if( servers && *count < max )
    {
        NET_StringToAdr( address, &adr );
        for( i = 0; i < *count; i++ )
        {
            if( NET_CompareAdr( servers[i].adr, adr ) )
            {
                break;
            }
        }
        if( i >= *count )
        {
            servers[*count].adr = adr;
            Q_strncpyz( servers[*count].hostName, name, sizeof( servers[*count].hostName ) );
            servers[*count].visible = true;
            ( *count )++;
            return 1;
        }
        return 0;
    }
    return -1;
}

/*
====================
LAN_RemoveServer
====================
*/
static void LAN_RemoveServer( int source, const char* addr )
{
    int* count, i;
    serverInfo_t* servers = NULL;
    count = 0;
    switch( source )
    {
        case AS_LOCAL:
            count = &cls.numlocalservers;
            servers = &cls.localServers[0];
            break;
        case AS_MPLAYER:
            count = &cls.nummplayerservers;
            servers = &cls.mplayerServers[0];
            break;
        case AS_GLOBAL:
            count = &cls.numglobalservers;
            servers = &cls.globalServers[0];
            break;
        case AS_FAVORITES:
            count = &cls.numfavoriteservers;
            servers = &cls.favoriteServers[0];
            break;
    }
    if( servers )
    {
        netadr_t comp;
        NET_StringToAdr( addr, &comp );
        for( i = 0; i < *count; i++ )
        {
            if( NET_CompareAdr( comp, servers[i].adr ) )
            {
                int j = i;
                while( j < *count - 1 )
                {
                    Com_Memcpy( &servers[j], &servers[j + 1], sizeof( servers[j] ) );
                    j++;
                }
                ( *count )--;
                break;
            }
        }
    }
}


/*
====================
LAN_GetServerCount
====================
*/
static int LAN_GetServerCount( int source )
{
    switch( source )
    {
        case AS_LOCAL:
            return cls.numlocalservers;
            break;
        case AS_MPLAYER:
            return cls.nummplayerservers;
            break;
        case AS_GLOBAL:
            return cls.numglobalservers;
            break;
        case AS_FAVORITES:
            return cls.numfavoriteservers;
            break;
    }
    return 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
static void LAN_GetServerAddressString( int source, int n, char* buf, int buflen )
{
    switch( source )
    {
        case AS_LOCAL:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                Q_strncpyz( buf, NET_AdrToString( cls.localServers[n].adr ), buflen );
                return;
            }
            break;
        case AS_MPLAYER:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                Q_strncpyz( buf, NET_AdrToString( cls.mplayerServers[n].adr ), buflen );
                return;
            }
            break;
        case AS_GLOBAL:
            if( n >= 0 && n < MAX_GLOBAL_SERVERS )
            {
                Q_strncpyz( buf, NET_AdrToString( cls.globalServers[n].adr ), buflen );
                return;
            }
            break;
        case AS_FAVORITES:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                Q_strncpyz( buf, NET_AdrToString( cls.favoriteServers[n].adr ), buflen );
                return;
            }
            break;
    }
    buf[0] = '\0';
}

/*
====================
LAN_GetServerInfo
====================
*/
static void LAN_GetServerInfo( int source, int n, char* buf, int buflen )
{
    char info[MAX_STRING_CHARS];
    serverInfo_t* server = NULL;
    info[0] = '\0';
    switch( source )
    {
        case AS_LOCAL:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.localServers[n];
            }
            break;
        case AS_MPLAYER:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.mplayerServers[n];
            }
            break;
        case AS_GLOBAL:
            if( n >= 0 && n < MAX_GLOBAL_SERVERS )
            {
                server = &cls.globalServers[n];
            }
            break;
        case AS_FAVORITES:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.favoriteServers[n];
            }
            break;
    }
    if( server && buf )
    {
        buf[0] = '\0';
        Info_SetValueForKey( info, "hostname", server->hostName );
        Info_SetValueForKey( info, "mapname", server->mapName );
        Info_SetValueForKey( info, "clients", va( "%i", server->clients ) );
        Info_SetValueForKey( info, "sv_maxclients", va( "%i", server->maxClients ) );
        Info_SetValueForKey( info, "ping", va( "%i", server->ping ) );
        Info_SetValueForKey( info, "minping", va( "%i", server->minPing ) );
        Info_SetValueForKey( info, "maxping", va( "%i", server->maxPing ) );
        Info_SetValueForKey( info, "game", server->game );
        Info_SetValueForKey( info, "gametype", va( "%i", server->gameType ) );
        Info_SetValueForKey( info, "nettype", va( "%i", server->netType ) );
        Info_SetValueForKey( info, "addr", NET_AdrToString( server->adr ) );
        Info_SetValueForKey( info, "sv_allowAnonymous", va( "%i", server->allowAnonymous ) );
        Q_strncpyz( buf, info, buflen );
    }
    else
    {
        if( buf )
        {
            buf[0] = '\0';
        }
    }
}

/*
====================
LAN_GetServerPing
====================
*/
static int LAN_GetServerPing( int source, int n )
{
    serverInfo_t* server = NULL;
    switch( source )
    {
        case AS_LOCAL:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.localServers[n];
            }
            break;
        case AS_MPLAYER:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.mplayerServers[n];
            }
            break;
        case AS_GLOBAL:
            if( n >= 0 && n < MAX_GLOBAL_SERVERS )
            {
                server = &cls.globalServers[n];
            }
            break;
        case AS_FAVORITES:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                server = &cls.favoriteServers[n];
            }
            break;
    }
    if( server )
    {
        return server->ping;
    }
    return -1;
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t* LAN_GetServerPtr( int source, int n )
{
    switch( source )
    {
        case AS_LOCAL:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return &cls.localServers[n];
            }
            break;
        case AS_MPLAYER:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return &cls.mplayerServers[n];
            }
            break;
        case AS_GLOBAL:
            if( n >= 0 && n < MAX_GLOBAL_SERVERS )
            {
                return &cls.globalServers[n];
            }
            break;
        case AS_FAVORITES:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return &cls.favoriteServers[n];
            }
            break;
    }
    return NULL;
}

/*
====================
LAN_CompareServers
====================
*/
static int LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 )
{
    int res;
    serverInfo_t* server1, *server2;
    
    server1 = LAN_GetServerPtr( source, s1 );
    server2 = LAN_GetServerPtr( source, s2 );
    if( !server1 || !server2 )
    {
        return 0;
    }
    
    res = 0;
    switch( sortKey )
    {
        case SORT_HOST:
            res = Q_stricmp( server1->hostName, server2->hostName );
            break;
            
        case SORT_MAP:
            res = Q_stricmp( server1->mapName, server2->mapName );
            break;
        case SORT_CLIENTS:
            if( server1->clients < server2->clients )
            {
                res = -1;
            }
            else if( server1->clients > server2->clients )
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
            break;
        case SORT_GAME:
            if( server1->gameType < server2->gameType )
            {
                res = -1;
            }
            else if( server1->gameType > server2->gameType )
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
            break;
        case SORT_PING:
            if( server1->ping < server2->ping )
            {
                res = -1;
            }
            else if( server1->ping > server2->ping )
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
            break;
    }
    
    if( sortDir )
    {
        if( res < 0 )
        {
            return 1;
        }
        if( res > 0 )
        {
            return -1;
        }
        return 0;
    }
    return res;
}

/*
====================
LAN_GetPingQueueCount
====================
*/
static int LAN_GetPingQueueCount( void )
{
    return ( CL_GetPingQueueCount() );
}

/*
====================
LAN_ClearPing
====================
*/
static void LAN_ClearPing( int n )
{
    CL_ClearPing( n );
}

/*
====================
LAN_GetPing
====================
*/
static void LAN_GetPing( int n, char* buf, int buflen, int* pingtime )
{
    CL_GetPing( n, buf, buflen, pingtime );
}

/*
====================
LAN_GetPingInfo
====================
*/
static void LAN_GetPingInfo( int n, char* buf, int buflen )
{
    CL_GetPingInfo( n, buf, buflen );
}

/*
====================
LAN_MarkServerVisible
====================
*/
static void LAN_MarkServerVisible( int source, int n, bool visible )
{
    if( n == -1 )
    {
        int count = MAX_OTHER_SERVERS;
        serverInfo_t* server = NULL;
        switch( source )
        {
            case AS_LOCAL:
                server = &cls.localServers[0];
                break;
            case AS_MPLAYER:
                server = &cls.mplayerServers[0];
                break;
            case AS_GLOBAL:
                server = &cls.globalServers[0];
                count = MAX_GLOBAL_SERVERS;
                break;
            case AS_FAVORITES:
                server = &cls.favoriteServers[0];
                break;
        }
        if( server )
        {
            for( n = 0; n < count; n++ )
            {
                server[n].visible = visible;
            }
        }
        
    }
    else
    {
        switch( source )
        {
            case AS_LOCAL:
                if( n >= 0 && n < MAX_OTHER_SERVERS )
                {
                    cls.localServers[n].visible = visible;
                }
                break;
            case AS_MPLAYER:
                if( n >= 0 && n < MAX_OTHER_SERVERS )
                {
                    cls.mplayerServers[n].visible = visible;
                }
                break;
            case AS_GLOBAL:
                if( n >= 0 && n < MAX_GLOBAL_SERVERS )
                {
                    cls.globalServers[n].visible = visible;
                }
                break;
            case AS_FAVORITES:
                if( n >= 0 && n < MAX_OTHER_SERVERS )
                {
                    cls.favoriteServers[n].visible = visible;
                }
                break;
        }
    }
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
static int LAN_ServerIsVisible( int source, int n )
{
    switch( source )
    {
        case AS_LOCAL:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return cls.localServers[n].visible;
            }
            break;
        case AS_MPLAYER:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return cls.mplayerServers[n].visible;
            }
            break;
        case AS_GLOBAL:
            if( n >= 0 && n < MAX_GLOBAL_SERVERS )
            {
                return cls.globalServers[n].visible;
            }
            break;
        case AS_FAVORITES:
            if( n >= 0 && n < MAX_OTHER_SERVERS )
            {
                return cls.favoriteServers[n].visible;
            }
            break;
    }
    return false;
}

/*
=======================
LAN_UpdateVisiblePings
=======================
*/
bool LAN_UpdateVisiblePings( int source )
{
    return CL_UpdateVisiblePings_f( source );
}

/*
====================
LAN_GetServerStatus
====================
*/
int LAN_GetServerStatus( char* serverAddress, char* serverStatus, int maxLen )
{
    return CL_ServerStatus( serverAddress, serverStatus, maxLen );
}

/*
====================
CL_GetGlConfig
====================
*/
static void CL_GetGlconfig( glconfig_t* config )
{
    *config = cls.glconfig;
}

/*
====================
GetClipboardData
====================
*/
static void GetClipboardData( char* buf, int buflen )
{
    char*    cbd;
    
    cbd = Sys_GetClipboardData();
    
    if( !cbd )
    {
        *buf = 0;
        return;
    }
    
    Q_strncpyz( buf, cbd, buflen );
    
    Z_Free( cbd );
}

/*
====================
Key_KeynumToStringBuf
====================
*/
static void Key_KeynumToStringBuf( int keynum, char* buf, int buflen )
{
    Q_strncpyz( buf, Key_KeynumToString( keynum, true ), buflen );
}

/*
====================
Key_GetBindingBuf
====================
*/
static void Key_GetBindingBuf( int keynum, char* buf, int buflen )
{
    char*    value;
    
    value = Key_GetBinding( keynum );
    if( value )
    {
        Q_strncpyz( buf, value, buflen );
    }
    else
    {
        *buf = 0;
    }
}

/*
====================
Key_GetCatcher
====================
*/
int Key_GetCatcher( void )
{
    return cls.keyCatchers;
}

/*
====================
Ket_SetCatcher
====================
*/
void Key_SetCatcher( int catcher )
{
    cls.keyCatchers = catcher;
}


/*
====================
CLUI_GetCDKey
====================
*/
static void CLUI_GetCDKey( char* buf, int buflen )
{
    cvar_t*  fs;
    fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
    if( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 )
    {
        memcpy( buf, &cl_cdkey[16], 16 );
        buf[16] = 0;
    }
    else
    {
        memcpy( buf, cl_cdkey, 16 );
        buf[16] = 0;
    }
}


/*
====================
CLUI_SetCDKey
====================
*/
static void CLUI_SetCDKey( char* buf )
{
    cvar_t*  fs;
    fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
    if( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 )
    {
        memcpy( &cl_cdkey[16], buf, 16 );
        cl_cdkey[32] = 0;
        // set the flag so the fle will be written at the next opportunity
        cvar_modifiedFlags |= CVAR_ARCHIVE;
    }
    else
    {
        memcpy( cl_cdkey, buf, 16 );
        // set the flag so the fle will be written at the next opportunity
        cvar_modifiedFlags |= CVAR_ARCHIVE;
    }
}


/*
====================
GetConfigString
====================
*/
static int GetConfigString( int index, char* buf, int size )
{
    int offset;
    
    if( index < 0 || index >= MAX_CONFIGSTRINGS )
    {
        return false;
    }
    
    offset = cl.gameState.stringOffsets[index];
    if( !offset )
    {
        if( size )
        {
            buf[0] = 0;
        }
        return false;
    }
    
    Q_strncpyz( buf, cl.gameState.stringData + offset, size );
    
    return true;
}

/*
====================
CL_ShutdownUI
====================
*/
void CL_ShutdownUI( void )
{
    cls.keyCatchers &= ~KEYCATCH_UI;
    cls.uiStarted = false;
    uiManager->Shutdown();
}

/*
====================
CL_InitUI
====================
*/

void CL_InitUI( void )
{
    uiManager->Init( cls.state >= CA_AUTHORIZING && cls.state < CA_ACTIVE );
}


bool UI_usesUniqueCDKey()
{
    return false;
}

/*
====================
UI_GameCommand

See if the current console command is claimed by the ui
====================
*/
bool UI_GameCommand( void )
{
    if( !uivm )
    {
        return false;
    }
    
    return uiManager->ConsoleCommand( cls.realtime );
}

//
// REMOVE THESE
//

#define PASSFLOAT(x) x

void trap_Print( const char* string )
{
    Com_Printf( string );
}

void trap_Error( const char* string )
{
    Com_Error( ERR_FATAL, string );
}

int trap_Milliseconds( void )
{
    return Sys_Milliseconds();
}

void trap_Cvar_Register( vmCvar_t* cvar, const char* var_name, const char* value, int flags )
{
    Cvar_Register( cvar, var_name, value, flags );
}

void trap_Cvar_Update( vmCvar_t* cvar )
{
    Cvar_Update( cvar );
}

void trap_Cvar_Set( const char* var_name, const char* value )
{
    Cvar_Set( var_name, value );
}

float trap_Cvar_VariableValue( const char* var_name )
{
    return Cvar_VariableValue( var_name );;
}

void trap_Cvar_VariableStringBuffer( const char* var_name, char* buffer, int bufsize )
{
    Cvar_VariableStringBuffer( var_name, buffer, bufsize );
}

void trap_Cvar_SetValue( const char* var_name, float value )
{
    Cvar_SetValue( var_name, PASSFLOAT( value ) );
}

void trap_Cvar_Reset( const char* name )
{
    Cvar_Reset( name );
}

void trap_Cvar_Create( const char* var_name, const char* var_value, int flags )
{
    Cvar_Get( var_name, var_value, flags );
}

void trap_Cvar_InfoStringBuffer( int bit, char* buffer, int bufsize )
{
    Cvar_InfoStringBuffer( bit, buffer, bufsize );
}

int trap_Argc( void )
{
    return Cmd_Argc();
}

void trap_Argv( int n, char* buffer, int bufferLength )
{
    Cmd_ArgvBuffer( n, buffer, bufferLength );
}

void trap_Cmd_ExecuteText( int exec_when, const char* text )
{
    Cbuf_ExecuteText( exec_when, text );
}

int trap_FS_FOpenFile( const char* qpath, fileHandle_t* f, fsMode_t mode )
{
    return FS_FOpenFileByMode( qpath, f, mode );
}

void trap_FS_Read( void* buffer, int len, fileHandle_t f )
{
    FS_Read( buffer, len, f );
}

//----(SA)	added
void trap_FS_Seek( fileHandle_t f, long offset, int origin )
{
    FS_Seek( f, offset, origin );
}
//----(SA)	end

void trap_FS_Write( const void* buffer, int len, fileHandle_t f )
{
    FS_Write( buffer, len, f );
}

void trap_FS_FCloseFile( fileHandle_t f )
{
    FS_FCloseFile( f );
}

int trap_FS_GetFileList( const char* path, const char* extension, char* listbuf, int bufsize )
{
    return FS_GetFileList( path, extension, listbuf, bufsize );
}

int trap_FS_Delete( const char* filename )
{
    return FS_Delete( ( char* )filename );
}

qhandle_t trap_R_RegisterModel( const char* name )
{
    return renderSystem->RegisterModel( name );
}

qhandle_t trap_R_RegisterSkin( const char* name )
{
    return renderSystem->RegisterSkin( name );
}

void trap_R_RegisterFont( const char* fontName, int pointSize, fontInfo_t* font )
{
    renderSystem->RegisterFont( fontName, pointSize, font );
}

qhandle_t trap_R_RegisterShaderNoMip( const char* name )
{
    return renderSystem->RegisterShaderNoMip( name );
}

void trap_R_ClearScene( void )
{
    renderSystem->ClearScene();
}

void trap_R_AddRefEntityToScene( const refEntity_t* re )
{
    renderSystem->AddRefEntityToScene( re );
}

void trap_R_AddPolyToScene( qhandle_t hShader, int numVerts, const polyVert_t* verts )
{
    renderSystem->AddPolyToScene( hShader, numVerts, verts );
}

void trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b, int overdraw )
{
    renderSystem->AddLightToScene( org, PASSFLOAT( intensity ), PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), overdraw );
}

void trap_R_AddCoronaToScene( const vec3_t org, float r, float g, float b, float scale, int id, int flags )
{
    renderSystem->AddCoronaToScene( org, PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), PASSFLOAT( scale ), id, flags );
}

void trap_R_RenderScene( const refdef_t* fd )
{
    renderSystem->RenderScene( fd );
}

void trap_R_SetColor( const float* rgba )
{
    renderSystem->SetColor( rgba );
}

void trap_R_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader )
{
    renderSystem->DrawStretchPic( PASSFLOAT( x ), PASSFLOAT( y ), PASSFLOAT( w ), PASSFLOAT( h ), PASSFLOAT( s1 ), PASSFLOAT( t1 ), PASSFLOAT( s2 ), PASSFLOAT( t2 ), hShader );
}

void    trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs )
{
    renderSystem->ModelBounds( model, mins, maxs );
}

void trap_UpdateScreen( void )
{
    SCR_UpdateScreen();
}

int trap_CM_LerpTag( orientation_t* tag, const refEntity_t* refent, const char* tagName, int startIndex )
{
    return renderSystem->LerpTag( tag, refent, tagName, 0 );         // NEFVE - SMF - fixed
}

void trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum )
{
    soundSystem->StartLocalSound( sfx, channelNum );
}

sfxHandle_t trap_S_RegisterSound( const char* sample )
{
    return soundSystem->RegisterSound( sample );
}

//----(SA)	added (already in cg)
void    trap_S_FadeBackgroundTrack( float targetvol, int time, int num )   // yes, i know.  fadebackground coming in, fadestreaming going out.  will have to see where functionality leads...
{
    soundSystem->FadeStreamingSound( PASSFLOAT( targetvol ), time, num ); // 'num' is '0' if it's music, '1' if it's "all streaming sounds"
}

void    trap_S_FadeAllSound( float targetvol, int time )
{
    soundSystem->FadeAllSounds( PASSFLOAT( targetvol ), time );
}
//----(SA)	end


void trap_Key_KeynumToStringBuf( int keynum, char* buf, int buflen )
{
    Key_KeynumToStringBuf( keynum, buf, buflen );
}

void trap_Key_GetBindingBuf( int keynum, char* buf, int buflen )
{
    Key_GetBindingBuf( keynum, buf, buflen );
}

void trap_Key_SetBinding( int keynum, const char* binding )
{
    Key_SetBinding( keynum, binding );
}

bool trap_Key_IsDown( int keynum )
{
    return Key_IsDown( keynum );
}

bool trap_Key_GetOverstrikeMode( void )
{
    return Key_GetOverstrikeMode();
}

void trap_Key_SetOverstrikeMode( bool state )
{
    Key_SetOverstrikeMode( state );
}

void trap_Key_ClearStates( void )
{
    Key_ClearStates();
}

int trap_Key_GetCatcher( void )
{
    return Key_GetCatcher();
}

void trap_Key_SetCatcher( int catcher )
{
    Key_SetCatcher( catcher );
}

void trap_GetClipboardData( char* buf, int bufsize )
{
    GetClipboardData( buf, bufsize );
}

void trap_GetClientState( uiClientState_t* state )
{
    GetClientState( state );
}

void trap_GetGlconfig( glconfig_t* glconfig )
{
    CL_GetGlconfig( glconfig );
}

int trap_GetConfigString( int index, char* buff, int buffsize )
{
    return GetConfigString( index, buff, buffsize );
}

int trap_LAN_GetLocalServerCount( void )
{
    //	return LAN_GetLocalServerCount( );
    return 0;
}

void trap_LAN_GetLocalServerAddressString( int n, char* buf, int buflen )
{
    //	LAN_GetLocalServerAddressString( n, buf, buflen );
}

int trap_LAN_GetGlobalServerCount( void )
{
    //	return LAN_GetGlobalServerCount();
    return 0;
}

void trap_LAN_GetGlobalServerAddressString( int n, char* buf, int buflen )
{
    //	LAN_GetGlobalServerAddressString( n, buf, buflen );
}

int trap_LAN_GetPingQueueCount( void )
{
    return LAN_GetPingQueueCount();
}

void trap_LAN_ClearPing( int n )
{
    LAN_ClearPing( n );
}

void trap_LAN_GetPing( int n, char* buf, int buflen, int* pingtime )
{
    LAN_GetPing( n, buf, buflen, pingtime );
}

void trap_LAN_GetPingInfo( int n, char* buf, int buflen )
{
    LAN_GetPingInfo( n, buf, buflen );
}

// NERVE - SMF
bool trap_LAN_UpdateVisiblePings( int source )
{
    return LAN_UpdateVisiblePings( source );
}

int trap_LAN_GetServerCount( int source )
{
    return LAN_GetServerCount( source );
}

int trap_LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 )
{
    return LAN_CompareServers( source, sortKey, sortDir, s1, s2 );
}

void trap_LAN_GetServerAddressString( int source, int n, char* buf, int buflen )
{
    LAN_GetServerAddressString( source, n, buf, buflen );
}

void trap_LAN_GetServerInfo( int source, int n, char* buf, int buflen )
{
    LAN_GetServerInfo( source, n, buf, buflen );
}

int trap_LAN_AddServer( int source, const char* name, const char* addr )
{
    return LAN_AddServer( source, name, addr );
}

void trap_LAN_RemoveServer( int source, const char* addr )
{
    LAN_RemoveServer( source, addr );
}

int trap_LAN_GetServerPing( int source, int n )
{
    return LAN_GetServerPing( source, n );
}

int trap_LAN_ServerIsVisible( int source, int n )
{
    return LAN_ServerIsVisible( source, n );
}

int trap_LAN_ServerStatus( const char* serverAddress, char* serverStatus, int maxLen )
{
    return LAN_GetServerStatus( ( char* )serverAddress, serverStatus, maxLen );
}

void trap_LAN_SaveCachedServers()
{
    LAN_SaveServersToCache();
}

void trap_LAN_LoadCachedServers()
{
    LAN_LoadCachedServers();
}

void trap_LAN_MarkServerVisible( int source, int n, bool visible )
{
    LAN_MarkServerVisible( source, n, visible );
}

void trap_LAN_ResetPings( int n )
{
    LAN_ResetPings( n );
}
// -NERVE - SMF

int trap_MemoryRemaining( void )
{
    return Hunk_MemoryRemaining();
}


void trap_GetCDKey( char* buf, int buflen )
{
    CLUI_GetCDKey( buf, buflen );
}

void trap_SetCDKey( char* buf )
{
    CLUI_SetCDKey( buf );
}

int trap_PC_AddGlobalDefine( char* define )
{
    return botlib_export->PC_AddGlobalDefine( define );
}

int trap_PC_LoadSource( const char* filename )
{
    return botlib_export->PC_LoadSourceHandle( filename );
}

int trap_PC_FreeSource( int handle )
{
    return botlib_export->PC_FreeSourceHandle( handle );
}

int trap_PC_ReadToken( int handle, pc_token_t* pc_token )
{
    return botlib_export->PC_ReadTokenHandle( handle, pc_token );
}

int trap_PC_SourceFileAndLine( int handle, char* filename, int* line )
{
    return botlib_export->PC_SourceFileAndLine( handle, filename, line );
}

void trap_S_StopBackgroundTrack( void )
{
    soundSystem->StopBackgroundTrack();
}

void trap_S_StartBackgroundTrack( const char* intro, const char* loop, int fadeupTime )
{
    soundSystem->StartBackgroundTrack( intro, loop, fadeupTime );
}

int trap_RealTime( qtime_t* qtime )
{
    return Com_RealTime( qtime );
}

// this returns a handle.  arg0 is the name in the format "idlogo.roq", set arg1 to NULL, alteredstates to false (do not alter gamestate)
int trap_CIN_PlayCinematic( const char* arg0, int xpos, int ypos, int width, int height, int bits )
{
    return CIN_PlayCinematic( arg0, xpos, ypos, width, height, bits );
}

// stops playing the cinematic and ends it.  should always return FMV_EOF
// cinematics must be stopped in reverse order of when they are started
e_status trap_CIN_StopCinematic( int handle )
{
    return ( e_status )CIN_StopCinematic( handle );
}


// will run a frame of the cinematic but will not draw it.  Will return FMV_EOF if the end of the cinematic has been reached.
e_status trap_CIN_RunCinematic( int handle )
{
    return ( e_status )CIN_RunCinematic( handle );
}


// draws the current frame
void trap_CIN_DrawCinematic( int handle )
{
    CIN_DrawCinematic( handle );
}


// allows you to resize the animation dynamically
void trap_CIN_SetExtents( int handle, int x, int y, int w, int h )
{
    CIN_SetExtents( handle, x, y, w, h );
}


void    trap_R_RemapShader( const char* oldShader, const char* newShader, const char* timeOffset )
{
    renderSystem->RemapShader( oldShader, newShader, timeOffset );
}

bool trap_VerifyCDKey( const char* key, const char* chksum )
{
    return CL_CDKeyValidate( key, chksum );
}

// NERVE - SMF
bool trap_GetLimboString( int index, char* buf )
{
    return CL_GetLimboString( index, buf );
}
// -NERVE - SMF