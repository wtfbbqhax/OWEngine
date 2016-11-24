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
//  File name:   l_math.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: math primitives
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "l_cmd.h"
#include "l_math.h"

vec3_t vec3_origin = {0, 0, 0};

void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
    float angle;
    static float sr, sp, sy, cr, cp, cy;
    // static to help MS compiler fp bugs
    
    angle = angles[YAW] * ( M_PI * 2 / 360 );
    sy = sin( angle );
    cy = cos( angle );
    angle = angles[PITCH] * ( M_PI * 2 / 360 );
    sp = sin( angle );
    cp = cos( angle );
    angle = angles[ROLL] * ( M_PI * 2 / 360 );
    sr = sin( angle );
    cr = cos( angle );
    
    if( forward )
    {
        forward[0] = cp * cy;
        forward[1] = cp * sy;
        forward[2] = -sp;
    }
    if( right )
    {
        right[0] = ( -1 * sr * sp * cy + - 1 * cr * -sy );
        right[1] = ( -1 * sr * sp * sy + - 1 * cr * cy );
        right[2] = -1 * sr * cp;
    }
    if( up )
    {
        up[0] = ( cr * sp * cy + - sr * -sy );
        up[1] = ( cr * sp * sy + - sr * cy );
        up[2] = cr * cp;
    }
}

/*
=================
RadiusFromBounds
=================
*/
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs )
{
    int i;
    vec3_t corner;
    float a, b;
    
    for( i = 0 ; i < 3 ; i++ )
    {
        a = fabs( mins[i] );
        b = fabs( maxs[i] );
        corner[i] = a > b ? a : b;
    }
    
    return VectorLength( corner );
}

/*
================
R_ConcatRotations
================
*/
void R_ConcatRotations( float in1[3][3], float in2[3][3], float out[3][3] )
{
    out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
                in1[0][2] * in2[2][0];
    out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
                in1[0][2] * in2[2][1];
    out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
                in1[0][2] * in2[2][2];
    out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
                in1[1][2] * in2[2][0];
    out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
                in1[1][2] * in2[2][1];
    out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
                in1[1][2] * in2[2][2];
    out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
                in1[2][2] * in2[2][0];
    out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
                in1[2][2] * in2[2][1];
    out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
                in1[2][2] * in2[2][2];
}

void AxisClear( vec3_t axis[3] )
{
    axis[0][0] = 1;
    axis[0][1] = 0;
    axis[0][2] = 0;
    axis[1][0] = 0;
    axis[1][1] = 1;
    axis[1][2] = 0;
    axis[2][0] = 0;
    axis[2][1] = 0;
    axis[2][2] = 1;
}

vec_t VectorLength( const vec3_t v )
{
    return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

vec_t VectorLengthSquared( const vec3_t v )
{
    return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

int VectorCompare( const vec3_t v1, const vec3_t v2 )
{
    if( v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] )
    {
        return 0;
    }
    
    return 1;
}


vec_t Q_rint( vec_t in )
{
    return floor( in + 0.5 );
}

void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross )
{
    cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
    cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void _VectorMA( vec3_t va, double scale, vec3_t vb, vec3_t vc )
{
    vc[0] = va[0] + scale * vb[0];
    vc[1] = va[1] + scale * vb[1];
    vc[2] = va[2] + scale * vb[2];
}

vec_t _DotProduct( vec3_t v1, vec3_t v2 )
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void _VectorSubtract( vec3_t va, vec3_t vb, vec3_t out )
{
    out[0] = va[0] - vb[0];
    out[1] = va[1] - vb[1];
    out[2] = va[2] - vb[2];
}

void _VectorAdd( vec3_t va, vec3_t vb, vec3_t out )
{
    out[0] = va[0] + vb[0];
    out[1] = va[1] + vb[1];
    out[2] = va[2] + vb[2];
}

void _VectorCopy( vec3_t in, vec3_t out )
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
}

void _VectorScale( vec3_t v, vec_t scale, vec3_t out )
{
    out[0] = v[0] * scale;
    out[1] = v[1] * scale;
    out[2] = v[2] * scale;
}

vec_t VectorNormalize( vec3_t inout )
{
    vec_t length, ilength;
    
    length = sqrt( inout[0] * inout[0] + inout[1] * inout[1] + inout[2] * inout[2] );
    if( length == 0 )
    {
        VectorClear( inout );
        return 0;
    }
    
    ilength = 1.0 / length;
    inout[0] = inout[0] * ilength;
    inout[1] = inout[1] * ilength;
    inout[2] = inout[2] * ilength;
    
    return length;
}

vec_t VectorNormalize2( const vec3_t in, vec3_t out )
{
    vec_t length, ilength;
    
    length = sqrt( in[0] * in[0] + in[1] * in[1] + in[2] * in[2] );
    if( length == 0 )
    {
        VectorClear( out );
        return 0;
    }
    
    ilength = 1.0 / length;
    out[0] = in[0] * ilength;
    out[1] = in[1] * ilength;
    out[2] = in[2] * ilength;
    
    return length;
}

vec_t ColorNormalize( vec3_t in, vec3_t out )
{
    float max, scale;
    
    max = in[0];
    if( in[1] > max )
    {
        max = in[1];
    }
    if( in[2] > max )
    {
        max = in[2];
    }
    
    if( max == 0 )
    {
        return 0;
    }
    
    scale = 1.0 / max;
    
    VectorScale( in, scale, out );
    
    return max;
}



void VectorInverse( vec3_t v )
{
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

void ClearBounds( vec3_t mins, vec3_t maxs )
{
    mins[0] = mins[1] = mins[2] = 99999;
    maxs[0] = maxs[1] = maxs[2] = -99999;
}

void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs )
{
    int i;
    vec_t val;
    
    for( i = 0 ; i < 3 ; i++ )
    {
        val = v[i];
        if( val < mins[i] )
        {
            mins[i] = val;
        }
        if( val > maxs[i] )
        {
            maxs[i] = val;
        }
    }
}

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s
{
    vec3_t normal;
    float dist;
    byte type;              // for fast side tests: 0,1,2 = axial, 3 = nonaxial
    byte signbits;          // signx + (signy<<1) + (signz<<2), used as lookup during collision
    byte pad[2];
} cplane_t;

int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, struct cplane_s* p )
{
    float dist1, dist2;
    int sides;
    
    // fast axial cases
    if( p->type < 3 )
    {
        if( p->dist <= emins[p->type] )
        {
            return 1;
        }
        if( p->dist >= emaxs[p->type] )
        {
            return 2;
        }
        return 3;
    }
    
    // general case
    switch( p->signbits )
    {
        case 0:
            dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
            dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
            break;
        case 1:
            dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
            dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
            break;
        case 2:
            dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
            dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
            break;
        case 3:
            dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
            dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
            break;
        case 4:
            dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
            dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
            break;
        case 5:
            dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
            dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
            break;
        case 6:
            dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
            dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
            break;
        case 7:
            dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
            dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
            break;
        default:
            dist1 = dist2 = 0;      // shut up compiler
            break;
    }
    
    sides = 0;
    if( dist1 >= p->dist )
    {
        sides = 1;
    }
    if( dist2 < p->dist )
    {
        sides |= 2;
    }
    
    return sides;
}

