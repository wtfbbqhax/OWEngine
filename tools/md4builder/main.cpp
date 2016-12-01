//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2010 JV Software
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
//  File name:   main.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "md4builder.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <direct.h>
#include <stdio.h>

globals_t globals;

/*
============
Com_Printf
============
*/
void Com_Printf( const char* fmt, ... )
{
    va_list argptr;
    char msg[4096];
    
    va_start( argptr, fmt );
    vsprintf( msg, fmt, argptr );
    va_end( argptr );
    
    printf( msg );
    OutputDebugStringA( msg );
}

/*
============
Main
============
*/
int main( int argc, char* argv[] )
{
    Com_Printf( "MD4Builder (c) 2016 Dusan Jocic. \n" );
    
    if( argc < 3 )
    {
        Com_Printf( "MD4Builder <command> <input> <output>\n" );
        return 0;
    }
    
    // Set the current directory to the one above this one.
    _chdir( "../" );
    
    // Init the FBX sdk.
    globals.fbxManager.Init();
    
    if( !strcmp( argv[1], "-buildmesh" ) )
    {
        owMD4Mesh meshbuilder;
        
        Com_Printf( "Compiling Mesh...\n" );
        
        // Load in the FBX file.
        if( !globals.fbxManager.LoadFBXFromFile( argv[2] ) )
        {
            return false;
        }
        
        // Build the MD4Mesh from the fbx file.
        if( !meshbuilder.BuildMesh() )
        {
            return false;
        }
        
        meshbuilder.ExportMesh( argv[3] );
    }
    else if( !strcmp( argv[1], "-buildanim" ) )
    {
        owMD4Anim animBuilder;
        
        Com_Printf( "Compiling Anim...\n" );
        
        // Load in the FBX file.
        if( !globals.fbxManager.LoadFBXFromFile( argv[2] ) )
        {
            return false;
        }
        
        if( strcmp( argv[3], "-export" ) )
        {
            Com_Printf( "Expected -export switch...\n" );
            return false;
        }
        
        int startframe = atoi( argv[4] );
        int endframe = atoi( argv[5] );
        
        if( !animBuilder.BuildAnim( startframe, endframe ) )
        {
            return false;
        }
        
        animBuilder.WriteAnim( argv[6] );
    }
    else
    {
        Com_Printf( "Unknown command %s\n", argv[1] );
    }
    
    return 0;
}
