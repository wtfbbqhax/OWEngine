﻿//////////////////////////////////////////////////////////////////////////////////////
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
//  File name:   r_main.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: main control flow for each frame
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "r_local.h"

trGlobals_t tr;

static float s_flipMatrix[16] =
{
    // convert from our coordinate system (looking down X)
    // to OpenGL's coordinate system (looking down -Z)
    0, 0, -1, 0,
    -1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1
};

idRenderSystemLocal	renderSystemLocal;
idRenderSystem*	renderSystem = &renderSystemLocal;

// entities that will have procedurally generated surfaces will just
// point at this for their sorting surface
surfaceType_t entitySurface = SF_ENTITY;

// fog stuff
glfog_t glfogsettings[NUM_FOGS];
glfogType_t glfogNum = FOG_NONE;
bool fogIsOn = false;


/*
=================
R_Fog (void)
=================
*/
void R_Fog( glfog_t* curfog )
{
    static glfog_t setfog;
    
    if( !r_wolffog->integer )
    {
        R_FogOff();
        return;
    }
    
    if( !curfog->registered )      //----(SA)
    {
        R_FogOff();
        return;
    }
    
    //----(SA) assme values of '0' for these parameters means 'use default'
    if( !curfog->density )
    {
        curfog->density = 1;
    }
    if( !curfog->hint )
    {
        curfog->hint = GL_DONT_CARE;
    }
    if( !curfog->mode )
    {
        curfog->mode = GL_LINEAR;
    }
    //----(SA)	end
    
    
    R_FogOn();
    
    // TODO: FIXME: only send changes if necessary
    
//	if(curfog->mode != setfog.mode || !setfog.registered) {
    glFogf( GL_FOG_MODE, curfog->mode );
//		setfog.mode = curfog->mode;
//	}
//	if(curfog->color[0] != setfog.color[0] || curfog->color[1] != setfog.color[1] || curfog->color[2] != setfog.color[2] || !setfog.registered) {
    glFogfv( GL_FOG_COLOR, curfog->color );
//		VectorCopy(setfog.color, curfog->color);
//	}
//	if(curfog->density != setfog.density || !setfog.registered) {
    glFogf( GL_FOG_DENSITY, curfog->density );
//		setfog.density = curfog->density;
//	}
//	if(curfog->hint != setfog.hint || !setfog.registered) {
    glHint( GL_FOG_HINT, curfog->hint );
//		setfog.hint = curfog->hint;
//	}
//	if(curfog->start != setfog.start || !setfog.registered) {
    if( backEnd.refdef.rdflags & RDF_SNOOPERVIEW )
    {
        glFogf( GL_FOG_START, curfog->end );       // snooper starts GL fog out further
    }
    else
    {
        glFogf( GL_FOG_START, curfog->start );
    }
//		setfog.start = curfog->start;
//	}

    if( r_zfar->value )                // (SA) allow override for helping level designers test fog distances
    {
//		if(setfog.end != r_zfar->value || !setfog.registered) {
        glFogf( GL_FOG_END, r_zfar->value );
//			setfog.end = r_zfar->value;
//		}
    }
    else
    {
//		if(curfog->end != setfog.end || !setfog.registered) {
        if( backEnd.refdef.rdflags & RDF_SNOOPERVIEW )
        {
            glFogf( GL_FOG_END, curfog->end + 1000 );      // snooper ends GL fog out further.  this works fine with our maps, but could be 'funky' with later maps
        }
//				glFogf (GL_FOG_END, curfog->end);
        else
        {
            glFogf( GL_FOG_END, curfog->end );
        }
//			setfog.end = curfog->end;
//		}
    }
    
    
//----(SA)	added
    // NV fog mode
    
//----(SA)	end

    setfog.registered = true;
    
    glClearColor( curfog->color[0], curfog->color[1], curfog->color[2], curfog->color[3] );
    
    
}

// Ridah, allow disabling fog temporarily
void R_FogOff( void )
{
    if( !fogIsOn )
    {
        return;
    }
    glDisable( GL_FOG );
    fogIsOn = false;
}

void R_FogOn( void )
{
    if( fogIsOn )
    {
        return;
    }
    
//	if(r_uiFullScreen->integer) {	// don't fog in the menu
//		R_FogOff();
//		return;
//	}

    if( backEnd.projection2D )     // no fog in 2d
    {
        R_FogOff();
        return;
    }
    
    if( !r_wolffog->integer )
    {
        return;
    }
    
//	if(backEnd.viewParms.isGLFogged) {
//		if(!(backEnd.viewParms.glFog.registered))
//			return;
//	}

    if( backEnd.refdef.rdflags & RDF_SKYBOXPORTAL )    // don't force world fog on portal sky
    {
        if( !( glfogsettings[FOG_PORTALVIEW].registered ) )
        {
            return;
        }
    }
    else if( !glfogNum )
    {
        return;
    }
    
    glEnable( GL_FOG );
    fogIsOn = true;
}
// done.



//----(SA)
/*
==============
idRenderSystemLocal::SetFog

  if fogvar == FOG_CMD_SWITCHFOG {
	fogvar is the command
	var1 is the fog to switch to
	var2 is the time to transition
  }
  else {
	fogvar is the fog that's being set
	var1 is the near fog z value
	var2 is the far fog z value
	rgb = color
	density is density, and is used to derive the values of 'mode', 'drawsky', and 'clearscreen'
  }
==============
*/
void idRenderSystemLocal::SetFog( int fogvar, int var1, int var2, float r, float g, float b, float density )
{
    if( fogvar != FOG_CMD_SWITCHFOG )      // just set the parameters and return
    {
    
        if( var1 == 0 && var2 == 0 )       // clear this fog
        {
            glfogsettings[fogvar].registered = false;
            return;
        }
        
        glfogsettings[fogvar].color[0]      = r;
        glfogsettings[fogvar].color[1]      = g;
        glfogsettings[fogvar].color[2]      = b;
        glfogsettings[fogvar].color[3]      = 1;
        glfogsettings[fogvar].start         = var1;
        glfogsettings[fogvar].end           = var2;
        if( density >= 1 )
        {
            glfogsettings[fogvar].mode          = GL_LINEAR;
            glfogsettings[fogvar].drawsky       = false;
            glfogsettings[fogvar].clearscreen   = true;
            glfogsettings[fogvar].density       = 1.0;
        }
        else
        {
            glfogsettings[fogvar].mode          = GL_EXP;
            glfogsettings[fogvar].drawsky       = true;
            glfogsettings[fogvar].clearscreen   = false;
            glfogsettings[fogvar].density       = density;
        }
        glfogsettings[fogvar].hint          = GL_DONT_CARE;
        glfogsettings[fogvar].registered    = true;
        
        return;
    }
    
    // FOG_MAP now used to mean 'no fog'
    if( var1 == FOG_MAP )
    {
    
        // transitioning from...
        if( glfogsettings[FOG_CURRENT].registered )
        {
            memcpy( &glfogsettings[FOG_LAST], &glfogsettings[FOG_CURRENT], sizeof( glfog_t ) );
        }
        
        memcpy( &glfogsettings[FOG_TARGET], &glfogsettings[glfogNum], sizeof( glfog_t ) );
        
        
        // clear, clear, clear
        memset( &glfogsettings[FOG_MAP], 0, sizeof( glfog_t ) );
//		memset(&glfogsettings[FOG_CURRENT], 0, sizeof(glfog_t));
        memset( &glfogsettings[FOG_TARGET], 0, sizeof( glfog_t ) );
//		glfogsettings[FOG_CURRENT].registered = false;
//		glfogsettings[FOG_TARGET].registered = false;
        glfogNum = FOG_NONE;
        return;
    }
    
    // don't switch to invalid fogs
    if( glfogsettings[var1].registered != true )
    {
        return;
    }
    
    glfogNum = ( glfogType_t )var1;
    
    // transitioning to new fog, store the current values as the 'from'
    
    if( glfogsettings[FOG_CURRENT].registered )
    {
        memcpy( &glfogsettings[FOG_LAST], &glfogsettings[FOG_CURRENT], sizeof( glfog_t ) );
    }
    else
    {
        // if no current fog fall back to world fog
        // FIXME: handle transition if there is no FOG_MAP fog
//		memcpy(&glfogsettings[FOG_LAST], &glfogsettings[FOG_MAP], sizeof(glfog_t));
        memcpy( &glfogsettings[FOG_LAST], &glfogsettings[glfogNum], sizeof( glfog_t ) );
    }
    
    memcpy( &glfogsettings[FOG_TARGET], &glfogsettings[glfogNum], sizeof( glfog_t ) );
    
    if( !var2 )    // instant
    {
        glfogsettings[FOG_TARGET].startTime = 0;
        glfogsettings[FOG_TARGET].finishTime = 0;
        glfogsettings[FOG_TARGET].dirty = 1;
        glfogsettings[FOG_CURRENT].dirty = 1;
    }
    else
    {
        // setup transition times
        glfogsettings[FOG_TARGET].startTime = tr.refdef.time;
        glfogsettings[FOG_TARGET].finishTime = tr.refdef.time + var2;
    }
}

