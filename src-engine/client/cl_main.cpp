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
//  File name:   cl_main.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: client main loop
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "client.h"
#include <limits.h>

#ifndef __ANDROID__
#include <motioncontrollers.h>
#endif

#ifdef __linux__
#include <sys/stat.h>
#endif

#include "../sound/snd_local.h"

cvar_t*  cl_wavefilerecord;
cvar_t*  cl_nodelta;
cvar_t*  cl_debugMove;

cvar_t*  cl_noprint;
cvar_t*  cl_motd;
cvar_t*  cl_autoupdate;

cvar_t*  rcon_client_password;
cvar_t*  rconAddress;

cvar_t*  cl_timeout;
cvar_t*  cl_maxpackets;
cvar_t*  cl_packetdup;
cvar_t*  cl_timeNudge;
cvar_t*  cl_showTimeDelta;
cvar_t*  cl_freezeDemo;

cvar_t*  cl_shownet = NULL;     // NERVE - SMF - This is referenced in msg.c and we need to make sure it is NULL
cvar_t*  cl_shownuments;
cvar_t*  cl_visibleClients;
cvar_t*  cl_showSend;
cvar_t*  cl_showServerCommands;
cvar_t*  cl_timedemo;
cvar_t*  cl_avidemo;
cvar_t*  cl_forceavidemo;
cvar_t*  cl_bypassMouseInput;
cvar_t*  cl_freelook;
cvar_t*  cl_sensitivity;

cvar_t*  cl_mouseAccel;
cvar_t*  cl_showMouseRate;

cvar_t*  m_pitch;
cvar_t*  m_yaw;
cvar_t*  m_forward;
cvar_t*  m_side;
cvar_t*  m_filter;

cvar_t*  cl_activeAction;

cvar_t*  cl_motdString;

cvar_t*  cl_allowDownload;
cvar_t*  cl_wwwDownload;
cvar_t*  cl_conXOffset;
cvar_t*  cl_inGameVideo;

cvar_t*  cl_serverStatusResendTime;
cvar_t*  cl_trn;
cvar_t*  cl_missionStats;
cvar_t*  cl_waitForFire;

// NERVE - SMF - localization
cvar_t*  cl_language;
cvar_t*  cl_debugTranslation;
// -NERVE - SMF

cvar_t*  cl_razerhydra;

char cl_cdkey[34] = "                                ";

clientActive_t cl;
clientConnection_t clc;
clientStatic_t cls;
vm_t*                cgvm;

ping_t cl_pinglist[MAX_PINGREQUESTS];

typedef struct serverStatus_s
{
    char string[BIG_INFO_STRING];
    netadr_t address;
    int time, startTime;
    bool pending;
    bool print;
    bool retrieved;
} serverStatus_t;

serverStatus_t cl_serverStatusList[MAX_SERVERSTATUSREQUESTS];
int serverStatusCount;

extern void SV_BotFrame( int time );
void CL_CheckForResend( void );
void CL_ShowIP_f( void );
void CL_ServerStatus_f( void );
void CL_ServerStatusResponse( netadr_t from, msg_t* msg );


/*
==============
CL_EndgameMenu

Called by Com_Error when a game has ended and is dropping out to main menu in the "endgame" menu ('credits' right now)
==============
*/
void CL_EndgameMenu( void )
{
    cls.endgamemenu = true;    // start it next frame
}


/*
===============
CL_CDDialog

Called by Com_Error when a cd is needed
===============
*/
void CL_CDDialog( void )
{
    cls.cddialog = true;   // start it next frame
}

void CL_PurgeCache( void )
{
    cls.doCachePurge = true;
}

void CL_DoPurgeCache( void )
{
    if( !cls.doCachePurge )
    {
        return;
    }
    
    cls.doCachePurge = false;
    
    if( !com_cl_running )
    {
        return;
    }
    
    if( !com_cl_running->integer )
    {
        return;
    }
    
    if( !cls.rendererStarted )
    {
        return;
    }
}

/*
=======================================================================

CLIENT RELIABLE COMMAND COMMUNICATION

=======================================================================
*/

/*
======================
CL_AddReliableCommand

The given command will be transmitted to the server, and is gauranteed to
not have future usercmd_t executed before it is executed
======================
*/
void CL_AddReliableCommand( const char* cmd )
{
    int index;
    
    // if we would be losing an old command that hasn't been acknowledged,
    // we must drop the connection
//	if(cl.cameraMode)
//		Com_Printf ("cmd: %s\n", cmd);

    if( clc.reliableSequence - clc.reliableAcknowledge > MAX_RELIABLE_COMMANDS )
    {
        Com_Error( ERR_DROP, "Client command overflow" );
    }
    clc.reliableSequence++;
    index = clc.reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
    Q_strncpyz( clc.reliableCommands[ index ], cmd, sizeof( clc.reliableCommands[ index ] ) );
}

/*
======================
CL_ChangeReliableCommand
======================
*/
void CL_ChangeReliableCommand( void )
{
    int r, index, l;
    
    r = clc.reliableSequence - ( random() * 5 );
    index = clc.reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
    l = strlen( clc.reliableCommands[ index ] );
    if( l >= MAX_STRING_CHARS - 1 )
    {
        l = MAX_STRING_CHARS - 2;
    }
    clc.reliableCommands[ index ][ l ] = '\n';
    clc.reliableCommands[ index ][ l + 1 ] = '\0';
}

/*
=======================================================================

CLIENT SIDE DEMO RECORDING

=======================================================================
*/

/*
====================
CL_WriteDemoMessage

Dumps the current net message, prefixed by the length
====================
*/
void CL_WriteDemoMessage( msg_t* msg, int headerBytes )
{
    int len, swlen;
    
    // write the packet sequence
    len = clc.serverMessageSequence;
    swlen = LittleLong( len );
    FS_Write( &swlen, 4, clc.demofile );
    
    // skip the packet sequencing information
    len = msg->cursize - headerBytes;
    swlen = LittleLong( len );
    FS_Write( &swlen, 4, clc.demofile );
    FS_Write( msg->data + headerBytes, len, clc.demofile );
}


/*
====================
CL_StopRecording_f

stop recording a demo
====================
*/
void CL_StopRecord_f( void )
{
    int len;
    
    if( !clc.demorecording )
    {
        Com_Printf( "Not recording a demo.\n" );
        return;
    }
    
    // finish up
    len = -1;
    FS_Write( &len, 4, clc.demofile );
    FS_Write( &len, 4, clc.demofile );
    FS_FCloseFile( clc.demofile );
    clc.demofile = 0;
    clc.demorecording = false;
    Com_Printf( "Stopped demo.\n" );
}

/*
==================
CL_DemoFilename
==================
*/
void CL_DemoFilename( int number, char* fileName )
{
    int a, b, c, d;
    
    if( number < 0 || number > 9999 )
    {
        Com_sprintf( fileName, MAX_OSPATH, "demo9999.tga" );
        return;
    }
    
    a = number / 1000;
    number -= a * 1000;
    b = number / 100;
    number -= b * 100;
    c = number / 10;
    number -= c * 10;
    d = number;
    
    Com_sprintf( fileName, MAX_OSPATH, "demo%i%i%i%i"
                 , a, b, c, d );
}

