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
//  File name:   r_shade.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "r_local.h"

/*

  THIS ENTIRE FILE IS BACK END

  This file deals with applying shaders to surface data in the tess struct.
*/

/*
================
R_ArrayElementDiscrete

This is just for OpenGL conformance testing, it should never be the fastest
================
*/
#if 0
static void APIENTRY R_ArrayElementDiscrete( GLint index )
{
    glColor4ubv( tess.svars.colors[ index ] );
    if( glState.currenttmu )
    {
        glMultiTexCoord2fARB( 0, tess.svars.texcoords[ 0 ][ index ][0], tess.svars.texcoords[ 0 ][ index ][1] );
        glMultiTexCoord2fARB( 1, tess.svars.texcoords[ 1 ][ index ][0], tess.svars.texcoords[ 1 ][ index ][1] );
    }
    else
    {
        glTexCoord2fv( tess.svars.texcoords[ 0 ][ index ] );
    }
    glVertex3fv( tess.xyz[ index ] );
}
#endif

/*
===================
R_DrawStripElements

===================
*/
static int c_vertexes;          // for seeing how long our average strips are
static int c_begins;
#if 0
static void R_DrawStripElements( int numIndexes, const glIndex_t* indexes, void ( APIENTRY* element )( GLint ) )
{
    int i;
    int last[3] = { -1, -1, -1 };
    bool even;
    
    c_begins++;
    
    if( numIndexes <= 0 )
    {
        return;
    }
    
    glBegin( GL_TRIANGLE_STRIP );
    
    // prime the strip
    element( indexes[0] );
    element( indexes[1] );
    element( indexes[2] );
    c_vertexes += 3;
    
    last[0] = indexes[0];
    last[1] = indexes[1];
    last[2] = indexes[2];
    
    even = false;
    
    for( i = 3; i < numIndexes; i += 3 )
    {
        // odd numbered triangle in potential strip
        if( !even )
        {
            // check previous triangle to see if we're continuing a strip
            if( ( indexes[i + 0] == last[2] ) && ( indexes[i + 1] == last[1] ) )
            {
                element( indexes[i + 2] );
                c_vertexes++;
                assert( indexes[i + 2] < tess.numVertexes );
                even = true;
            }
            // otherwise we're done with this strip so finish it and start
            // a new one
            else
            {
                glEnd();
                
                glBegin( GL_TRIANGLE_STRIP );
                c_begins++;
                
                element( indexes[i + 0] );
                element( indexes[i + 1] );
                element( indexes[i + 2] );
                
                c_vertexes += 3;
                
                even = false;
            }
        }
        else
        {
            // check previous triangle to see if we're continuing a strip
            if( ( last[2] == indexes[i + 1] ) && ( last[0] == indexes[i + 0] ) )
            {
                element( indexes[i + 2] );
                c_vertexes++;
                
                even = false;
            }
            // otherwise we're done with this strip so finish it and start
            // a new one
            else
            {
                glEnd();
                
                glBegin( GL_TRIANGLE_STRIP );
                c_begins++;
                
                element( indexes[i + 0] );
                element( indexes[i + 1] );
                element( indexes[i + 2] );
                c_vertexes += 3;
                
                even = false;
            }
        }
        
        // cache the last three vertices
        last[0] = indexes[i + 0];
        last[1] = indexes[i + 1];
        last[2] = indexes[i + 2];
    }
    
    glEnd();
}
#endif


/*
==================
R_DrawElements

Optionally performs our own glDrawElements that looks for strip conditions
instead of using the single glDrawElements call that may be inefficient
without compiled vertex arrays.
==================
*/
static void R_DrawElements( int numIndexes, const glIndex_t* indexes )
{
    glDrawElements( GL_TRIANGLES, numIndexes, GL_INDEX_TYPE, indexes );
}

/*
=============================================================

SURFACE SHADERS

=============================================================
*/

shaderCommands_t tess;
static bool setArraysOnce;

/*
=================
R_BindAnimatedImage
=================
*/
static void R_BindAnimatedImage( textureBundle_t* bundle )
{
    int index;
    
    if( bundle->isVideoMap )
    {
        CIN_RunCinematic( bundle->videoMapHandle );
        CIN_UploadCinematic( bundle->videoMapHandle );
        return;
    }
    
    if( bundle->numImageAnimations <= 1 )
    {
        if( bundle->isLightmap && ( backEnd.refdef.rdflags & RDF_SNOOPERVIEW ) )
        {
            GL_Bind( tr.whiteImage );
        }
        else
        {
            GL_Bind( bundle->image[0] );
        }
        return;
    }
    
    // it is necessary to do this messy calc to make sure animations line up
    // exactly with waveforms of the same frequency
    index = myftol( tess.shaderTime * bundle->imageAnimationSpeed * FUNCTABLE_SIZE );
    index >>= FUNCTABLE_SIZE2;
    
    if( index < 0 )
    {
        index = 0;  // may happen with shader time offsets
    }
    index %= bundle->numImageAnimations;
    
    if( bundle->isLightmap && ( backEnd.refdef.rdflags & RDF_SNOOPERVIEW ) )
    {
        GL_Bind( tr.whiteImage );
    }
    else
    {
        GL_Bind( bundle->image[ index ] );
    }
}

/*
================
DrawTris

Draws triangle outlines for debugging
================
*/
static void DrawTris( shaderCommands_t* input )
{
    GL_Bind( tr.whiteImage );
    glColor4f( 1, 1, 1, 1 );
    
    GL_State( GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE );
    
    if( r_showtris->integer == 1 )
    {
#if defined (__ANDROID__)
        glDepthRangef( 0, 0 );
#else
        glDepthRange( 0, 0 );
#endif
    }
    
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    
    glVertexPointer( 3, GL_FLOAT, 16, input->xyz ); // padded for SIMD
    
    if( glLockArraysEXT )
    {
        glLockArraysEXT( 0, input->numVertexes );
        GLimp_LogComment( "glLockArraysEXT\n" );
    }
    
    R_DrawElements( input->numIndexes, input->indexes );
    
    if( glUnlockArraysEXT )
    {
        glUnlockArraysEXT();
        GLimp_LogComment( "glUnlockArraysEXT\n" );
    }
#if defined (__ANDROID__)
    glDepthRangef( 0, 1 );
#else
    glDepthRange( 0, 1 );
#endif
}