//----(SA) end

/*
=============
R_CalcTangentSpace
Tr3B - recoded from Nvidia's SDK
=============
*/
void R_CalcTangentSpace( vec3_t tangent, vec3_t binormal, vec3_t normal,
                         const vec3_t v0, const vec3_t v1, const vec3_t v2,
                         const vec2_t t0, const vec2_t t1, const vec2_t t2,
                         const vec3_t n )
{
#if 1
    vec3_t cp, e0, e1;
    VectorSet( e0, v1[0] - v0[0], t1[0] - t0[0], t1[1] - t0[1] );
    VectorSet( e1, v2[0] - v0[0], t2[0] - t0[0], t2[1] - t0[1] );
    
    CrossProduct( e0, e1, cp );
    
    if( Q_fabs( cp[0] ) > 10e-6 )
    {
        tangent[0] = -cp[1] / cp[0];
        binormal[0] = -cp[2] / cp[0];
    }
    
    e0[0] = v1[1] - v0[1];
    e1[0] = v2[1] - v0[1];
    
    CrossProduct( e0, e1, cp );
    if( Q_fabs( cp[0] ) > 10e-6 )
    {
        tangent[1] = -cp[1] / cp[0];
        binormal[1] = -cp[2] / cp[0];
    }
    
    e0[0] = v1[2] - v0[2];
    e1[0] = v2[2] - v0[2];
    
    CrossProduct( e0, e1, cp );
    if( Q_fabs( cp[0] ) > 10e-6 )
    {
        tangent[2] = -cp[1] / cp[0];
        binormal[2] = -cp[2] / cp[0];
    }
    
    VectorNormalizeFast( tangent );
    VectorNormalizeFast( binormal );
    
    // normal...
    // compute the cross product TxB
    CrossProduct( tangent, binormal, normal );
    VectorNormalizeFast( normal );
    
    // Gram-Schmidt orthogonalization process for B
    // compute the cross product B=NxT to obtain
    // an orthogonal basis
    CrossProduct( normal, tangent, binormal );
    
    if( DotProduct( normal, n ) < 0 )
    {
        VectorInverse( normal );
    }
#else
    
    // other solution
    // http://members.rogers.com/deseric/tangentspace.htm
    
    int		i;
    vec3_t	planes[3];
    vec3_t	e0;
    vec3_t	e1;
    for( i = 0; i < 3; i++ )
    {
        VectorSet( e0, v1[i] - v0[i], t1[0] - t0[0], t1[1] - t0[1] );
        VectorSet( e1, v2[i] - v0[i], t2[0] - t0[0], t2[1] - t0[1] );
    
        CrossProduct( e0, e1, planes[i] );
    }
    
    //So your tangent space will be defined by this :
    //Normal = Normal of the triangle or Tangent X Binormal (careful with the cross product,
    // you have to make sure the normal points in the right direction)
    //Tangent = ( dp(Fx(s,t)) / ds,  dp(Fy(s,t)) / ds, dp(Fz(s,t)) / ds )   or     ( -Bx/Ax, -By/Ay, - Bz/Az )
    //Binormal =  ( dp(Fx(s,t)) / dt,  dp(Fy(s,t)) / dt, dp(Fz(s,t)) / dt )  or     ( -Cx/Ax, -Cy/Ay, -Cz/Az )
    
    // tangent...
    tangent[0] = -planes[0][1] / planes[0][0];
    tangent[1] = -planes[1][1] / planes[1][0];
    tangent[2] = -planes[2][1] / planes[2][0];
    VectorNormalizeFast( tangent );
    
    // binormal...
    binormal[0] = -planes[0][2] / planes[0][0];
    binormal[1] = -planes[1][2] / planes[1][0];
    binormal[2] = -planes[2][2] / planes[2][0];
    VectorNormalizeFast( binormal );
    
#if 1
    // normal...
    // compute the cross product TxB
    CrossProduct( tangent, binormal, normal );
    VectorNormalizeFast( normal );
    
    // Gram-Schmidt orthogonalization process for B
    // compute the cross product B=NxT to obtain
    // an orthogonal basis
    CrossProduct( normal, tangent, binormal );
    
    if( DotProduct( normal, n ) < 0 )
    {
        VectorInverse( normal );
    }
#else
    VectorCopy( n, normal );
#endif
    
#endif
}

/*
=================
R_CullLocalBox

Returns CULL_IN, CULL_CLIP, or CULL_OUT
=================
*/
int R_CullLocalBox( vec3_t bounds[2] )
{
    int i, j;
    vec3_t transformed[8];
    float dists[8];
    vec3_t v;
    cplane_t*    frust;
    int anyBack;
    int front, back;
    
    if( r_nocull->integer )
    {
        return CULL_CLIP;
    }
    
    // transform into world space
    for( i = 0 ; i < 8 ; i++ )
    {
        v[0] = bounds[i & 1][0];
        v[1] = bounds[( i >> 1 ) & 1][1];
        v[2] = bounds[( i >> 2 ) & 1][2];
        
        VectorCopy( tr.or.origin, transformed[i] );
        VectorMA( transformed[i], v[0], tr.or.axis[0], transformed[i] );
        VectorMA( transformed[i], v[1], tr.or.axis[1], transformed[i] );
        VectorMA( transformed[i], v[2], tr.or.axis[2], transformed[i] );
    }
    
    // check against frustum planes
    anyBack = 0;
    for( i = 0 ; i < 4 ; i++ )
    {
        frust = &tr.viewParms.frustum[i];
        
        front = back = 0;
        for( j = 0 ; j < 8 ; j++ )
        {
            dists[j] = DotProduct( transformed[j], frust->normal );
            if( dists[j] > frust->dist )
            {
                front = 1;
                if( back )
                {
                    break;      // a point is in front
                }
            }
            else
            {
                back = 1;
            }
        }
        if( !front )
        {
            // all points were behind one of the planes
            return CULL_OUT;
        }
        anyBack |= back;
    }
    
    if( !anyBack )
    {
        return CULL_IN;     // completely inside frustum
    }
    
    return CULL_CLIP;       // partially clipped
}

/*
** R_CullLocalPointAndRadius
*/
int R_CullLocalPointAndRadius( vec3_t pt, float radius )
{
    vec3_t transformed;
    
    R_LocalPointToWorld( pt, transformed );
    
    return R_CullPointAndRadius( transformed, radius );
}

/*
** R_CullPointAndRadius
*/
int R_CullPointAndRadius( vec3_t pt, float radius )
{
    int i;
    float dist;
    cplane_t*    frust;
    bool mightBeClipped = false;
    
    if( r_nocull->integer )
    {
        return CULL_CLIP;
    }
    
    // check against frustum planes
    for( i = 0 ; i < 4 ; i++ )
    {
        frust = &tr.viewParms.frustum[i];
        
        dist = DotProduct( pt, frust->normal ) - frust->dist;
        if( dist < -radius )
        {
            return CULL_OUT;
        }
        else if( dist <= radius )
        {
            mightBeClipped = true;
        }
    }
    
    if( mightBeClipped )
    {
        return CULL_CLIP;
    }
    
    return CULL_IN;     // completely inside frustum
}


/*
=================
R_LocalNormalToWorld

=================
*/
void R_LocalNormalToWorld( vec3_t local, vec3_t world )
{
    world[0] = local[0] * tr.or.axis[0][0] + local[1] * tr.or.axis[1][0] + local[2] * tr.or.axis[2][0];
    world[1] = local[0] * tr.or.axis[0][1] + local[1] * tr.or.axis[1][1] + local[2] * tr.or.axis[2][1];
    world[2] = local[0] * tr.or.axis[0][2] + local[1] * tr.or.axis[1][2] + local[2] * tr.or.axis[2][2];
}

/*
=================
R_LocalPointToWorld

=================
*/
void R_LocalPointToWorld( vec3_t local, vec3_t world )
{
    world[0] = local[0] * tr.or.axis[0][0] + local[1] * tr.or.axis[1][0] + local[2] * tr.or.axis[2][0] + tr.or.origin[0];
    world[1] = local[0] * tr.or.axis[0][1] + local[1] * tr.or.axis[1][1] + local[2] * tr.or.axis[2][1] + tr.or.origin[1];
    world[2] = local[0] * tr.or.axis[0][2] + local[1] * tr.or.axis[1][2] + local[2] * tr.or.axis[2][2] + tr.or.origin[2];
}

/*
=================
R_WorldToLocal

=================
*/
void R_WorldToLocal( vec3_t world, vec3_t local )
{
    local[0] = DotProduct( world, tr.or.axis[0] );
    local[1] = DotProduct( world, tr.or.axis[1] );
    local[2] = DotProduct( world, tr.or.axis[2] );
}

