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
//  File name:   physics_public.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHYSICS_PUBLIC_H__
#define __PHYSICS_PUBLIC_H__

#ifdef _PHYSICSLIB
#include "../tools/bsp/bspfile_abstract.h"

#undef MAX_LIGHTMAPS // Breaks vertex struct!
#endif

//
// idPhysicsAbstractBSP
//
struct idPhysicsAbstractBSP
{
#ifdef IDBSP_ABSTRACT
    int				numShaders;
    bspShader_t*		shaders;
    
    int				numLeafs;
    bspLeaf_t*		leafs;
    
    int				numPlanes;
    bspPlane_t*		planes;
    
    int				numNodes;
    bspNode_t*		nodes;
    
    int				numLeafSurfaces;
    int*				leafSurfaces;
    
    int				numLeafBrushes;
    int*				leafBrushes;
    
    int				numBrushes;
    bspBrush_t*		brushes;
    
    int				numBrushSides;
    bspBrushSide_t*	brushSides;
#else
    void*			unknown;
#endif
};

//
// owPhysicsManager
//
class owPhysicsManager
{
public:
    // Init the physics engine.
    virtual void			Init( void ) = 0;
    
    // Shutsdown the physics engine.
    virtual void			Shutdown( void ) = 0;
    
    // Sets the gravity for the current world.
    virtual void			SetGravity( const idVec3& gravity ) = 0;
    
    // Sets up a world from a collision model.
    virtual void			CreateCollisionModelFromBSP( void* collisionModel ) = 0;
    
    // Used by q3map2 to create a bsp from the abstracted bsp memory.
    virtual void			CreateCollisionModelFromAbstractBSP( idPhysicsAbstractBSP* collisionModel ) = 0;
    
    // Has to be called once per frame to update the simulation.
    virtual void			Frame( void ) = 0;
    
    virtual void			WriteBulletPhysicsFile( const char* fullpath ) = 0;
    
    // Creates a trace model.
    virtual idTraceModel*	AllocTraceModel() = 0;
    
    virtual idTraceModel*	GetTraceModelForEntity( int entityNum ) = 0;
    
    virtual void			DrawDebug() = 0;
    
    virtual void			TraceModel( trace_t* results, idTraceModel* model, const idVec3& start, const idVec3& end, int mask ) = 0;
    virtual void			Reset( void ) = 0;
};

extern owPhysicsManager*		physicsManager;

#endif // !__PHYSICS_PUBLIC_H__