/*
================
DrawNormals

Draws vertex normals for debugging
================
*/
static void DrawNormals( shaderCommands_t* input )
{
    int		i;
    vec3_t	temp;
    vec3_t verts[2 * SHADER_MAX_VERTEXES];
    unsigned short indicies[2 * SHADER_MAX_VERTEXES];
    
    for( i = 0; i < input->numVertexes; i++ )
    {
        VectorCopy( input->xyz[i], verts[i * 2] );
        VectorMA( input->xyz[i], 2, input->normals[i], temp );
        VectorCopy( temp, verts[( i * 2 ) + 1] );
        indicies[( i * 2 )] = i * 2;
        indicies[( i * 2 ) + 1] = ( i * 2 ) + 1;
    }
    
    GL_Bind( tr.whiteImage );
    glColor4f( 1, 1, 1, 1 );
#if defined (__ANDROID__)
    glDepthRangef( 0, 0 );	// never occluded
#else
    glDepthRange( 0, 0 );	// never occluded
#endif
    GL_State( GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE );
    
    glVertexPointer( 3, GL_FLOAT, 0, verts );
    glDrawElements( GL_LINES, i, GL_UNSIGNED_SHORT, indicies );
    
#if defined (__ANDROID__)
    glDepthRangef( 0, 1 );
#else
    glDepthRange( 0, 1 );
#endif
}

/*
==============
RB_BeginSurface

We must set some things up before beginning any tesselation,
because a surface may be forced to perform a RB_End due
to overflow.
==============
*/
void RB_BeginSurface( shader_t* shader, int fogNum )
{

    shader_t* state = ( shader->remappedShader ) ? shader->remappedShader : shader;
    
    tess.ATI_tess = false;     //----(SA)	added
    tess.numIndexes = 0;
    tess.numVertexes = 0;
    tess.shader = state;
    tess.fogNum = fogNum;
    tess.dlightBits = 0;        // will be OR'd in by surface functions
    tess.xstages = state->stages;
    tess.numPasses = state->numUnfoggedPasses;
    tess.currentStageIteratorFunc = state->optimalStageIteratorFunc;
    
    tess.shaderTime = backEnd.refdef.floatTime - tess.shader->timeOffset;
    if( tess.shader->clampTime && tess.shaderTime >= tess.shader->clampTime )
    {
        tess.shaderTime = tess.shader->clampTime;
    }
    // done.
}

/*
===================
DrawMultitextured

output = t0 * t1 or t0 + t1

t0 = most upstream according to spec
t1 = most downstream according to spec
===================
*/
static void DrawMultitextured( shaderCommands_t* input, int stage )
{
    shaderStage_t*   pStage;
    
    pStage = tess.xstages[stage];
    
    // Ridah
    if( tess.shader->noFog && pStage->isFogged )
    {
        R_FogOn();
    }
    else if( tess.shader->noFog && !pStage->isFogged )
    {
        R_FogOff(); // turn it back off
    }
    else        // make sure it's on
    {
        R_FogOn();
    }
    // done.
    
    GL_State( pStage->stateBits );
    
    // this is an ugly hack to work around a GeForce driver
    // bug with multitexture and clip planes
#if 0
    if( backEnd.viewParms.isPortal )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
#endif
    
    //
    // base
    //
    GL_SelectTexture( 0 );
    glTexCoordPointer( 2, GL_FLOAT, 0, input->svars.texcoords[0] );
    R_BindAnimatedImage( &pStage->bundle[0] );
    
    //
    // lightmap/secondary pass
    //
    GL_SelectTexture( 1 );
    glEnable( GL_TEXTURE_2D );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    if( r_lightmap->integer )
    {
        GL_TexEnv( GL_REPLACE );
    }
    else
    {
        GL_TexEnv( tess.shader->multitextureEnv );
    }
    
    glTexCoordPointer( 2, GL_FLOAT, 0, input->svars.texcoords[1] );
    
    R_BindAnimatedImage( &pStage->bundle[1] );
    
    R_DrawElements( input->numIndexes, input->indexes );
    
    //
    // disable texturing on TEXTURE1, then select TEXTURE0
    //
    //glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisable( GL_TEXTURE_2D );
    
    GL_SelectTexture( 0 );
}