/*
==========================
R_TransformModelToClip

==========================
*/
void R_TransformModelToClip( const vec3_t src, const float* modelMatrix, const float* projectionMatrix,
                             vec4_t eye, vec4_t dst )
{
    int i;
    
    for( i = 0 ; i < 4 ; i++ )
    {
        eye[i] =
            src[0] * modelMatrix[ i + 0 * 4 ] +
            src[1] * modelMatrix[ i + 1 * 4 ] +
            src[2] * modelMatrix[ i + 2 * 4 ] +
            1 * modelMatrix[ i + 3 * 4 ];
    }
    
    for( i = 0 ; i < 4 ; i++ )
    {
        dst[i] =
            eye[0] * projectionMatrix[ i + 0 * 4 ] +
            eye[1] * projectionMatrix[ i + 1 * 4 ] +
            eye[2] * projectionMatrix[ i + 2 * 4 ] +
            eye[3] * projectionMatrix[ i + 3 * 4 ];
    }
}

/*
==========================
R_TransformClipToWindow

==========================
*/
void R_TransformClipToWindow( const vec4_t clip, const viewParms_t* view, vec4_t normalized, vec4_t window )
{
    normalized[0] = clip[0] / clip[3];
    normalized[1] = clip[1] / clip[3];
    normalized[2] = ( clip[2] + clip[3] ) / ( 2 * clip[3] );
    
    window[0] = 0.5f * ( 1.0f + normalized[0] ) * view->viewportWidth;
    window[1] = 0.5f * ( 1.0f + normalized[1] ) * view->viewportHeight;
    window[2] = normalized[2];
    
    window[0] = ( int )( window[0] + 0.5 );
    window[1] = ( int )( window[1] + 0.5 );
}


/*
==========================
myGlMultMatrix

==========================
*/
void myGlMultMatrix( const float* a, const float* b, float* out )
{
    int i, j;
    
    for( i = 0 ; i < 4 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            out[ i * 4 + j ] =
                a [ i * 4 + 0 ] * b [ 0 * 4 + j ]
                + a [ i * 4 + 1 ] * b [ 1 * 4 + j ]
                + a [ i * 4 + 2 ] * b [ 2 * 4 + j ]
                + a [ i * 4 + 3 ] * b [ 3 * 4 + j ];
        }
    }
}

/*
=================
R_RotateForEntity

Generates an orientation for an entity and viewParms
Does NOT produce any GL calls
Called by both the front end and the back end
=================
*/
void R_RotateForEntity( const trRefEntity_t* ent, const viewParms_t* viewParms,
                        orientationr_t* or )
{
    float glMatrix[16];
    vec3_t delta;
    float axisLength;
    
    if( ent->e.reType != RT_MODEL )
    {
        * or = viewParms->world;
        return;
    }
    
    VectorCopy( ent->e.origin, or ->origin );
    
    VectorCopy( ent->e.axis[0], or ->axis[0] );
    VectorCopy( ent->e.axis[1], or ->axis[1] );
    VectorCopy( ent->e.axis[2], or ->axis[2] );
    
    glMatrix[0] = or ->axis[0][0];
    glMatrix[4] = or ->axis[1][0];
    glMatrix[8] = or ->axis[2][0];
    glMatrix[12] = or ->origin[0];
    
    glMatrix[1] = or ->axis[0][1];
    glMatrix[5] = or ->axis[1][1];
    glMatrix[9] = or ->axis[2][1];
    glMatrix[13] = or ->origin[1];
    
    glMatrix[2] = or ->axis[0][2];
    glMatrix[6] = or ->axis[1][2];
    glMatrix[10] = or ->axis[2][2];
    glMatrix[14] = or ->origin[2];
    
    glMatrix[3] = 0;
    glMatrix[7] = 0;
    glMatrix[11] = 0;
    glMatrix[15] = 1;
    
    myGlMultMatrix( glMatrix, viewParms->world.modelMatrix, or ->modelMatrix );
    
    // calculate the viewer origin in the model's space
    // needed for fog, specular, and environment mapping
    VectorSubtract( viewParms->or.origin, or ->origin, delta );
    
    // compensate for scale in the axes if necessary
    if( ent->e.nonNormalizedAxes )
    {
        axisLength = VectorLength( ent->e.axis[0] );
        if( !axisLength )
        {
            axisLength = 0;
        }
        else
        {
            axisLength = 1.0f / axisLength;
        }
    }
    else
    {
        axisLength = 1.0f;
    }
    
    or ->viewOrigin[0] = DotProduct( delta, or ->axis[0] ) * axisLength;
    or ->viewOrigin[1] = DotProduct( delta, or ->axis[1] ) * axisLength;
    or ->viewOrigin[2] = DotProduct( delta, or ->axis[2] ) * axisLength;
}

/*
=================
R_RotateForViewer

Sets up the modelview matrix for a given viewParm
=================
*/
void R_RotateForViewer( viewParms_t* dest )
{
    float viewerMatrix[16];
    float viewerTranslate[16];
    float glviewMatrix[16];
    vec3_t origin;
    
    memset( &tr.or, 0, sizeof( tr.or ) );
    tr.or.axis[0][0] = 1;
    tr.or.axis[1][1] = 1;
    tr.or.axis[2][2] = 1;
    VectorCopy( tr.viewParms.or.origin, tr.or.viewOrigin );
    
    // transform by the camera placement
    VectorCopy( tr.viewParms.or.origin, origin );
    
    viewerMatrix[0] = tr.viewParms.or.axis[0][0];
    viewerMatrix[4] = tr.viewParms.or.axis[0][1];
    viewerMatrix[8] = tr.viewParms.or.axis[0][2];
    viewerMatrix[12] = -origin[0] * viewerMatrix[0] + - origin[1] * viewerMatrix[4] + - origin[2] * viewerMatrix[8];
    
    viewerMatrix[1] = tr.viewParms.or.axis[1][0];
    viewerMatrix[5] = tr.viewParms.or.axis[1][1];
    viewerMatrix[9] = tr.viewParms.or.axis[1][2];
    viewerMatrix[13] = -origin[0] * viewerMatrix[1] + - origin[1] * viewerMatrix[5] + -origin[2] * viewerMatrix[9];
    
    viewerMatrix[2] = tr.viewParms.or.axis[2][0];
    viewerMatrix[6] = tr.viewParms.or.axis[2][1];
    viewerMatrix[10] = tr.viewParms.or.axis[2][2];
    viewerMatrix[14] = -origin[0] * viewerMatrix[2] + - origin[1] * viewerMatrix[6] + -origin[2] * viewerMatrix[10];
    
    viewerMatrix[3] = 0;
    viewerMatrix[7] = 0;
    viewerMatrix[11] = 0;
    viewerMatrix[15] = 1;
    
    // convert from our coordinate system (looking down X)
    // to OpenGL's coordinate system (looking down -Z)
    myGlMultMatrix( viewerMatrix, s_flipMatrix, glviewMatrix ); //tr.or.modelMatrix );
    
    viewerTranslate[0] = 1.0;
    viewerTranslate[1] = 0.0;
    viewerTranslate[2] = 0.0;
    viewerTranslate[3] = 0.0;
    
    viewerTranslate[4] = 0.0;
    viewerTranslate[5] = 1.0;
    viewerTranslate[6] = 0.0;
    viewerTranslate[7] = 0.0;
    
    viewerTranslate[8] = 0.0;
    viewerTranslate[9] = 0.0;
    viewerTranslate[10] = 1.0;
    viewerTranslate[11] = 0.0;
    
    viewerTranslate[12] = 0.5 * HMD.InterpupillaryDistance;
    if( dest != NULL && dest->stereoFrame == STEREO_RIGHT )
    {
        viewerTranslate[12] *= -1.0;
    }
    viewerTranslate[13] = 0.0;
    viewerTranslate[14] = 0.0;
    viewerTranslate[15] = 1.0;
    
    myGlMultMatrix( viewerTranslate, glviewMatrix, tr.or.modelMatrix );
    
    tr.viewParms.world = tr.or;
}



