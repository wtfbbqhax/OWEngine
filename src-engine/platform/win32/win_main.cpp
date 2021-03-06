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
//  File name:   win_main.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../../client/client.h"
#include "../../qcommon/qcommon.h"
#include "win_local.h"
#include "resource.h"
#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <conio.h>

//#define	CD_BASEDIR	"wolf"
#define CD_BASEDIR  ""
//#define	CD_EXE		"wolf.exe"
#define CD_EXE      "setup\\setup.exe"
//#define	CD_BASEDIR_LINUX	"bin\\x86\\glibc-2.1"
#define CD_BASEDIR_LINUX    "bin\\x86\\glibc-2.1"
//#define	CD_EXE_LINUX "wolf"
#define CD_EXE_LINUX "setup\\setup"

#define MEM_THRESHOLD 96 * 1024 * 1024

static char sys_cmdline[MAX_STRING_CHARS];

#include <motioncontrollers.h>
struct OculusVR_HMDInfo HMD;
int OculusVRDetected;

char* WinGetLastError()
{
    static char buf[4096];
    
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                   buf,
                   4096, NULL );
                   
    return buf;
}

/*
==================
Sys_LowPhysicalMemory()
==================
*/
bool Sys_LowPhysicalMemory()
{
    MEMORYSTATUS stat;
    GlobalMemoryStatus( &stat );
    return ( stat.dwTotalPhys <= MEM_THRESHOLD ) ? true : false;
}

//NOTE TTimo: heavily NON PORTABLE, PLZ DON'T USE
//  show_bug.cgi?id=447
#if 0
//----(SA) added
/*
==============
Sys_ShellExecute

-	Windows only

	Performs an operation on a specified file.

	See info on ShellExecute() for details

==============
*/
int Sys_ShellExecute( char* op, char* file, bool doexit, char* params, char* dir )
{
    unsigned int retval;
    char* se_op;
    
    // set default operation to "open"
    if( op )
    {
        se_op = op;
    }
    else
    {
        se_op = "open";
    }
    
    
    // probably need to protect this some in the future so people have
    // less chance of system invasion with this powerful interface
    // (okay, not so invasive, but could be annoying/rude)
    
    
    retval = ( UINT )ShellExecute( NULL, se_op, file, params, dir, SW_NORMAL ); // only option forced by game is 'sw_normal'
    
    if( retval <= 32 )    // ERROR
    {
        Com_DPrintf( "Sys_ShellExecuteERROR: %d\n", retval );
        return retval;
    }
    
    if( doexit )
    {
        // (SA) this works better for exiting cleanly...
        Cbuf_ExecuteText( EXEC_APPEND, "quit" );
    }
    
    return 999; // success
}
//----(SA) end
#endif

