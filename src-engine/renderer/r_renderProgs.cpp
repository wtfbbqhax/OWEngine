//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
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
//  File name:   r_renderProgs.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "r_local.h"
#include "../splines/math_matrix.h"
#include "../splines/math_vector.h"

/*
==============================================================================
SHADER LOADING
==============================================================================
*/

static char				r_shaderDefines[4096];
static shader_t* 		r_shaders[MAX_SHADERS];
static int				r_numShaders;

/*
 ==================
 GL_BindProgram
 ==================
 */
void GL_BindProgram( shaderProgram_t* program )
{
    if( !program )
    {
        backEnd.glState.program = NULL;
        glUseProgram( 0 );
        return;
    }
    
    backEnd.glState.program = ( GLhandleARB ) program;
    
    glUseProgram( program->program );
}


/*
==================
RenderProgs_ShowShaderInfoLog
==================
*/
static void RenderProgs_ShowShaderInfoLog( shader_t* shader )
{
    char	infoLog[4096];
    int		infoLogLen;
    
    glGetShaderiv( shader->shaderId, GL_INFO_LOG_LENGTH, &infoLogLen );
    if( infoLogLen <= 1 )
    {
        return;
    }
    
    glGetShaderInfoLog( shader->shaderId, sizeof( infoLog ), NULL, infoLog );
    
    ri.Printf( PRINT_DEVELOPER, "---------- RenderProg Info Log ----------\n" );
    ri.Printf( PRINT_DEVELOPER, "%s", infoLog );
    
    if( infoLogLen >= sizeof( infoLog ) )
    {
        ri.Printf( PRINT_DEVELOPER, "...\n" );
    }
    
    ri.Printf( PRINT_DEVELOPER, "-----------------------------------------\n" );
}

/*
==================
RenderProgs_CompileShader
==================
*/
static void RenderProgs_CompileShader( shader_t* shader, const char* defines, const char* code )
{
    const char* source[2];
    
    switch( shader->type )
    {
        case GL_VERTEX_SHADER:
            ri.Printf( PRINT_ALL, "Compiling GLSL vertex shader '%s'...\n", shader->name );
            break;
        case GL_FRAGMENT_SHADER:
            ri.Printf( PRINT_ALL, "Compiling GLSL fragment shader '%s'...\n", shader->name );
            break;
        case GL_GEOMETRY_SHADER:
            ri.Printf( PRINT_ALL, "Compiling GLSL geometry shader '%s'...\n", shader->name );
            break;
    }
    
    // Create the shader
    shader->shaderId = glCreateShader( shader->type );
    
    // Upload the shader source
    source[0] = defines;
    source[1] = code;
    
    glShaderSource( shader->shaderId, 2, source, NULL );
    
    // Compile the shader
    glCompileShader( shader->shaderId );
    
    // if an info log is available, print it to the console
    RenderProgs_ShowShaderInfoLog( shader );
    
    // check if the compile was successful
    glGetShaderiv( shader->shaderId, GL_COMPILE_STATUS, ( GLint* )&shader->compiled );
    
    if( !shader->compiled )
    {
        switch( shader->type )
        {
            case GL_VERTEX_SHADER:
                ri.Printf( PRINT_ALL, S_COLOR_RED "Failed to compile vertex shader '%s'\n", shader->name );
                break;
            case GL_FRAGMENT_SHADER:
                ri.Printf( PRINT_ALL, S_COLOR_RED "Failed to compile fragment shader '%s'\n", shader->name );
                break;
            case GL_GEOMETRY_SHADER:
                ri.Printf( PRINT_ALL, S_COLOR_RED "Failed to compile geometry shader '%s'\n", shader->name );
                break;
        }
        
        return;
    }
}

/*
==================
RenderProgs_LoadShader
==================
*/
static shader_t* RenderProgs_LoadShader( const char* name, GLenum type, char* code )
{
    shader_t*	shader;
    
    if( r_numShaders == MAX_SHADERS )
    {
        ri.Error( PRINT_ERROR, "RenderProgs_LoadShader: MAX_SHADER hit" );
    }
    
    r_shaders[r_numShaders++] = shader = ( shader_t* )malloc( sizeof( shader_t ) );
    
    // Fill it in
    strncpy( shader->name, name, sizeof( shader->name ) );
    shader->type = type;
    shader->references = 0;
    
    RenderProgs_CompileShader( shader, r_shaderDefines, code );
    
    return shader;
}

/*
==================
RenderProgs_FindShader
==================
*/
shader_t* RenderProgs_FindShader( const char* name, GLenum type )
{
    shader_t*	shader;
    char*		code;
    idStr       fullPath = "renderProgs/";
    fullPath += name;
    
    switch( type )
    {
        case GL_VERTEX_SHADER:
            fullPath += ".vertex";
            break;
        case GL_FRAGMENT_SHADER:
            fullPath += ".fragment";
            break;
        case GL_GEOMETRY_SHADER:
            fullPath += ".geometry";
            break;
    }
    
    if( !name || !name[0] )
    {
        return NULL;
    }
    
    // load it from disk
    ri.Printf( PRINT_ALL, "%s", fullPath.c_str() );
    
    ri.FS_ReadFile( fullPath, ( void** )&code );
    if( !code )
    {
        ri.Printf( PRINT_ALL, ": File not found\n" );
        return NULL;
    }
    
    ri.Printf( PRINT_ALL, "\n" );
    
    // load the shader
    shader = RenderProgs_LoadShader( name, type, code );
    
    ri.FS_FreeFile( code );
    
    if( !shader->compiled )
    {
        return NULL;
    }
    
    return shader;
}

//========================================================

/*
==================
RenderProgs_InitShaders
==================
*/
void RenderProgs_InitShaders( void )
{
    // build defines string
    strncpy( r_shaderDefines, "#version 330" "\n", sizeof( r_shaderDefines ) );
}