/*
===================
ProjectDlightTexture

Perform dynamic lighting with another rendering pass
===================
*/
static void ProjectDlightTexture( void )
{
    int i, l;
    vec3_t origin;
    float*   texCoords;
    byte*    colors;
    byte clipBits[SHADER_MAX_VERTEXES];
    MAC_STATIC float texCoordsArray[SHADER_MAX_VERTEXES][2];
    byte colorArray[SHADER_MAX_VERTEXES][4];
    unsigned short hitIndexes[SHADER_MAX_INDEXES];
    int numIndexes;
    float scale;
    float radius;
    vec3_t floatColor;
    
    if( !backEnd.refdef.num_dlights )
    {
        return;
    }
    
    
    if( backEnd.refdef.rdflags & RDF_SNOOPERVIEW )     // no dlights for snooper
    {
        return;
    }
    
    
    for( l = 0 ; l < backEnd.refdef.num_dlights ; l++ )
    {
        dlight_t*    dl;
        
        if( !( tess.dlightBits & ( 1 << l ) ) )
        {
            continue;   // this surface definately doesn't have any of this light
        }
        texCoords = texCoordsArray[0];
        colors = colorArray[0];
        
        dl = &backEnd.refdef.dlights[l];
        VectorCopy( dl->transformed, origin );
        radius = dl->radius;
        scale = 1.0f / radius;
        floatColor[0] = dl->color[0] * 255.0f;
        floatColor[1] = dl->color[1] * 255.0f;
        floatColor[2] = dl->color[2] * 255.0f;
        
        for( i = 0 ; i < tess.numVertexes ; i++, texCoords += 2, colors += 4 )
        {
            vec3_t dist;
            int clip;
            float modulate;
            
            if( 0 )
            {
                clipBits[i] = 255;  // definately not dlighted
                continue;
            }
            
            VectorSubtract( origin, tess.xyz[i], dist );
            
//			if(!r_dlightBacks->integer) {
//				vec3_t	dir;
//				VectorNormalize2(dist, dir);
//				if( DotProduct( tess.normal[i], dir) < 0) {
//					clipBits[i] = 255;	// not lighted (backface)
//					continue;
//				}
//			}

            backEnd.pc.c_dlightVertexes++;
            
            texCoords[0] = 0.5f + dist[0] * scale;
            texCoords[1] = 0.5f + dist[1] * scale;
            
            clip = 0;
            if( texCoords[0] < 0.0f )
            {
                clip |= 1;
            }
            else if( texCoords[0] > 1.0f )
            {
                clip |= 2;
            }
            if( texCoords[1] < 0.0f )
            {
                clip |= 4;
            }
            else if( texCoords[1] > 1.0f )
            {
                clip |= 8;
            }
            // modulate the strength based on the height and color
            if( dist[2] > radius )
            {
                clip |= 16;
                modulate = 0.0f;
            }
            else if( dist[2] < -radius )
            {
                clip |= 32;
                modulate = 0.0f;
            }
            else
            {
                modulate = dist[2] * scale;
                modulate = 1 - modulate * modulate;
            }
            clipBits[i] = clip;
            
            colors[0] = myftol( floatColor[0] * modulate );
            colors[1] = myftol( floatColor[1] * modulate );
            colors[2] = myftol( floatColor[2] * modulate );
            colors[3] = 255;
        }
        
        // build a list of triangles that need light
        numIndexes = 0;
        for( i = 0 ; i < tess.numIndexes ; i += 3 )
        {
            short a, b, c;
            vec3_t va, vb, vc, vx;
            
            a = tess.indexes[i];
            b = tess.indexes[i + 1];
            c = tess.indexes[i + 2];
            if( clipBits[a] & clipBits[b] & clipBits[c] )
            {
                continue;   // not lighted
            }
            
//			if(!r_dlightBacks->integer) {
//				vec3_t	dir;
//				VectorSubtract( origin, tess.xyz[a], dir );
//				VectorNormalize(dir);
//				if( DotProduct( tess.normal[i], dir) < 0) {
//					continue;	// not lighted (backface)
//				}
//			}

            VectorSubtract( origin, tess.xyz[a], va );
            VectorSubtract( tess.xyz[a], tess.xyz[b], vb );
            VectorSubtract( tess.xyz[a], tess.xyz[c], vc );
            CrossProduct( vb, vc, vx );
            if( DotProduct( vx, va ) > 0 )
            {
                continue;
            }
            
            hitIndexes[numIndexes] = a;
            hitIndexes[numIndexes + 1] = b;
            hitIndexes[numIndexes + 2] = c;
            numIndexes += 3;
        }
        
        if( !numIndexes )
        {
            continue;
        }
        
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, texCoordsArray[0] );
        
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorArray );
        
        //----(SA) creating dlight shader to allow for special blends or alternate dlight texture
        {
            shader_t* dls = dl->dlshader;
            if( dls )
            {
//				if (!glActiveTextureARB || dls->numUnfoggedPasses < 2) {
                for( i = 0; i < dls->numUnfoggedPasses; i++ )
                {
                    shaderStage_t* stage = dls->stages[i];
                    R_BindAnimatedImage( &dls->stages[i]->bundle[0] );
                    GL_State( stage->stateBits | GLS_DEPTHFUNC_EQUAL );
                    R_DrawElements( numIndexes, hitIndexes );
                    backEnd.pc.c_totalIndexes += numIndexes;
                    backEnd.pc.c_dlightIndexes += numIndexes;
                }
                /*
                				} else {	// optimize for multitexture
                
                					for(i=0;i<dls->numUnfoggedPasses;)
                					{
                						shaderStage_t *stage = dls->stages[i];
                
                						GL_State(stage->stateBits | GLS_DEPTHFUNC_EQUAL);
                
                						// setup each TMU
                						for (tmu=0; tmu<glConfig.maxActiveTextures && i<dls->numUnfoggedPasses; tmu++, i++) {
                
                							GL_SelectTexture( tmu );
                
                							if (tmu) {
                								glEnable( GL_TEXTURE_2D );
                							}
                
                							R_BindAnimatedImage( &dls->stages[i]->bundle[0] );
                						}
                
                						// draw the elements
                						R_DrawElements( numIndexes, hitIndexes );
                						backEnd.pc.c_totalIndexes += numIndexes;
                						backEnd.pc.c_dlightIndexes += numIndexes;
                					}
                
                					// turn off unused TMU's
                					for (tmu=1; tmu<glConfig.maxActiveTextures; tmu++) {
                						// set back to default state
                						GL_SelectTexture( tmu );
                						glDisable( GL_TEXTURE_2D );
                					}
                
                					// return to TEXTURE0
                					GL_SelectTexture( 0 );
                				}
                */
            }
            else
            {
                R_FogOff();
                
//				if (!dl->overdraw || !glActiveTextureARB) {
                GL_Bind( tr.dlightImage );
                // include GLS_DEPTHFUNC_EQUAL so alpha tested surfaces don't add light
                // where they aren't rendered
                GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
                R_DrawElements( numIndexes, hitIndexes );
                backEnd.pc.c_totalIndexes += numIndexes;
                backEnd.pc.c_dlightIndexes += numIndexes;
                
                // Ridah, overdraw lights several times, rather than sending
                //	multiple lights through
                for( i = 0; i < dl->overdraw; i++ )
                {
                    R_DrawElements( numIndexes, hitIndexes );
                    backEnd.pc.c_totalIndexes += numIndexes;
                    backEnd.pc.c_dlightIndexes += numIndexes;
                }
                /*
                				} else {	// optimize for multitexture
                
                					GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
                
                					// setup each TMU (use all available TMU's)
                					for (tmu=0; tmu<glConfig.maxActiveTextures && tmu<(dl->overdraw+1); tmu++) {
                						GL_SelectTexture( tmu );
                						if (tmu) {
                							glEnable( GL_TEXTURE_2D );
                							GL_TexEnv( GL_ADD );
                							GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
                							glEnableClientState( GL_TEXTURE_COORD_ARRAY );
                							glTexCoordPointer( 2, GL_FLOAT, 0, texCoordsArray[0] );
                							glEnableClientState( GL_COLOR_ARRAY );
                							glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorArray );
                						}
                						GL_Bind( tr.dlightImage );
                					}
                
                					// draw each bundle
                					for(i=0; i<(dl->overdraw+1); i+=glConfig.maxActiveTextures)
                					{
                						// make sure we dont draw with too many TMU's
                						if (i+glConfig.maxActiveTextures>(dl->overdraw+1)) {
                							for (tmu=0; tmu<glConfig.maxActiveTextures; tmu++) {
                								if (tmu+i>=(dl->overdraw+1)) {
                									GL_SelectTexture( tmu );
                									glDisable( GL_TEXTURE_2D );
                								}
                							}
                						}
                						// draw the elements
                						R_DrawElements( numIndexes, hitIndexes );
                						backEnd.pc.c_totalIndexes += numIndexes;
                						backEnd.pc.c_dlightIndexes += numIndexes;
                					}
                
                					// turn off unused TMU's
                					for (tmu=1; tmu<glConfig.maxActiveTextures; tmu++) {
                						// set back to default state
                						GL_SelectTexture( tmu );
                						glDisable( GL_TEXTURE_2D );
                					}
                
                					// return to TEXTURE0
                					GL_SelectTexture( 0 );
                				}
                */
                R_FogOn();
            }
        }
    }
}


/*
===================
RB_FogPass

Blends a fog texture on top of everything else
===================
*/
static void RB_FogPass( void )
{
    fog_t*       fog;
    int i;
    
    if( tr.refdef.rdflags & RDF_SNOOPERVIEW )    // no fog pass in snooper
    {
        return;
    }
    
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, tess.svars.colors );
    
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 0, tess.svars.texcoords[0] );
    
    fog = tr.world->fogs + tess.fogNum;
    
    for( i = 0; i < tess.numVertexes; i++ )
    {
        *( int* )&tess.svars.colors[i] = fog->colorInt;
    }
    
    RB_CalcFogTexCoords( ( float* ) tess.svars.texcoords[0] );
    
    GL_Bind( tr.fogImage );
    
    if( tess.shader->fogPass == FP_EQUAL )
    {
        GL_State( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL );
    }
    else
    {
        GL_State( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA );
    }
    
    R_DrawElements( tess.numIndexes, tess.indexes );
}