//----(SA)	from NERVE MP codebase (10/15/01)  (checkins at time of this file should be related)
/*
==================
Sys_StartProcess
==================
*/
void Sys_StartProcess( const char* exeName, bool doexit )             // NERVE - SMF
{
    TCHAR szPathOrig[_MAX_PATH];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si );
    
    GetCurrentDirectory( _MAX_PATH, szPathOrig );
    if( !CreateProcess( NULL, va( "%s\\%s", szPathOrig, exeName ), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
    {
        // couldn't start it, popup error box
        Com_Error( ERR_DROP, "Could not start process: '%s\\%s' ", szPathOrig, exeName );
        return;
    }
    
    // TTimo: similar way of exiting as used in Sys_OpenURL below
    if( doexit )
    {
        Cbuf_ExecuteText( EXEC_APPEND, "quit" );
    }
}

/*
==================
Sys_OpenURL
==================
*/
void Sys_OpenURL( const char* url, bool doexit )                  // NERVE - SMF
{
    HWND wnd;
    
    if( !ShellExecute( NULL, "open", url, NULL, NULL, SW_RESTORE ) )
    {
        // couldn't start it, popup error box
        Com_Error( ERR_DROP, "Could not open url: '%s' ", url );
        return;
    }
    
    wnd = GetForegroundWindow();
    
    if( wnd )
    {
        ShowWindow( wnd, SW_MAXIMIZE );
    }
    
    if( doexit )
    {
        Cbuf_ExecuteText( EXEC_APPEND, "quit\n" );
    }
}
//----(SA)	end

/*
==================
Sys_BeginProfiling
==================
*/
void Sys_BeginProfiling( void )
{
    // this is just used on the mac build
}

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void Sys_Error( const char* error, ... )
{
    va_list argptr;
    char text[4096];
    MSG msg;
    
    va_start( argptr, error );
    vsprintf( text, error, argptr );
    va_end( argptr );
    
    Conbuf_AppendText( text );
    Conbuf_AppendText( "\n" );
    
    Sys_SetErrorText( text );
    Sys_ShowConsole( 1, true );
    
    timeEndPeriod( 1 );
    
    IN_Shutdown();
    
    // wait for the user to quit
    while( 1 )
    {
        if( !GetMessage( &msg, NULL, 0, 0 ) )
        {
            Com_Quit_f();
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    
    Sys_DestroyConsole();
    
    exit( 1 );
}

/*
==============
Sys_Print
==============
*/
void Sys_Print( const char* msg )
{
    Conbuf_AppendText( msg );
    OutputDebugString( msg );
}


/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char* path )
{
    _mkdir( path );
}

/*
==============
Sys_Cwd
==============
*/
char* Sys_Cwd( void )
{
    static char cwd[MAX_OSPATH];
    
    _getcwd( cwd, sizeof( cwd ) - 1 );
    cwd[MAX_OSPATH - 1] = 0;
    
    return cwd;
}

/*
==============
Sys_DefaultCDPath
==============
*/
char* Sys_DefaultCDPath( void )
{
    return "";
}

/*
==============
Sys_DefaultBasePath
==============
*/
char* Sys_DefaultBasePath( void )
{
    return Sys_Cwd();
}

/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define MAX_FOUND_FILES 0x1000

void Sys_ListFilteredFiles( const char* basedir, char* subdirs, char* filter, char** list, int* numfiles )
{
    char search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
    char filename[MAX_OSPATH];
    int findhandle;
    struct _finddata_t findinfo;
    
    if( *numfiles >= MAX_FOUND_FILES - 1 )
    {
        return;
    }
    
    if( strlen( subdirs ) )
    {
        Com_sprintf( search, sizeof( search ), "%s\\%s\\*", basedir, subdirs );
    }
    else
    {
        Com_sprintf( search, sizeof( search ), "%s\\*", basedir );
    }
    
    findhandle = _findfirst( search, &findinfo );
    if( findhandle == -1 )
    {
        return;
    }
    
    do
    {
        if( findinfo.attrib & _A_SUBDIR )
        {
            if( Q_stricmp( findinfo.name, "." ) && Q_stricmp( findinfo.name, ".." ) )
            {
                if( strlen( subdirs ) )
                {
                    Com_sprintf( newsubdirs, sizeof( newsubdirs ), "%s\\%s", subdirs, findinfo.name );
                }
                else
                {
                    Com_sprintf( newsubdirs, sizeof( newsubdirs ), "%s", findinfo.name );
                }
                Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
            }
        }
        if( *numfiles >= MAX_FOUND_FILES - 1 )
        {
            break;
        }
        Com_sprintf( filename, sizeof( filename ), "%s\\%s", subdirs, findinfo.name );
        if( !Com_FilterPath( filter, filename, false ) )
        {
            continue;
        }
        list[ *numfiles ] = CopyString( filename );
        ( *numfiles )++;
    }
    while( _findnext( findhandle, &findinfo ) != -1 );
    
    _findclose( findhandle );
}

static bool strgtr( const char* s0, const char* s1 )
{
    int l0, l1, i;
    
    l0 = strlen( s0 );
    l1 = strlen( s1 );
    
    if( l1 < l0 )
    {
        l0 = l1;
    }
    
    for( i = 0; i < l0; i++ )
    {
        if( s1[i] > s0[i] )
        {
            return true;
        }
        if( s1[i] < s0[i] )
        {
            return false;
        }
    }
    return false;
}

char** Sys_ListFiles( const char* directory, const char* extension, char* filter, int* numfiles, bool wantsubs )
{
    char search[MAX_OSPATH];
    int nfiles;
    char**        listCopy;
    char*        list[MAX_FOUND_FILES];
    struct _finddata_t findinfo;
    int findhandle;
    int flag;
    int i;
    
    if( filter )
    {
    
        nfiles = 0;
        Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );
        
        list[ nfiles ] = 0;
        *numfiles = nfiles;
        
        if( !nfiles )
        {
            return NULL;
        }
        
        listCopy = ( char** )Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
        for( i = 0 ; i < nfiles ; i++ )
        {
            listCopy[i] = list[i];
        }
        listCopy[i] = NULL;
        
        return listCopy;
    }
    
    if( !extension )
    {
        extension = "";
    }
    
    // passing a slash as extension will find directories
    if( extension[0] == '/' && extension[1] == 0 )
    {
        extension = "";
        flag = 0;
    }
    else
    {
        flag = _A_SUBDIR;
    }
    
    Com_sprintf( search, sizeof( search ), "%s\\*%s", directory, extension );
    
    // search
    nfiles = 0;
    
    findhandle = _findfirst( search, &findinfo );
    if( findhandle == -1 )
    {
        *numfiles = 0;
        return NULL;
    }
    
    do
    {
        if( ( !wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR ) ) || ( wantsubs && findinfo.attrib & _A_SUBDIR ) )
        {
            if( nfiles == MAX_FOUND_FILES - 1 )
            {
                break;
            }
            list[ nfiles ] = CopyString( findinfo.name );
            nfiles++;
        }
    }
    while( _findnext( findhandle, &findinfo ) != -1 );
    
    list[ nfiles ] = 0;
    
    _findclose( findhandle );
    
    // return a copy of the list
    *numfiles = nfiles;
    
    if( !nfiles )
    {
        return NULL;
    }
    
    listCopy = ( char** )Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
    for( i = 0 ; i < nfiles ; i++ )
    {
        listCopy[i] = list[i];
    }
    listCopy[i] = NULL;
    
    do
    {
        flag = 0;
        for( i = 1; i < nfiles; i++ )
        {
            if( strgtr( listCopy[i - 1], listCopy[i] ) )
            {
                char* temp = listCopy[i];
                listCopy[i] = listCopy[i - 1];
                listCopy[i - 1] = temp;
                flag = 1;
            }
        }
    }
    while( flag );
    
    return listCopy;
}

