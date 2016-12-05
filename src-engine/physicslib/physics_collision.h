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
//  File name:   physics_collision.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

//#include "../renderer/r_local.h"

//
// idTraceModel
//
class idTraceModelLocal : public idTraceModel
{
public:
    idTraceModelLocal()
    {
        collisionShape = NULL;
    };
    virtual void			Init( int entityNum, float mass, idVec3 origin, idVec3 angle, idVec3& mins, idVec3& maxs );
    virtual void			InitFromModel( const char* qpath, idVec3 origin, idVec3 angle, int entityNum, float mass );
    virtual void			CreateCapsuleShape( idVec3& mins, idVec3& maxs );
    virtual void			SetTransform( idVec3& origin, idVec3& angle );
    virtual int				GetEntityNum()
    {
        return entityNum;
    };
    virtual void			GetTransform( idVec3& xyz, idVec3& angle );
    virtual void			SetVelocity( idVec3& vel );
    virtual float			GetInverseMass( void );
    virtual void			PhysicsApplyCentralImpulse( idVec3& direction );
    virtual void			GetUpVectorBasis( idVec3& upVector );
    virtual void			SetPhysicsActive( void );
public:
    void Free( void );
    btRigidBody* GetBody()
    {
        return body;
    };
private:
    void CreateLocalInertia( float mass, btVector3& localInertia );
    void CreateInitialTransform( idVec3& origin, idVec3& angle, float mass, btVector3& localInertia );
    void CreateGhostObject( void );
    //void InitCMFromMDVModel(model_t *model);
    void InitCMFromBrushModel( idCollisionShape_t* shape );
    
    btCollisionShape*	collisionShape;
    btGhostObject*		ghostObject;
    btRigidBody*			body;
    int	entityNum;
};

//
// owPhysicsCollisionDetection
//
class owPhysicsCollisionDetection
{
public:
    void		Init( void );
    void		Shutdown( void );
    
    void		CreateCollisionModelFromBSP( clipMap_t* collisionModel );
    void		CreateCollisionModelFromAbstractBSP( idPhysicsAbstractBSP* collisionModel );
    
    idTraceModel* AllocTraceModel();
    idTraceModel* GetTraceModelForEntity( int entityNum );
    
    void	addConvexVerticesCollider( btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation );
    void	addConvexShapeToCompound( btCompoundShape* compoundShape, btAlignedObjectArray<btVector3>& vertices );
    void	TraceModel( trace_t* results, idTraceModel* model, const idVec3& start, const idVec3& end, int mask );
    btRigidBody*	CreateRigidBody( float mass, const btTransform& startTransform, btCollisionShape* shape );
    
    //
    // RegisterCollisionShape
    //
    void RegisterCollisionShape( btCollisionShape* shape )
    {
        collisionShapes.push_back( shape );
    }
    
    //
    // GetCollisionDispatcher
    //
    btCollisionDispatcher*	GetCollisionDispatcher( void )
    {
        return dispatcher;
    }
    
    //
    // GetCollisionConfiguration
    //
    btDefaultCollisionConfiguration* GetCollisionConfiguration( void )
    {
        return collisionConfiguration;
    }
protected:
    btCollisionDispatcher*	dispatcher;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btAlignedObjectArray<btCollisionShape*>	collisionShapes;
    btAlignedObjectArray<idTraceModelLocal*> traceModels;
};