/*
====================
CL_Record_f

record <demoname>

Begins recording a demo from the current position
====================
*/
static char demoName[MAX_QPATH];        // compiler bug workaround
void CL_Record_f( void )
{
    char name[MAX_OSPATH];
    byte bufData[MAX_MSGLEN];
    msg_t buf;
    int i;
    int len;
    entityState_t*   ent;
    entityState_t nullstate;
    char*        s;
    
    if( Cmd_Argc() > 2 )
    {
        Com_Printf( "record <demoname>\n" );
        return;
    }
    
    if( clc.demorecording )
    {
        Com_Printf( "Already recording.\n" );
        return;
    }
    
    if( cls.state != CA_ACTIVE )
    {
        Com_Printf( "You must be in a level to record.\n" );
        return;
    }
    
    if( !Cvar_VariableValue( "g_synchronousClients" ) )
    {
        Com_Printf( S_COLOR_YELLOW "WARNING: You should set 'g_synchronousClients 1' for smoother demo recording\n" );
    }
    
    if( Cmd_Argc() == 2 )
    {
        s = Cmd_Argv( 1 );
        Q_strncpyz( demoName, s, sizeof( demoName ) );
        Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", demoName, PROTOCOL_VERSION );
    }
    else
    {
        int number;
        
        // scan for a free demo name
        for( number = 0 ; number <= 9999 ; number++ )
        {
            CL_DemoFilename( number, demoName );
            Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", demoName, PROTOCOL_VERSION );
            
            len = FS_ReadFile( name, NULL );
            if( len <= 0 )
            {
                break;  // file doesn't exist
            }
        }
    }
    
    // open the demo file
#ifdef __MACOS__    //DAJ MacOS file typing
    {
        extern _MSL_IMP_EXP_C long _fcreator, _ftype;
        _ftype = 'WlfB';
        _fcreator = 'WlfS';
    }
#endif
    
    Com_Printf( "recording to %s.\n", name );
    clc.demofile = FS_FOpenFileWrite( name );
    if( !clc.demofile )
    {
        Com_Printf( "ERROR: couldn't open.\n" );
        return;
    }
    clc.demorecording = true;
    Q_strncpyz( clc.demoName, demoName, sizeof( clc.demoName ) );
    
    // don't start saving messages until a non-delta compressed message is received
    clc.demowaiting = true;
    
    // write out the gamestate message
    MSG_Init( &buf, bufData, sizeof( bufData ) );
    MSG_Bitstream( &buf );
    
    // NOTE, MRE: all server->client messages now acknowledge
    MSG_WriteLong( &buf, clc.reliableSequence );
    
    MSG_WriteByte( &buf, svc_gamestate );
    MSG_WriteLong( &buf, clc.serverCommandSequence );
    
    // configstrings
    for( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
    {
        if( !cl.gameState.stringOffsets[i] )
        {
            continue;
        }
        s = cl.gameState.stringData + cl.gameState.stringOffsets[i];
        MSG_WriteByte( &buf, svc_configstring );
        MSG_WriteShort( &buf, i );
        MSG_WriteBigString( &buf, s );
    }
    
    // baselines
    memset( &nullstate, 0, sizeof( nullstate ) );
    for( i = 0; i < MAX_GENTITIES ; i++ )
    {
        ent = &cl.entityBaselines[i];
        if( !ent->number )
        {
            continue;
        }
        MSG_WriteByte( &buf, svc_baseline );
        MSG_WriteDeltaEntity( &buf, &nullstate, ent, true );
    }
    
    MSG_WriteByte( &buf, svc_EOF );
    
    // finished writing the gamestate stuff
    
    // write the client num
    MSG_WriteLong( &buf, clc.clientNum );
    // write the checksum feed
    MSG_WriteLong( &buf, clc.checksumFeed );
    
    // finished writing the client packet
    MSG_WriteByte( &buf, svc_EOF );
    
    // write it to the demo file
    len = LittleLong( clc.serverMessageSequence - 1 );
    FS_Write( &len, 4, clc.demofile );
    
    len = LittleLong( buf.cursize );
    FS_Write( &len, 4, clc.demofile );
    FS_Write( buf.data, buf.cursize, clc.demofile );
    
    // the rest of the demo file will be copied from net messages
}

/*
=======================================================================

CLIENT SIDE DEMO PLAYBACK

=======================================================================
*/

/*
=================
CL_DemoCompleted
=================
*/
void CL_DemoCompleted( void )
{
    if( cl_timedemo && cl_timedemo->integer )
    {
        int time;
        
        time = Sys_Milliseconds() - clc.timeDemoStart;
        if( time > 0 )
        {
            Com_Printf( "%i frames, %3.1f seconds: %3.1f fps\n", clc.timeDemoFrames,
                        time / 1000.0, clc.timeDemoFrames * 1000.0 / time );
        }
    }
    
    CL_Disconnect( true );
    CL_NextDemo();
}

/*
=================
CL_ReadDemoMessage
=================
*/
void CL_ReadDemoMessage( void )
{
    int r;
    msg_t buf;
    byte bufData[ MAX_MSGLEN ];
    int s;
    
    if( !clc.demofile )
    {
        CL_DemoCompleted();
        return;
    }
    
    // get the sequence number
    r = FS_Read( &s, 4, clc.demofile );
    if( r != 4 )
    {
        CL_DemoCompleted();
        return;
    }
    clc.serverMessageSequence = LittleLong( s );
    
    // init the message
    MSG_Init( &buf, bufData, sizeof( bufData ) );
    
    // get the length
    r = FS_Read( &buf.cursize, 4, clc.demofile );
    if( r != 4 )
    {
        CL_DemoCompleted();
        return;
    }
    buf.cursize = LittleLong( buf.cursize );
    if( buf.cursize == -1 )
    {
        CL_DemoCompleted();
        return;
    }
    if( buf.cursize > buf.maxsize )
    {
        Com_Error( ERR_DROP, "CL_ReadDemoMessage: demoMsglen > MAX_MSGLEN" );
    }
    r = FS_Read( buf.data, buf.cursize, clc.demofile );
    if( r != buf.cursize )
    {
        Com_Printf( "Demo file was truncated.\n" );
        CL_DemoCompleted();
        return;
    }
    
    clc.lastPacketTime = cls.realtime;
    buf.readcount = 0;
    CL_ParseServerMessage( &buf );
}

/*
====================
Wave file saving functions
====================
*/

void CL_WriteWaveOpen()
{
    // we will just save it as a 16bit stereo 22050kz pcm file
    clc.wavefile = FS_FOpenFileWrite( "demodata.pcm" );
    clc.wavetime = -1;
}

void CL_WriteWaveClose()
{
    // and we're outta here
    FS_FCloseFile( clc.wavefile );
}

extern int s_soundtime;
extern portable_samplepair_t* paintbuffer;

void CL_WriteWaveFilePacket()
{
    int total, i;
    if( clc.wavetime == -1 )
    {
        clc.wavetime = s_soundtime;
        return;
    }
    
    total = s_soundtime - clc.wavetime;
    clc.wavetime = s_soundtime;
    
    for( i = 0; i < total; i++ )
    {
        int parm;
        short out;
        parm = ( paintbuffer[i].left ) >> 8;
        if( parm > 32767 )
        {
            parm = 32767;
        }
        if( parm < -32768 )
        {
            parm = -32768;
        }
        out = parm;
        FS_Write( &out, 2, clc.wavefile );
        parm = ( paintbuffer[i].right ) >> 8;
        if( parm > 32767 )
        {
            parm = 32767;
        }
        if( parm < -32768 )
        {
            parm = -32768;
        }
        out = parm;
        FS_Write( &out, 2, clc.wavefile );
    }
}


/*
====================
CL_PlayDemo_f

demo <demoname>

====================
*/
void CL_PlayDemo_f( void )
{
    char name[MAX_OSPATH], extension[32];
    char*        arg;
    
    if( Cmd_Argc() != 2 )
    {
        Com_Printf( "playdemo <demoname>\n" );
        return;
    }
    
    // make sure a local server is killed
    Cvar_Set( "sv_killserver", "1" );
    
    CL_Disconnect( true );
    
    
//	CL_FlushMemory();	//----(SA)	MEM NOTE: in missionpack, this is moved to CL_DownloadsComplete


    // open the demo file
    arg = Cmd_Argv( 1 );
    Com_sprintf( extension, sizeof( extension ), ".dm_%d", PROTOCOL_VERSION );
    if( !Q_stricmp( arg + strlen( arg ) - strlen( extension ), extension ) )
    {
        Com_sprintf( name, sizeof( name ), "demos/%s", arg );
    }
    else
    {
        Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", arg, PROTOCOL_VERSION );
    }
    
    FS_FOpenFileRead( name, &clc.demofile, true );
    if( !clc.demofile )
    {
        Com_Error( ERR_DROP, "couldn't open %s", name );
        return;
    }
    Q_strncpyz( clc.demoName, Cmd_Argv( 1 ), sizeof( clc.demoName ) );
    
    Con_Close();
    
    cls.state = CA_CONNECTED;
    clc.demoplaying = true;
    
    if( Cvar_VariableValue( "cl_wavefilerecord" ) )
    {
        CL_WriteWaveOpen();
        clc.waverecording = true;
    }
    
    Q_strncpyz( cls.servername, Cmd_Argv( 1 ), sizeof( cls.servername ) );
    
    // read demo messages until connected
    while( cls.state >= CA_CONNECTED && cls.state < CA_PRIMED )
    {
        CL_ReadDemoMessage();
        if( clc.waverecording )
        {
            CL_WriteWaveFilePacket();
        }
    }
    // don't get the first snapshot this frame, to prevent the long
    // time from the gamestate load from messing causing a time skip
    clc.firstDemoFrameSkipped = false;
    if( clc.waverecording )
    {
        CL_WriteWaveClose();
        clc.waverecording = false;
    }
}


/*
====================
CL_StartDemoLoop

Closing the main menu will restart the demo loop
====================
*/
void CL_StartDemoLoop( void )
{
    // start the demo loop again
    Cbuf_AddText( "d1\n" );
    cls.keyCatchers = 0;
}

/*
==================
CL_NextDemo

Called when a demo or cinematic finishes
If the "nextdemo" cvar is set, that command will be issued
==================
*/
void CL_NextDemo( void )
{
    char v[MAX_STRING_CHARS];
    
    Q_strncpyz( v, Cvar_VariableString( "nextdemo" ), sizeof( v ) );
    v[MAX_STRING_CHARS - 1] = 0;
    Com_DPrintf( "CL_NextDemo: %s\n", v );
    if( !v[0] )
    {
        return;
    }
    
    Cvar_Set( "nextdemo", "" );
    Cbuf_AddText( v );
    Cbuf_AddText( "\n" );
    Cbuf_Execute();
}


//======================================================================

/*
=====================
CL_ShutdownAll
=====================
*/
void CL_ShutdownAll( void )
{
    // clear sounds
    soundSystem->DisableSounds();
    // download subsystem
    DL_Shutdown();
    // shutdown CGame
    CL_ShutdownCGame();
    // shutdown UI
    CL_ShutdownUI();
    
    // shutdown the renderer
    renderSystem->Shutdown( false );
    
    cls.uiStarted = false;
    cls.cgameStarted = false;
    cls.rendererStarted = false;
    cls.soundRegistered = false;
}

/*
=================
CL_FlushMemory

Called by CL_MapLoading, CL_Connect_f, CL_PlayDemo_f, and CL_ParseGamestate the only
ways a client gets into a game
Also called by Com_Error
=================
*/
void CL_FlushMemory( void )
{

    // shutdown all the client stuff
    CL_ShutdownAll();
    
    // if not running a server clear the whole hunk
    if( !com_sv_running->integer )
    {
        // clear the whole hunk
        Hunk_Clear();
        // clear collision map data
        collisionModelManager->ClearMap();
    }
    else
    {
        // clear all the client data on the hunk
        Hunk_ClearToMark();
    }
    
    CL_StartHunkUsers();
}

/*
=====================
CL_MapLoading

A local server is starting to load a map, so update the
screen to let the user know about it, then dump all client
memory on the hunk from cgame, ui, and renderer
=====================
*/
void CL_MapLoading( void )
{
    if( !com_cl_running->integer )
    {
        return;
    }
    
    Con_Close();
    cls.keyCatchers = 0;
    
// this was for multi-threaded music
//	S_StartBackgroundTrack( "sound/music/l_briefing_1.wav", "", -2);	// '-2' for 'queue looping track' (QUEUED_PLAY_LOOPED)

    // if we are already connected to the local host, stay connected
    if( cls.state >= CA_CONNECTED && !Q_stricmp( cls.servername, "localhost" ) )
    {
        cls.state = CA_CONNECTED;       // so the connect screen is drawn
        memset( cls.updateInfoString, 0, sizeof( cls.updateInfoString ) );
        memset( clc.serverMessage, 0, sizeof( clc.serverMessage ) );
        memset( &cl.gameState, 0, sizeof( cl.gameState ) );
        clc.lastPacketSentTime = -9999;
        SCR_UpdateScreen();
    }
    else
    {
        // clear nextmap so the cinematic shutdown doesn't execute it
        Cvar_Set( "nextmap", "" );
        CL_Disconnect( true );
        Q_strncpyz( cls.servername, "localhost", sizeof( cls.servername ) );
        cls.state = CA_CHALLENGING;     // so the connect screen is drawn
        cls.keyCatchers = 0;
        SCR_UpdateScreen();
        clc.connectTime = -RETRANSMIT_TIMEOUT;
        NET_StringToAdr( cls.servername, &clc.serverAddress );
        // we don't need a challenge on the localhost
        
        CL_CheckForResend();
    }
    
    // make sure sound is quiet
    soundSystem->FadeAllSounds( 0, 0 );
}

/*
=====================
CL_ClearState

Called before parsing a gamestate
=====================
*/
void CL_ClearState( void )
{
    soundSystem->StopAllSounds();
    
    memset( &cl, 0, sizeof( cl ) );
}

/*
=====================
CL_ClearStaticDownload
Clear download information that we keep in cls (disconnected download support)
=====================
*/
void CL_ClearStaticDownload( void )
{
    assert( !cls.bWWWDlDisconnected ); // reset before calling
    cls.downloadRestart = false;
    cls.downloadTempName[0] = '\0';
    cls.downloadName[0] = '\0';
    cls.originalDownloadName[0] = '\0';
}

/*
=====================
CL_Disconnect

Called when a connection, demo, or cinematic is being terminated.
Goes from a connected state to either a menu state or a console state
Sends a disconnect message to the server
This is also called on Com_Error and Com_Quit, so it shouldn't cause any errors
=====================
*/
void CL_Disconnect( bool showMainMenu )
{
    if( !com_cl_running || !com_cl_running->integer )
    {
        return;
    }
    
    // shutting down the client so enter full screen ui mode
    Cvar_Set( "r_uiFullScreen", "1" );
    
    if( clc.demorecording )
    {
        CL_StopRecord_f();
    }
    
    if( !cls.bWWWDlDisconnected )
    {
        if( clc.download )
        {
            FS_FCloseFile( clc.download );
            clc.download = 0;
        }
        *cls.downloadTempName = *cls.downloadName = 0;
        Cvar_Set( "cl_downloadName", "" );
    }
    
    if( clc.demofile )
    {
        FS_FCloseFile( clc.demofile );
        clc.demofile = 0;
    }
    
    if( showMainMenu )
    {
        uiManager->SetActiveMenu( UIMENU_NONE );
    }
    
    SCR_StopCinematic();
    S_ClearSoundBuffer( true );  //----(SA)	modified
    
    // send a disconnect message to the server
    // send it a few times in case one is dropped
    if( cls.state >= CA_CONNECTED )
    {
        CL_AddReliableCommand( "disconnect" );
        CL_WritePacket();
        CL_WritePacket();
        CL_WritePacket();
    }
    
    CL_ClearState();
    
    // wipe the client connection
    memset( &clc, 0, sizeof( clc ) );
    
    if( !cls.bWWWDlDisconnected )
    {
        CL_ClearStaticDownload();
    }
    
    cls.state = CA_DISCONNECTED;
    
    // allow cheats locally
#ifndef WOLF_SP_DEMO
    // except for demo
    Cvar_Set( "sv_cheats", "1" );
#endif
    
    // not connected to a pure server anymore
    cl_connectedToPureServer = false;
    
    // don't try a restart if uivm is NULL, as we might be in the middle of a restart already
    if( cgvm && cls.state > CA_DISCONNECTED )
    {
        // restart the UI
        cls.state = CA_DISCONNECTED;
        
        // shutdown the UI
        CL_ShutdownUI();
        
        // init the UI
        CL_InitUI();
    }
    else
    {
        cls.state = CA_DISCONNECTED;
    }
}


/*
===================
CL_ForwardCommandToServer

adds the current command line as a clientCommand
things like godmode, noclip, etc, are commands directed to the server,
so when they are typed in at the console, they will need to be forwarded.
===================
*/
void CL_ForwardCommandToServer( const char* string )
{
    char*    cmd;
    
    cmd = Cmd_Argv( 0 );
    
    // ignore key up commands
    if( cmd[0] == '-' )
    {
        return;
    }
    
    if( clc.demoplaying || cls.state < CA_CONNECTED || cmd[0] == '+' )
    {
        Com_Printf( "Unknown command \"%s\"\n", cmd );
        return;
    }
    
    if( Cmd_Argc() > 1 )
    {
        CL_AddReliableCommand( string );
    }
    else
    {
        CL_AddReliableCommand( cmd );
    }
}

/*
===================
CL_RequestMotd

===================
*/
void CL_RequestMotd( void )
{
    char info[MAX_INFO_STRING];
    
    if( !cl_motd->integer )
    {
        return;
    }
    Com_Printf( "Resolving %s\n", UPDATE_SERVER_NAME );
    if( !NET_StringToAdr( UPDATE_SERVER_NAME, &cls.updateServer ) )
    {
        Com_Printf( "Couldn't resolve address\n" );
        return;
    }
    cls.updateServer.port = BigShort( PORT_UPDATE );
    Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", UPDATE_SERVER_NAME,
                cls.updateServer.ip[0], cls.updateServer.ip[1],
                cls.updateServer.ip[2], cls.updateServer.ip[3],
                BigShort( cls.updateServer.port ) );
                
    info[0] = 0;
    Com_sprintf( cls.updateChallenge, sizeof( cls.updateChallenge ), "%i", rand() );
    
    Info_SetValueForKey( info, "challenge", cls.updateChallenge );
    Info_SetValueForKey( info, "renderer", cls.glconfig.renderer_string );
    Info_SetValueForKey( info, "version", com_version->string );
    
    NET_OutOfBandPrint( NS_CLIENT, cls.updateServer, "getmotd \"%s\"\n", info );
}


/*
===================
CL_RequestAuthorization

Authorization server protocol
-----------------------------

All commands are text in Q3 out of band packets (leading 0xff 0xff 0xff 0xff).

Whenever the client tries to get a challenge from the server it wants to
connect to, it also blindly fires off a packet to the authorize server:

getKeyAuthorize <challenge> <cdkey>

cdkey may be "demo"


#OLD The authorize server returns a:
#OLD
#OLD keyAthorize <challenge> <accept | deny>
#OLD
#OLD A client will be accepted if the cdkey is valid and it has not been used by any other IP
#OLD address in the last 15 minutes.


The server sends a:

getIpAuthorize <challenge> <ip>

The authorize server returns a:

ipAuthorize <challenge> <accept | deny | demo | unknown >

A client will be accepted if a valid cdkey was sent by that ip (only) in the last 15 minutes.
If no response is received from the authorize server after two tries, the client will be let
in anyway.
===================
*/
void CL_RequestAuthorization( void )
{
    char nums[64];
    int i, j, l;
    cvar_t*  fs;
    
    if( !cls.authorizeServer.port )
    {
        Com_Printf( "Resolving %s\n", AUTHORIZE_SERVER_NAME );
        if( !NET_StringToAdr( AUTHORIZE_SERVER_NAME, &cls.authorizeServer ) )
        {
            Com_Printf( "Couldn't resolve address\n" );
            return;
        }
        
        cls.authorizeServer.port = BigShort( PORT_AUTHORIZE );
        Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", AUTHORIZE_SERVER_NAME,
                    cls.authorizeServer.ip[0], cls.authorizeServer.ip[1],
                    cls.authorizeServer.ip[2], cls.authorizeServer.ip[3],
                    BigShort( cls.authorizeServer.port ) );
    }
    if( cls.authorizeServer.type == NA_BAD )
    {
        return;
    }
    
    if( Cvar_VariableValue( "fs_restrict" ) )
    {
        Q_strncpyz( nums, "demo", sizeof( nums ) );
    }
    else
    {
        // only grab the alphanumeric values from the cdkey, to avoid any dashes or spaces
        j = 0;
        l = strlen( cl_cdkey );
        if( l > 32 )
        {
            l = 32;
        }
        for( i = 0 ; i < l ; i++ )
        {
            if( ( cl_cdkey[i] >= '0' && cl_cdkey[i] <= '9' )
                    || ( cl_cdkey[i] >= 'a' && cl_cdkey[i] <= 'z' )
                    || ( cl_cdkey[i] >= 'A' && cl_cdkey[i] <= 'Z' )
              )
            {
                nums[j] = cl_cdkey[i];
                j++;
            }
        }
        nums[j] = 0;
    }
    
    fs = Cvar_Get( "cl_anonymous", "0", CVAR_INIT | CVAR_SYSTEMINFO );
    NET_OutOfBandPrint( NS_CLIENT, cls.authorizeServer, va( "getKeyAuthorize %i %s", fs->integer, nums ) );
}

/*
======================================================================

CONSOLE COMMANDS

======================================================================
*/

/*
==================
CL_ForwardToServer_f
==================
*/
void CL_ForwardToServer_f( void )
{
    if( cls.state != CA_ACTIVE || clc.demoplaying )
    {
        Com_Printf( "Not connected to a server.\n" );
        return;
    }
    
    // don't forward the first argument
    if( Cmd_Argc() > 1 )
    {
        CL_AddReliableCommand( Cmd_Args() );
    }
}