void    Sys_FreeFileList( char** list )
{
    int i;
    
    if( !list )
    {
        return;
    }
    
    for( i = 0 ; list[i] ; i++ )
    {
        Z_Free( list[i] );
    }
    
    Z_Free( list );
}

//========================================================


/*
================
Sys_ScanForCD

Search all the drives to see if there is a valid CD to grab
the cddir from
================
*/
bool Sys_ScanForCD( void )
{
    static char cddir[MAX_OSPATH];
    char drive[4];
    FILE*        f;
    char test[MAX_OSPATH];
#if 0
    // don't override a cdpath on the command line
    if( strstr( sys_cmdline, "cdpath" ) )
    {
        return;
    }
#endif
    
    drive[0] = 'c';
    drive[1] = ':';
    drive[2] = '\\';
    drive[3] = 0;
    
    // scan the drives
    for( drive[0] = 'c' ; drive[0] <= 'z' ; drive[0]++ )
    {
        if( GetDriveType( drive ) != DRIVE_CDROM )
        {
            continue;
        }
        
        sprintf( cddir, "%s%s", drive, CD_BASEDIR );
        sprintf( test, "%s\\%s", cddir, CD_EXE );
        f = fopen( test, "r" );
        if( f )
        {
            fclose( f );
            return true;
            
        }
        else
        {
            sprintf( cddir, "%s%s", drive, CD_BASEDIR_LINUX );
            sprintf( test, "%s\\%s", cddir, CD_EXE_LINUX );
            f = fopen( test, "r" );
            if( f )
            {
                fclose( f );
                return true;
            }
        }
    }
    
    return false;
}

/*
================
Sys_CheckCD

Return true if the proper CD is in the drive
================
*/
bool    Sys_CheckCD( void )
{
    // FIXME: mission pack
    return true;
//	return Sys_ScanForCD();
}


/*
================
Sys_GetClipboardData

================
*/
char* Sys_GetClipboardData( void )
{
    char* data = NULL;
    char* cliptext;
    
    if( OpenClipboard( NULL ) != 0 )
    {
        HANDLE hClipboardData;
        
        if( ( hClipboardData = GetClipboardData( CF_TEXT ) ) != 0 )
        {
            if( ( cliptext = ( char* )GlobalLock( hClipboardData ) ) != 0 )
            {
                data = ( char* )Z_Malloc( GlobalSize( hClipboardData ) + 1 );
                Q_strncpyz( data, cliptext, GlobalSize( hClipboardData ) );
                GlobalUnlock( hClipboardData );
                
                strtok( data, "\n\r\b" );
            }
        }
        CloseClipboard();
    }
    return data;
}


/*
========================================================================

LOAD/UNLOAD DLL

========================================================================
*/

/*
=================
Sys_UnloadDll

=================
*/
void Sys_UnloadDll( void* dllHandle )
{
    if( !dllHandle )
    {
        return;
    }
    if( !FreeLibrary( ( HMODULE )dllHandle ) )
    {
        Com_Error( ERR_FATAL, "Sys_UnloadDll FreeLibrary failed" );
    }
}


char* Sys_GetDLLName( const char* name )
{
    return va( "%sx86.dll", name );
}

/*
=================
Sys_LoadDll

Used to load a development dll instead of a virtual machine
=================
*/
extern char*     FS_BuildOSPath( const char* base, const char* game, const char* qpath );