/*
==================
RenderProgs_ShutdownShaders
==================
*/
void RenderProgs_ShutdownShaders( void )
{
    shader_t*	shader;
    int			i;
    
    // delete all the shaders
    for( i = 0; i < r_numShaders; i++ )
    {
        shader = r_shaders[i];
        glDeleteShader( shader->shaderId );
    }
    
    // clear shaders list
    memset( r_shaders, 0, sizeof( r_shaders ) );
    
    r_numShaders = 0;
}


/*
==============================================================================
PROGRAM LOADING
==============================================================================
*/

static shaderProgram_t* r_programs[MAX_PROGRAMS];
static int				r_numPrograms;

/*
==================
RenderProgs_PrintProgramInfoLog
==================
*/
static void RenderProgs_PrintProgramInfoLog( shaderProgram_t* program )
{
    char	infoLog[4096];
    int		infoLogLen;
    
    glGetProgramiv( program->program, GL_INFO_LOG_LENGTH, &infoLogLen );
    if( infoLogLen <= 1 )
    {
        return;
    }
    
    glGetProgramInfoLog( program->program, sizeof( infoLog ), NULL, infoLog );
    
    ri.Printf( PRINT_DEVELOPER, "---------- Program Info Log ----------\n" );
    ri.Printf( PRINT_DEVELOPER, "%s", infoLog );
    
    if( infoLogLen >= sizeof( infoLog ) )
    {
        ri.Printf( PRINT_DEVELOPER, "...\n" );
    }
    
    ri.Printf( PRINT_DEVELOPER, "--------------------------------------\n" );
}

/*
=================
RenderProgs_LinkProgram
links the GLSL vertex, fragment and geometry shaders together to form a GLSL program
=================
*/
static void RenderProgs_LinkProgram( shaderProgram_t* shaderProgram )
{
    // create shader program object
    shaderProgram->program = glCreateProgram();
    
    // attach the vertex, fragment andgeometry shaders
    glAttachShader( shaderProgram->program, shaderProgram->vertexShader->shaderId );
    glAttachShader( shaderProgram->program, shaderProgram->fragmentShader->shaderId );
    glAttachShader( shaderProgram->program, shaderProgram->geometryShader->shaderId );
    
    // link into a shader program
    glLinkProgram( shaderProgram->program );
    
    // if an info log is available, print it to the console
    RenderProgs_PrintProgramInfoLog( shaderProgram );
    
    // make sure we linked succesfully
    glGetProgramiv( shaderProgram->program, GL_OBJECT_LINK_STATUS_ARB, ( GLint* )&shaderProgram->linked );
    if( !shaderProgram->linked )
    {
        ri.Printf( PRINT_ALL, S_COLOR_RED "R_LinkGLSLShader: program failed to link\n" );
        return;
    }
}

/*
==================
RenderProgs_LoadProgram
==================
*/
static shaderProgram_t* RenderProgs_LoadProgram( const char* name, shader_t* vertexShader, shader_t* fragmentShader, shader_t* geometryShader )
{
    shaderProgram_t*	program;
    
    if( r_numPrograms == MAX_PROGRAMS )
    {
        ri.Error( PRINT_ERROR, "RenderProgs_LoadProgram: MAX_PROGRAMS hit" );
    }
    
    r_programs[r_numPrograms++] = program = ( shaderProgram_t* )malloc( sizeof( shaderProgram_t ) );
    
    // fill it in
    strncpy( program->name, name, sizeof( program->name ) );
    program->vertexShader = vertexShader;
    program->fragmentShader = fragmentShader;
    program->geometryShader = geometryShader;
    
    RenderProgs_LinkProgram( program );
    
    return program;
}

/*
==================
RenderProgs_FindProgram
==================
*/
shaderProgram_t* RenderProgs_FindProgram( const char* name, shader_t* vertexShader, shader_t* fragmentShader, shader_t* geometryShader )
{
    shaderProgram_t*	program;
    
    if( !vertexShader || !fragmentShader || !geometryShader )
    {
        ri.Error( PRINT_ERROR, "RenderProgs_FindProgram: NULL shader" );
    }
    
    if( !name || !name[0] )
    {
        name = va( "%s & %s & %s", vertexShader->name, fragmentShader->name, geometryShader->name );
    }
    
    // Load the program
    program = RenderProgs_LoadProgram( name, vertexShader, fragmentShader, geometryShader );
    
    if( !program->linked )
    {
        return NULL;
    }
    
    return program;
}

/*
==================
R_ShutdownPrograms
==================
*/
void RenderProgs_ShutdownPrograms( void )
{
    shaderProgram_t*	program;
    int			        i;
    
    // delete all the programs
    glUseProgram( 0 );
    
    for( i = 0; i < r_numPrograms; i++ )
    {
        program = r_programs[i];
        
        glDetachShader( program->program, program->vertexShader->shaderId );
        glDetachShader( program->program, program->fragmentShader->shaderId );
        glDetachShader( program->program, program->geometryShader->shaderId );
        
        glDeleteProgram( program->program );
    }
    
    // clear programs list
    memset( r_programs, 0, sizeof( r_programs ) );
    
    r_numPrograms = 0;
}

/*
==================
RenderProgsInitialization
==================
*/
void RenderProgsInitialization( void )
{
    shader_t* vertexShader, *fragmentShader, *geometrySahder;
    
    // load shader programs
    vertexShader = RenderProgs_FindShader( "", GL_VERTEX_SHADER );
    fragmentShader = RenderProgs_FindShader( "", GL_FRAGMENT_SHADER );
    geometrySahder = RenderProgs_FindShader( "", GL_GEOMETRY_SHADER );
}

// ============================================================================