/*
==============
R_SetFrameFog
==============
*/
void R_SetFrameFog( void )
{

    if( r_speeds->integer == 5 )
    {
        if( !glfogsettings[FOG_TARGET].registered )
        {
            Com_Printf( "no fog - calc zFar: %0.1f\n", tr.viewParms.zFar );
            return;
        }
    }
    
    // still fading
    if( glfogsettings[FOG_TARGET].finishTime && glfogsettings[FOG_TARGET].finishTime >= tr.refdef.time )
    {
        float lerpPos;
        int fadeTime;
        
        // transitioning from density to distance
        if( glfogsettings[FOG_LAST].mode == GL_EXP && glfogsettings[FOG_TARGET].mode == GL_LINEAR )
        {
            // for now just fast transition to the target when dissimilar fogs are
            memcpy( &glfogsettings[FOG_CURRENT], &glfogsettings[FOG_TARGET], sizeof( glfog_t ) );
            glfogsettings[FOG_TARGET].finishTime = 0;
        }
        // transitioning from distance to density
        else if( glfogsettings[FOG_LAST].mode == GL_LINEAR && glfogsettings[FOG_TARGET].mode == GL_EXP )
        {
            memcpy( &glfogsettings[FOG_CURRENT], &glfogsettings[FOG_TARGET], sizeof( glfog_t ) );
            glfogsettings[FOG_TARGET].finishTime = 0;
        }
        // transitioning like fog modes
        else
        {
        
            fadeTime = glfogsettings[FOG_TARGET].finishTime - glfogsettings[FOG_TARGET].startTime;
            if( fadeTime <= 0 )
            {
                fadeTime = 1;   // avoid divide by zero
                
                
            }
            lerpPos = ( float )( tr.refdef.time - glfogsettings[FOG_TARGET].startTime ) / ( float )fadeTime;
            if( lerpPos > 1 )
            {
                lerpPos = 1;
            }
            
            // lerp near/far
            glfogsettings[FOG_CURRENT].start        = glfogsettings[FOG_LAST].start + ( ( glfogsettings[FOG_TARGET].start - glfogsettings[FOG_LAST].start ) * lerpPos );
            glfogsettings[FOG_CURRENT].end          = glfogsettings[FOG_LAST].end + ( ( glfogsettings[FOG_TARGET].end - glfogsettings[FOG_LAST].end ) * lerpPos );
            
            // lerp color
            glfogsettings[FOG_CURRENT].color[0]     = glfogsettings[FOG_LAST].color[0] + ( ( glfogsettings[FOG_TARGET].color[0] - glfogsettings[FOG_LAST].color[0] ) * lerpPos );
            glfogsettings[FOG_CURRENT].color[1]     = glfogsettings[FOG_LAST].color[1] + ( ( glfogsettings[FOG_TARGET].color[1] - glfogsettings[FOG_LAST].color[1] ) * lerpPos );
            glfogsettings[FOG_CURRENT].color[2]     = glfogsettings[FOG_LAST].color[2] + ( ( glfogsettings[FOG_TARGET].color[2] - glfogsettings[FOG_LAST].color[2] ) * lerpPos );
            
            glfogsettings[FOG_CURRENT].density      = glfogsettings[FOG_TARGET].density;
            glfogsettings[FOG_CURRENT].mode         = glfogsettings[FOG_TARGET].mode;
            glfogsettings[FOG_CURRENT].registered   = true;
            
            // if either fog in the transition clears the screen, clear the background this frame to avoid hall of mirrors
            glfogsettings[FOG_CURRENT].clearscreen  = ( glfogsettings[FOG_TARGET].clearscreen || glfogsettings[FOG_LAST].clearscreen );
        }
        
        glfogsettings[FOG_CURRENT].dirty = 1;
    }
    else
    {
        // potential FIXME: since this is the most common occurance, diff first and only set changes
//		if(glfogsettings[FOG_CURRENT].dirty) {
        memcpy( &glfogsettings[FOG_CURRENT], &glfogsettings[FOG_TARGET], sizeof( glfog_t ) );
        glfogsettings[FOG_CURRENT].dirty = 0;
//		}
    }
    
    
    // shorten the far clip if the fog opaque distance is closer than the procedural farcip dist
    
    if( glfogsettings[FOG_CURRENT].mode == GL_LINEAR )
    {
        if( glfogsettings[FOG_CURRENT].end < tr.viewParms.zFar )
        {
            tr.viewParms.zFar = glfogsettings[FOG_CURRENT].end;
        }
        if( backEnd.refdef.rdflags & RDF_SNOOPERVIEW )
        {
            tr.viewParms.zFar += 1000;  // zfar out slightly further for snooper.  this works fine with our maps, but could be 'funky' with later maps
            
        }
    }
//	else
//		glfogsettings[FOG_CURRENT].end = 5;


    if( r_speeds->integer == 5 )
    {
        if( glfogsettings[FOG_CURRENT].mode == GL_LINEAR )
        {
            Com_Printf( "farclip fog - den: %0.1f  calc zFar: %0.1f  fog zfar: %0.1f\n", glfogsettings[FOG_CURRENT].density, tr.viewParms.zFar, glfogsettings[FOG_CURRENT].end );
        }
        else
        {
            Com_Printf( "density fog - den: %0.6f  calc zFar: %0.1f  fog zFar: %0.1f\n", glfogsettings[FOG_CURRENT].density, tr.viewParms.zFar, glfogsettings[FOG_CURRENT].end );
        }
    }
}


/*
==============
SetFarClip
==============
*/
static void SetFarClip( void )
{
    float farthestCornerDistance = 0;
    int i;
    
    // if not rendering the world (icons, menus, etc)
    // set a 2k far clip plane
    if( tr.refdef.rdflags & RDF_NOWORLDMODEL )
    {
        tr.viewParms.zFar = 2048;
        return;
    }
    
    //----(SA)	this lets you use r_zfar from the command line to experiment with different
    //			distances, but setting it back to 0 uses the map (or procedurally generated) default
    if( r_zfar->value )
    {
    
        tr.viewParms.zFar = r_zfar->integer;
        R_SetFrameFog();
        
        if( r_speeds->integer == 5 )
        {
            Com_Printf( "r_zfar value forcing farclip at: %f\n", tr.viewParms.zFar );
        }
        
        return;
    }
    
    //
    // set far clipping planes dynamically
    //
    farthestCornerDistance = 0;
    for( i = 0; i < 8; i++ )
    {
        vec3_t v;
        vec3_t vecTo;
        float distance;
        
        if( i & 1 )
        {
            v[0] = tr.viewParms.visBounds[0][0];
        }
        else
        {
            v[0] = tr.viewParms.visBounds[1][0];
        }
        
        if( i & 2 )
        {
            v[1] = tr.viewParms.visBounds[0][1];
        }
        else
        {
            v[1] = tr.viewParms.visBounds[1][1];
        }
        
        if( i & 4 )
        {
            v[2] = tr.viewParms.visBounds[0][2];
        }
        else
        {
            v[2] = tr.viewParms.visBounds[1][2];
        }
        
        VectorSubtract( v, tr.viewParms.or.origin, vecTo );
        
        distance = vecTo[0] * vecTo[0] + vecTo[1] * vecTo[1] + vecTo[2] * vecTo[2];
        
        if( distance > farthestCornerDistance )
        {
            farthestCornerDistance = distance;
        }
    }
    
    tr.viewParms.zFar = sqrt( farthestCornerDistance );
    R_SetFrameFog();
}


/*
===============
R_SetupProjection
===============
*/
void R_SetupProjection( void )
{
    float xmin, xmax, ymin, ymax;
    float width, height, depth;
#if !defined( __ANDROID__ )
    float stereoSep = r_stereoSeparation->value;
    float frameMultiplier = 1;
    float a = ( float )HMD.HResolution / ( 2.0 * ( float )HMD.VResolution );
    float fov = 2.0 * atan( ( float )HMD.VScreenSize /
                            ( 2.0 * ( float )HMD.EyeToScreenDistance ) ) - ovr_fovoffset->value;
#endif
    float zFar = 1000.0;
    float zNear = 0.001;
    
    // dynamically compute far clip plane distance
    SetFarClip();
    
    //
    // set up projection matrix
    //
    zNear   = r_znear->value;
    if( r_zfar->value )
    {
        zFar = r_zfar->value;   // (SA) allow override for helping level designers test fog distances
    }
    else
    {
        zFar = tr.viewParms.zFar;
    }
    
    ymax = zNear * tan( tr.refdef.fov_y * M_PI / 360.0f );
    ymin = -ymax;
    
    xmax = zNear * tan( tr.refdef.fov_x * M_PI / 360.0f );
    xmin = -xmax;
    
    width = xmax - xmin;
    height = ymax - ymin;
    depth = zFar - zNear;
    
    tr.viewParms.projectionMatrix[0] = 2 * zNear / width;
    tr.viewParms.projectionMatrix[4] = 0;
    tr.viewParms.projectionMatrix[8] = ( xmax + xmin ) / width; // normally 0
    tr.viewParms.projectionMatrix[12] = 0;
    
    tr.viewParms.projectionMatrix[1] = 0;
    tr.viewParms.projectionMatrix[5] = 2 * zNear / height;
    tr.viewParms.projectionMatrix[9] = ( ymax + ymin ) / height;    // normally 0
    tr.viewParms.projectionMatrix[13] = 0;
    
    tr.viewParms.projectionMatrix[2] = 0;
    tr.viewParms.projectionMatrix[6] = 0;
    tr.viewParms.projectionMatrix[10] = -( zFar + zNear ) / depth;
    tr.viewParms.projectionMatrix[14] = -2 * zFar * zNear / depth;
    
    tr.viewParms.projectionMatrix[3] = 0;
    tr.viewParms.projectionMatrix[7] = 0;
    tr.viewParms.projectionMatrix[11] = -1;
    tr.viewParms.projectionMatrix[15] = 0;
    
#if !defined( __ANDROID__ )
    {
        float projectionMatrix[16];
        float H[16];
        float hMeters;
        float h;
        
        hMeters = ( ( float )HMD.HScreenSize / 4.0 ) - ( HMD.InterpupillaryDistance / 2.0 );
        h = 4.0 * hMeters / ( float )HMD.HScreenSize;
        
        projectionMatrix[0] = 1.0 / ( a * tan( fov / 2 ) );
        projectionMatrix[1] = 0;
        projectionMatrix[2] = 0;
        projectionMatrix[3] = 0;
        
        projectionMatrix[4] = 0;
        projectionMatrix[5] = 1.0 / ( tan( fov / 2 ) );
        projectionMatrix[6] = 0;
        projectionMatrix[7] = 0;
        
        projectionMatrix[8] = 0;
        projectionMatrix[9] = 0;
        projectionMatrix[10] = zFar / ( zNear - zFar );
        projectionMatrix[11] = -1.0;
        
        projectionMatrix[12] = 0;
        projectionMatrix[13] = 0;
        projectionMatrix[14] = ( zFar * zNear ) / ( zNear - zFar );
        projectionMatrix[15] = 0;
        
        
        H[0] = 1.0;
        H[1] = 0.0;
        H[2] = 0.0;
        H[3] = 0.0;
        
        H[4] = 0.0;
        H[5] = 1.0;
        H[6] = 0.0;
        H[7] = 0.0;
        
        H[8] = 0.0;
        H[9] = 0.0;
        H[10] = 1.0;
        H[11] = 0.0;
        
        H[12] = h;
        H[13] = 0.0;
        H[14] = 0.0;
        H[15] = 1.0;
        
    }
#endif
}