/*
==================
CL_Setenv_f

Mostly for controlling voodoo environment variables
==================
*/
void CL_Setenv_f( void )
{
    int argc = Cmd_Argc();
    
    if( argc > 2 )
    {
        char buffer[1024];
        int i;
        
        strcpy( buffer, Cmd_Argv( 1 ) );
        strcat( buffer, "=" );
        
        for( i = 2; i < argc; i++ )
        {
            strcat( buffer, Cmd_Argv( i ) );
            strcat( buffer, " " );
        }
        
#ifdef _WIN32
        _putenv( buffer );
#else
        putenv( buffer );
#endif
    }
    else if( argc == 2 )
    {
        char* env = 0;
        
        if( env )
        {
            Com_Printf( "%s=%s\n", Cmd_Argv( 1 ), env );
        }
        else
        {
            Com_Printf( "%s undefined\n", Cmd_Argv( 1 ), env );
        }
    }
}


/*
==================
CL_Disconnect_f
==================
*/
void CL_Disconnect_f( void )
{
    SCR_StopCinematic();
    // RF, make sure loading variables are turned off
    Cvar_Set( "savegame_loading", "0" );
    Cvar_Set( "g_reloading", "0" );
    if( cls.state != CA_DISCONNECTED && cls.state != CA_CINEMATIC )
    {
        Com_Error( ERR_DISCONNECT, "Disconnected from server" );
    }
}


/*
================
CL_Reconnect_f
================
*/
void CL_Reconnect_f( void )
{
    if( !strlen( cls.servername ) || !strcmp( cls.servername, "localhost" ) )
    {
        Com_Printf( "Can't reconnect to localhost.\n" );
        return;
    }
    Cbuf_AddText( va( "connect %s\n", cls.servername ) );
}

/*
================
CL_Connect_f

================
*/
void CL_Connect_f( void )
{
    char*    server;
    const char*	serverString;
    
    if( Cmd_Argc() != 2 )
    {
        Com_Printf( "usage: connect [server]\n" );
        return;
    }
    
    soundSystem->StopAllSounds();
    
    // starting to load a map so we get out of full screen ui mode
    Cvar_Set( "r_uiFullScreen", "0" );
    
    // fire a message off to the motd server
    CL_RequestMotd();
    
    // clear any previous "server full" type messages
    clc.serverMessage[0] = 0;
    
    server = Cmd_Argv( 1 );
    
    if( com_sv_running->integer && !strcmp( server, "localhost" ) )
    {
        // if running a local server, kill it
        SV_Shutdown( "Server quit\n" );
    }
    
    // make sure a local server is killed
    Cvar_Set( "sv_killserver", "1" );
    SV_Frame( 0 );
    
    CL_Disconnect( true );
    Con_Close();
    
//	CL_FlushMemory();	//----(SA)	MEM NOTE: in missionpack, this is moved to CL_DownloadsComplete

    Q_strncpyz( cls.servername, server, sizeof( cls.servername ) );
    
    if( !NET_StringToAdr( cls.servername, &clc.serverAddress ) )
    {
        Com_Printf( "Bad server address\n" );
        cls.state = CA_DISCONNECTED;
        return;
    }
    if( clc.serverAddress.port == 0 )
    {
        clc.serverAddress.port = BigShort( PORT_SERVER );
    }
    serverString = NET_AdrToStringwPort( clc.serverAddress );
    Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", cls.servername,
                clc.serverAddress.ip[0], clc.serverAddress.ip[1],
                clc.serverAddress.ip[2], clc.serverAddress.ip[3],
                BigShort( clc.serverAddress.port ) );
                
    // if we aren't playing on a lan, we need to authenticate
    // with the cd key
    if( NET_IsLocalAddress( clc.serverAddress ) )
    {
        cls.state = CA_CHALLENGING;
    }
    else
    {
        cls.state = CA_CONNECTING;
    }
    
    // prepare to catch a connection process that would turn bad
    Cvar_Set( "com_errorDiagnoseIP", NET_AdrToString( clc.serverAddress ) );
    // we need to setup a correct default for this, otherwise the first val we set might reappear
    Cvar_Set( "com_errorMessage", "" );
    
    cls.keyCatchers = 0;
    clc.connectTime = -99999;   // CL_CheckForResend() will fire immediately
    clc.connectPacketCount = 0;
    
    // server connection string
    Cvar_Set( "cl_currentServerAddress", server );
    Cvar_Set( "mp_playerType", "0" );
    Cvar_Set( "mp_currentPlayerType", "0" );
    Cvar_Set( "mp_weapon", "0" );
    Cvar_Set( "mp_team", "0" );
    Cvar_Set( "mp_currentTeam", "0" );
    
    Cvar_Set( "ui_limboOptions", "0" );
    Cvar_Set( "ui_limboPrevOptions", "0" );
    Cvar_Set( "ui_limboObjective", "0" );
}

/*
=====================
CL_Rcon_f

  Send the rest of the command line over as
  an unconnected command.
=====================
*/
void CL_Rcon_f( void )
{
    char message[1024];
    netadr_t to;
    
    if( !rcon_client_password->string )
    {
        Com_Printf( "You must set 'rconpassword' before\n"
                    "issuing an rcon command.\n" );
        return;
    }
    
    message[0] = -1;
    message[1] = -1;
    message[2] = -1;
    message[3] = -1;
    message[4] = 0;
    
    strcat( message, "rcon " );
    
    strcat( message, rcon_client_password->string );
    strcat( message, " " );
    
    strcat( message, Cmd_Cmd() + 5 );
    
    if( cls.state >= CA_CONNECTED )
    {
        to = clc.netchan.remoteAddress;
    }
    else
    {
        if( !strlen( rconAddress->string ) )
        {
            Com_Printf( "You must either be connected,\n"
                        "or set the 'rconAddress' cvar\n"
                        "to issue rcon commands\n" );
                        
            return;
        }
        NET_StringToAdr( rconAddress->string, &to );
        if( to.port == 0 )
        {
            to.port = BigShort( PORT_SERVER );
        }
    }
    
    NET_SendPacket( NS_CLIENT, strlen( message ) + 1, message, to );
}

/*
=================
CL_SendPureChecksums
=================
*/
void CL_SendPureChecksums( void )
{
    const char* pChecksums;
    char cMsg[MAX_INFO_VALUE];
    int i;
    
    // if we are pure we need to send back a command with our referenced pk3 checksums
    pChecksums = FS_ReferencedPakPureChecksums();
    
    // "cp"
    Com_sprintf( cMsg, sizeof( cMsg ), "Va " );
    Q_strcat( cMsg, sizeof( cMsg ), va( "%d ", cl.serverId ) );
    Q_strcat( cMsg, sizeof( cMsg ), pChecksums );
    for( i = 0; i < 2; i++ )
    {
        cMsg[i] += 13 + ( i * 2 );
    }
    CL_AddReliableCommand( cMsg );
}

/*
=================
CL_ResetPureClientAtServer
=================
*/
void CL_ResetPureClientAtServer( void )
{
    CL_AddReliableCommand( va( "vdr" ) );
}

/*
=================
CL_Vid_Restart_f

Restart the video subsystem

we also have to reload the UI and CGame because the renderer
doesn't know what graphics to reload
=================
*/
void CL_Vid_Restart_f( void )
{
    vmCvar_t musicCvar;
    
    // RF, don't show percent bar, since the memory usage will just sit at the same level anyway
    Cvar_Set( "com_expectedhunkusage", "-1" );
    
    // don't let them loop during the restart
    soundSystem->StopAllSounds();
    // shutdown the UI
    CL_ShutdownUI();
    // shutdown the CGame
    CL_ShutdownCGame();
    // shutdown the renderer and clear the renderer interface
    CL_ShutdownRef();
    // client is no longer pure untill new checksums are sent
    CL_ResetPureClientAtServer();
    // clear pak references
    FS_ClearPakReferences( FS_UI_REF | FS_CGAME_REF );
    // reinitialize the filesystem if the game directory or checksum has changed
    FS_ConditionalRestart( clc.checksumFeed );
    
    collisionModelManager->ClearMap();
    
    soundSystem->BeginRegistration();  // all sound handles are now invalid
    
    cls.rendererStarted = false;
    cls.uiStarted = false;
    cls.cgameStarted = false;
    cls.soundRegistered = false;
    
    // unpause so the cgame definately gets a snapshot and renders a frame
    Cvar_Set( "cl_paused", "0" );
    
    // if not running a server clear the whole hunk
    if( !com_sv_running->integer )
    {
        // clear the whole hunk
        Hunk_Clear();
    }
    else
    {
        // clear all the client data on the hunk
        Hunk_ClearToMark();
    }
    
    // initialize the renderer interface
    CL_InitRef();
    
    // startup all the client stuff
    CL_StartHunkUsers();
    
    // start the cgame if connected
    if( cls.state > CA_CONNECTED && cls.state != CA_CINEMATIC )
    {
        cls.cgameStarted = true;
        CL_InitCGame();
        // send pure checksums
        CL_SendPureChecksums();
    }
    
    // start music if there was any
    
    Cvar_Register( &musicCvar, "s_currentMusic", "", CVAR_ROM );
    if( strlen( musicCvar.string ) )
    {
        soundSystem->StartBackgroundTrack( musicCvar.string, musicCvar.string, 1000 );
    }
    
    // fade up volume
    soundSystem->FadeAllSounds( 1, 0 );
}

/*
=================
CL_UI_Restart_f

Restart the ui subsystem
=================
*/
void CL_UI_Restart_f( void )
{
    // shutdown the UI
    CL_ShutdownUI();
    
    // init the UI
    CL_InitUI();
}

/*
=================
CL_Snd_Restart_f

Restart the sound subsystem
The cgame and game must also be forced to restart because
handles will be invalid
=================
*/
void CL_Snd_Restart_f( void )
{
    soundSystem->Shutdown();
    soundSystem->Init();
    
    CL_Vid_Restart_f();
}


/*
==================
CL_PK3List_f
==================
*/
void CL_OpenedPK3List_f( void )
{
    Com_Printf( "Opened PK3 Names: %s\n", FS_LoadedPakNames() );
}

/*
==================
CL_PureList_f
==================
*/
void CL_ReferencedPK3List_f( void )
{
    Com_Printf( "Referenced PK3 Names: %s\n", FS_ReferencedPakNames() );
}