/*
===============
ComputeColors
===============
*/
static void ComputeColors( shaderStage_t* pStage )
{
    int i;
    
    //
    // rgbGen
    //
    switch( pStage->rgbGen )
    {
        case CGEN_IDENTITY:
            memset( tess.svars.colors, 0xff, tess.numVertexes * 4 );
            break;
        default:
        case CGEN_IDENTITY_LIGHTING:
            memset( tess.svars.colors, tr.identityLightByte, tess.numVertexes * 4 );
            break;
        case CGEN_LIGHTING_DIFFUSE:
            RB_CalcDiffuseColor( ( unsigned char* ) tess.svars.colors );
            break;
        case CGEN_EXACT_VERTEX:
            memcpy( tess.svars.colors, tess.vertexColors, tess.numVertexes * sizeof( tess.vertexColors[0] ) );
            break;
        case CGEN_CONST:
            for( i = 0; i < tess.numVertexes; i++ )
            {
                *( int* )tess.svars.colors[i] = *( int* )pStage->constantColor;
            }
            break;
        case CGEN_VERTEX:
            if( tr.identityLight == 1 )
            {
                memcpy( tess.svars.colors, tess.vertexColors, tess.numVertexes * sizeof( tess.vertexColors[0] ) );
            }
            else
            {
                for( i = 0; i < tess.numVertexes; i++ )
                {
                    tess.svars.colors[i][0] = tess.vertexColors[i][0] * tr.identityLight;
                    tess.svars.colors[i][1] = tess.vertexColors[i][1] * tr.identityLight;
                    tess.svars.colors[i][2] = tess.vertexColors[i][2] * tr.identityLight;
                    tess.svars.colors[i][3] = tess.vertexColors[i][3];
                }
            }
            break;
        case CGEN_ONE_MINUS_VERTEX:
            if( tr.identityLight == 1 )
            {
                for( i = 0; i < tess.numVertexes; i++ )
                {
                    tess.svars.colors[i][0] = 255 - tess.vertexColors[i][0];
                    tess.svars.colors[i][1] = 255 - tess.vertexColors[i][1];
                    tess.svars.colors[i][2] = 255 - tess.vertexColors[i][2];
                }
            }
            else
            {
                for( i = 0; i < tess.numVertexes; i++ )
                {
                    tess.svars.colors[i][0] = ( 255 - tess.vertexColors[i][0] ) * tr.identityLight;
                    tess.svars.colors[i][1] = ( 255 - tess.vertexColors[i][1] ) * tr.identityLight;
                    tess.svars.colors[i][2] = ( 255 - tess.vertexColors[i][2] ) * tr.identityLight;
                }
            }
            break;
        case CGEN_FOG:
        {
            fog_t*       fog;
            
            fog = tr.world->fogs + tess.fogNum;
            
            for( i = 0; i < tess.numVertexes; i++ )
            {
                *( int* )&tess.svars.colors[i] = fog->colorInt;
            }
        }
        break;
        case CGEN_WAVEFORM:
            RB_CalcWaveColor( &pStage->rgbWave, ( unsigned char* ) tess.svars.colors );
            break;
        case CGEN_ENTITY:
            RB_CalcColorFromEntity( ( unsigned char* ) tess.svars.colors );
            break;
        case CGEN_ONE_MINUS_ENTITY:
            RB_CalcColorFromOneMinusEntity( ( unsigned char* ) tess.svars.colors );
            break;
    }
    
    //
    // alphaGen
    //
    switch( pStage->alphaGen )
    {
        case AGEN_SKIP:
            break;
        case AGEN_IDENTITY:
            if( pStage->rgbGen != CGEN_IDENTITY )
            {
                if( ( pStage->rgbGen == CGEN_VERTEX && tr.identityLight != 1 ) ||
                        pStage->rgbGen != CGEN_VERTEX )
                {
                    for( i = 0; i < tess.numVertexes; i++ )
                    {
                        tess.svars.colors[i][3] = 0xff;
                    }
                }
            }
            break;
        case AGEN_CONST:
            if( pStage->rgbGen != CGEN_CONST )
            {
                for( i = 0; i < tess.numVertexes; i++ )
                {
                    tess.svars.colors[i][3] = pStage->constantColor[3];
                }
            }
            break;
        case AGEN_WAVEFORM:
            RB_CalcWaveAlpha( &pStage->alphaWave, ( unsigned char* ) tess.svars.colors );
            break;
        case AGEN_LIGHTING_SPECULAR:
            RB_CalcSpecularAlpha( ( unsigned char* ) tess.svars.colors );
            break;
        case AGEN_ENTITY:
            RB_CalcAlphaFromEntity( ( unsigned char* ) tess.svars.colors );
            break;
        case AGEN_ONE_MINUS_ENTITY:
            RB_CalcAlphaFromOneMinusEntity( ( unsigned char* ) tess.svars.colors );
            break;
            // Ridah
        case AGEN_NORMALZFADE:
        {
            float alpha, range, lowest, highest, dot;
            vec3_t worldUp;
            bool zombieEffect = false;
            
            if( VectorCompare( backEnd.currentEntity->e.fireRiseDir, vec3_origin ) )
            {
                VectorSet( backEnd.currentEntity->e.fireRiseDir, 0, 0, 1 );
            }
            
            if( backEnd.currentEntity->e.hModel )       // world surfaces dont have an axis
            {
                VectorRotate( backEnd.currentEntity->e.fireRiseDir, backEnd.currentEntity->e.axis, worldUp );
            }
            else
            {
                VectorCopy( backEnd.currentEntity->e.fireRiseDir, worldUp );
            }
            
            lowest = pStage->zFadeBounds[0];
            if( lowest == -1000 )       // use entity alpha
            {
                lowest = backEnd.currentEntity->e.shaderTime;
                zombieEffect = true;
            }
            highest = pStage->zFadeBounds[1];
            if( highest == -1000 )      // use entity alpha
            {
                highest = backEnd.currentEntity->e.shaderTime;
                zombieEffect = true;
            }
            range = highest - lowest;
            for( i = 0; i < tess.numVertexes; i++ )
            {
                dot = DotProduct( tess.normals[i], worldUp );
                
                // special handling for Zombie fade effect
                if( zombieEffect )
                {
                    alpha = ( float )backEnd.currentEntity->e.shaderRGBA[3] * ( dot + 1.0 ) / 2.0;
                    alpha += ( 2.0 * ( float )backEnd.currentEntity->e.shaderRGBA[3] ) * ( 1.0 - ( dot + 1.0 ) / 2.0 );
                    if( alpha > 255.0 )
                    {
                        alpha = 255.0;
                    }
                    else if( alpha < 0.0 )
                    {
                        alpha = 0.0;
                    }
                    tess.svars.colors[i][3] = ( byte )( alpha );
                    continue;
                }
                
                if( dot < highest )
                {
                    if( dot > lowest )
                    {
                        if( dot < lowest + range / 2 )
                        {
                            alpha = ( ( float )pStage->constantColor[3] * ( ( dot - lowest ) / ( range / 2 ) ) );
                        }
                        else
                        {
                            alpha = ( ( float )pStage->constantColor[3] * ( 1.0 - ( ( dot - lowest - range / 2 ) / ( range / 2 ) ) ) );
                        }
                        if( alpha > 255.0 )
                        {
                            alpha = 255.0;
                        }
                        else if( alpha < 0.0 )
                        {
                            alpha = 0.0;
                        }
                        
                        // finally, scale according to the entity's alpha
                        if( backEnd.currentEntity->e.hModel )
                        {
                            alpha *= ( float )backEnd.currentEntity->e.shaderRGBA[3] / 255.0;
                        }
                        
                        tess.svars.colors[i][3] = ( byte )( alpha );
                    }
                    else
                    {
                        tess.svars.colors[i][3] = 0;
                    }
                }
                else
                {
                    tess.svars.colors[i][3] = 0;
                }
            }
        }
        break;
        // done.
        case AGEN_VERTEX:
            if( pStage->rgbGen != CGEN_VERTEX )
            {
                for( i = 0; i < tess.numVertexes; i++ )
                {
                    tess.svars.colors[i][3] = tess.vertexColors[i][3];
                }
            }
            break;
        case AGEN_ONE_MINUS_VERTEX:
            for( i = 0; i < tess.numVertexes; i++ )
            {
                tess.svars.colors[i][3] = 255 - tess.vertexColors[i][3];
            }
            break;
        case AGEN_PORTAL:
        {
            unsigned char alpha;
            
            for( i = 0; i < tess.numVertexes; i++ )
            {
                float len;
                vec3_t v;
                
                VectorSubtract( tess.xyz[i], backEnd.viewParms.or.origin, v );
                len = VectorLength( v );
                
                len /= tess.shader->portalRange;
                
                if( len < 0 )
                {
                    alpha = 0;
                }
                else if( len > 1 )
                {
                    alpha = 0xff;
                }
                else
                {
                    alpha = len * 0xff;
                }
                
                tess.svars.colors[i][3] = alpha;
            }
        }
        break;
    }
    
    //
    // fog adjustment for colors to fade out as fog increases
    //
    if( tess.fogNum )
    {
        switch( pStage->adjustColorsForFog )
        {
            case ACFF_MODULATE_RGB:
                RB_CalcModulateColorsByFog( ( unsigned char* ) tess.svars.colors );
                break;
            case ACFF_MODULATE_ALPHA:
                RB_CalcModulateAlphasByFog( ( unsigned char* ) tess.svars.colors );
                break;
            case ACFF_MODULATE_RGBA:
                RB_CalcModulateRGBAsByFog( ( unsigned char* ) tess.svars.colors );
                break;
            case ACFF_NONE:
                break;
        }
    }
}

