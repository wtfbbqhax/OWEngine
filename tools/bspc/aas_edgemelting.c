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
//  File name:   aas_edgemelting.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: Melting of Edges
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "qbsp.h"
#include "../../src-engine/botlib/aasfile.h"
#include "aas_create.h"

//===========================================================================
// try to melt the windings of the two faces
// FIXME: this is buggy
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_MeltFaceWinding( tmp_face_t* face1, tmp_face_t* face2 )
{
    int i, n;
    int splits = 0;
    winding_t* w2, *neww;
    plane_t* plane1;
    
#ifdef DEBUG
    if( !face1->winding )
    {
        Error( "face1 %d without winding", face1->num );
    }
    if( !face2->winding )
    {
        Error( "face2 %d without winding", face2->num );
    }
#endif //DEBUG
    w2 = face2->winding;
    plane1 = &mapplanes[face1->planenum];
    for( i = 0; i < w2->numpoints; i++ )
    {
        if( PointOnWinding( face1->winding, plane1->normal, plane1->dist, w2->p[i], &n ) )
        {
            neww = AddWindingPoint( face1->winding, w2->p[i], n );
            FreeWinding( face1->winding );
            face1->winding = neww;
            
            splits++;
        } //end if
    } //end for
    return splits;
} //end of the function AAS_MeltFaceWinding
//===========================================================================
// melt the windings of the area faces
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_MeltFaceWindingsOfArea( tmp_area_t* tmparea )
{
    int side1, side2, num_windingsplits = 0;
    tmp_face_t* face1, *face2;
    
    for( face1 = tmparea->tmpfaces; face1; face1 = face1->next[side1] )
    {
        side1 = face1->frontarea != tmparea;
        for( face2 = tmparea->tmpfaces; face2; face2 = face2->next[side2] )
        {
            side2 = face2->frontarea != tmparea;
            if( face1 == face2 )
            {
                continue;
            }
            num_windingsplits += AAS_MeltFaceWinding( face1, face2 );
        } //end for
    } //end for
    return num_windingsplits;
} //end of the function AAS_MeltFaceWindingsOfArea
//===========================================================================
// melt the windings of the faces of all areas
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_MeltAreaFaceWindings( void )
{
    tmp_area_t* tmparea;
    int num_windingsplits = 0;
    
    Log_Write( "AAS_MeltAreaFaceWindings\r\n" );
    qprintf( "%6d edges melted", num_windingsplits );
    //NOTE: first convex area (zero) is a dummy
    for( tmparea = tmpaasworld.areas; tmparea; tmparea = tmparea->l_next )
    {
        num_windingsplits += AAS_MeltFaceWindingsOfArea( tmparea );
        qprintf( "\r%6d", num_windingsplits );
    } //end for
    qprintf( "\n" );
    Log_Write( "%6d edges melted\r\n", num_windingsplits );
} //end of the function AAS_MeltAreaFaceWindings

