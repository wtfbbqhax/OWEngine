//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2011 JV Software
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
//  File name:   cm_model.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

//
// idCollisionShape_t
//
struct idCollisionShape_t
{
    int numVertexes;
    idVec3*	xyz;
};

//
// idCollisionModel
//
class idCollisionModel
{
public:
    void			Init( cmHeader_t* cm );
    idCollisionShape_t* GetBrushShape( int brushNum );
private:
    cmBrushModel_t*		GetModel( cmHeader_t* cm, int modelNum );
    cmBrushSurface_t*	GetSurface( cmHeader_t* cm, int surfaceNum );
    cmBrushVertex_t*		GetVertex( cmHeader_t* cm, int vertexNum );
    idCollisionShape_t*	cm_shapes;
    idVec3*				cm_shape_vertexes;
};

ID_INLINE cmBrushModel_t* idCollisionModel::GetModel( cmHeader_t* cm, int modelNum )
{
    return ( cmBrushModel_t* )( ( byte* )cm + cm->bmodelOffest + ( sizeof( cmBrushModel_t ) * modelNum ) );
}

ID_INLINE cmBrushSurface_t* idCollisionModel::GetSurface( cmHeader_t* cm, int surfaceNum )
{
    return ( cmBrushSurface_t* )( ( byte* )cm + cm->surfaceOffset + ( sizeof( cmBrushSurface_t ) * surfaceNum ) );
}

ID_INLINE cmBrushVertex_t* idCollisionModel::GetVertex( cmHeader_t* cm, int vertexNum )
{
    return ( cmBrushVertex_t* )( ( byte* )cm + cm->vertexOffset + ( sizeof( cmBrushVertex_t ) * vertexNum ) );
}