/*
===============
ComputeTexCoords
===============
*/
static void ComputeTexCoords( shaderStage_t* pStage )
{
    int i;
    int b;
    
    for( b = 0; b < NUM_TEXTURE_BUNDLES; b++ )
    {
        int tm;
        
        //
        // generate the texture coordinates
        //
        switch( pStage->bundle[b].tcGen )
        {
            case TCGEN_IDENTITY:
                memset( tess.svars.texcoords[b], 0, sizeof( float ) * 2 * tess.numVertexes );
                break;
            case TCGEN_TEXTURE:
                for( i = 0 ; i < tess.numVertexes ; i++ )
                {
                    tess.svars.texcoords[b][i][0] = tess.texCoords[i][0][0];
                    tess.svars.texcoords[b][i][1] = tess.texCoords[i][0][1];
                }
                break;
            case TCGEN_LIGHTMAP:
                for( i = 0 ; i < tess.numVertexes ; i++ )
                {
                    tess.svars.texcoords[b][i][0] = tess.texCoords[i][1][0];
                    tess.svars.texcoords[b][i][1] = tess.texCoords[i][1][1];
                }
                break;
            case TCGEN_VECTOR:
                for( i = 0 ; i < tess.numVertexes ; i++ )
                {
                    tess.svars.texcoords[b][i][0] = DotProduct( tess.xyz[i], pStage->bundle[b].tcGenVectors[0] );
                    tess.svars.texcoords[b][i][1] = DotProduct( tess.xyz[i], pStage->bundle[b].tcGenVectors[1] );
                }
                break;
            case TCGEN_FOG:
                RB_CalcFogTexCoords( ( float* ) tess.svars.texcoords[b] );
                break;
            case TCGEN_ENVIRONMENT_MAPPED:
                RB_CalcEnvironmentTexCoords( ( float* ) tess.svars.texcoords[b] );
                break;
            case TCGEN_FIRERISEENV_MAPPED:
                RB_CalcFireRiseEnvTexCoords( ( float* ) tess.svars.texcoords[b] );
                break;
            case TCGEN_BAD:
                return;
        }
        
        //
        // alter texture coordinates
        //
        for( tm = 0; tm < pStage->bundle[b].numTexMods ; tm++ )
        {
            switch( pStage->bundle[b].texMods[tm].type )
            {
                case TMOD_NONE:
                    tm = TR_MAX_TEXMODS;        // break out of for loop
                    break;
                    
                case TMOD_SWAP:
                    RB_CalcSwapTexCoords( ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_TURBULENT:
                    RB_CalcTurbulentTexCoords( &pStage->bundle[b].texMods[tm].wave,
                                               ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_ENTITY_TRANSLATE:
                    RB_CalcScrollTexCoords( backEnd.currentEntity->e.shaderTexCoord,
                                            ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_SCROLL:
                    RB_CalcScrollTexCoords( pStage->bundle[b].texMods[tm].scroll,
                                            ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_SCALE:
                    RB_CalcScaleTexCoords( pStage->bundle[b].texMods[tm].scale,
                                           ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_STRETCH:
                    RB_CalcStretchTexCoords( &pStage->bundle[b].texMods[tm].wave,
                                             ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_TRANSFORM:
                    RB_CalcTransformTexCoords( &pStage->bundle[b].texMods[tm],
                                               ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                case TMOD_ROTATE:
                    RB_CalcRotateTexCoords( pStage->bundle[b].texMods[tm].rotateSpeed,
                                            ( float* ) tess.svars.texcoords[b] );
                    break;
                    
                default:
                    Com_Error( ERR_DROP, "ERROR: unknown texmod '%d' in shader '%s'\n", pStage->bundle[b].texMods[tm].type, tess.shader->name );
                    break;
            }
        }
    }
}



extern void R_Fog( glfog_t* curfog );

/*
==============
SetIteratorFog
	set the fog parameters for this pass
==============
*/
void SetIteratorFog( void )
{
    // changed for problem when you start the game with r_fastsky set to '1'
//	if(r_fastsky->integer || backEnd.refdef.rdflags & RDF_NOWORLDMODEL ) {
    if( backEnd.refdef.rdflags & RDF_NOWORLDMODEL )
    {
        R_FogOff();
        return;
    }
    
    if( backEnd.refdef.rdflags & RDF_DRAWINGSKY )
    {
        if( glfogsettings[FOG_SKY].registered )
        {
            R_Fog( &glfogsettings[FOG_SKY] );
        }
        else
        {
            R_FogOff();
        }
        
        return;
    }
    
    if( skyboxportal && backEnd.refdef.rdflags & RDF_SKYBOXPORTAL )
    {
        if( glfogsettings[FOG_PORTALVIEW].registered )
        {
            R_Fog( &glfogsettings[FOG_PORTALVIEW] );
        }
        else
        {
            R_FogOff();
        }
    }
    else
    {
        if( glfogNum > FOG_NONE )
        {
            R_Fog( &glfogsettings[FOG_CURRENT] );
        }
        else
        {
            R_FogOff();
        }
    }
}


/*
** RB_IterateStagesGeneric
*/
static void RB_IterateStagesGeneric( shaderCommands_t* input )
{
    int stage;
    
    for( stage = 0; stage < MAX_SHADER_STAGES; stage++ )
    {
        shaderStage_t* pStage = tess.xstages[stage];
        
        if( !pStage )
        {
            break;
        }
        
        ComputeColors( pStage );
        ComputeTexCoords( pStage );
        
        if( !setArraysOnce )
        {
            glEnableClientState( GL_COLOR_ARRAY );
            glColorPointer( 4, GL_UNSIGNED_BYTE, 0, input->svars.colors );
        }
        
        //
        // do multitexture
        //
        if( pStage->bundle[1].image[0] != 0 )
        {
            DrawMultitextured( input, stage );
        }
        else
        {
            int fadeStart, fadeEnd;
            
            if( !setArraysOnce )
            {
                glTexCoordPointer( 2, GL_FLOAT, 0, input->svars.texcoords[0] );
            }
            
            //
            // set state
            //
            if( pStage->bundle[0].vertexLightmap && ( ( r_vertexLight->integer && !r_uiFullScreen->integer ) || glConfig.hardwareType == GLHW_PERMEDIA2 ) && r_lightmap->integer )
            {
                GL_Bind( tr.whiteImage );
            }
            else
            {
                R_BindAnimatedImage( &pStage->bundle[0] );
            }
            
            // Ridah, per stage fogging (detail textures)
            if( tess.shader->noFog && pStage->isFogged )
            {
                R_FogOn();
            }
            else if( tess.shader->noFog && !pStage->isFogged )
            {
                R_FogOff(); // turn it back off
            }
            else        // make sure it's on
            {
                R_FogOn();
            }
            // done.
            
            //----(SA)	fading model stuff
            fadeStart = backEnd.currentEntity->e.fadeStartTime;
            
            if( fadeStart )
            {
                fadeEnd = backEnd.currentEntity->e.fadeEndTime;
                if( fadeStart > tr.refdef.time )          // has not started to fade yet
                {
                    GL_State( pStage->stateBits );
                }
                else
                {
                    int i;
                    unsigned int tempState;
                    float alphaval;
                    
                    if( fadeEnd < tr.refdef.time )        // entity faded out completely
                    {
                        continue;
                    }
                    
                    alphaval = ( float )( fadeEnd - tr.refdef.time ) / ( float )( fadeEnd - fadeStart );
                    
                    tempState = pStage->stateBits;
                    // remove the current blend, and don't write to Z buffer
                    tempState &= ~( GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS | GLS_DEPTHMASK_TRUE );
                    // set the blend to src_alpha, dst_one_minus_src_alpha
                    tempState |= ( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA );
                    GL_State( tempState );
                    GL_Cull( CT_FRONT_SIDED );
                    // modulate the alpha component of each vertex in the render list
                    for( i = 0; i < tess.numVertexes; i++ )
                    {
                        tess.svars.colors[i][0] *= alphaval;
                        tess.svars.colors[i][1] *= alphaval;
                        tess.svars.colors[i][2] *= alphaval;
                        tess.svars.colors[i][3] *= alphaval;
                    }
                }
            }
            else
            {
                GL_State( pStage->stateBits );
            }
            //----(SA)	end
            
            //
            // draw
            //
            R_DrawElements( input->numIndexes, input->indexes );
        }
        // allow skipping out to show just lightmaps during development
        if( r_lightmap->integer && ( pStage->bundle[0].isLightmap || pStage->bundle[1].isLightmap || pStage->bundle[0].vertexLightmap ) )
        {
            break;
        }
    }
}


/*
** RB_StageIteratorGeneric
*/
void RB_StageIteratorGeneric( void )
{
    shaderCommands_t* input;
    
    input = &tess;
    
    RB_DeformTessGeometry();
    
    //
    // log this call
    //
    if( r_logFile->integer )
    {
        // don't just call LogComment, or we will get
        // a call to va() every frame!
        GLimp_LogComment( va( "--- RB_StageIteratorGeneric( %s ) ---\n", tess.shader->name ) );
    }
    
    // set GL fog
    SetIteratorFog();
    
    //
    // set face culling appropriately
    //
    GL_Cull( input->shader->cullType );
    
    // set polygon offset if necessary
    if( input->shader->polygonOffset )
    {
        glEnable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( r_offsetFactor->value, r_offsetUnits->value );
    }
    
    //
    // if there is only a single pass then we can enable color
    // and texture arrays before we compile, otherwise we need
    // to avoid compiling those arrays since they will change
    // during multipass rendering
    //
    if( tess.numPasses > 1 || input->shader->multitextureEnv )
    {
        setArraysOnce = false;
        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    }
    else
    {
        setArraysOnce = true;
        
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, tess.svars.colors );
        
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, tess.svars.texcoords[0] );
    }
    
    // RF, send normals only if required
    // This must be done first, since we can't change the arrays once they have been
    // locked
    
    //
    // lock XYZ
    //
    glVertexPointer( 3, GL_FLOAT, 16, input->xyz ); // padded for SIMD
    if( glLockArraysEXT )
    {
        glLockArraysEXT( 0, input->numVertexes );
        GLimp_LogComment( "glLockArraysEXT\n" );
    }
    
    //
    // enable color and texcoord arrays after the lock if necessary
    //
    if( !setArraysOnce )
    {
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
    }
    
    //
    // call shader function
    //
    RB_IterateStagesGeneric( input );
    
    //
    // now do any dynamic lighting needed
    //
    if( tess.dlightBits && tess.shader->sort <= SS_OPAQUE
            && !( tess.shader->surfaceFlags & ( SURF_NODLIGHT | SURF_SKY ) ) )
    {
        ProjectDlightTexture();
    }
    
    //
    // now do fog
    //
    if( tess.fogNum && tess.shader->fogPass )
    {
        RB_FogPass();
    }
    
    //
    // unlock arrays
    //
    if( glUnlockArraysEXT )
    {
        glUnlockArraysEXT();
        GLimp_LogComment( "glUnlockArraysEXT\n" );
    }
    
    //
    // reset polygon offset
    //
    if( input->shader->polygonOffset )
    {
        glDisable( GL_POLYGON_OFFSET_FILL );
    }
    
    // turn truform back off
    
}


/*
** RB_StageIteratorVertexLitTexture
*/
void RB_StageIteratorVertexLitTexture( void )
{
    shaderCommands_t* input;
    shader_t*        shader;
    
    input = &tess;
    
    shader = input->shader;
    
    //
    // compute colors
    //
    RB_CalcDiffuseColor( ( unsigned char* ) tess.svars.colors );
    
    //
    // log this call
    //
    if( r_logFile->integer )
    {
        // don't just call LogComment, or we will get
        // a call to va() every frame!
        GLimp_LogComment( va( "--- RB_StageIteratorVertexLitTexturedUnfogged( %s ) ---\n", tess.shader->name ) );
    }
    
    
    // set GL fog
    SetIteratorFog();
    
    //
    // set face culling appropriately
    //
    GL_Cull( input->shader->cullType );
    
    //
    // set arrays and lock
    //
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, tess.svars.colors );
    glTexCoordPointer( 2, GL_FLOAT, 16, tess.texCoords[0][0] );
    glVertexPointer( 3, GL_FLOAT, 16, input->xyz );
    
    
    if( glLockArraysEXT )
    {
        glLockArraysEXT( 0, input->numVertexes );
        GLimp_LogComment( "glLockArraysEXT\n" );
    }
    
    //
    // call special shade routine
    //
    R_BindAnimatedImage( &tess.xstages[0]->bundle[0] );
    GL_State( tess.xstages[0]->stateBits );
    R_DrawElements( input->numIndexes, input->indexes );
    
    //
    // now do any dynamic lighting needed
    //
    if( tess.dlightBits && tess.shader->sort <= SS_OPAQUE )
    {
        ProjectDlightTexture();
    }
    
    //
    // now do fog
    //
    if( tess.fogNum && tess.shader->fogPass )
    {
        RB_FogPass();
    }
    
    //
    // unlock arrays
    //
    if( glUnlockArraysEXT )
    {
        glUnlockArraysEXT();
        GLimp_LogComment( "glUnlockArraysEXT\n" );
    }
    
}

//define	REPLACE_MODE

void RB_StageIteratorLightmappedMultitexture( void )
{
    shaderCommands_t* input;
    
    input = &tess;
    
    //
    // log this call
    //
    if( r_logFile->integer )
    {
        // don't just call LogComment, or we will get
        // a call to va() every frame!
        GLimp_LogComment( va( "--- RB_StageIteratorLightmappedMultitexture( %s ) ---\n", tess.shader->name ) );
    }
    
    // set GL fog
    SetIteratorFog();
    
    //
    // set face culling appropriately
    //
    GL_Cull( input->shader->cullType );
    
    //
    // set color, pointers, and lock
    //
    GL_State( GLS_DEFAULT );
    glVertexPointer( 3, GL_FLOAT, 16, input->xyz );
    
#ifdef REPLACE_MODE
    glDisableClientState( GL_COLOR_ARRAY );
    glColor3f( 1, 1, 1 );
    glShadeModel( GL_FLAT );
#else
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, tess.constantColor255 );
#endif
    
    //
    // select base stage
    //
    GL_SelectTexture( 0 );
    
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    R_BindAnimatedImage( &tess.xstages[0]->bundle[0] );
    glTexCoordPointer( 2, GL_FLOAT, 16, tess.texCoords[0][0] );
    
    //
    // configure second stage
    //
    GL_SelectTexture( 1 );
    glEnable( GL_TEXTURE_2D );
    if( r_lightmap->integer )
    {
        GL_TexEnv( GL_REPLACE );
    }
    else
    {
        GL_TexEnv( GL_MODULATE );
    }
    
//----(SA)	modified for snooper
    if( tess.xstages[0]->bundle[1].isLightmap && ( backEnd.refdef.rdflags & RDF_SNOOPERVIEW ) )
    {
        GL_Bind( tr.whiteImage );
    }
    else
    {
        R_BindAnimatedImage( &tess.xstages[0]->bundle[1] );
    }
    
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 16, tess.texCoords[0][1] );
    
    //
    // lock arrays
    //
    if( glLockArraysEXT )
    {
        glLockArraysEXT( 0, input->numVertexes );
        GLimp_LogComment( "glLockArraysEXT\n" );
    }
    
    R_DrawElements( input->numIndexes, input->indexes );
    
    //
    // disable texturing on TEXTURE1, then select TEXTURE0
    //
    glDisable( GL_TEXTURE_2D );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    
    GL_SelectTexture( 0 );
#ifdef REPLACE_MODE
    GL_TexEnv( GL_MODULATE );
    glShadeModel( GL_SMOOTH );
#endif
    
    //
    // now do any dynamic lighting needed
    //
    if( tess.dlightBits && tess.shader->sort <= SS_OPAQUE )
    {
        ProjectDlightTexture();
    }
    
    //
    // now do fog
    //
    if( tess.fogNum && tess.shader->fogPass )
    {
        RB_FogPass();
    }
    
    //
    // unlock arrays
    //
    if( glUnlockArraysEXT )
    {
        glUnlockArraysEXT();
        GLimp_LogComment( "glUnlockArraysEXT\n" );
    }
    
}

void R_CalcTBNForTriangle( vec3_t tangent, vec3_t binormal, vec3_t normal, const vec3_t v1, const vec3_t v2, const vec3_t v3, const vec2_t t1, const vec2_t t2, const vec2_t t3 )
{
    vec3_t			v2v1;
    vec3_t			v3v1;
    
    float			c2c1_T;
    float			c2c1_B;
    
    float			c3c1_T;
    float			c3c1_B;
    
    float			denominator;
    float			scale1, scale2;
    
    vec3_t			T, B, N, C;
    
    
    // Calculate the tangent basis for each vertex of the triangle
    // UPDATE: In the 3rd edition of the accompanying article, the for-loop located here has
    // been removed as it was redundant (the entire TBN matrix was calculated three times
    // instead of just one).
    //
    // Please note, that this function relies on the fact that the input geometry are triangles
    // and the tangent basis for each vertex thus is identical!
    //
    
    // Calculate the vectors from the current vertex to the two other vertices in the triangle
    VectorSubtract( v2, v1, v2v1 );
    VectorSubtract( v3, v1, v3v1 );
    
    // The equation presented in the article states that:
    // c2c1_T = V2.texcoord.x � V1.texcoord.x
    // c2c1_B = V2.texcoord.y � V1.texcoord.y
    // c3c1_T = V3.texcoord.x � V1.texcoord.x
    // c3c1_B = V3.texcoord.y � V1.texcoord.y
    
    // Calculate c2c1_T and c2c1_B
    c2c1_T = t2[0] - t1[0];
    c2c1_B = t2[1] - t2[1];
    
    // Calculate c3c1_T and c3c1_B
    c3c1_T = t3[0] - t1[0];
    c3c1_B = t3[1] - t1[1];
    
    denominator = c2c1_T * c3c1_B - c3c1_T * c2c1_B;
    //if(ROUNDOFF(fDenominator) == 0.0f)
    if( denominator == 0.0f )
    {
        // We won't risk a divide by zero, so set the tangent matrix to the identity matrix
        VectorSet( tangent, 1, 0, 0 );
        VectorSet( binormal, 0, 1, 0 );
        VectorSet( normal, 0, 0, 1 );
    }
    else
    {
        // Calculate the reciprocal value once and for all (to achieve speed)
        scale1 = 1.0f / denominator;
        
        // T and B are calculated just as the equation in the article states
        VectorSet( T, ( c3c1_B * v2v1[0] - c2c1_B * v3v1[0] ) * scale1,
                   ( c3c1_B * v2v1[1] - c2c1_B * v3v1[1] ) * scale1,
                   ( c3c1_B * v2v1[2] - c2c1_B * v3v1[2] ) * scale1 );
                   
        VectorSet( B, ( -c3c1_T * v2v1[0] + c2c1_T * v3v1[0] ) * scale1,
                   ( -c3c1_T * v2v1[1] + c2c1_T * v3v1[1] ) * scale1,
                   ( -c3c1_T * v2v1[2] + c2c1_T * v3v1[2] ) * scale1 );
                   
        // The normal N is calculated as the cross product between T and B
        CrossProduct( T, B, N );
        
#if 0
        VectorCopy( T, tangent );
        VectorCopy( B, binormal );
        VectorCopy( N, normal );
#else
        // Calculate the reciprocal value once and for all (to achieve speed)
        scale2 = 1.0f / ( ( T[0] * B[1] * N[2] - T[2] * B[1] * N[0] ) +
                          ( B[0] * N[1] * T[2] - B[2] * N[1] * T[0] ) +
                          ( N[0] * T[1] * B[2] - N[2] * T[1] * B[0] ) );
        
        // Calculate the inverse if the TBN matrix using the formula described in the article.
        // We store the basis vectors directly in the provided TBN matrix: pvTBNMatrix
        CrossProduct( B, N, C );
        tangent[0] = C[0] * scale2;
        CrossProduct( N, T, C );
        tangent[1] = -C[0] * scale2;
        CrossProduct( T, B, C );
        tangent[2] = C[0] * scale2;
        VectorNormalize( tangent );
        
        CrossProduct( B, N, C );
        binormal[0] = -C[1] * scale2;
        CrossProduct( N, T, C );
        binormal[1] = C[1] * scale2;
        CrossProduct( T, B, C );
        binormal[2] = -C[1] * scale2;
        VectorNormalize( binormal );
        
        CrossProduct( B, N, C );
        normal[0] = C[2] * scale2;
        CrossProduct( N, T, C );
        normal[1] = -C[2] * scale2;
        CrossProduct( T, B, C );
        normal[2] = C[2] * scale2;
        VectorNormalize( normal );
#endif
    }
}

//
// RB_CalculateTBNForTess
// We need to create a new struct so we can precalc the TBN matrix.
//
void RB_CalculateTBNForTess( void )
{
    int indexes[3];
    for( int i = 0; i < tess.numIndexes; i += 3 )
    {
        indexes[0] = tess.indexes[i + 0];
        indexes[1] = tess.indexes[i + 1];
        indexes[2] = tess.indexes[i + 2];
        vec3_t tangent, binormal, normal;
        
        R_CalcTBNForTriangle( tangent, binormal, normal, tess.xyz[indexes[0]], tess.xyz[indexes[1]], tess.xyz[indexes[2]],
                              tess.texCoords[indexes[0]][0], tess.texCoords[indexes[1]][0], tess.texCoords[indexes[2]][0] );
                              
        for( int c = 0; c < 3; c++ )
        {
            VectorCopy( tangent, tess.tangents[indexes[c]] );
            VectorCopy( binormal, tess.binormals[indexes[c]] );
            VectorCopy( normal, tess.normals[indexes[c]] );
        }
    }
}

/*
** RB_EndSurface
*/
void RB_EndSurface( void )
{
    shaderCommands_t* input;
    
    input = &tess;
    
    if( input->numIndexes == 0 )
    {
        return;
    }
    
    if( input->indexes[SHADER_MAX_INDEXES - 1] != 0 )
    {
        Com_Error( ERR_DROP, "RB_EndSurface() - SHADER_MAX_INDEXES hit" );
    }
    if( input->xyz[SHADER_MAX_VERTEXES - 1][0] != 0 )
    {
        Com_Error( ERR_DROP, "RB_EndSurface() - SHADER_MAX_VERTEXES hit" );
    }
    
    if( tess.shader == tr.shadowShader )
    {
        RB_ShadowTessEnd();
        return;
    }
    
    // for debugging of sort order issues, stop rendering after a given sort value
    if( r_debugSort->integer && r_debugSort->integer < tess.shader->sort )
    {
        return;
    }
    
    if( skyboxportal )
    {
        // world
        if( !( backEnd.refdef.rdflags & RDF_SKYBOXPORTAL ) )
        {
            if( tess.currentStageIteratorFunc == RB_StageIteratorSky )     // don't process these tris at all
            {
                return;
            }
        }
        // portal sky
        else
        {
            if( !drawskyboxportal )
            {
                if( !( tess.currentStageIteratorFunc == RB_StageIteratorSky ) )     // /only/ process sky tris
                {
                    return;
                }
            }
        }
    }
    
    //
    // update performance counters
    //
    backEnd.pc.c_shaders++;
    backEnd.pc.c_vertexes += tess.numVertexes;
    backEnd.pc.c_indexes += tess.numIndexes;
    backEnd.pc.c_totalIndexes += tess.numIndexes * tess.numPasses;
    
    // Update the Tangent matrix if needed.
    //
    if( tess.shader->updateTangentMatrix )
    {
        RB_CalculateTBNForTess();
    }
    
    //
    // call off to shader specific tess end function
    //
    tess.currentStageIteratorFunc();
    
    //
    // draw debugging stuff
    //
    if( r_showtris->integer )
    {
        DrawTris( input );
    }
    if( r_shownormals->integer )
    {
        DrawNormals( input );
    }
    
    
    // clear shader so we can tell we don't have any unclosed surfaces
    tess.numIndexes = 0;
    
    GLimp_LogComment( "----------\n" );
}