void* Sys_LoadDll( const char* name )
{
    static int lastWarning = 0;
    HINSTANCE libHandle;
    char*    basepath;
    char*    cdpath;
    char*    gamedir;
    char*    fn;
    char filename[MAX_QPATH];
    
    Q_strncpyz( filename, Sys_GetDLLName( name ), sizeof( filename ) );
    
    // check current folder only if we are a developer
    if( 1 )  //----(SA)	always dll
    {
        //	if (Cvar_VariableIntegerValue( "devdll" )) {
        libHandle = LoadLibrary( filename );
        if( libHandle )
        {
            return libHandle;
        }
    }
    
    basepath = Cvar_VariableString( "fs_basepath" );
    cdpath = Cvar_VariableString( "fs_cdpath" );
    gamedir = Cvar_VariableString( "fs_game" );
    
    fn = FS_BuildOSPath( basepath, gamedir, filename );
    
    // this is only relevant for full client
    // if a full client runs a dedicated server, it's not affected by this
#if !defined( DEDICATED )
    // extract dlls from pak file for security
    // we have to handle the game dll a little differently
    // passing the exact path to check against
    //   (compatibility with other OSes loading procedure)
    if( cl_connectedToPureServer && Q_strncmp( name, "qagame", 6 ) )
    {
        if( !FS_CL_ExtractFromPakFile( fn, gamedir, filename, NULL ) )
        {
            Com_Error( ERR_DROP, "Game code(%s) failed Pure Server check", filename );
        }
    }
#endif
    
    libHandle = LoadLibrary( fn );
    
    if( !libHandle )
    {
        if( cdpath[0] )
        {
            fn = FS_BuildOSPath( basepath, gamedir, filename );
            libHandle = LoadLibrary( fn );
        }
        
        if( !libHandle )
        {
            return NULL;
        }
    }
    
    return libHandle;
}

void* Sys_GetProcAddress( void* dllhandle, const char* name )
{
    return GetProcAddress( ( HMODULE )dllhandle, name );
}

void* Sys_LoadDLLSimple( const char* name )
{
    return ( void* )LoadLibrary( name );
}

void* Sys_LoadFunction( void* dllHandle, const char* functionName )
{
    return ( void* )GetProcAddress( ( HMODULE )dllHandle, functionName );
}

char* Sys_DLLError()
{
    static char buf[4096];
    
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                   buf,
                   4096, NULL );
                   
    return buf;
}



/*
========================================================================

BACKGROUND FILE STREAMING

========================================================================
*/

typedef struct
{
    fileHandle_t file;
    byte*    buffer;
    bool eof;
    bool active;
    int bufferSize;
    int streamPosition;     // next byte to be returned by Sys_StreamRead
    int threadPosition;     // next byte to be read from file
} streamsIO_t;

typedef struct
{
    HANDLE threadHandle;
    int threadId;
    HANDLE musicThreadHandle;
    int musicThreadId;
    CRITICAL_SECTION crit;
    streamsIO_t sIO[64];
} streamState_t;

static streamState_t stream;

int FS_Read2( void* buffer, int len, fileHandle_t f );

void Sys_MusicThread( void )
{
    while( 1 )
    {
        Sleep( 33 );
        soundSystem->UpdateThread();
    }
}

#if 0

void Sys_InitStreamThread( void )
{
}

void Sys_ShutdownStreamThread( void )
{
}

void Sys_BeginStreamedFile( fileHandle_t f, int readAhead )
{
}

void Sys_EndStreamedFile( fileHandle_t f )
{
}

int Sys_StreamedRead( void* buffer, int size, int count, fileHandle_t f )
{
    return FS_Read( buffer, size * count, f );
}

void Sys_StreamSeek( fileHandle_t f, int offset, int origin )
{
    FS_Seek( f, offset, origin );
}


void* Sys_InitializeCriticalSection()
{
    return ( void* ) - 1;
}

void Sys_EnterCriticalSection( void* ptr )
{
}

void Sys_LeaveCriticalSection( void* ptr )
{
}

#else

void Sys_StreamFillBuffer( int i )
{
    int count;
    int r;
    int buffer;
    int readCount;
    int bufferPoint;

    // if there is any space left in the buffer, fill it up
    if( stream.sIO[i].active  && !stream.sIO[i].eof )
    {
        count = stream.sIO[i].bufferSize - ( stream.sIO[i].threadPosition - stream.sIO[i].streamPosition );
        if( !count )
        {
            return;
        }

        bufferPoint = stream.sIO[i].threadPosition % stream.sIO[i].bufferSize;
        buffer = stream.sIO[i].bufferSize - bufferPoint;
        readCount = buffer < count ? buffer : count;

        r = FS_Read2( stream.sIO[i].buffer + bufferPoint, readCount, stream.sIO[i].file );
        if( r != readCount )
        {
            stream.sIO[i].eof = true;
        }
        stream.sIO[i].threadPosition += r;
    }
}

