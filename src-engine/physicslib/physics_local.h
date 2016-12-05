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
//  File name:   physics_local.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../cm/cm_local.h"
#include "../qcommon/surfaceflags.h"
#include "../splines/math_angles.h"
#include "../splines/math_matrix.h"
#include "../splines/math_angles.h"
#include "Bullet-C-Api.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btGeometryUtil.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "physics_collision.h"

//
// owPhysicsManagerLocal
//
class owPhysicsManagerLocal : public owPhysicsManager
{
public:
    virtual void			Init( void );
    virtual void			Shutdown( void );
    virtual void			SetGravity( const idVec3& gravity );
    virtual void			Frame( void );
    virtual void			CreateCollisionModelFromBSP( void* collisionModel );
    virtual void			CreateCollisionModelFromAbstractBSP( idPhysicsAbstractBSP* collisionModel );
    virtual void			WriteBulletPhysicsFile( const char* fullpath );
    virtual idTraceModel*	AllocTraceModel();
    virtual idTraceModel*	GetTraceModelForEntity( int entityNum );
    virtual void			DrawDebug();
    virtual void			TraceModel( trace_t* results, idTraceModel* model, const idVec3& start, const idVec3& end, int mask );
    virtual void			Reset( void );
    
    //
    // GetCollisionManager
    //
    owPhysicsCollisionDetection* GetCollisionManager( void )
    {
        return &collision;
    }
    
    //
    // GetDefaultContactProcessingThreshold
    //
    float GetDefaultContactProcessingThreshold( void )
    {
        return BT_LARGE_FLOAT;
    }
    
    //
    // GetWorldDynamics
    //
    btDiscreteDynamicsWorld* GetWorldDynamics( void )
    {
        return dynamicsWorld;
    }
private:
    btScalar getDeltaTimeMicroseconds();
protected:
    btClock m_clock;
    
    btBroadphaseInterface*	broadphase;
    btConstraintSolver*		solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    
    owPhysicsCollisionDetection collision;
};

class GLDebugDrawer : public btIDebugDraw
{
    int m_debugMode;
    
public:

    GLDebugDrawer();
    
    
    virtual void	drawLine( const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor );
    
    virtual void	drawLine( const btVector3& from, const btVector3& to, const btVector3& color );
    
    virtual void	drawSphere( const btVector3& p, btScalar radius, const btVector3& color );
    virtual void	drawBox( const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha );
    
    virtual void	drawTriangle( const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha );
    
    virtual void	drawContactPoint( const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color );
    
    virtual void	reportErrorWarning( const char* warningString );
    
    virtual void	draw3dText( const btVector3& location, const char* textString );
    
    virtual void	setDebugMode( int debugMode );
    
    virtual int		getDebugMode() const
    {
        return m_debugMode;
    }
};

extern owPhysicsManagerLocal physicsManagerLocal;