/*
=================
R_SetupFrustum

Setup that culling frustum planes for the current view
=================
*/
void R_SetupFrustum( void )
{
    int i;
    float xs, xc;
    float ang;
    
    ang = tr.viewParms.fovX / 180 * M_PI * 0.5f;
    xs = sin( ang );
    xc = cos( ang );
    
    VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[0].normal );
    VectorMA( tr.viewParms.frustum[0].normal, xc, tr.viewParms.or.axis[1], tr.viewParms.frustum[0].normal );
    
    VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[1].normal );
    VectorMA( tr.viewParms.frustum[1].normal, -xc, tr.viewParms.or.axis[1], tr.viewParms.frustum[1].normal );
    
    ang = tr.viewParms.fovY / 180 * M_PI * 0.5f;
    xs = sin( ang );
    xc = cos( ang );
    
    VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[2].normal );
    VectorMA( tr.viewParms.frustum[2].normal, xc, tr.viewParms.or.axis[2], tr.viewParms.frustum[2].normal );
    
    VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[3].normal );
    VectorMA( tr.viewParms.frustum[3].normal, -xc, tr.viewParms.or.axis[2], tr.viewParms.frustum[3].normal );
    
    for( i = 0 ; i < 4 ; i++ )
    {
        tr.viewParms.frustum[i].type = PLANE_NON_AXIAL;
        tr.viewParms.frustum[i].dist = DotProduct( tr.viewParms.or.origin, tr.viewParms.frustum[i].normal );
        SetPlaneSignbits( &tr.viewParms.frustum[i] );
    }
}


/*
=================
R_MirrorPoint
=================
*/
void R_MirrorPoint( vec3_t in, orientation_t* surface, orientation_t* camera, vec3_t out )
{
    int i;
    vec3_t local;
    vec3_t transformed;
    float d;
    
    VectorSubtract( in, surface->origin, local );
    
    VectorClear( transformed );
    for( i = 0 ; i < 3 ; i++ )
    {
        d = DotProduct( local, surface->axis[i] );
        VectorMA( transformed, d, camera->axis[i], transformed );
    }
    
    VectorAdd( transformed, camera->origin, out );
}

void R_MirrorVector( vec3_t in, orientation_t* surface, orientation_t* camera, vec3_t out )
{
    int i;
    float d;
    
    VectorClear( out );
    for( i = 0 ; i < 3 ; i++ )
    {
        d = DotProduct( in, surface->axis[i] );
        VectorMA( out, d, camera->axis[i], out );
    }
}


/*
=============
R_PlaneForSurface
=============
*/
void R_PlaneForSurface( surfaceType_t* surfType, cplane_t* plane )
{
    srfTriangles_t*  tri;
    srfPoly_t*       poly;
    srfVert_t*       v1, *v2, *v3;
    vec4_t plane4;
    
    if( !surfType )
    {
        memset( plane, 0, sizeof( *plane ) );
        plane->normal[0] = 1;
        return;
    }
    switch( *surfType )
    {
        case SF_FACE:
            *plane = ( ( srfSurfaceFace_t* )surfType )->plane;
            return;
        case SF_TRIANGLES:
            tri = ( srfTriangles_t* )surfType;
            v1 = tri->verts + tri->indexes[0];
            v2 = tri->verts + tri->indexes[1];
            v3 = tri->verts + tri->indexes[2];
            PlaneFromPoints( plane4, v1->xyz, v2->xyz, v3->xyz );
            VectorCopy( plane4, plane->normal );
            plane->dist = plane4[3];
            return;
        case SF_POLY:
            poly = ( srfPoly_t* )surfType;
            PlaneFromPoints( plane4, poly->verts[0].xyz, poly->verts[1].xyz, poly->verts[2].xyz );
            VectorCopy( plane4, plane->normal );
            plane->dist = plane4[3];
            return;
        default:
            memset( plane, 0, sizeof( *plane ) );
            plane->normal[0] = 1;
            return;
    }
}

/*
=================
R_GetPortalOrientation

entityNum is the entity that the portal surface is a part of, which may
be moving and rotating.

Returns true if it should be mirrored
=================
*/
bool R_GetPortalOrientations( drawSurf_t* drawSurf, int entityNum,
                              orientation_t* surface, orientation_t* camera,
                              vec3_t pvsOrigin, bool* mirror )
{
    int i;
    cplane_t originalPlane, plane;
    trRefEntity_t*   e;
    float d;
    vec3_t transformed;
    
    // create plane axis for the portal we are seeing
    R_PlaneForSurface( drawSurf->surface, &originalPlane );
    
    // rotate the plane if necessary
    if( entityNum != ENTITYNUM_WORLD )
    {
        tr.currentEntityNum = entityNum;
        tr.currentEntity = &tr.refdef.entities[entityNum];
        
        // get the orientation of the entity
        R_RotateForEntity( tr.currentEntity, &tr.viewParms, &tr.or );
        
        // rotate the plane, but keep the non-rotated version for matching
        // against the portalSurface entities
        R_LocalNormalToWorld( originalPlane.normal, plane.normal );
        plane.dist = originalPlane.dist + DotProduct( plane.normal, tr.or.origin );
        
        // translate the original plane
        originalPlane.dist = originalPlane.dist + DotProduct( originalPlane.normal, tr.or.origin );
    }
    else
    {
        plane = originalPlane;
    }
    
    VectorCopy( plane.normal, surface->axis[0] );
    PerpendicularVector( surface->axis[1], surface->axis[0] );
    CrossProduct( surface->axis[0], surface->axis[1], surface->axis[2] );
    
    // locate the portal entity closest to this plane.
    // origin will be the origin of the portal, origin2 will be
    // the origin of the camera
    for( i = 0 ; i < tr.refdef.num_entities ; i++ )
    {
        e = &tr.refdef.entities[i];
        if( e->e.reType != RT_PORTALSURFACE )
        {
            continue;
        }
        
        d = DotProduct( e->e.origin, originalPlane.normal ) - originalPlane.dist;
        if( d > 64 || d < -64 )
        {
            continue;
        }
        
        // get the pvsOrigin from the entity
        VectorCopy( e->e.oldorigin, pvsOrigin );
        
        // if the entity is just a mirror, don't use as a camera point
        if( e->e.oldorigin[0] == e->e.origin[0] &&
                e->e.oldorigin[1] == e->e.origin[1] &&
                e->e.oldorigin[2] == e->e.origin[2] )
        {
            VectorScale( plane.normal, plane.dist, surface->origin );
            VectorCopy( surface->origin, camera->origin );
            VectorSubtract( vec3_origin, surface->axis[0], camera->axis[0] );
            VectorCopy( surface->axis[1], camera->axis[1] );
            VectorCopy( surface->axis[2], camera->axis[2] );
            
            *mirror = true;
            return true;
        }
        
        // project the origin onto the surface plane to get
        // an origin point we can rotate around
        d = DotProduct( e->e.origin, plane.normal ) - plane.dist;
        VectorMA( e->e.origin, -d, surface->axis[0], surface->origin );
        
        // now get the camera origin and orientation
        VectorCopy( e->e.oldorigin, camera->origin );
        AxisCopy( e->e.axis, camera->axis );
        VectorSubtract( vec3_origin, camera->axis[0], camera->axis[0] );
        VectorSubtract( vec3_origin, camera->axis[1], camera->axis[1] );
        
        // optionally rotate
        if( e->e.oldframe )
        {
            // if a speed is specified
            if( e->e.frame )
            {
                // continuous rotate
                d = ( tr.refdef.time / 1000.0f ) * e->e.frame;
                VectorCopy( camera->axis[1], transformed );
                RotatePointAroundVector( camera->axis[1], camera->axis[0], transformed, d );
                CrossProduct( camera->axis[0], camera->axis[1], camera->axis[2] );
            }
            else
            {
                // bobbing rotate, with skinNum being the rotation offset
                d = sin( tr.refdef.time * 0.003f );
                d = e->e.skinNum + d * 4;
                VectorCopy( camera->axis[1], transformed );
                RotatePointAroundVector( camera->axis[1], camera->axis[0], transformed, d );
                CrossProduct( camera->axis[0], camera->axis[1], camera->axis[2] );
            }
        }
        else if( e->e.skinNum )
        {
            d = e->e.skinNum;
            VectorCopy( camera->axis[1], transformed );
            RotatePointAroundVector( camera->axis[1], camera->axis[0], transformed, d );
            CrossProduct( camera->axis[0], camera->axis[1], camera->axis[2] );
        }
        *mirror = false;
        return true;
    }
    
    // if we didn't locate a portal entity, don't render anything.
    // We don't want to just treat it as a mirror, because without a
    // portal entity the server won't have communicated a proper entity set
    // in the snapshot
    
    // unfortunately, with local movement prediction it is easily possible
    // to see a surface before the server has communicated the matching
    // portal surface entity, so we don't want to print anything here...
    
    //Com_Printf( "Portal surface without a portal entity\n" );
    
    return false;
}

