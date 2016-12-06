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
//  File name:   cm_public.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __CM_PUBLIC_H__
#define __CM_PUBLIC_H__

#include "../qcommon/qfiles.h"
#include "../shared/cm_header.h"
#include "cm_tracemodel.h"

// plane types are used to speed some tests
// 0-2 are axial planes
#define PLANE_X         0
#define PLANE_Y         1
#define PLANE_Z         2
#define PLANE_NON_AXIAL 3


/*
=================
PlaneTypeForNormal
=================
*/

#define PlaneTypeForNormal( x ) ( x[0] == 1.0 ? PLANE_X : ( x[1] == 1.0 ? PLANE_Y : ( x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL ) ) )

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

// trace->entityNum can also be 0 to (MAX_GENTITIES-1)
// or ENTITYNUM_NONE, ENTITYNUM_WORLD

// a trace is returned when a box is swept through the world
typedef struct
{
    bool allsolid;      // if true, plane is not valid
    bool startsolid;    // if true, the initial point was in a solid area
    float fraction;         // time completed, 1.0 = didn't hit anything
    vec3_t endpos;          // final position
    cplane_t plane;         // surface normal at impact, transformed to world space
    int surfaceFlags;           // surface hit
    int contents;           // contents on other side of surface hit
    int entityNum;          // entity the contacted sirface is a part of
    idVec3 fractionreal;
} trace_t;

// markfragments are returned by CM_MarkFragments()
typedef struct
{
    int firstPoint;
    int numPoints;
} markFragment_t;

typedef struct
{
    vec3_t origin;
    vec3_t axis[3];
} orientation_t;

//
// idCollisionModelManager
//
class idCollisionModelManager
{
public:
    virtual void        LoadMap( const char* name, bool clientload, int* checksum ) = 0;
    virtual void		FreeMap( void ) = 0;
    virtual clipHandle_t InlineModel( int index ) = 0;       // 0 = world, 1 + are bmodels
    virtual clipHandle_t TempBoxModel( const vec3_t mins, const vec3_t maxs, int capsule ) = 0;
    virtual idTraceModel* GetTraceModelForEntity( int entityNum ) = 0;
    virtual void        ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs ) = 0;
    
    virtual int         NumClusters( void ) = 0;
    virtual int         NumInlineModels( void ) = 0;
    virtual char*        EntityString( void ) = 0;
    
    // returns an ORed contents mask
    virtual int         PointContents( const vec3_t p, clipHandle_t model ) = 0;
    virtual int         TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles ) = 0;
    virtual void		TraceModel( trace_t* results, idTraceModel* model, const idVec3& start, const idVec3& end, int mask ) = 0;
    virtual void        BoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
                                  const vec3_t mins, const vec3_t maxs,
                                  clipHandle_t model, int brushmask, int capsule ) = 0;
    virtual void        SetTempBoxModelContents( int contents ) = 0;
    virtual void        TransformedBoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
            const vec3_t mins, const vec3_t maxs,
            clipHandle_t model, int brushmask,
            const vec3_t origin, const vec3_t angles, int capsule ) = 0;
            
    virtual byte*        ClusterPVS( int cluster ) = 0;
    
    virtual int         PointLeafnum( const vec3_t p ) = 0;
    
    // only returns non-solid leafs
    // overflow if return listsize and if *lastLeaf != list[listsize-1]
    virtual int         BoxLeafnums( const vec3_t mins, const vec3_t maxs, int* list,
                                     int listsize, int* lastLeaf ) = 0;
                                     
    virtual int         LeafCluster( int leafnum ) = 0;
    virtual int         LeafArea( int leafnum ) = 0;
    
    virtual void        AdjustAreaPortalState( int area1, int area2, bool open ) = 0;
    virtual bool        AreasConnected( int area1, int area2 ) = 0;
    
    virtual int         WriteAreaBits( byte* buffer, int area ) = 0;
    virtual void* 		GetBrushModelVertexes( int bmodelNum ) = 0;
    virtual void        ClearMap( void ) = 0;
    
    // cm_patch.c
    virtual void DrawDebugSurface( void ( *drawPoly )( int color, int numPoints, float* points ) ) = 0;
};

extern idCollisionModelManager* collisionModelManager;

#endif // !__CM_PUBLIC_H__