/*
===============
Sys_StreamThread

A thread will be sitting in this loop forever
================
*/
void Sys_StreamThread( void )
{
    int i;

    while( 1 )
    {
        Sleep( 10 );
        EnterCriticalSection( &stream.crit );
        for( i = 1; i < 64; i++ )
        {
            Sys_StreamFillBuffer( i );
        }
        LeaveCriticalSection( &stream.crit );
    }
}


/*
===============
Sys_InitStreamThread
================
*/
void Sys_InitStreamThread( void )
{
    int i;

    InitializeCriticalSection( &stream.crit );

    stream.threadHandle = CreateThread(
                              NULL,   // LPSECURITY_ATTRIBUTES lpsa,
                              0,      // DWORD cbStack,
                              ( LPTHREAD_START_ROUTINE )Sys_StreamThread, // LPTHREAD_START_ROUTINE lpStartAddr,
                              0,      // LPVOID lpvThreadParm,
                              0,      //   DWORD fdwCreate,
                              ( LPDWORD )&stream.threadId );

    for( i = 0; i < 64; i++ )
    {
        stream.sIO[i].active = false;
    }

    stream.musicThreadHandle = CreateThread(
                                   NULL,   // LPSECURITY_ATTRIBUTES lpsa,
                                   0,      // DWORD cbStack,
                                   ( LPTHREAD_START_ROUTINE )Sys_MusicThread, // LPTHREAD_START_ROUTINE lpStartAddr,
                                   0,          // LPVOID lpvThreadParm,
                                   0,          //   DWORD fdwCreate,
                                   ( LPDWORD )&stream.musicThreadId );
}

/*
===============
Sys_ShutdownStreamThread
================
*/
void Sys_ShutdownStreamThread( void )
{
}


/*
===============
Sys_BeginStreamedFile
================
*/
void Sys_BeginStreamedFile( fileHandle_t f, int readAhead )
{
    if( stream.sIO[f].file )
    {
        Sys_EndStreamedFile( stream.sIO[f].file );
    }

    stream.sIO[f].buffer = ( byte* )Z_Malloc( readAhead );
    stream.sIO[f].bufferSize = readAhead;
    stream.sIO[f].streamPosition = 0;
    stream.sIO[f].threadPosition = 0;
    stream.sIO[f].eof = false;
    stream.sIO[f].file = f;
    stream.sIO[f].active = true;

    EnterCriticalSection( &stream.crit );

    Sys_StreamFillBuffer( f );

    LeaveCriticalSection( &stream.crit );
}

/*
===============
Sys_EndStreamedFile

================
*/
void Sys_EndStreamedFile( fileHandle_t f )
{
    if( f != stream.sIO[f].file )
    {
        Com_Error( ERR_FATAL, "Sys_EndStreamedFile: wrong file" );
    }

    EnterCriticalSection( &stream.crit );

    stream.sIO[f].active = false;
    stream.sIO[f].file = 0;

    Z_Free( stream.sIO[f].buffer );

    stream.sIO[f].buffer = NULL;

    LeaveCriticalSection( &stream.crit );
}


/*
===============
Sys_StreamedRead

================
*/
int Sys_StreamedRead( void* buffer, int size, int count, fileHandle_t f )
{
    int available;
    int remaining;
    int sleepCount;
    int copy;
    int bufferCount;
    int bufferPoint;
    byte*    dest;

    if( stream.sIO[f].active == false )
    {
        Com_Error( ERR_FATAL, "Streamed read with non-streaming file" );
    }

    dest = ( byte* )buffer;
    remaining = size * count;

    if( remaining <= 0 )
    {
        Com_Error( ERR_FATAL, "Streamed read with non-positive size" );
    }

    sleepCount = 0;
    while( remaining > 0 )
    {
        available = stream.sIO[f].threadPosition - stream.sIO[f].streamPosition;
        if( !available )
        {
            if( stream.sIO[f].eof )
            {
                break;
            }
            if( sleepCount == 1 )
            {
                Com_DPrintf( "Sys_StreamedRead: waiting\n" );
            }
            if( ++sleepCount > 100 )
            {
                Com_Error( ERR_FATAL, "Sys_StreamedRead: thread has died" );
            }
            Sleep( 10 );
            continue;
        }

        bufferPoint = stream.sIO[f].streamPosition % stream.sIO[f].bufferSize;
        bufferCount = stream.sIO[f].bufferSize - bufferPoint;

        copy = available < bufferCount ? available : bufferCount;
        if( copy > remaining )
        {
            copy = remaining;
        }
        memcpy( dest, stream.sIO[f].buffer + bufferPoint, copy );
        stream.sIO[f].streamPosition += copy;
        dest += copy;
        remaining -= copy;
    }

    return ( count * size - remaining ) / size;
}