static bool IsMirror( const drawSurf_t* drawSurf, int entityNum )
{
    int i;
    cplane_t originalPlane, plane;
    trRefEntity_t*   e;
    float d;
    
    // create plane axis for the portal we are seeing
    R_PlaneForSurface( drawSurf->surface, &originalPlane );
    
    // rotate the plane if necessary
    if( entityNum != ENTITYNUM_WORLD )
    {
        tr.currentEntityNum = entityNum;
        tr.currentEntity = &tr.refdef.entities[entityNum];
        
        // get the orientation of the entity
        R_RotateForEntity( tr.currentEntity, &tr.viewParms, &tr.or );
        
        // rotate the plane, but keep the non-rotated version for matching
        // against the portalSurface entities
        R_LocalNormalToWorld( originalPlane.normal, plane.normal );
        plane.dist = originalPlane.dist + DotProduct( plane.normal, tr.or.origin );
        
        // translate the original plane
        originalPlane.dist = originalPlane.dist + DotProduct( originalPlane.normal, tr.or.origin );
    }
    else
    {
        plane = originalPlane;
    }
    
    // locate the portal entity closest to this plane.
    // origin will be the origin of the portal, origin2 will be
    // the origin of the camera
    for( i = 0 ; i < tr.refdef.num_entities ; i++ )
    {
        e = &tr.refdef.entities[i];
        if( e->e.reType != RT_PORTALSURFACE )
        {
            continue;
        }
        
        d = DotProduct( e->e.origin, originalPlane.normal ) - originalPlane.dist;
        if( d > 64 || d < -64 )
        {
            continue;
        }
        
        // if the entity is just a mirror, don't use as a camera point
        if( e->e.oldorigin[0] == e->e.origin[0] &&
                e->e.oldorigin[1] == e->e.origin[1] &&
                e->e.oldorigin[2] == e->e.origin[2] )
        {
            return true;
        }
        
        return false;
    }
    return false;
}

/*
** SurfIsOffscreen
**
** Determines if a surface is completely offscreen.
*/
static bool SurfIsOffscreen( const drawSurf_t* drawSurf, vec4_t clipDest[128] )
{
    float shortest = 100000000;
    int entityNum;
    int numTriangles;
    shader_t* shader;
    int fogNum;
    int dlighted;
// GR - tessellation flag
    int atiTess;
    vec4_t clip, eye;
    int i;
    unsigned int pointOr = 0;
    unsigned int pointAnd = ( unsigned int )~0;
    
    if( glConfig.smpActive )        // FIXME!  we can't do RB_BeginSurface/RB_EndSurface stuff with smp!
    {
        return false;
    }
    
    R_RotateForViewer( NULL );
    
// GR - decompose with tessellation flag
    R_DecomposeSort( drawSurf->sort, &entityNum, &shader, &fogNum, &dlighted, &atiTess );
    RB_BeginSurface( shader, fogNum );
    rb_surfaceTable[ *drawSurf->surface ]( drawSurf->surface );
    
    assert( tess.numVertexes < 128 );
    
    for( i = 0; i < tess.numVertexes; i++ )
    {
        int j;
        unsigned int pointFlags = 0;
        
        R_TransformModelToClip( tess.xyz[i], tr.or.modelMatrix, tr.viewParms.projectionMatrix, eye, clip );
        
        for( j = 0; j < 3; j++ )
        {
            if( clip[j] >= clip[3] )
            {
                pointFlags |= ( 1 << ( j * 2 ) );
            }
            else if( clip[j] <= -clip[3] )
            {
                pointFlags |= ( 1 << ( j * 2 + 1 ) );
            }
        }
        pointAnd &= pointFlags;
        pointOr |= pointFlags;
    }
    
    // trivially reject
    if( pointAnd )
    {
        return true;
    }
    
    // determine if this surface is backfaced and also determine the distance
    // to the nearest vertex so we can cull based on portal range.  Culling
    // based on vertex distance isn't 100% correct (we should be checking for
    // range to the surface), but it's good enough for the types of portals
    // we have in the game right now.
    numTriangles = tess.numIndexes / 3;
    
    for( i = 0; i < tess.numIndexes; i += 3 )
    {
        vec3_t normal;
        float dot;
        float len;
        
        VectorSubtract( tess.xyz[tess.indexes[i]], tr.viewParms.or.origin, normal );
        
        len = VectorLengthSquared( normal );            // lose the sqrt
        if( len < shortest )
        {
            shortest = len;
        }
        
        if( ( dot = DotProduct( normal, tess.normals[tess.indexes[i]] ) ) >= 0 )
        {
            numTriangles--;
        }
    }
    if( !numTriangles )
    {
        return true;
    }
    
    // mirrors can early out at this point, since we don't do a fade over distance
    // with them (although we could)
    if( IsMirror( drawSurf, entityNum ) )
    {
        return false;
    }
    
    if( shortest > ( tess.shader->portalRange * tess.shader->portalRange ) )
    {
        return true;
    }
    
    return false;
}

/*
========================
R_MirrorViewBySurface

Returns true if another view has been rendered
========================
*/
bool R_MirrorViewBySurface( drawSurf_t* drawSurf, int entityNum )
{
    vec4_t clipDest[128];
    viewParms_t newParms;
    viewParms_t oldParms;
    orientation_t surface, camera;
    
    // don't recursively mirror
    if( tr.viewParms.isPortal )
    {
        Com_Printf( "WARNING: recursive mirror/portal found\n" );
        return false;
    }
    
//	if ( r_noportals->integer || r_fastsky->integer || tr.levelGLFog) {
    if( r_noportals->integer || r_fastsky->integer )
    {
        return false;
    }
    
    // trivially reject portal/mirror
    if( SurfIsOffscreen( drawSurf, clipDest ) )
    {
        return false;
    }
    
    // save old viewParms so we can return to it after the mirror view
    oldParms = tr.viewParms;
    
    newParms = tr.viewParms;
    newParms.isPortal = true;
    if( !R_GetPortalOrientations( drawSurf, entityNum, &surface, &camera,
                                  newParms.pvsOrigin, &newParms.isMirror ) )
    {
        return false;      // bad portal, no portalentity
    }
    
    R_MirrorPoint( oldParms.or.origin, &surface, &camera, newParms.or.origin );
    
    VectorSubtract( vec3_origin, camera.axis[0], newParms.portalPlane.normal );
    newParms.portalPlane.dist = DotProduct( camera.origin, newParms.portalPlane.normal );
    
    R_MirrorVector( oldParms.or.axis[0], &surface, &camera, newParms.or.axis[0] );
    R_MirrorVector( oldParms.or.axis[1], &surface, &camera, newParms.or.axis[1] );
    R_MirrorVector( oldParms.or.axis[2], &surface, &camera, newParms.or.axis[2] );
    
    // OPTIMIZE: restrict the viewport on the mirrored view
    
    // render the mirror view
    R_RenderView( &newParms );
    
    tr.viewParms = oldParms;
    
    return true;
}