/*
==================
CL_Configstrings_f
==================
*/
void CL_Configstrings_f( void )
{
    int i;
    int ofs;
    
    if( cls.state != CA_ACTIVE )
    {
        Com_Printf( "Not connected to a server.\n" );
        return;
    }
    
    for( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
    {
        ofs = cl.gameState.stringOffsets[ i ];
        if( !ofs )
        {
            continue;
        }
        Com_Printf( "%4i: %s\n", i, cl.gameState.stringData + ofs );
    }
}

/*
==============
CL_Clientinfo_f
==============
*/
void CL_Clientinfo_f( void )
{
    Com_Printf( "--------- Client Information ---------\n" );
    Com_Printf( "state: %i\n", cls.state );
    Com_Printf( "Server: %s\n", cls.servername );
    Com_Printf( "User info settings:\n" );
    Info_Print( Cvar_InfoString( CVAR_USERINFO ) );
    Com_Printf( "--------------------------------------\n" );
}


//====================================================================

/*
=================
CL_DownloadsComplete

Called when all downloading has been completed
=================
*/
void CL_DownloadsComplete( void )
{
    // if we downloaded files we need to restart the file system
    if( cls.downloadRestart )
    {
        cls.downloadRestart = false;
        
        FS_Restart( clc.checksumFeed ); // We possibly downloaded a pak, restart the file system to load it
        
        // inform the server so we get new gamestate info
        if( !cls.bWWWDlDisconnected )
        {
            CL_AddReliableCommand( "donedl" );
        }
        
        // we can reset that now
        cls.bWWWDlDisconnected = false;
        CL_ClearStaticDownload();
        
        // by sending the donenl command we request a new gamestate
        // so we don't want to load stuff yet
        return;
    }
    
    // TTimo: I wonder if that happens - it should not but I suspect it could happen if a download fails in the middle or is aborted
    assert( !cls.bWWWDlDisconnected );
    
    // let the client game init and load data
    cls.state = CA_LOADING;
    
    // Pump the loop, this may change gamestate!
    Com_EventLoop();
    
    // if the gamestate was changed by calling Com_EventLoop
    // then we loaded everything already and we don't want to do it again.
    if( cls.state != CA_LOADING )
    {
        return;
    }
    
    // starting to load a map so we get out of full screen ui mode
    Cvar_Set( "r_uiFullScreen", "0" );
    
    // flush client memory and start loading stuff
    // this will also (re)load the UI
    // if this is a local client then only the client part of the hunk
    // will be cleared, note that this is done after the hunk mark has been set
    CL_FlushMemory();
    
    // initialize the CGame
    cls.cgameStarted = true;
    CL_InitCGame();
    
    // set pure checksums
    CL_SendPureChecksums();
    
    CL_WritePacket();
    CL_WritePacket();
    CL_WritePacket();
}

/*
=================
CL_BeginDownload

Requests a file to download from the server.  Stores it in the current
game directory.
=================
*/
void CL_BeginDownload( const char* localName, const char* remoteName )
{

    Com_DPrintf( "***** CL_BeginDownload *****\n"
                 "Localname: %s\n"
                 "Remotename: %s\n"
                 "****************************\n", localName, remoteName );
                 
    Q_strncpyz( cls.downloadName, localName, sizeof( cls.downloadName ) );
    Com_sprintf( cls.downloadTempName, sizeof( cls.downloadTempName ), "%s.tmp", localName );
    
    // Set so UI gets access to it
    Cvar_Set( "cl_downloadName", remoteName );
    Cvar_Set( "cl_downloadSize", "0" );
    Cvar_Set( "cl_downloadCount", "0" );
    Cvar_SetValue( "cl_downloadTime", cls.realtime );
    
    clc.downloadBlock = 0; // Starting new file
    clc.downloadCount = 0;
    
    CL_AddReliableCommand( va( "download %s", remoteName ) );
}

/*
=================
CL_NextDownload

A download completed or failed
=================
*/
void CL_NextDownload( void )
{
    char* s;
    char* remoteName, *localName;
    
    // We are looking to start a download here
    if( *clc.downloadList )
    {
        s = clc.downloadList;
        
        // format is:
        //  @remotename@localname@remotename@localname, etc.
        
        if( *s == '@' )
        {
            s++;
        }
        remoteName = s;
        
        if( ( s = strchr( s, '@' ) ) == NULL )
        {
            CL_DownloadsComplete();
            return;
        }
        
        *s++ = 0;
        localName = s;
        if( ( s = strchr( s, '@' ) ) != NULL )
        {
            *s++ = 0;
        }
        else
        {
            s = localName + strlen( localName ); // point at the nul byte
        }
        CL_BeginDownload( localName, remoteName );
        
        cls.downloadRestart = true;
        
        // move over the rest
        memmove( clc.downloadList, s, strlen( s ) + 1 );
        
        return;
    }
    
    CL_DownloadsComplete();
}

/*
=================
CL_InitDownloads

After receiving a valid game state, we valid the cgame and local zip files here
and determine if we need to download them
=================
*/
void CL_InitDownloads( void )
{
    char missingfiles[1024];
    
    // TTimo
    // init some of the www dl data
    clc.bWWWDl = false;
    clc.bWWWDlAborting = false;
    cls.bWWWDlDisconnected = false;
    CL_ClearStaticDownload();
    
    // whatever autodownlad configuration, store missing files in a cvar, use later in the ui maybe
    if( FS_ComparePaks( missingfiles, sizeof( missingfiles ), false ) )
    {
        Cvar_Set( "com_missingFiles", missingfiles );
    }
    else
    {
        Cvar_Set( "com_missingFiles", "" );
    }
    
    // reset the redirect checksum tracking
    clc.redirectedList[0] = '\0';
    
    if( cl_allowDownload->integer && FS_ComparePaks( clc.downloadList, sizeof( clc.downloadList ), false ) )
    {
        if( *clc.downloadList )
        {
            // if autodownloading is not enabled on the server
            cls.state = CA_CONNECTED;
            CL_NextDownload();
            return;
        }
    }
    
    CL_DownloadsComplete();
}

/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend( void )
{
    int port, i;
    char info[MAX_INFO_STRING];
    char data[MAX_INFO_STRING];
    
    // don't send anything if playing back a demo
    if( clc.demoplaying )
    {
        return;
    }
    
    // resend if we haven't gotten a reply yet
    if( cls.state != CA_CONNECTING && cls.state != CA_CHALLENGING )
    {
        return;
    }
    
    if( cls.realtime - clc.connectTime < RETRANSMIT_TIMEOUT )
    {
        return;
    }
    
    clc.connectTime = cls.realtime; // for retransmit requests
    clc.connectPacketCount++;
    
    
    switch( cls.state )
    {
        case CA_CONNECTING:
            // requesting a challenge
            if( !Sys_IsLANAddress( clc.serverAddress ) )
            {
                CL_RequestAuthorization();
            }
            NET_OutOfBandPrint( NS_CLIENT, clc.serverAddress, "getchallenge" );
            break;
            
        case CA_CHALLENGING:
            // sending back the challenge
            port = Cvar_VariableValue( "net_qport" );
            
            Q_strncpyz( info, Cvar_InfoString( CVAR_USERINFO ), sizeof( info ) );
            Info_SetValueForKey( info, "protocol", va( "%i", PROTOCOL_VERSION ) );
            Info_SetValueForKey( info, "qport", va( "%i", port ) );
            Info_SetValueForKey( info, "challenge", va( "%i", clc.challenge ) );
            
            strcpy( data, "connect " );
            
            data[8] = '\"'; // spaces in name bugfix
            
            for( i = 0; i < strlen( info ); i++ )
            {
                data[9 + i] = info[i];    // + (clc.challenge)&0x3;
            }
            data[9 + i] = '\"';  // spaces in name bugfix
            data[10 + i] = 0;
            
            NET_OutOfBandData( NS_CLIENT, clc.serverAddress, ( byte* )&data[0], i + 10 );
            
            // the most current userinfo has been sent, so watch for any
            // newer changes to userinfo variables
            cvar_modifiedFlags &= ~CVAR_USERINFO;
            break;
            
        default:
            Com_Error( ERR_FATAL, "CL_CheckForResend: bad cls.state" );
    }
}


/*
===================
CL_DisconnectPacket

Sometimes the server can drop the client and the netchan based
disconnect can be lost.  If the client continues to send packets
to the server, the server will send out of band disconnect packets
to the client so it doesn't have to wait for the full timeout period.
===================
*/
void CL_DisconnectPacket( netadr_t from )
{
    const char* message;
    
    if( cls.state < CA_AUTHORIZING )
    {
        return;
    }
    
    // if not from our server, ignore it
    if( !NET_CompareAdr( from, clc.netchan.remoteAddress ) )
    {
        return;
    }
    
    // if we have received packets within three seconds, ignore (it might be a malicious spoof)
    // NOTE TTimo:
    // there used to be a  clc.lastPacketTime = cls.realtime; line in CL_PacketEvent before calling CL_ConnectionLessPacket
    // therefore .. packets never got through this check, clients never disconnected
    // switched the clc.lastPacketTime = cls.realtime to happen after the connectionless packets have been processed
    // you still can't spoof disconnects, cause legal netchan packets will maintain realtime - lastPacketTime below the threshold
    if( cls.realtime - clc.lastPacketTime < 3000 )
    {
        return;
    }
    
    // if we are doing a disconnected download, leave the 'connecting' screen on with the progress information
    if( !cls.bWWWDlDisconnected )
    {
        // drop the connection
        message = "Server disconnected for unknown reason\n";
        Com_Printf( message );
        Cvar_Set( "com_errorMessage", message );
        CL_Disconnect( true );
    }
    else
    {
        CL_Disconnect( false );
    }
}


/*
===================
CL_MotdPacket
===================
*/
void CL_MotdPacket( netadr_t from )
{
    char*    challenge;
    char*    info;
    
    // if not from our server, ignore it
    if( !NET_CompareAdr( from, cls.updateServer ) )
    {
        return;
    }
    
    info = Cmd_Argv( 1 );
    
    // check challenge
    challenge = Info_ValueForKey( info, "challenge" );
    if( strcmp( challenge, cls.updateChallenge ) )
    {
        return;
    }
    
    challenge = Info_ValueForKey( info, "motd" );
    
    Q_strncpyz( cls.updateInfoString, info, sizeof( cls.updateInfoString ) );
    Cvar_Set( "cl_motdString", challenge );
}

/*
===================
CL_PrintPackets
an OOB message from server, with potential markups
print OOB are the only messages we handle markups in
[err_dialog]: used to indicate that the connection should be aborted
no further information, just do an error diagnostic screen afterwards
[err_prot]: HACK. This is a protocol error. The client uses a custom
protocol error message (client sided) in the diagnostic window.
The space for the error message on the connection screen is limited
to 256 chars.
===================
*/
void CL_PrintPacket( netadr_t from, msg_t* msg )
{
    char* s;
    s = MSG_ReadBigString( msg );
    if( !Q_stricmpn( s, "[err_dialog]", 12 ) )
    {
        Q_strncpyz( clc.serverMessage, s + 12, sizeof( clc.serverMessage ) );
        Cvar_Set( "com_errorMessage", clc.serverMessage );
    }
    else if( !Q_stricmpn( s, "[err_prot]", 10 ) )
    {
        Q_strncpyz( clc.serverMessage, s + 10, sizeof( clc.serverMessage ) );
        Cvar_Set( "com_errorMessage", PROTOCOL_MISMATCH_ERROR_LONG );
    }
    else
    {
        Q_strncpyz( clc.serverMessage, s, sizeof( clc.serverMessage ) );
    }
    Com_Printf( "%s", clc.serverMessage );
}

/*
===================
CL_InitServerInfo
===================
*/
void CL_InitServerInfo( serverInfo_t* server, serverAddress_t* address )
{
    server->adr.type  = NA_IP;
    server->adr.ip[0] = address->ip[0];
    server->adr.ip[1] = address->ip[1];
    server->adr.ip[2] = address->ip[2];
    server->adr.ip[3] = address->ip[3];
    server->adr.port  = address->port;
    server->clients = 0;
    server->hostName[0] = '\0';
    server->mapName[0] = '\0';
    server->maxClients = 0;
    server->maxPing = 0;
    server->minPing = 0;
    server->ping = -1;
    server->game[0] = '\0';
    server->gameType = 0;
    server->netType = 0;
    server->allowAnonymous = 0;
}

#define MAX_SERVERSPERPACKET    256

/*
===================
CL_ServersResponsePacket
===================
*/
void CL_ServersResponsePacket( netadr_t from, msg_t* msg )
{
    int i, count, max, total;
    serverAddress_t addresses[MAX_SERVERSPERPACKET];
    int numservers;
    byte*           buffptr;
    byte*           buffend;
    
    Com_Printf( "CL_ServersResponsePacket\n" );
    
    if( cls.numglobalservers == -1 )
    {
        // state to detect lack of servers or lack of response
        cls.numglobalservers = 0;
        cls.numGlobalServerAddresses = 0;
    }
    
    if( cls.nummplayerservers == -1 )
    {
        cls.nummplayerservers = 0;
    }
    
    // parse through server response string
    numservers = 0;
    buffptr    = msg->data;
    buffend    = buffptr + msg->cursize;
    while( buffptr + 1 < buffend )
    {
        // advance to initial token
        do
        {
            if( *buffptr++ == '\\' )
            {
                break;
            }
        }
        while( buffptr < buffend );
        
        if( buffptr >= buffend - 6 )
        {
            break;
        }
        
        // parse out ip
        addresses[numservers].ip[0] = *buffptr++;
        addresses[numservers].ip[1] = *buffptr++;
        addresses[numservers].ip[2] = *buffptr++;
        addresses[numservers].ip[3] = *buffptr++;
        
        // parse out port
        addresses[numservers].port = ( *buffptr++ ) << 8;
        addresses[numservers].port += *buffptr++;
        addresses[numservers].port = BigShort( addresses[numservers].port );
        
        // syntax check
        if( *buffptr != '\\' )
        {
            break;
        }
        
        Com_DPrintf( "server: %d ip: %d.%d.%d.%d:%d\n", numservers,
                     addresses[numservers].ip[0],
                     addresses[numservers].ip[1],
                     addresses[numservers].ip[2],
                     addresses[numservers].ip[3],
                     addresses[numservers].port );
                     
        numservers++;
        if( numservers >= MAX_SERVERSPERPACKET )
        {
            break;
        }
        
        // parse out EOT
        if( buffptr[1] == 'E' && buffptr[2] == 'O' && buffptr[3] == 'T' )
        {
            break;
        }
    }
    
    if( cls.masterNum == 0 )
    {
        count = cls.numglobalservers;
        max = MAX_GLOBAL_SERVERS;
    }
    else
    {
        count = cls.nummplayerservers;
        max = MAX_OTHER_SERVERS;
    }
    
    for( i = 0; i < numservers && count < max; i++ )
    {
        // build net address
        serverInfo_t* server = ( cls.masterNum == 0 ) ? &cls.globalServers[count] : &cls.mplayerServers[count];
        
        CL_InitServerInfo( server, &addresses[i] );
        // advance to next slot
        count++;
    }
    
    // if getting the global list
    if( cls.masterNum == 0 )
    {
        if( cls.numGlobalServerAddresses < MAX_GLOBAL_SERVERS )
        {
            // if we couldn't store the servers in the main list anymore
            for( ; i < numservers && count >= max; i++ )
            {
                serverAddress_t* addr;
                // just store the addresses in an additional list
                addr = &cls.globalServerAddresses[cls.numGlobalServerAddresses++];
                addr->ip[0] = addresses[i].ip[0];
                addr->ip[1] = addresses[i].ip[1];
                addr->ip[2] = addresses[i].ip[2];
                addr->ip[3] = addresses[i].ip[3];
                addr->port  = addresses[i].port;
            }
        }
    }
    
    if( cls.masterNum == 0 )
    {
        cls.numglobalservers = count;
        total = count + cls.numGlobalServerAddresses;
    }
    else
    {
        cls.nummplayerservers = count;
        total = count;
    }
    
    Com_Printf( "%d servers parsed (total %d)\n", numservers, total );
}

/*
=================
CL_ConnectionlessPacket

Responses to broadcasts, etc
=================
*/
void CL_ConnectionlessPacket( netadr_t from, msg_t* msg )
{
    char*    s;
    char*    c;
    
    MSG_BeginReadingOOB( msg );
    MSG_ReadLong( msg );    // skip the -1
    
    s = MSG_ReadStringLine( msg );
    
    Cmd_TokenizeString( s );
    
    c = Cmd_Argv( 0 );
    
    Com_DPrintf( "CL packet %s: %s\n", NET_AdrToString( from ), c );
    
    // challenge from the server we are connecting to
    if( !Q_stricmp( c, "challengeResponse" ) )
    {
        if( cls.state != CA_CONNECTING )
        {
            Com_Printf( "Unwanted challenge response received.  Ignored.\n" );
        }
        else
        {
            // start sending challenge repsonse instead of challenge request packets
            clc.challenge = atoi( Cmd_Argv( 1 ) );
            if( Cmd_Argc() > 2 )
            {
                clc.onlyVisibleClients = atoi( Cmd_Argv( 2 ) );
            }
            else
            {
                clc.onlyVisibleClients = 0;
            }
            cls.state = CA_CHALLENGING;
            clc.connectPacketCount = 0;
            clc.connectTime = -99999;
            
            // take this address as the new server address.  This allows
            // a server proxy to hand off connections to multiple servers
            clc.serverAddress = from;
            Com_DPrintf( "challenge: %d\n", clc.challenge );
        }
        return;
    }
    
    // server connection
    if( !Q_stricmp( c, "connectResponse" ) )
    {
        if( cls.state >= CA_CONNECTED )
        {
            Com_Printf( "Dup connect received.  Ignored.\n" );
            return;
        }
        if( cls.state != CA_CHALLENGING )
        {
            Com_Printf( "connectResponse packet while not connecting.  Ignored.\n" );
            return;
        }
        if( !NET_CompareBaseAdr( from, clc.serverAddress ) )
        {
            Com_Printf( "connectResponse from a different address.  Ignored.\n" );
            Com_Printf( "%s should have been %s\n", NET_AdrToString( from ),
                        NET_AdrToString( clc.serverAddress ) );
            return;
        }
        Netchan_Setup( NS_CLIENT, &clc.netchan, from, Cvar_VariableValue( "net_qport" ) );
        cls.state = CA_CONNECTED;
        clc.lastPacketSentTime = -9999;     // send first packet immediately
        return;
    }
    
    // server responding to an info broadcast
    if( !Q_stricmp( c, "infoResponse" ) )
    {
        CL_ServerInfoPacket( from, msg );
        return;
    }
    
    // server responding to a get playerlist
    if( !Q_stricmp( c, "statusResponse" ) )
    {
        CL_ServerStatusResponse( from, msg );
        return;
    }
    
    // a disconnect message from the server, which will happen if the server
    // dropped the connection but it is still getting packets from us
    if( !Q_stricmp( c, "disconnect" ) )
    {
        CL_DisconnectPacket( from );
        return;
    }
    
    // echo request from server
    if( !Q_stricmp( c, "echo" ) )
    {
        NET_OutOfBandPrint( NS_CLIENT, from, "%s", Cmd_Argv( 1 ) );
        return;
    }
    
    // cd check
    if( !Q_stricmp( c, "keyAuthorize" ) )
    {
        // we don't use these now, so dump them on the floor
        return;
    }
    
    // global MOTD from id
    if( !Q_stricmp( c, "motd" ) )
    {
        CL_MotdPacket( from );
        return;
    }
    
    // echo request from server
    if( !Q_stricmp( c, "print" ) )
    {
        CL_PrintPacket( from, msg );
        return;
    }
    
    // bugfix, make this compare first n chars so it doesnt bail if token is parsed incorrectly
    // echo request from server
    if( !Q_strncmp( c, "getserversResponse", 18 ) )
    {
        CL_ServersResponsePacket( from, msg );
        return;
    }
    
    Com_DPrintf( "Unknown connectionless packet command.\n" );
}


/*
=================
CL_PacketEvent

A packet has arrived from the main event loop
=================
*/
void CL_PacketEvent( netadr_t from, msg_t* msg )
{
    int headerBytes;
    
    if( msg->cursize >= 4 && *( int* )msg->data == -1 )
    {
        CL_ConnectionlessPacket( from, msg );
        return;
    }
    
    clc.lastPacketTime = cls.realtime;
    
    if( cls.state < CA_CONNECTED )
    {
        return;     // can't be a valid sequenced packet
    }
    
    if( msg->cursize < 4 )
    {
        Com_Printf( "%s: Runt packet\n", NET_AdrToString( from ) );
        return;
    }
    
    //
    // packet from server
    //
    if( !NET_CompareAdr( from, clc.netchan.remoteAddress ) )
    {
        Com_DPrintf( "%s:sequenced packet without connection\n"
                     , NET_AdrToString( from ) );
        // FIXME: send a client disconnect?
        return;
    }
    
    if( !CL_Netchan_Process( &clc.netchan, msg ) )
    {
        return;     // out of order, duplicated, etc
    }
    
    // the header is different lengths for reliable and unreliable messages
    headerBytes = msg->readcount;
    
    // track the last message received so it can be returned in
    // client messages, allowing the server to detect a dropped
    // gamestate
    clc.serverMessageSequence = LittleLong( *( int* )msg->data );
    
    clc.lastPacketTime = cls.realtime;
    CL_ParseServerMessage( msg );
    
    //
    // we don't know if it is ok to save a demo message until
    // after we have parsed the frame
    //
    if( clc.demorecording && !clc.demowaiting )
    {
        CL_WriteDemoMessage( msg, headerBytes );
    }
}

/*
==================
CL_CheckTimeout

==================
*/
void CL_CheckTimeout( void )
{
    //
    // check timeout
    //
    if( ( !cl_paused->integer || !sv_paused->integer )
            && cls.state >= CA_CONNECTED && cls.state != CA_CINEMATIC
            && cls.realtime - clc.lastPacketTime > cl_timeout->value * 1000 )
    {
        if( ++cl.timeoutcount > 5 )       // timeoutcount saves debugger
        {
            Com_Printf( "\nServer connection timed out.\n" );
            CL_Disconnect( true );
            return;
        }
    }
    else
    {
        cl.timeoutcount = 0;
    }
}


//============================================================================

/*
==================
CL_CheckUserinfo

==================
*/
void CL_CheckUserinfo( void )
{
    // don't add reliable commands when not yet connected
    if( cls.state < CA_CHALLENGING )
    {
        return;
    }
    // don't overflow the reliable command buffer when paused
    if( cl_paused->integer )
    {
        return;
    }
    // send a reliable userinfo update if needed
    if( cvar_modifiedFlags & CVAR_USERINFO )
    {
        cvar_modifiedFlags &= ~CVAR_USERINFO;
        CL_AddReliableCommand( va( "userinfo \"%s\"", Cvar_InfoString( CVAR_USERINFO ) ) );
    }
    
}

/*
==================
CL_WWWDownload
==================
*/
void CL_WWWDownload( void )
{
    char* to_ospath;
    dlStatus_t ret;
    static bool bAbort = false;
    
    if( clc.bWWWDlAborting )
    {
        if( !bAbort )
        {
            Com_DPrintf( "CL_WWWDownload: WWWDlAborting\n" );
            bAbort = true;
        }
        return;
    }
    if( bAbort )
    {
        Com_DPrintf( "CL_WWWDownload: WWWDlAborting done\n" );
        bAbort = false;
    }
    
    ret = DL_DownloadLoop();
    
    if( ret == DL_CONTINUE )
    {
        return;
    }
    
    if( ret == DL_DONE )
    {
        // taken from CL_ParseDownload
        // we work with OS paths
        clc.download = 0;
        to_ospath = FS_BuildOSPath( Cvar_VariableString( "fs_homepath" ), cls.originalDownloadName, "" );
        to_ospath[strlen( to_ospath ) - 1] = '\0';
        if( rename( cls.downloadTempName, to_ospath ) )
        {
            FS_CopyFile( cls.downloadTempName, to_ospath );
            remove( cls.downloadTempName );
        }
        *cls.downloadTempName = *cls.downloadName = 0;
        Cvar_Set( "cl_downloadName", "" );
        if( cls.bWWWDlDisconnected )
        {
            CL_AddReliableCommand( "wwwdl done" );
            // tracking potential web redirects leading us to wrong checksum - only works in connected mode
            if( strlen( clc.redirectedList ) + strlen( cls.originalDownloadName ) + 1 >= sizeof( clc.redirectedList ) )
            {
                // just to be safe
                Com_Printf( "ERROR: redirectedList overflow (%s)\n", clc.redirectedList );
            }
            else
            {
                strcat( clc.redirectedList, "@" );
                strcat( clc.redirectedList, cls.originalDownloadName );
            }
        }
    }
    else
    {
        if( cls.bWWWDlDisconnected )
        {
            // in a connected download, we'd tell the server about failure and wait for a reply
            // but in this case we can't get anything from server
            // if we just reconnect it's likely we'll get the same disconnected download message, and error out again
            // this may happen for a regular dl or an auto update
            const char* error = va( "Download failure while getting '%s'\n", cls.downloadName ); // get the msg before clearing structs
            cls.bWWWDlDisconnected = false; // need clearing structs before ERR_DROP, or it goes into endless reload
            CL_ClearStaticDownload();
            Com_Error( ERR_DROP, error );
        }
        else
        {
            // see CL_ParseDownload, same abort strategy
            Com_Printf( "Download failure while getting '%s'\n", cls.downloadName );
            CL_AddReliableCommand( "wwwdl fail" );
            clc.bWWWDlAborting = true;
        }
        return;
    }
    
    clc.bWWWDl = false;
    CL_NextDownload();
}

/*
==================
CL_WWWBadChecksum

FS code calls this when doing FS_ComparePaks
we can detect files that we got from a www dl redirect with a wrong checksum
this indicates that the redirect setup is broken, and next dl attempt should NOT redirect
==================
*/
bool CL_WWWBadChecksum( const char* pakname )
{
    if( strstr( clc.redirectedList, va( "@%s", pakname ) ) )
    {
        Com_Printf( "WARNING: file %s obtained through download redirect has wrong checksum\n", pakname );
        Com_Printf( "         this likely means the server configuration is broken\n" );
        if( strlen( clc.badChecksumList ) + strlen( pakname ) + 1 >= sizeof( clc.badChecksumList ) )
        {
            Com_Printf( "ERROR: badChecksumList overflowed (%s)\n", clc.badChecksumList );
            return false;
        }
        strcat( clc.badChecksumList, "@" );
        strcat( clc.badChecksumList, pakname );
        Com_DPrintf( "bad checksums: %s\n", clc.badChecksumList );
        return true;
    }
    return false;
}



/*
==================
CL_Frame
==================
*/
void CL_Frame( int msec )
{

    if( !com_cl_running->integer )
    {
        return;
    }
    
    if( cls.cddialog )
    {
        // bring up the cd error dialog if needed
        cls.cddialog = false;
        uiManager->SetActiveMenu( UIMENU_NEED_CD );
    }
    else if( cls.endgamemenu )
    {
        cls.endgamemenu = false;
        uiManager->SetActiveMenu( UIMENU_ENDGAME );
    }
    else if( cls.state == CA_DISCONNECTED && !( cls.keyCatchers & KEYCATCH_UI )
             && !com_sv_running->integer )
    {
        // if disconnected, bring up the menu
        soundSystem->StopAllSounds();
        uiManager->SetActiveMenu( UIMENU_MAIN );
    }
    
    // if recording an avi, lock to a fixed fps
    if( cl_avidemo->integer && msec )
    {
        // save the current screen
        if( cls.state == CA_ACTIVE || cl_forceavidemo->integer )
        {
            Cbuf_ExecuteText( EXEC_NOW, "screenshot silent\n" );
        }
        // fixed time for next frame
        msec = ( 1000 / cl_avidemo->integer ) * com_timescale->value;
        if( msec == 0 )
        {
            msec = 1;
        }
    }
    
    // save the msec before checking pause
    cls.realFrametime = msec;
    
    // decide the simulation time
    cls.frametime = msec;
    
    cls.realtime += cls.frametime;
    
    if( cl_timegraph->integer )
    {
        SCR_DebugGraph( cls.realFrametime * 0.25, 0 );
    }
    
    // see if we need to update any userinfo
    CL_CheckUserinfo();
    
    // if we haven't gotten a packet in a long time,
    // drop the connection
    CL_CheckTimeout();
    
    // wwwdl download may survive a server disconnect
    if( ( cls.state == CA_CONNECTED && clc.bWWWDl ) || cls.bWWWDlDisconnected )
    {
        CL_WWWDownload();
    }
    
    // send intentions now
    CL_SendCmd();
    
    // resend a connection request if necessary
    CL_CheckForResend();
    
    // decide on the serverTime to render
    CL_SetCGameTime();
    
    // update the screen
    SCR_UpdateScreen();
    
    // Ridah, don't update if we're doing a quick reload
//	if (Cvar_VariableIntegerValue("savegame_loading") != 2) {
//		// if waiting at intermission, don't update sound
//		char buf[MAX_QPATH];
//		Cvar_VariableStringBuffer( "g_missionStats", buf, sizeof(buf) );
//		if (strlen(buf) <= 1 ) {
//			// update audio
    soundSystem->Update();
//		}
//	}

    // advance local effects for next frame
    SCR_RunCinematic();
    
    Con_RunConsole();
    
    cls.framecount++;
}


//============================================================================
// Ridah, startup-caching system
typedef struct
{
    char name[MAX_QPATH];
    int hits;
    int lastSetIndex;
} cacheItem_t;
typedef enum
{
    CACHE_SOUNDS,
    CACHE_MODELS,
    CACHE_IMAGES,
    
    CACHE_NUMGROUPS
} cacheGroup_t;
static cacheItem_t cacheGroups[CACHE_NUMGROUPS] =
{
    {{'s', 'o', 'u', 'n', 'd', 0}, CACHE_SOUNDS},
    {{'m', 'o', 'd', 'e', 'l', 0}, CACHE_MODELS},
    {{'i', 'm', 'a', 'g', 'e', 0}, CACHE_IMAGES},
};
#define MAX_CACHE_ITEMS     4096
#define CACHE_HIT_RATIO     0.75        // if hit on this percentage of maps, it'll get cached

static int cacheIndex;
static cacheItem_t cacheItems[CACHE_NUMGROUPS][MAX_CACHE_ITEMS];

static void CL_Cache_StartGather_f( void )
{
    cacheIndex = 0;
    memset( cacheItems, 0, sizeof( cacheItems ) );
    
    Cvar_Set( "cl_cacheGathering", "1" );
}

static void CL_Cache_UsedFile_f( void )
{
    char groupStr[MAX_QPATH];
    char itemStr[MAX_QPATH];
    int i, group;
    cacheItem_t* item;
    
    if( Cmd_Argc() < 2 )
    {
        Com_Error( ERR_DROP, "usedfile without enough parameters\n" );
        return;
    }
    
    strcpy( groupStr, Cmd_Argv( 1 ) );
    
    strcpy( itemStr, Cmd_Argv( 2 ) );
    for( i = 3; i < Cmd_Argc(); i++ )
    {
        strcat( itemStr, " " );
        strcat( itemStr, Cmd_Argv( i ) );
    }
    Q_strlwr( itemStr );
    
    // find the cache group
    for( i = 0; i < CACHE_NUMGROUPS; i++ )
    {
        if( !Q_strncmp( groupStr, cacheGroups[i].name, MAX_QPATH ) )
        {
            break;
        }
    }
    if( i == CACHE_NUMGROUPS )
    {
        Com_Error( ERR_DROP, "usedfile without a valid cache group\n" );
        return;
    }
    
    // see if it's already there
    group = i;
    for( i = 0, item = cacheItems[group]; i < MAX_CACHE_ITEMS; i++, item++ )
    {
        if( !item->name[0] )
        {
            // didn't find it, so add it here
            Q_strncpyz( item->name, itemStr, MAX_QPATH );
            if( cacheIndex > 9999 )    // hack, but yeh
            {
                item->hits = cacheIndex;
            }
            else
            {
                item->hits++;
            }
            item->lastSetIndex = cacheIndex;
            break;
        }
        if( item->name[0] == itemStr[0] && !Q_strncmp( item->name, itemStr, MAX_QPATH ) )
        {
            if( item->lastSetIndex != cacheIndex )
            {
                item->hits++;
                item->lastSetIndex = cacheIndex;
            }
            break;
        }
    }
}

static void CL_Cache_SetIndex_f( void )
{
    if( Cmd_Argc() < 2 )
    {
        Com_Error( ERR_DROP, "setindex needs an index\n" );
        return;
    }
    
    cacheIndex = atoi( Cmd_Argv( 1 ) );
}

static void CL_Cache_MapChange_f( void )
{
    cacheIndex++;
}

static void CL_Cache_EndGather_f( void )
{
    // save the frequently used files to the cache list file
    int i, j, handle, cachePass;
    char filename[MAX_QPATH];
    
    cachePass = ( int )floor( ( float )cacheIndex * CACHE_HIT_RATIO );
    
    for( i = 0; i < CACHE_NUMGROUPS; i++ )
    {
        Q_strncpyz( filename, cacheGroups[i].name, MAX_QPATH );
        Q_strcat( filename, MAX_QPATH, ".cache" );
        
#ifdef __MACOS__    //DAJ MacOS file typing
        {
            extern _MSL_IMP_EXP_C long _fcreator, _ftype;
            _ftype = 'WlfB';
            _fcreator = 'WlfS';
        }
#endif
        handle = FS_FOpenFileWrite( filename );
        
        for( j = 0; j < MAX_CACHE_ITEMS; j++ )
        {
            // if it's a valid filename, and it's been hit enough times, cache it
            if( cacheItems[i][j].hits >= cachePass && strstr( cacheItems[i][j].name, "/" ) )
            {
                FS_Write( cacheItems[i][j].name, strlen( cacheItems[i][j].name ), handle );
                FS_Write( "\n", 1, handle );
            }
        }
        
        FS_FCloseFile( handle );
    }
    
    Cvar_Set( "cl_cacheGathering", "0" );
}

// done.
//============================================================================

/*
================
CL_MapRestart_f
================
*/
void CL_MapRestart_f( void )
{
    if( !com_cl_running )
    {
        return;
    }
    if( !com_cl_running->integer )
    {
        return;
    }
    Com_Printf( "This command is no longer functional.\nUse \"loadgame current\" to load the current map." );
}

/*
================
CL_SetRecommended_f
================
*/
void CL_SetRecommended_f( void )
{
    if( Cmd_Argc() > 1 )
    {
        Com_SetRecommended( true );
    }
    else
    {
        Com_SetRecommended( false );
    }
}

/*
================
CL_RefPrintf

DLL glue
================
*/
void CL_RefPrintf( int print_level, const char* fmt, ... )
{
    va_list argptr;
#define MAXPRINTMSG 4096
    char msg[MAXPRINTMSG];
    
    va_start( argptr, fmt );
    vsprintf( msg, fmt, argptr );
    va_end( argptr );
    
    if( print_level == PRINT_ALL )
    {
        Com_Printf( "%s", msg );
    }
    else if( print_level == PRINT_WARNING )
    {
        Com_Printf( S_COLOR_YELLOW "%s", msg );       // yellow
    }
    else if( print_level == PRINT_DEVELOPER )
    {
        Com_DPrintf( S_COLOR_RED "%s", msg );     // red
    }
}

/*
============
CL_InitRenderer
============
*/
void CL_InitRenderer( void )
{
    // this sets up the renderer and calls R_Init
    renderSystem->Init( &cls.glconfig );
    
    // load character sets
    cls.charSetShader = renderSystem->RegisterShader( "gfx/2d/bigchars" );
    cls.whiteShader = renderSystem->RegisterShader( "white" );
    cls.consoleShader = renderSystem->RegisterShader( "console" );
    cls.consoleShader2 = renderSystem->RegisterShader( "console2" );
    g_console_field_width = cls.glconfig.vidWidth / SMALLCHAR_WIDTH - 2;
    g_consoleField.widthInChars = g_console_field_width;
}

/*
============================
CL_StartHunkUsers

After the server has cleared the hunk, these will need to be restarted
This is the only place that any of these functions are called from
============================
*/
void CL_StartHunkUsers( void )
{
    if( !com_cl_running )
    {
        return;
    }
    
    if( !com_cl_running->integer )
    {
        return;
    }
    
    if( !cls.rendererStarted )
    {
        cls.rendererStarted = true;
        CL_InitRenderer();
    }
    
    if( !cls.soundStarted )
    {
        cls.soundStarted = true;
        soundSystem->Init();
    }
    
    if( !cls.soundRegistered )
    {
        cls.soundRegistered = true;
        soundSystem->BeginRegistration();
    }
    
    if( !cls.uiStarted )
    {
        cls.uiStarted = true;
        CL_InitUI();
    }
}

// DHM - Nerve
int CL_ScaledMilliseconds( void )
{
    return Sys_Milliseconds() * com_timescale->value;
}

/*
============
CL_InitRef
============
*/
void CL_InitRef( void )
{
    Com_Printf( "----- Initializing Renderer ----\n" );
    
    // unpause so the cgame definately gets a snapshot and renders a frame
    Cvar_Set( "cl_paused", "0" );
}

/*
============
CL_ShutdownRef
============
*/
void CL_ShutdownRef( void )
{
    renderSystem->Shutdown( true );
}

// RF, trap manual client damage commands so users can't issue them manually
void CL_ClientDamageCommand( void )
{
    // do nothing
}

// NERVE - SMF
void CL_startMultiplayer_f( void )
{
#ifdef __MACOS__    //DAJ
    Sys_StartProcess( "Wolfenstein MP", true );
#elif defined( __linux__ )
    Sys_StartProcess( "./wolf.x86", true );
#else
    Sys_StartProcess( "WolfMP.exe", true );
#endif
}
// -NERVE - SMF

//----(SA) added
/*
==============
CL_ShellExecute_URL_f
Format:
  shellExecute "open" <url> <doExit>

TTimo
  show_bug.cgi?id=447
  only supporting "open" syntax for URL openings, others are not portable or need to be added on a case-by-case basis
  the shellExecute syntax as been kept to remain compatible with win32 SP demo pk3, but this thing only does open URL

==============
*/

void CL_ShellExecute_URL_f( void )
{
    bool doexit;
    
    Com_DPrintf( "CL_ShellExecute_URL_f\n" );
    
    if( Q_stricmp( Cmd_Argv( 1 ), "open" ) )
    {
        Com_DPrintf( "invalid CL_ShellExecute_URL_f syntax (shellExecute \"open\" <url> <doExit>)\n" );
        return;
    }
    
    if( Cmd_Argc() < 4 )
    {
        doexit = true;
    }
    else
    {
        doexit = ( bool )( atoi( Cmd_Argv( 3 ) ) );
    }
    
    Sys_OpenURL( Cmd_Argv( 2 ), doexit );
}
//----(SA) end
//===========================================================================================

/*
====================
CL_Init
====================
*/
void CL_Init( void )
{
    Com_Printf( "----- Client Initialization -----\n" );
    
    Con_Init();
    
    CL_ClearState();
    
    cls.state = CA_DISCONNECTED;    // no longer CA_UNINITIALIZED
    
    cls.realtime = 0;
    
    CL_InitInput();
    
    //
    // register our variables
    //
    cl_noprint = Cvar_Get( "cl_noprint", "0", 0 );
    cl_motd = Cvar_Get( "cl_motd", "1", 0 );
    cl_timeout = Cvar_Get( "cl_timeout", "200", 0 );
    cl_wavefilerecord = Cvar_Get( "cl_wavefilerecord", "0", CVAR_TEMP );
    cl_timeNudge = Cvar_Get( "cl_timeNudge", "0", CVAR_TEMP );
    cl_shownuments = Cvar_Get( "cl_shownuments", "0", CVAR_TEMP );
    cl_visibleClients = Cvar_Get( "cl_visibleClients", "0", CVAR_TEMP );
    cl_showServerCommands = Cvar_Get( "cl_showServerCommands", "0", 0 );
    cl_shownet = Cvar_Get( "cl_shownet", "0", CVAR_TEMP );
    cl_showSend = Cvar_Get( "cl_showSend", "0", CVAR_TEMP );
    cl_showTimeDelta = Cvar_Get( "cl_showTimeDelta", "0", CVAR_TEMP );
    cl_freezeDemo = Cvar_Get( "cl_freezeDemo", "0", CVAR_TEMP );
    rcon_client_password = Cvar_Get( "rconPassword", "", CVAR_TEMP );
    cl_activeAction = Cvar_Get( "activeAction", "", CVAR_TEMP );
    
    cl_timedemo = Cvar_Get( "timedemo", "0", 0 );
    cl_avidemo = Cvar_Get( "cl_avidemo", "0", 0 );
    cl_forceavidemo = Cvar_Get( "cl_forceavidemo", "0", 0 );
    
    rconAddress = Cvar_Get( "rconAddress", "", 0 );
    
    cl_yawspeed = Cvar_Get( "cl_yawspeed", "140", CVAR_ARCHIVE );
    cl_pitchspeed = Cvar_Get( "cl_pitchspeed", "140", CVAR_ARCHIVE );
    cl_anglespeedkey = Cvar_Get( "cl_anglespeedkey", "1.5", 0 );
    
    cl_maxpackets = Cvar_Get( "cl_maxpackets", "30", CVAR_ARCHIVE );
    cl_packetdup = Cvar_Get( "cl_packetdup", "1", CVAR_ARCHIVE );
    
    cl_run = Cvar_Get( "cl_run", "1", CVAR_ARCHIVE );
    cl_sensitivity = Cvar_Get( "sensitivity", "5", CVAR_ARCHIVE );
    cl_mouseAccel = Cvar_Get( "cl_mouseAccel", "0", CVAR_ARCHIVE );
    cl_freelook = Cvar_Get( "cl_freelook", "1", CVAR_ARCHIVE );
    
    cl_showMouseRate = Cvar_Get( "cl_showmouserate", "0", 0 );
    
    cl_allowDownload = Cvar_Get( "cl_allowDownload", "0", CVAR_ARCHIVE );
    cl_wwwDownload = Cvar_Get( "cl_wwwDownload", "1", CVAR_USERINFO | CVAR_ARCHIVE );
    
    // init autoswitch so the ui will have it correctly even
    // if the cgame hasn't been started
    Cvar_Get( "cg_autoswitch", "2", CVAR_ARCHIVE );
    
    // Rafael - particle switch
    Cvar_Get( "cg_wolfparticles", "1", CVAR_ARCHIVE );
    // done
    
    cl_conXOffset = Cvar_Get( "cl_conXOffset", "0", 0 );
    cl_inGameVideo = Cvar_Get( "r_inGameVideo", "1", CVAR_ARCHIVE );
    
    cl_serverStatusResendTime = Cvar_Get( "cl_serverStatusResendTime", "750", 0 );
    
    // RF
    cl_recoilPitch = Cvar_Get( "cg_recoilPitch", "0", CVAR_ROM );
    
    cl_bypassMouseInput = Cvar_Get( "cl_bypassMouseInput", "0", 0 ); //CVAR_ROM );			// NERVE - SMF
    
    m_pitch = Cvar_Get( "m_pitch", "0.022", CVAR_ARCHIVE );
    m_yaw = Cvar_Get( "m_yaw", "0.022", CVAR_ARCHIVE );
    m_forward = Cvar_Get( "m_forward", "0.25", CVAR_ARCHIVE );
    m_side = Cvar_Get( "m_side", "0.25", CVAR_ARCHIVE );
    m_filter = Cvar_Get( "m_filter", "0", CVAR_ARCHIVE );
    
    cl_motdString = Cvar_Get( "cl_motdString", "", CVAR_ROM );
    
    cl_razerhydra = Cvar_Get( "cl_razerhydra", "0", CVAR_ARCHIVE );
    
    Cvar_Get( "cl_maxPing", "800", CVAR_ARCHIVE );
    
    // NERVE - SMF
    Cvar_Get( "cg_drawCompass", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_drawNotifyText", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_quickMessageAlt", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_popupLimboMenu", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_descriptiveText", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_drawTeamOverlay", "2", CVAR_ARCHIVE );
    Cvar_Get( "cg_uselessNostalgia", "0", CVAR_ARCHIVE ); // JPW NERVE
    Cvar_Get( "cg_drawGun", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_cursorHints", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_voiceSpriteTime", "6000", CVAR_ARCHIVE );
    Cvar_Get( "cg_teamChatsOnly", "0", CVAR_ARCHIVE );
    Cvar_Get( "cg_noVoiceChats", "0", CVAR_ARCHIVE );
    Cvar_Get( "cg_noVoiceText", "0", CVAR_ARCHIVE );
    Cvar_Get( "cg_crosshairSize", "48", CVAR_ARCHIVE );
    Cvar_Get( "cg_drawCrosshair", "1", CVAR_ARCHIVE );
    Cvar_Get( "cg_zoomDefaultSniper", "20", CVAR_ARCHIVE );
    Cvar_Get( "cg_zoomstepsniper", "2", CVAR_ARCHIVE );
    
    Cvar_Get( "mp_playerType", "0", 0 );
    Cvar_Get( "mp_currentPlayerType", "0", 0 );
    Cvar_Get( "mp_weapon", "0", 0 );
    Cvar_Get( "mp_team", "0", 0 );
    Cvar_Get( "mp_currentTeam", "0", 0 );
    
    // userinfo
    Cvar_Get( "name", "Player", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "rate", "3000", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "snaps", "20", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "model", "bj2", CVAR_USERINFO | CVAR_ARCHIVE ); // temp until we have an skeletal american model
    Cvar_Get( "head", "default", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "color", "4", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "handicap", "100", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "sex", "male", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "cl_anonymous", "0", CVAR_USERINFO | CVAR_ARCHIVE );
    
    Cvar_Get( "password", "", CVAR_USERINFO );
    Cvar_Get( "cg_predictItems", "1", CVAR_USERINFO | CVAR_ARCHIVE );
    
//----(SA) added
    Cvar_Get( "cg_autoactivate", "1", CVAR_USERINFO | CVAR_ARCHIVE );
    Cvar_Get( "cg_emptyswitch", "0", CVAR_USERINFO | CVAR_ARCHIVE );
//----(SA) end

    // cgame might not be initialized before menu is used
    Cvar_Get( "cg_viewsize", "100", CVAR_ARCHIVE );
    
    Cvar_Get( "cg_autoReload", "1", CVAR_ARCHIVE | CVAR_USERINFO );
    cl_missionStats = Cvar_Get( "g_missionStats", "0", CVAR_ROM );
    cl_waitForFire = Cvar_Get( "cl_waitForFire", "0", CVAR_ROM );
    
    // NERVE - SMF - localization
    cl_language = Cvar_Get( "cl_language", "0", CVAR_ARCHIVE );
    cl_debugTranslation = Cvar_Get( "cl_debugTranslation", "0", 0 );
    // -NERVE - SMF
    
    
    //
    // register our commands
    //
    Cmd_AddCommand( "cmd", CL_ForwardToServer_f );
    Cmd_AddCommand( "configstrings", CL_Configstrings_f );
    Cmd_AddCommand( "clientinfo", CL_Clientinfo_f );
    Cmd_AddCommand( "snd_restart", CL_Snd_Restart_f );
    Cmd_AddCommand( "vid_restart", CL_Vid_Restart_f );
    Cmd_AddCommand( "ui_restart", CL_UI_Restart_f );
    Cmd_AddCommand( "disconnect", CL_Disconnect_f );
    Cmd_AddCommand( "record", CL_Record_f );
    Cmd_AddCommand( "demo", CL_PlayDemo_f );
    Cmd_AddCommand( "cinematic", CL_PlayCinematic_f );
    Cmd_AddCommand( "stoprecord", CL_StopRecord_f );
    Cmd_AddCommand( "connect", CL_Connect_f );
    Cmd_AddCommand( "reconnect", CL_Reconnect_f );
    Cmd_AddCommand( "localservers", CL_LocalServers_f );
    Cmd_AddCommand( "globalservers", CL_GlobalServers_f );
    Cmd_AddCommand( "rcon", CL_Rcon_f );
    Cmd_AddCommand( "setenv", CL_Setenv_f );
    Cmd_AddCommand( "ping", CL_Ping_f );
    Cmd_AddCommand( "serverstatus", CL_ServerStatus_f );
    Cmd_AddCommand( "showip", CL_ShowIP_f );
    Cmd_AddCommand( "fs_openedList", CL_OpenedPK3List_f );
    Cmd_AddCommand( "fs_referencedList", CL_ReferencedPK3List_f );
    
    // Ridah, startup-caching system
    Cmd_AddCommand( "cache_startgather", CL_Cache_StartGather_f );
    Cmd_AddCommand( "cache_usedfile", CL_Cache_UsedFile_f );
    Cmd_AddCommand( "cache_setindex", CL_Cache_SetIndex_f );
    Cmd_AddCommand( "cache_mapchange", CL_Cache_MapChange_f );
    Cmd_AddCommand( "cache_endgather", CL_Cache_EndGather_f );
    
    Cmd_AddCommand( "updatehunkusage", CL_UpdateLevelHunkUsage );
    Cmd_AddCommand( "updatescreen", SCR_UpdateScreen );
    
    
    // RF, add this command so clients can't bind a key to send client damage commands to the server
    Cmd_AddCommand( "cld", CL_ClientDamageCommand );
    
    Cmd_AddCommand( "startMultiplayer", CL_startMultiplayer_f );        // NERVE - SMF
    
    // TTimo
    // show_bug.cgi?id=447
    Cmd_AddCommand( "shellExecute", CL_ShellExecute_URL_f );
    //Cmd_AddCommand ( "shellExecute", CL_ShellExecute_f );	//----(SA) added (mainly for opening web pages from the menu)
    
    // RF, prevent users from issuing a map_restart manually
    Cmd_AddCommand( "map_restart", CL_MapRestart_f );
    
    Cmd_AddCommand( "setRecommended", CL_SetRecommended_f );
    
    CL_InitRef();
    
    SCR_Init();
    
    Cbuf_Execute();
    
    Cvar_Set( "cl_running", "1" );
    
    Com_Printf( "----- Client Initialization Complete -----\n" );
}


/*
===============
CL_Shutdown

===============
*/
void CL_Shutdown( void )
{
    static bool recursive = false;
    
    Com_Printf( "----- CL_Shutdown -----\n" );
    
    if( recursive )
    {
        printf( "recursive shutdown\n" );
        return;
    }
    recursive = true;
    
    CL_Disconnect( true );
    
    soundSystem->Shutdown();
    DL_Shutdown();
    CL_ShutdownRef();
    
    CL_ShutdownUI();
    
    Cmd_RemoveCommand( "cmd" );
    Cmd_RemoveCommand( "configstrings" );
    Cmd_RemoveCommand( "userinfo" );
    Cmd_RemoveCommand( "snd_restart" );
    Cmd_RemoveCommand( "vid_restart" );
    Cmd_RemoveCommand( "disconnect" );
    Cmd_RemoveCommand( "record" );
    Cmd_RemoveCommand( "demo" );
    Cmd_RemoveCommand( "cinematic" );
    Cmd_RemoveCommand( "stoprecord" );
    Cmd_RemoveCommand( "connect" );
    Cmd_RemoveCommand( "localservers" );
    Cmd_RemoveCommand( "globalservers" );
    Cmd_RemoveCommand( "rcon" );
    Cmd_RemoveCommand( "setenv" );
    Cmd_RemoveCommand( "ping" );
    Cmd_RemoveCommand( "serverstatus" );
    Cmd_RemoveCommand( "showip" );
    Cmd_RemoveCommand( "model" );
    
    // Ridah, startup-caching system
    Cmd_RemoveCommand( "cache_startgather" );
    Cmd_RemoveCommand( "cache_usedfile" );
    Cmd_RemoveCommand( "cache_setindex" );
    Cmd_RemoveCommand( "cache_mapchange" );
    Cmd_RemoveCommand( "cache_endgather" );
    
    Cmd_RemoveCommand( "updatehunkusage" );
    // done.
    
    Cvar_Set( "cl_running", "0" );
    
    recursive = false;
    
    memset( &cls, 0, sizeof( cls ) );
    
    Com_Printf( "-----------------------\n" );
}


static void CL_SetServerInfo( serverInfo_t* server, const char* info, int ping )
{
    if( server )
    {
        if( info )
        {
            server->clients = atoi( Info_ValueForKey( info, "clients" ) );
            Q_strncpyz( server->hostName, Info_ValueForKey( info, "hostname" ), MAX_NAME_LENGTH );
            Q_strncpyz( server->mapName, Info_ValueForKey( info, "mapname" ), MAX_NAME_LENGTH );
            server->maxClients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
            Q_strncpyz( server->game, Info_ValueForKey( info, "game" ), MAX_NAME_LENGTH );
            server->gameType = atoi( Info_ValueForKey( info, "gametype" ) );
            server->netType = atoi( Info_ValueForKey( info, "nettype" ) );
            server->minPing = atoi( Info_ValueForKey( info, "minping" ) );
            server->maxPing = atoi( Info_ValueForKey( info, "maxping" ) );
            server->allowAnonymous = atoi( Info_ValueForKey( info, "sv_allowAnonymous" ) );
            server->friendlyFire = atoi( Info_ValueForKey( info, "friendlyFire" ) );
            server->maxlives = atoi( Info_ValueForKey( info, "maxlives" ) );
            server->tourney = atoi( Info_ValueForKey( info, "tourney" ) );
            
            Q_strncpyz( server->gameName, Info_ValueForKey( info, "gamename" ), MAX_NAME_LENGTH );
            server->antilag = atoi( Info_ValueForKey( info, "g_antilag" ) );
        }
        server->ping = ping;
    }
}

static void CL_SetServerInfoByAddress( netadr_t from, const char* info, int ping )
{
    int i;
    
    for( i = 0; i < MAX_OTHER_SERVERS; i++ )
    {
        if( NET_CompareAdr( from, cls.localServers[i].adr ) )
        {
            CL_SetServerInfo( &cls.localServers[i], info, ping );
        }
    }
    
    for( i = 0; i < MAX_OTHER_SERVERS; i++ )
    {
        if( NET_CompareAdr( from, cls.mplayerServers[i].adr ) )
        {
            CL_SetServerInfo( &cls.mplayerServers[i], info, ping );
        }
    }
    
    for( i = 0; i < MAX_GLOBAL_SERVERS; i++ )
    {
        if( NET_CompareAdr( from, cls.globalServers[i].adr ) )
        {
            CL_SetServerInfo( &cls.globalServers[i], info, ping );
        }
    }
    
    for( i = 0; i < MAX_OTHER_SERVERS; i++ )
    {
        if( NET_CompareAdr( from, cls.favoriteServers[i].adr ) )
        {
            CL_SetServerInfo( &cls.favoriteServers[i], info, ping );
        }
    }
    
}

/*
===================
CL_ServerInfoPacket
===================
*/
void CL_ServerInfoPacket( netadr_t from, msg_t* msg )
{
    int i, type;
    char info[MAX_INFO_STRING];
    char*   str;
    char*    infoString;
    int prot;
    char*    gameName;
    
    infoString = MSG_ReadString( msg );
    
    // if this isn't the correct game, ignore it
    gameName = Info_ValueForKey( infoString, "gamename" );
    if( !gameName[0] || Q_stricmp( gameName, GAMENAME_STRING ) )
    {
        Com_DPrintf( "Different game info packet: %s\n", infoString );
        return;
    }
    
    // if this isn't the correct protocol version, ignore it
    prot = atoi( Info_ValueForKey( infoString, "protocol" ) );
    if( prot != PROTOCOL_VERSION )
    {
        Com_DPrintf( "Different protocol info packet: %s\n", infoString );
        return;
    }
    
    // iterate servers waiting for ping response
    for( i = 0; i < MAX_PINGREQUESTS; i++ )
    {
        if( cl_pinglist[i].adr.port && !cl_pinglist[i].time && NET_CompareAdr( from, cl_pinglist[i].adr ) )
        {
            // calc ping time
            cl_pinglist[i].time = cls.realtime - cl_pinglist[i].start + 1;
            Com_DPrintf( "ping time %dms from %s\n", cl_pinglist[i].time, NET_AdrToString( from ) );
            
            // save of info
            Q_strncpyz( cl_pinglist[i].info, infoString, sizeof( cl_pinglist[i].info ) );
            
            // tack on the net type
            // NOTE: make sure these types are in sync with the netnames strings in the UI
            switch( from.type )
            {
                case NA_BROADCAST:
                case NA_IP:
                    str = "udp";
                    type = 1;
                    break;
                    
                case NA_IPX:
                case NA_BROADCAST_IPX:
                    str = "ipx";
                    type = 2;
                    break;
                    
                default:
                    str = "???";
                    type = 0;
                    break;
            }
            Info_SetValueForKey( cl_pinglist[i].info, "nettype", va( "%d", type ) );
            CL_SetServerInfoByAddress( from, infoString, cl_pinglist[i].time );
            
            return;
        }
    }
    
    // if not just sent a local broadcast or pinging local servers
    if( cls.pingUpdateSource != AS_LOCAL )
    {
        return;
    }
    
    for( i = 0 ; i < MAX_OTHER_SERVERS ; i++ )
    {
        // empty slot
        if( cls.localServers[i].adr.port == 0 )
        {
            break;
        }
        
        // avoid duplicate
        if( NET_CompareAdr( from, cls.localServers[i].adr ) )
        {
            return;
        }
    }
    
    if( i == MAX_OTHER_SERVERS )
    {
        Com_DPrintf( "MAX_OTHER_SERVERS hit, dropping infoResponse\n" );
        return;
    }
    
    // add this to the list
    cls.numlocalservers = i + 1;
    cls.localServers[i].adr = from;
    cls.localServers[i].clients = 0;
    cls.localServers[i].hostName[0] = '\0';
    cls.localServers[i].mapName[0] = '\0';
    cls.localServers[i].maxClients = 0;
    cls.localServers[i].maxPing = 0;
    cls.localServers[i].minPing = 0;
    cls.localServers[i].ping = -1;
    cls.localServers[i].game[0] = '\0';
    cls.localServers[i].gameType = 0;
    cls.localServers[i].netType = from.type;
    cls.localServers[i].allowAnonymous = 0;
    cls.localServers[i].friendlyFire = 0;
    cls.localServers[i].maxlives = 0;
    cls.localServers[i].tourney = 0;
    cls.localServers[i].gameName[0] = '\0';
    
    Q_strncpyz( info, MSG_ReadString( msg ), MAX_INFO_STRING );
    if( strlen( info ) )
    {
        if( info[strlen( info ) - 1] != '\n' )
        {
            strncat( info, "\n", sizeof( info ) );
        }
        Com_Printf( "%s: %s", NET_AdrToString( from ), info );
    }
}

/*
===================
CL_GetServerStatus
===================
*/
serverStatus_t* CL_GetServerStatus( netadr_t from )
{
    serverStatus_t* serverStatus;
    int i, oldest, oldestTime;
    
    serverStatus = NULL;
    for( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ )
    {
        if( NET_CompareAdr( from, cl_serverStatusList[i].address ) )
        {
            return &cl_serverStatusList[i];
        }
    }
    for( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ )
    {
        if( cl_serverStatusList[i].retrieved )
        {
            return &cl_serverStatusList[i];
        }
    }
    oldest = -1;
    oldestTime = 0;
    for( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ )
    {
        if( oldest == -1 || cl_serverStatusList[i].startTime < oldestTime )
        {
            oldest = i;
            oldestTime = cl_serverStatusList[i].startTime;
        }
    }
    if( oldest != -1 )
    {
        return &cl_serverStatusList[oldest];
    }
    serverStatusCount++;
    return &cl_serverStatusList[serverStatusCount & ( MAX_SERVERSTATUSREQUESTS - 1 )];
}

/*
===================
CL_ServerStatus
===================
*/
int CL_ServerStatus( char* serverAddress, char* serverStatusString, int maxLen )
{
    int i;
    netadr_t to;
    serverStatus_t* serverStatus;
    
    // if no server address then reset all server status requests
    if( !serverAddress )
    {
        for( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ )
        {
            cl_serverStatusList[i].address.port = 0;
            cl_serverStatusList[i].retrieved = true;
        }
        return false;
    }
    // get the address
    if( !NET_StringToAdr( serverAddress, &to ) )
    {
        return false;
    }
    serverStatus = CL_GetServerStatus( to );
    // if no server status string then reset the server status request for this address
    if( !serverStatusString )
    {
        serverStatus->retrieved = true;
        return false;
    }
    
    // if this server status request has the same address
    if( NET_CompareAdr( to, serverStatus->address ) )
    {
        // if we recieved an response for this server status request
        if( !serverStatus->pending )
        {
            Q_strncpyz( serverStatusString, serverStatus->string, maxLen );
            serverStatus->retrieved = true;
            serverStatus->startTime = 0;
            return true;
        }
        // resend the request regularly
        else if( serverStatus->startTime < Sys_Milliseconds() - cl_serverStatusResendTime->integer )
        {
            serverStatus->print = false;
            serverStatus->pending = true;
            serverStatus->retrieved = false;
            serverStatus->time = 0;
            serverStatus->startTime = Sys_Milliseconds();
            NET_OutOfBandPrint( NS_CLIENT, to, "getstatus" );
            return false;
        }
    }
    // if retrieved
    else if( serverStatus->retrieved )
    {
        serverStatus->address = to;
        serverStatus->print = false;
        serverStatus->pending = true;
        serverStatus->retrieved = false;
        serverStatus->startTime = Sys_Milliseconds();
        serverStatus->time = 0;
        NET_OutOfBandPrint( NS_CLIENT, to, "getstatus" );
        return false;
    }
    return false;
}

/*
===================
CL_ServerStatusResponse
===================
*/
void CL_ServerStatusResponse( netadr_t from, msg_t* msg )
{
    char*    s;
    char info[MAX_INFO_STRING];
    int i, l, score, ping;
    int len;
    serverStatus_t* serverStatus;
    
    serverStatus = NULL;
    for( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ )
    {
        if( NET_CompareAdr( from, cl_serverStatusList[i].address ) )
        {
            serverStatus = &cl_serverStatusList[i];
            break;
        }
    }
    // if we didn't request this server status
    if( !serverStatus )
    {
        return;
    }
    
    s = MSG_ReadStringLine( msg );
    
    len = 0;
    Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "%s", s );
    
    if( serverStatus->print )
    {
        Com_Printf( "Server settings:\n" );
        // print cvars
        while( *s )
        {
            for( i = 0; i < 2 && *s; i++ )
            {
                if( *s == '\\' )
                {
                    s++;
                }
                l = 0;
                while( *s )
                {
                    info[l++] = *s;
                    if( l >= MAX_INFO_STRING - 1 )
                    {
                        break;
                    }
                    s++;
                    if( *s == '\\' )
                    {
                        break;
                    }
                }
                info[l] = '\0';
                if( i )
                {
                    Com_Printf( "%s\n", info );
                }
                else
                {
                    Com_Printf( "%-24s", info );
                }
            }
        }
    }
    
    len = strlen( serverStatus->string );
    Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\" );
    
    if( serverStatus->print )
    {
        Com_Printf( "\nPlayers:\n" );
        Com_Printf( "num: score: ping: name:\n" );
    }
    for( i = 0, s = MSG_ReadStringLine( msg ); *s; s = MSG_ReadStringLine( msg ), i++ )
    {
    
        len = strlen( serverStatus->string );
        Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\%s", s );
        
        if( serverStatus->print )
        {
            score = ping = 0;
            sscanf( s, "%d %d", &score, &ping );
            s = strchr( s, ' ' );
            if( s )
            {
                s = strchr( s + 1, ' ' );
            }
            if( s )
            {
                s++;
            }
            else
            {
                s = "unknown";
            }
            Com_Printf( "%-2d   %-3d    %-3d   %s\n", i, score, ping, s );
        }
    }
    len = strlen( serverStatus->string );
    Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\" );
    
    serverStatus->time = Sys_Milliseconds();
    serverStatus->address = from;
    serverStatus->pending = false;
    if( serverStatus->print )
    {
        serverStatus->retrieved = true;
    }
}

/*
==================
CL_LocalServers_f
==================
*/
void CL_LocalServers_f( void )
{
    char*        message;
    int i, j;
    netadr_t to;
    
    Com_Printf( "Scanning for servers on the local network...\n" );
    
    // reset the list, waiting for response
    cls.numlocalservers = 0;
    cls.pingUpdateSource = AS_LOCAL;
    
    for( i = 0; i < MAX_OTHER_SERVERS; i++ )
    {
        bool b = cls.localServers[i].visible;
        Com_Memset( &cls.localServers[i], 0, sizeof( cls.localServers[i] ) );
        cls.localServers[i].visible = b;
    }
    Com_Memset( &to, 0, sizeof( to ) );
    
    // The 'xxx' in the message is a challenge that will be echoed back
    // by the server.  We don't care about that here, but master servers
    // can use that to prevent spoofed server responses from invalid ip
    message = "\377\377\377\377getinfo xxx";
    
    // send each message twice in case one is dropped
    for( i = 0 ; i < 2 ; i++ )
    {
        // send a broadcast packet on each server port
        // we support multiple server ports so a single machine
        // can nicely run multiple servers
        for( j = 0 ; j < NUM_SERVER_PORTS ; j++ )
        {
            to.port = BigShort( ( short )( PORT_SERVER + j ) );
            
            to.type = NA_BROADCAST;
            NET_SendPacket( NS_CLIENT, strlen( message ), message, to );
            
            to.type = NA_BROADCAST_IPX;
            NET_SendPacket( NS_CLIENT, strlen( message ), message, to );
        }
    }
}

/*
==================
CL_GlobalServers_f
==================
*/
void CL_GlobalServers_f( void )
{
    netadr_t to;
    int i;
    int count;
    char*        buffptr;
    char command[1024];
    
    if( Cmd_Argc() < 3 )
    {
        Com_Printf( "usage: globalservers <master# 0-1> <protocol> [keywords]\n" );
        return;
    }
    
    cls.masterNum = atoi( Cmd_Argv( 1 ) );
    
    Com_Printf( "Requesting servers from the master...\n" );
    
    // reset the list, waiting for response
    // -1 is used to distinguish a "no response"
    
    if( cls.masterNum == 1 )
    {
        NET_StringToAdr( "master.quake3world.com", &to );
        cls.nummplayerservers = -1;
        cls.pingUpdateSource = AS_MPLAYER;
    }
    else
    {
        NET_StringToAdr( MASTER_SERVER_NAME, &to );
        cls.numglobalservers = -1;
        cls.pingUpdateSource = AS_GLOBAL;
    }
    to.type = NA_IP;
    to.port = BigShort( PORT_MASTER );
    
    sprintf( command, "getservers %s", Cmd_Argv( 2 ) );
    
    // tack on keywords
    buffptr = command + strlen( command );
    count   = Cmd_Argc();
    for( i = 3; i < count; i++ )
        buffptr += sprintf( buffptr, " %s", Cmd_Argv( i ) );
        
    // if we are a demo, automatically add a "demo" keyword
    if( Cvar_VariableValue( "fs_restrict" ) )
    {
        buffptr += sprintf( buffptr, " demo" );
    }
    
    NET_OutOfBandPrint( NS_SERVER, to, command );
}


/*
==================
CL_GetPing
==================
*/
void CL_GetPing( int n, char* buf, int buflen, int* pingtime )
{
    const char*  str;
    int time;
    int maxPing;
    
    if( !cl_pinglist[n].adr.port )
    {
        // empty slot
        buf[0]    = '\0';
        *pingtime = 0;
        return;
    }
    
    str = NET_AdrToString( cl_pinglist[n].adr );
    Q_strncpyz( buf, str, buflen );
    
    time = cl_pinglist[n].time;
    if( !time )
    {
        // check for timeout
        time = cls.realtime - cl_pinglist[n].start;
        maxPing = Cvar_VariableIntegerValue( "cl_maxPing" );
        if( maxPing < 100 )
        {
            maxPing = 100;
        }
        if( time < maxPing )
        {
            // not timed out yet
            time = 0;
        }
    }
    
    CL_SetServerInfoByAddress( cl_pinglist[n].adr, cl_pinglist[n].info, cl_pinglist[n].time );
    
    *pingtime = time;
}

/*
==================
CL_UpdateServerInfo
==================
*/
void CL_UpdateServerInfo( int n )
{
    if( !cl_pinglist[n].adr.port )
    {
        return;
    }
    
    CL_SetServerInfoByAddress( cl_pinglist[n].adr, cl_pinglist[n].info, cl_pinglist[n].time );
}

/*
==================
CL_GetPingInfo
==================
*/
void CL_GetPingInfo( int n, char* buf, int buflen )
{
    if( !cl_pinglist[n].adr.port )
    {
        // empty slot
        if( buflen )
        {
            buf[0] = '\0';
        }
        return;
    }
    
    Q_strncpyz( buf, cl_pinglist[n].info, buflen );
}

/*
==================
CL_ClearPing
==================
*/
void CL_ClearPing( int n )
{
    if( n < 0 || n >= MAX_PINGREQUESTS )
    {
        return;
    }
    
    cl_pinglist[n].adr.port = 0;
}

/*
==================
CL_GetPingQueueCount
==================
*/
int CL_GetPingQueueCount( void )
{
    int i;
    int count;
    ping_t* pingptr;
    
    count   = 0;
    pingptr = cl_pinglist;
    
    for( i = 0; i < MAX_PINGREQUESTS; i++, pingptr++ )
    {
        if( pingptr->adr.port )
        {
            count++;
        }
    }
    
    return ( count );
}

/*
==================
CL_GetFreePing
==================
*/
ping_t* CL_GetFreePing( void )
{
    ping_t* pingptr;
    ping_t* best;
    int oldest;
    int i;
    int time;
    
    pingptr = cl_pinglist;
    for( i = 0; i < MAX_PINGREQUESTS; i++, pingptr++ )
    {
        // find free ping slot
        if( pingptr->adr.port )
        {
            if( !pingptr->time )
            {
                if( cls.realtime - pingptr->start < 500 )
                {
                    // still waiting for response
                    continue;
                }
            }
            else if( pingptr->time < 500 )
            {
                // results have not been queried
                continue;
            }
        }
        
        // clear it
        pingptr->adr.port = 0;
        return ( pingptr );
    }
    
    // use oldest entry
    pingptr = cl_pinglist;
    best    = cl_pinglist;
    oldest  = INT_MIN;
    for( i = 0; i < MAX_PINGREQUESTS; i++, pingptr++ )
    {
        // scan for oldest
        time = cls.realtime - pingptr->start;
        if( time > oldest )
        {
            oldest = time;
            best   = pingptr;
        }
    }
    
    return ( best );
}

/*
==================
CL_Ping_f
==================
*/
void CL_Ping_f( void )
{
    netadr_t to;
    ping_t*     pingptr;
    char*       server;
    
    if( Cmd_Argc() != 2 )
    {
        Com_Printf( "usage: ping [server]\n" );
        return;
    }
    
    memset( &to, 0, sizeof( netadr_t ) );
    
    server = Cmd_Argv( 1 );
    
    if( !NET_StringToAdr( server, &to ) )
    {
        return;
    }
    
    pingptr = CL_GetFreePing();
    
    memcpy( &pingptr->adr, &to, sizeof( netadr_t ) );
    pingptr->start = cls.realtime;
    pingptr->time  = 0;
    
    CL_SetServerInfoByAddress( pingptr->adr, NULL, 0 );
    
    NET_OutOfBandPrint( NS_CLIENT, to, "getinfo xxx" );
}

/*
==================
CL_UpdateVisiblePings_f
==================
*/
bool CL_UpdateVisiblePings_f( int source )
{
    int slots, i;
    char buff[MAX_STRING_CHARS];
    int pingTime;
    int max;
    bool status = false;
    
    if( source < 0 || source > AS_FAVORITES )
    {
        return false;
    }
    
    cls.pingUpdateSource = source;
    
    slots = CL_GetPingQueueCount();
    if( slots < MAX_PINGREQUESTS )
    {
        serverInfo_t* server = NULL;
        
        max = ( source == AS_GLOBAL ) ? MAX_GLOBAL_SERVERS : MAX_OTHER_SERVERS;
        switch( source )
        {
            case AS_LOCAL:
                server = &cls.localServers[0];
                max = cls.numlocalservers;
                break;
            case AS_MPLAYER:
                server = &cls.mplayerServers[0];
                max = cls.nummplayerservers;
                break;
            case AS_GLOBAL:
                server = &cls.globalServers[0];
                max = cls.numglobalservers;
                break;
            case AS_FAVORITES:
                server = &cls.favoriteServers[0];
                max = cls.numfavoriteservers;
                break;
        }
        for( i = 0; i < max; i++ )
        {
            if( server[i].visible )
            {
                if( server[i].ping == -1 )
                {
                    int j;
                    
                    if( slots >= MAX_PINGREQUESTS )
                    {
                        break;
                    }
                    for( j = 0; j < MAX_PINGREQUESTS; j++ )
                    {
                        if( !cl_pinglist[j].adr.port )
                        {
                            continue;
                        }
                        if( NET_CompareAdr( cl_pinglist[j].adr, server[i].adr ) )
                        {
                            // already on the list
                            break;
                        }
                    }
                    if( j >= MAX_PINGREQUESTS )
                    {
                        status = true;
                        for( j = 0; j < MAX_PINGREQUESTS; j++ )
                        {
                            if( !cl_pinglist[j].adr.port )
                            {
                                break;
                            }
                        }
                        memcpy( &cl_pinglist[j].adr, &server[i].adr, sizeof( netadr_t ) );
                        cl_pinglist[j].start = cls.realtime;
                        cl_pinglist[j].time = 0;
                        NET_OutOfBandPrint( NS_CLIENT, cl_pinglist[j].adr, "getinfo xxx" );
                        slots++;
                    }
                }
                // if the server has a ping higher than cl_maxPing or
                // the ping packet got lost
                else if( server[i].ping == 0 )
                {
                    // if we are updating global servers
                    if( source == AS_GLOBAL )
                    {
                        //
                        if( cls.numGlobalServerAddresses > 0 )
                        {
                            // overwrite this server with one from the additional global servers
                            cls.numGlobalServerAddresses--;
                            CL_InitServerInfo( &server[i], &cls.globalServerAddresses[cls.numGlobalServerAddresses] );
                            // NOTE: the server[i].visible flag stays untouched
                        }
                    }
                }
            }
        }
    }
    
    if( slots )
    {
        status = true;
    }
    for( i = 0; i < MAX_PINGREQUESTS; i++ )
    {
        if( !cl_pinglist[i].adr.port )
        {
            continue;
        }
        CL_GetPing( i, buff, MAX_STRING_CHARS, &pingTime );
        if( pingTime != 0 )
        {
            CL_ClearPing( i );
            status = true;
        }
    }
    
    return status;
}

/*
==================
CL_ServerStatus_f
==================
*/
void CL_ServerStatus_f( void )
{
    netadr_t to;
    char*        server;
    serverStatus_t* serverStatus;
    
    Com_Memset( &to, 0, sizeof( netadr_t ) );
    
    if( Cmd_Argc() != 2 )
    {
        if( cls.state != CA_ACTIVE || clc.demoplaying )
        {
            Com_Printf( "Not connected to a server.\n" );
            Com_Printf( "Usage: serverstatus [server]\n" );
            return;
        }
        server = cls.servername;
    }
    else
    {
        server = Cmd_Argv( 1 );
    }
    
    if( !NET_StringToAdr( server, &to ) )
    {
        return;
    }
    
    NET_OutOfBandPrint( NS_CLIENT, to, "getstatus" );
    
    serverStatus = CL_GetServerStatus( to );
    serverStatus->address = to;
    serverStatus->print = true;
    serverStatus->pending = true;
}

/*
==================
CL_ShowIP_f
==================
*/
void CL_ShowIP_f( void )
{
    Sys_ShowIP();
}

/*
=================
bool CL_CDKeyValidate
=================
*/
bool CL_CDKeyValidate( const char* key, const char* checksum )
{
    char ch;
    byte sum;
    char chs[3];
    int i, len;
    
    len = strlen( key );
    if( len != CDKEY_LEN )
    {
        return false;
    }
    
    if( checksum && strlen( checksum ) != CDCHKSUM_LEN )
    {
        return false;
    }
    
    sum = 0;
    // for loop gets rid of conditional assignment warning
    for( i = 0; i < len; i++ )
    {
        ch = *key++;
        if( ch >= 'a' && ch <= 'z' )
        {
            ch -= 32;
        }
        switch( ch )
        {
            case '2':
            case '3':
            case '7':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'G':
            case 'H':
            case 'J':
            case 'L':
            case 'P':
            case 'R':
            case 'S':
            case 'T':
            case 'W':
                sum = ( sum << 1 ) ^ ch;
                continue;
            default:
                return false;
        }
    }
    
    
    sprintf( chs, "%02x", sum );
    
    if( checksum && !Q_stricmp( chs, checksum ) )
    {
        return true;
    }
    
    if( !checksum )
    {
        return true;
    }
    
    return false;
}

// NERVE - SMF
/*
=======================
CL_AddToLimboChat

=======================
*/
void CL_AddToLimboChat( const char* str )
{
    int len;
    char* p, *ls;
    int lastcolor;
    int chatHeight;
    int i;
    
    chatHeight = LIMBOCHAT_HEIGHT;
    cl.limboChatPos = LIMBOCHAT_HEIGHT - 1;
    len = 0;
    
    // copy old strings
    for( i = cl.limboChatPos; i > 0; i-- )
    {
        strcpy( cl.limboChatMsgs[i], cl.limboChatMsgs[i - 1] );
    }
    
    // copy new string
    p = cl.limboChatMsgs[0];
    *p = 0;
    
    lastcolor = '7';
    
    ls = NULL;
    while( *str )
    {
        if( len > LIMBOCHAT_WIDTH - 1 )
        {
            break;
        }
        
        if( Q_IsColorString( str ) )
        {
            *p++ = *str++;
            lastcolor = *str;
            *p++ = *str++;
            continue;
        }
        if( *str == ' ' )
        {
            ls = p;
        }
        *p++ = *str++;
        len++;
    }
    *p = 0;
}

/*
=======================
CL_GetLimboString

=======================
*/
bool CL_GetLimboString( int index, char* buf )
{
    if( index >= LIMBOCHAT_HEIGHT )
    {
        return false;
    }
    
    strncpy( buf, cl.limboChatMsgs[index], 140 );
    return true;
}