/*
===============
Sys_StreamSeek

================
*/
void Sys_StreamSeek( fileHandle_t f, int offset, int origin )
{

    // halt the thread
    EnterCriticalSection( &stream.crit );

    // clear to that point
    FS_Seek( f, offset, origin );
    stream.sIO[f].streamPosition = 0;
    stream.sIO[f].threadPosition = 0;
    stream.sIO[f].eof = false;

    // let the thread start running at the new position
    LeaveCriticalSection( &stream.crit );
}

void* Sys_InitializeCriticalSection()
{
    LPCRITICAL_SECTION crit;

    crit = ( LPCRITICAL_SECTION )malloc( sizeof( CRITICAL_SECTION ) );
    InitializeCriticalSection( crit );
    return crit;
}

void Sys_EnterCriticalSection( void* ptr )
{
    LPCRITICAL_SECTION crit;
    crit = ( LPCRITICAL_SECTION )ptr;
    EnterCriticalSection( crit );
}

void Sys_LeaveCriticalSection( void* ptr )
{
    LPCRITICAL_SECTION crit;
    crit = ( LPCRITICAL_SECTION )ptr;
    LeaveCriticalSection( crit );
}

#endif

/*
========================================================================

EVENT LOOP

========================================================================
*/

#define MAX_QUED_EVENTS     256
#define MASK_QUED_EVENTS    ( MAX_QUED_EVENTS - 1 )

sysEvent_t eventQue[MAX_QUED_EVENTS];
int eventHead, eventTail;
byte sys_packetReceived[MAX_MSGLEN];

/*
================
Sys_QueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void* ptr )
{
    sysEvent_t*  ev;
    
    ev = &eventQue[ eventHead & MASK_QUED_EVENTS ];
    if( eventHead - eventTail >= MAX_QUED_EVENTS )
    {
        Com_Printf( "Sys_QueEvent: overflow\n" );
        // we are discarding an event, but don't leak memory
        if( ev->evPtr )
        {
            Z_Free( ev->evPtr );
        }
        eventTail++;
    }
    
    eventHead++;
    
    if( time == 0 )
    {
        time = Sys_Milliseconds();
    }
    
    ev->evTime = time;
    ev->evType = type;
    ev->evValue = value;
    ev->evValue2 = value2;
    ev->evPtrLength = ptrLength;
    ev->evPtr = ptr;
}

/*
================
Sys_GetEvent

================
*/
sysEvent_t Sys_GetEvent( void )
{
    MSG msg;
    sysEvent_t ev;
    char*        s;
    msg_t netmsg;
    netadr_t adr;
    
    // return if we have data
    if( eventHead > eventTail )
    {
        eventTail++;
        return eventQue[( eventTail - 1 ) & MASK_QUED_EVENTS ];
    }
    
    // pump the message loop
    while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( !GetMessage( &msg, NULL, 0, 0 ) )
        {
            Com_Quit_f();
        }
        
        // save the msg time, because wndprocs don't have access to the timestamp
        g_wv.sysMsgTime = msg.time;
        
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    
    // check for console commands
    s = Sys_ConsoleInput();
    if( s )
    {
        char*    b;
        int len;
        
        len = strlen( s ) + 1;
        b = ( char* )Z_Malloc( len );
        Q_strncpyz( b, s, len - 1 );
        Sys_QueEvent( 0, SE_CONSOLE, 0, 0, len, b );
    }
    
    // check for network packets
    MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
    if( Sys_GetPacket( &adr, &netmsg ) )
    {
        netadr_t*        buf;
        int len;
        
        // copy out to a seperate buffer for qeueing
        // the readcount stepahead is for SOCKS support
        len = sizeof( netadr_t ) + netmsg.cursize - netmsg.readcount;
        buf = ( netadr_t* )Z_Malloc( len );
        *buf = adr;
        memcpy( buf + 1, &netmsg.data[netmsg.readcount], netmsg.cursize - netmsg.readcount );
        Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
    }
    
    // return if we have data
    if( eventHead > eventTail )
    {
        eventTail++;
        return eventQue[( eventTail - 1 ) & MASK_QUED_EVENTS ];
    }
    
    // create an empty event to return
    
    memset( &ev, 0, sizeof( ev ) );
    ev.evTime = timeGetTime();
    
    return ev;
}