/*
=================
R_SpriteFogNum

See if a sprite is inside a fog volume
=================
*/
int R_SpriteFogNum( trRefEntity_t* ent )
{
    int i, j;
    fog_t*           fog;
    
    if( tr.refdef.rdflags & RDF_NOWORLDMODEL )
    {
        return 0;
    }
    
    for( i = 1 ; i < tr.world->numfogs ; i++ )
    {
        fog = &tr.world->fogs[i];
        for( j = 0 ; j < 3 ; j++ )
        {
            if( ent->e.origin[j] - ent->e.radius >= fog->bounds[1][j] )
            {
                break;
            }
            if( ent->e.origin[j] + ent->e.radius <= fog->bounds[0][j] )
            {
                break;
            }
        }
        if( j == 3 )
        {
            return i;
        }
    }
    
    return 0;
}

/*
==========================================================================================

DRAWSURF SORTING

==========================================================================================
*/

/*
=================
qsort replacement

=================
*/
#define SWAP_DRAW_SURF(a,b) temp=*((drawSurf_t*)a),*((drawSurf_t*)a)=*((drawSurf_t*)b),*((drawSurf_t*)b)=temp

/* this parameter defines the cutoff between using quick sort and
   insertion sort for arrays; arrays with lengths shorter or equal to the
   below value use insertion sort */

#define CUTOFF 8            /* testing shows that this is good value */

static void shortsort( drawSurf_t* lo, drawSurf_t* hi )
{
    drawSurf_t*  p, *max;
    drawSurf_t temp;
    
    while( hi > lo )
    {
        max = lo;
        for( p = lo + 1; p <= hi; p++ )
        {
            if( p->sort > max->sort )
            {
                max = p;
            }
        }
        SWAP_DRAW_SURF( max, hi );
        hi--;
    }
}


/* sort the array between lo and hi (inclusive)
FIXME: this was lifted and modified from the microsoft lib source...
 */

void qsortFast(
    void* base,
    unsigned num,
    unsigned width
)
{
    char* lo, *hi;              /* ends of sub-array currently sorting */
    char* mid;                  /* points to middle of subarray */
    char* loguy, *higuy;        /* traveling pointers for partition step */
    unsigned size;              /* size of the sub-array */
    char* lostk[30], *histk[30];
    int stkptr;                 /* stack for saving sub-array to be processed */
    
    drawSurf_t temp;
    
    /* Note: the number of stack entries required is no more than
       1 + log2(size), so 30 is sufficient for any array */
    
    if( num < 2 || width == 0 )
    {
        return;                 /* nothing to do */
        
    }
    stkptr = 0;                 /* initialize stack */
    
    lo = ( char* )base;
    hi = ( char* )base + width * ( num - 1 );       /* initialize limits */
    
    /* this entry point is for pseudo-recursion calling: setting
       lo and hi and jumping to here is like recursion, but stkptr is
       prserved, locals aren't, so we preserve stuff on the stack */
recurse:

    size = ( hi - lo ) / width + 1;        /* number of el's to sort */
    
    /* below a certain size, it is faster to use a O(n^2) sorting method */
    if( size <= CUTOFF )
    {
        shortsort( ( drawSurf_t* )lo, ( drawSurf_t* )hi );
    }
    else
    {
        /* First we pick a partititioning element.  The efficiency of the
           algorithm demands that we find one that is approximately the
           median of the values, but also that we select one fast.  Using
           the first one produces bad performace if the array is already
           sorted, so we use the middle one, which would require a very
           wierdly arranged array for worst case performance.  Testing shows
           that a median-of-three algorithm does not, in general, increase
           performance. */
        
        mid = lo + ( size / 2 ) * width;      /* find middle element */
        SWAP_DRAW_SURF( mid, lo );               /* swap it to beginning of array */
        
        /* We now wish to partition the array into three pieces, one
           consisiting of elements <= partition element, one of elements
           equal to the parition element, and one of element >= to it.  This
           is done below; comments indicate conditions established at every
           step. */
        
        loguy = lo;
        higuy = hi + width;
        
        /* Note that higuy decreases and loguy increases on every iteration,
           so loop must terminate. */
        for( ;; )
        {
            /* lo <= loguy < hi, lo < higuy <= hi + 1,
               A[i] <= A[lo] for lo <= i <= loguy,
               A[i] >= A[lo] for higuy <= i <= hi */
            
            do
            {
                loguy += width;
            }
            while( loguy <= hi &&
                    ( ( ( drawSurf_t* )loguy )->sort <= ( ( drawSurf_t* )lo )->sort ) );
                    
            /* lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
               either loguy > hi or A[loguy] > A[lo] */
            
            do
            {
                higuy -= width;
            }
            while( higuy > lo &&
                    ( ( ( drawSurf_t* )higuy )->sort >= ( ( drawSurf_t* )lo )->sort ) );
                    
            /* lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
               either higuy <= lo or A[higuy] < A[lo] */
            
            if( higuy < loguy )
            {
                break;
            }
            
            /* if loguy > hi or higuy <= lo, then we would have exited, so
               A[loguy] > A[lo], A[higuy] < A[lo],
               loguy < hi, highy > lo */
            
            SWAP_DRAW_SURF( loguy, higuy );
            
            /* A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
               of loop is re-established */
        }
        
        /*     A[i] >= A[lo] for higuy < i <= hi,
        	   A[i] <= A[lo] for lo <= i < loguy,
        	   higuy < loguy, lo <= higuy <= hi
           implying:
        	   A[i] >= A[lo] for loguy <= i <= hi,
        	   A[i] <= A[lo] for lo <= i <= higuy,
        	   A[i] = A[lo] for higuy < i < loguy */
        
        SWAP_DRAW_SURF( lo, higuy );     /* put partition element in place */
        
        /* OK, now we have the following:
        	  A[i] >= A[higuy] for loguy <= i <= hi,
        	  A[i] <= A[higuy] for lo <= i < higuy
        	  A[i] = A[lo] for higuy <= i < loguy    */
        
        /* We've finished the partition, now we want to sort the subarrays
           [lo, higuy-1] and [loguy, hi].
           We do the smaller one first to minimize stack usage.
           We only sort arrays of length 2 or more.*/
        
        if( higuy - 1 - lo >= hi - loguy )
        {
            if( lo + width < higuy )
            {
                lostk[stkptr] = lo;
                histk[stkptr] = higuy - width;
                ++stkptr;
            }                           /* save big recursion for later */
            
            if( loguy < hi )
            {
                lo = loguy;
                goto recurse;           /* do small recursion */
            }
        }
        else
        {
            if( loguy < hi )
            {
                lostk[stkptr] = loguy;
                histk[stkptr] = hi;
                ++stkptr;               /* save big recursion for later */
            }
            
            if( lo + width < higuy )
            {
                hi = higuy - width;
                goto recurse;           /* do small recursion */
            }
        }
    }
    
    /* We have sorted the array, except for any pending sorts on the stack.
       Check if there are any, and do them. */
    
    --stkptr;
    if( stkptr >= 0 )
    {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;           /* pop subarray from stack */
    }
    else
    {
        return;                 /* all subarrays done */
    }
}


//==========================================================================================

/*
=================
R_AddDrawSurf
=================
*/
void R_AddDrawSurf( surfaceType_t* surface, shader_t* shader,
                    int fogIndex, int dlightMap, int atiTess )
{
    int index;
    
    // instead of checking for overflow, we just mask the index
    // so it wraps around
    index = tr.refdef.numDrawSurfs & DRAWSURF_MASK;
    // the sort data is packed into a single 32 bit value so it can be
    // compared quickly during the qsorting process
// GR - add tesselation flag to the sort
    tr.refdef.drawSurfs[index].sort = ( shader->sortedIndex << QSORT_SHADERNUM_SHIFT )
                                      | ( atiTess << QSORT_ATI_TESS_SHIFT )
                                      | tr.shiftedEntityNum | ( fogIndex << QSORT_FOGNUM_SHIFT ) | ( int )dlightMap;
    tr.refdef.drawSurfs[index].surface = surface;
    tr.refdef.numDrawSurfs++;
}

/*
=================
R_DecomposeSort
=================
*/
// GR - decompose  with tessellation flag
void R_DecomposeSort( unsigned sort, int* entityNum, shader_t** shader,
                      int* fogNum, int* dlightMap, int* atiTess )
{
    *fogNum = ( sort >> QSORT_FOGNUM_SHIFT ) & 31;
    *shader = tr.sortedShaders[( sort >> QSORT_SHADERNUM_SHIFT ) & ( MAX_SHADERS - 1 ) ];
//	*entityNum = ( sort >> QSORT_ENTITYNUM_SHIFT ) & 1023;
    *entityNum = ( sort >> QSORT_ENTITYNUM_SHIFT ) & ( MAX_GENTITIES - 1 );   // (SA) uppded entity count for Wolf to 11 bits
    *dlightMap = sort & 3;
//GR - extract tessellation flag
    *atiTess = ( sort >> QSORT_ATI_TESS_SHIFT ) & 1;
}

