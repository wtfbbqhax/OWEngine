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

#include "qfiles.h"
#include "../renderer/r_types.h"
#include"q_shared.h"

void CM_LoadMap( const char* name, bool clientload, int* checksum );
void CM_ClearMap( void );

clipHandle_t CM_InlineModel( int index );       // 0 = world, 1 + are bmodels
clipHandle_t CM_TempBoxModel( const vec3_t mins, const vec3_t maxs, int capsule );

void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
void CM_SetTempBoxModelContents( int contents );
int CM_NumClusters( void );
int CM_NumInlineModels( void );
char* CM_EntityString( void );
// returns an ORed contents mask
int CM_PointContents( const vec3_t p, clipHandle_t model );
int CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
void CM_BoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
                  const vec3_t mins, const vec3_t maxs,
                  clipHandle_t model, int brushmask, int capsule );
void CM_TransformedBoxTrace( trace_t* results, const vec3_t start, const vec3_t end,
                             const vec3_t mins, const vec3_t maxs,
                             clipHandle_t model, int brushmask,
                             const vec3_t origin, const vec3_t angles, int capsule );
byte* CM_ClusterPVS( int cluster );
int CM_PointLeafnum( const vec3_t p );
// only returns non-solid leafs
// overflow if return listsize and if *lastLeaf != list[listsize-1]
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int* list, int listsize, int* lastLeaf );
int CM_LeafCluster( int leafnum );
int CM_LeafArea( int leafnum );
bool CM_AreasConnected( int area1, int area2 );
int CM_WriteAreaBits( byte* buffer, int area );
void CM_AdjustAreaPortalState( int area1, int area2, bool open );
void CM_DrawDebugSurface( void ( *drawPoly )( int color, int numPoints, float* points ) );

#endif // !__CM_PUBLIC_H__