//================================================================

/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( void )
{
    IN_Shutdown();
    IN_Init();
}


/*
=================
Sys_Net_Restart_f

Restart the network subsystem
=================
*/
void Sys_Net_Restart_f( void )
{
    NET_Restart();
}


/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/
#define OSR2_BUILD_NUMBER 1111
#define WIN98_BUILD_NUMBER 1998

void Sys_Init( void )
{
    int cpuid;
    
    // make sure the timer is high precision, otherwise
    // NT gets 18ms resolution
    timeBeginPeriod( 1 );
    
    Cmd_AddCommand( "in_restart", Sys_In_Restart_f );
    Cmd_AddCommand( "net_restart", Sys_Net_Restart_f );
    
    g_wv.osversion.dwOSVersionInfoSize = sizeof( g_wv.osversion );
    
    if( !GetVersionEx( &g_wv.osversion ) )
    {
        Sys_Error( "Couldn't get OS info" );
    }
    
    if( g_wv.osversion.dwMajorVersion < 4 )
    {
        Sys_Error( "Wolf requires Windows version 4 or greater" );
    }
    if( g_wv.osversion.dwPlatformId == VER_PLATFORM_WIN32s )
    {
        Sys_Error( "Wolf doesn't run on Win32s" );
    }
    
    if( g_wv.osversion.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        Cvar_Set( "arch", "winnt" );
    }
    else if( g_wv.osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
    {
        if( LOWORD( g_wv.osversion.dwBuildNumber ) >= WIN98_BUILD_NUMBER )
        {
            Cvar_Set( "arch", "win98" );
        }
        else if( LOWORD( g_wv.osversion.dwBuildNumber ) >= OSR2_BUILD_NUMBER )
        {
            Cvar_Set( "arch", "win95 osr2.x" );
        }
        else
        {
            Cvar_Set( "arch", "win95" );
        }
    }
    else
    {
        Cvar_Set( "arch", "unknown Windows variant" );
    }
    
    // save out a couple things in rom cvars for the renderer to access
    Cvar_Get( "win_hinstance", va( "%i", ( int )g_wv.hInstance ), CVAR_ROM );
    Cvar_Get( "win_wndproc", va( "%i", ( int )MainWndProc ), CVAR_ROM );
    
    //
    // figure out our CPU
    //
    Cvar_Get( "sys_cpustring", "detect", 0 );
    if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "detect" ) )
    {
        Com_Printf( "...detecting CPU, found " );
        
        cpuid = Sys_GetProcessorId();
        
        switch( cpuid )
        {
            case CPUID_GENERIC:
                Cvar_Set( "sys_cpustring", "generic" );
                break;
            case CPUID_INTEL_UNSUPPORTED:
                Cvar_Set( "sys_cpustring", "x86 (pre-Pentium)" );
                break;
            case CPUID_INTEL_PENTIUM:
                Cvar_Set( "sys_cpustring", "x86 (P5/PPro, non-MMX)" );
                break;
            case CPUID_INTEL_MMX:
                Cvar_Set( "sys_cpustring", "x86 (P5/Pentium2, MMX)" );
                break;
            case CPUID_INTEL_KATMAI:
                Cvar_Set( "sys_cpustring", "Intel Pentium III" );
                break;
            case CPUID_AMD_3DNOW:
                Cvar_Set( "sys_cpustring", "AMD w/ 3DNow!" );
                break;
            case CPUID_AXP:
                Cvar_Set( "sys_cpustring", "Alpha AXP" );
                break;
            default:
                Com_Error( ERR_FATAL, "Unknown cpu type %d\n", cpuid );
                break;
        }
    }
    else
    {
        Com_Printf( "...forcing CPU type to " );
        if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "generic" ) )
        {
            cpuid = CPUID_GENERIC;
        }
        else if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "x87" ) )
        {
            cpuid = CPUID_INTEL_PENTIUM;
        }
        else if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "mmx" ) )
        {
            cpuid = CPUID_INTEL_MMX;
        }
        else if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "3dnow" ) )
        {
            cpuid = CPUID_AMD_3DNOW;
        }
        else if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "PentiumIII" ) )
        {
            cpuid = CPUID_INTEL_KATMAI;
        }
        else if( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "axp" ) )
        {
            cpuid = CPUID_AXP;
        }
        else
        {
            Com_Printf( "WARNING: unknown sys_cpustring '%s'\n", Cvar_VariableString( "sys_cpustring" ) );
            cpuid = CPUID_GENERIC;
        }
    }
    Cvar_SetValue( "sys_cpuid", cpuid );
    Com_Printf( "%s\n", Cvar_VariableString( "sys_cpustring" ) );
    
    Cvar_Set( "username", Sys_GetCurrentUser() );
    
    IN_Init();      // FIXME: not in dedicated?
}