/*
=================
R_SortDrawSurfs
=================
*/
void R_SortDrawSurfs( drawSurf_t* drawSurfs, int numDrawSurfs )
{
    shader_t*        shader;
    int fogNum;
    int entityNum;
    int dlighted;
    int i;
// GR - tessellation flag
    int atiTess;
    
    // it is possible for some views to not have any surfaces
    if( numDrawSurfs < 1 )
    {
        // we still need to add it for hyperspace cases
        R_AddDrawSurfCmd( drawSurfs, numDrawSurfs );
        return;
    }
    
    // sort the drawsurfs by sort type, then orientation, then shader
    qsortFast( drawSurfs, numDrawSurfs, sizeof( drawSurf_t ) );
    
    // check for any pass through drawing, which
    // may cause another view to be rendered first
    for( i = 0 ; i < numDrawSurfs ; i++ )
    {
// GR - decompose with tessellation flag
        R_DecomposeSort( ( drawSurfs + i )->sort, &entityNum, &shader, &fogNum, &dlighted, &atiTess );
        
        if( shader->sort > SS_PORTAL )
        {
            break;
        }
        
        // no shader should ever have this sort type
        if( shader->sort == SS_BAD )
        {
            Com_Error( ERR_DROP, "Shader '%s'with sort == SS_BAD", shader->name );
        }
        
        // if the mirror was completely clipped away, we may need to check another surface
        if( R_MirrorViewBySurface( ( drawSurfs + i ), entityNum ) )
        {
            // this is a debug option to see exactly what is being mirrored
            if( r_portalOnly->integer )
            {
                return;
            }
            break;      // only one mirror view at a time
        }
    }
    
    R_AddDrawSurfCmd( drawSurfs, numDrawSurfs );
}

/*
=============
R_AddEntitySurfaces
=============
*/
void R_AddEntitySurfaces( void )
{
    trRefEntity_t*   ent;
    shader_t*        shader;
    
    if( !r_drawentities->integer )
    {
        return;
    }
    
    for( tr.currentEntityNum = 0;
            tr.currentEntityNum < tr.refdef.num_entities;
            tr.currentEntityNum++ )
    {
        ent = tr.currentEntity = &tr.refdef.entities[tr.currentEntityNum];
        
        ent->needDlights = false;
        
        // preshift the value we are going to OR into the drawsurf sort
        tr.shiftedEntityNum = tr.currentEntityNum << QSORT_ENTITYNUM_SHIFT;
        
        //
        // the weapon model must be handled special --
        // we don't want the hacked weapon position showing in
        // mirrors, because the true body position will already be drawn
        //
        if( ( ent->e.renderfx & RF_FIRST_PERSON ) && tr.viewParms.isPortal )
        {
            continue;
        }
        
        // simple generated models, like sprites and beams, are not culled
        switch( ent->e.reType )
        {
            case RT_PORTALSURFACE:
                break;      // don't draw anything
            case RT_SPRITE:
            case RT_SPLASH:
            case RT_BEAM:
            case RT_LIGHTNING:
            case RT_RAIL_CORE:
            case RT_RAIL_CORE_TAPER:
            case RT_RAIL_RINGS:
                // self blood sprites, talk balloons, etc should not be drawn in the primary
                // view.  We can't just do this check for all entities, because md3
                // entities may still want to cast shadows from them
                if( ( ent->e.renderfx & RF_THIRD_PERSON ) && !tr.viewParms.isPortal )
                {
                    continue;
                }
                shader = R_GetShaderByHandle( ent->e.customShader );
// GR - these entities are not tessellated
                R_AddDrawSurf( &entitySurface, shader, R_SpriteFogNum( ent ), 0, ATI_TESS_NONE );
                break;
                
            case RT_MODEL:
                // we must set up parts of tr.or for model culling
                R_RotateForEntity( ent, &tr.viewParms, &tr.or );
                
                tr.currentModel = R_GetModelByHandle( ent->e.hModel );
                if( !tr.currentModel )
                {
// GR - not tessellated
                    R_AddDrawSurf( &entitySurface, tr.defaultShader, 0, 0, ATI_TESS_NONE );
                }
                else
                {
                    switch( tr.currentModel->type )
                    {
                        case MOD_MESH:
                            R_AddMD3Surfaces( ent );
                            break;
                            // Ridah
                        case MOD_MDC:
                            R_AddMDCSurfaces( ent );
                            break;
                            // done.
                        case MOD_MDV:
                            R_AddMDVSurfaces( ent );
                            break;
                        case MOD_MDS:
                            R_AddAnimSurfaces( ent );
                            break;
                        case MOD_MD4MESH:
                            R_AddMD4MeshSurfaces( ent );
                            break;
                        case MOD_BRUSH:
                            R_AddBrushModelSurfaces( ent );
                            break;
                        case MOD_BAD:       // null model axis
                            if( ( ent->e.renderfx & RF_THIRD_PERSON ) && !tr.viewParms.isPortal )
                            {
                                break;
                            }
                            shader = R_GetShaderByHandle( ent->e.customShader );
// GR - not tessellated
                            R_AddDrawSurf( &entitySurface, tr.defaultShader, 0, 0, ATI_TESS_NONE );
                            break;
                        default:
                            Com_Error( ERR_DROP, "R_AddEntitySurfaces: Bad modeltype" );
                            break;
                    }
                }
                break;
            default:
                Com_Error( ERR_DROP, "R_AddEntitySurfaces: Bad reType" );
        }
    }
    
}


/*
====================
R_GenerateDrawSurfs
====================
*/
void R_GenerateDrawSurfs( void )
{
    R_AddWorldSurfaces();
    
    R_AddPolygonSurfaces();
    
    // set the projection matrix with the minimum zfar
    // now that we have the world bounded
    // this needs to be done before entities are
    // added, because they use the projection
    // matrix for lod calculation
    R_SetupProjection();
    
    R_AddEntitySurfaces();
}

/*
================
R_DebugPolygon
================
*/
void R_DebugPolygon( int color, int numPoints, float* points )
{
#if 0
    int i;
    
    GL_State( GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE );
    
    // draw solid shade
    
    glColor3f( color & 1, ( color >> 1 ) & 1, ( color >> 2 ) & 1 );
    glBegin( GL_POLYGON );
    for( i = 0 ; i < numPoints ; i++ )
    {
        glVertex3fv( points + i * 3 );
    }
    glEnd();
    
    // draw wireframe outline
    GL_State( GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE );
    glDepthRange( 0, 0 );
    glColor3f( 1, 1, 1 );
    glBegin( GL_POLYGON );
    for( i = 0 ; i < numPoints ; i++ )
    {
        glVertex3fv( points + i * 3 );
    }
    glEnd();
    glDepthRange( 0, 1 );
#endif
}

/*
====================
R_DebugGraphics

Visualization aid for movement clipping debugging
====================
*/
void R_DebugGraphics( void )
{
    if( !r_debugSurface->integer )
    {
        return;
    }
    
    R_FogOff(); // moved this in here to keep from /always/ doing the fog state change
    
    // the render thread can't make callbacks to the main thread
    R_SyncRenderThread();
    
    GL_Bind( tr.whiteImage );
    GL_Cull( CT_FRONT_SIDED );
    collisionModelManager->DrawDebugSurface( R_DebugPolygon );
}


/*
================
R_RenderView

A view may be either the actual camera view,
or a mirror / remote location
================
*/
void R_RenderView( viewParms_t* parms )
{
    int firstDrawSurf, numDrawSurfs;
    
    if( parms->viewportWidth <= 0 || parms->viewportHeight <= 0 )
    {
        return;
    }
    
    // Ridah, purge media that were left over from the last level
    if( r_cache->integer )
    {
        extern void R_PurgeBackupImages( int purgeCount );
        static int lastTime;
        
        if( ( lastTime > tr.refdef.time ) || ( lastTime < ( tr.refdef.time - 200 ) ) )
        {
            R_FreeImageBuffer();    // clear all image buffers
            R_PurgeShaders( 10 );
            R_PurgeBackupImages( 1 );
            R_PurgeModels( 1 );
            lastTime = tr.refdef.time;
        }
    }
    // done.
    
    tr.viewCount++;
    
    tr.viewParms = *parms;
    tr.viewParms.frameSceneNum = tr.frameSceneNum;
    tr.viewParms.frameCount = tr.frameCount;
    
    firstDrawSurf = tr.refdef.numDrawSurfs;
    
    tr.viewCount++;
    
    // set viewParms.world
    R_RotateForViewer( &tr.viewParms );
    
    R_SetupFrustum();
    
    R_GenerateDrawSurfs();
    
    // if we overflowed MAX_DRAWSURFS, the drawsurfs
    // wrapped around in the buffer and we will be missing
    // the first surfaces, not the last ones
    numDrawSurfs = tr.refdef.numDrawSurfs;
    if( numDrawSurfs > MAX_DRAWSURFS )
    {
        numDrawSurfs = MAX_DRAWSURFS;
    }
    
    R_SortDrawSurfs( tr.refdef.drawSurfs + firstDrawSurf, numDrawSurfs - firstDrawSurf );
    
    // draw main system development information (surface outlines, etc)
    R_FogOff();
    R_DebugGraphics();
    R_FogOn();
    
}