void* Sys_GetSystemHandles( void )
{
    return &g_wv;
}

//=======================================================================

int totalMsec, countMsec;

/*
==============
Sys_Quit
==============
*/
void Sys_Quit( void )
{
    timeEndPeriod( 1 );
    IN_Shutdown();
    Sys_DestroyConsole();
    OculusVR_Exit();
    
    exit( 0 );
}

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    char cwd[MAX_OSPATH];
    int startTime, endTime;
    
    // should never get a previous instance in Win32
    if( hPrevInstance )
    {
        return 0;
    }
    
    g_wv.classRegistered = false;
    
    g_wv.hInstance = hInstance;
    Q_strncpyz( sys_cmdline, lpCmdLine, sizeof( sys_cmdline ) );
    
    // done before Com/Sys_Init since we need this for error output
    Sys_CreateConsole();
    
    // no abort/retry/fail errors
    SetErrorMode( SEM_FAILCRITICALERRORS );
    
    // get the initial time base
    Sys_Milliseconds();
    
// re-enabled CD checking for proper 'setup.exe' file on game cd
// (SA) enable to do cd check for setup\setup.exe
//#if 1
#if 0
    // if we find the CD, add a +set cddir xxx command line
    if( !Sys_ScanForCD() )
    {
        Sys_Error( "Game CD not in drive" );
    }
    
#endif
    
    Sys_InitStreamThread();
    
    // Init Oculus SDK
    if( OculusVR_Init() == 0 )
    {
        OculusVRDetected = 0;
        Com_Printf( "[OVR] OculusVR_Init() Failed!\n" );
        HMD.HResolution = 1280;
        HMD.VResolution = 800;
        HMD.HScreenSize = 0.14976;
        HMD.VScreenSize = 0.0935;
        HMD.EyeToScreenDistance = 0.04;
        HMD.InterpupillaryDistance = 0.058;
    }
    else
    {
        OculusVRDetected = 1;
        Com_Printf( "[OVR] OculusVR_Init() Success!\n" );
        if( OculusVR_QueryHMD( &HMD ) != 0 )
        {
            Com_Printf( "[OVR] Device Name       : %s\n", HMD.DisplayDeviceName );
            Com_Printf( "[OVR] IPD               : %f\n", HMD.InterpupillaryDistance );
            Com_Printf( "[OVR] Lens Separation D.: %f\n", HMD.LensSeparationDistance );
            Com_Printf( "[OVR] Eye To Screen Dist: %f\n", HMD.EyeToScreenDistance );
            Com_Printf( "[OVR] Screen Size       : %f, %f\n", HMD.HScreenSize, HMD.VScreenSize );
            Com_Printf( "[OVR] Resolution        : %d, %d\n", HMD.HResolution, HMD.VResolution );
            Com_Printf( "[OVR] Desktop Size      : %d, %d\n", HMD.DesktopX, HMD.DesktopY );
            Com_Printf( "[OVR] VScreen Center    : %f\n", HMD.VScreenCenter );
            Com_Printf( "[OVR] Distortion K      : %f, %f, %f, %f\n", HMD.DistortionK[0], HMD.DistortionK[1], HMD.DistortionK[2], HMD.DistortionK[3] );
        }
    }
    
    Com_Init( sys_cmdline );
    NET_Init();
    
    _getcwd( cwd, sizeof( cwd ) );
    Com_Printf( "Working directory: %s\n", cwd );
    
    // hide the early console since we've reached the point where we
    // have a working graphics subsystems
    if( !com_dedicated->integer && !com_viewlog->integer )
    {
        Sys_ShowConsole( 0, false );
    }
    
    SetFocus( g_wv.hWnd );
    
    // main game loop
    while( 1 )
    {
        // if not running as a game client, sleep a bit
        if( g_wv.isMinimized || ( com_dedicated && com_dedicated->integer ) )
        {
            Sleep( 5 );
        }
        
        // set low precision every frame, because some system calls
        // reset it arbitrarily
//		_controlfp( _PC_24, _MCW_PC );
//    _controlfp( -1, _MCW_EM  ); // no exceptions, even if some crappy
        // syscall turns them back on!
        
        startTime = Sys_Milliseconds();
        
        // make sure mouse and joystick are only called once a frame
        IN_Frame();
        
        // run the game
        Com_Frame();
        
        endTime = Sys_Milliseconds();
        totalMsec += endTime - startTime;
        countMsec++;
    }
    
    // never gets here
}

