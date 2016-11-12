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
//  File name:   md4anim.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "md4builder.h"
#include "../../src/game/q_shared.h"

/*
============
owMD4Mesh::ExportMesh
============
*/
void owMD4Anim::SetupJoints( KFbxMesh*	mesh )
{
    KFbxLayerElementArrayTemplate<KFbxVector2>* uvValues = NULL;
    
    int lPolygonCount, lPolyVertexCount, lVertexCount;
    
    lPolygonCount = mesh->GetPolygonCount();
    if( lPolygonCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no polygons\n" );
        return;
    }
    
    lPolyVertexCount = mesh->GetPolygonVertexCount();
    if( lPolyVertexCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no poly verts\n" );
        return;
    }
    
    lVertexCount = mesh->GetControlPointsCount();
    if( lVertexCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no vertexs\n" );
        return;
    }
    
    int lSkinCount = mesh->GetDeformerCount( KFbxDeformer::eSKIN );
    float totalWeight = 0;
    
    for( int s = 0; s < lSkinCount; s++ )
    {
        KFbxSkin* skin = ( ( KFbxSkin* )( mesh->GetDeformer( 0, KFbxDeformer::eSKIN ) ) );
        KFbxGeometry* poly = skin->GetGeometry();
        
        int clusterCount = skin->GetClusterCount();
        if( clusterCount == 0 )
        {
            continue;
        }
        
        for( int c = 0; c < clusterCount; c++ )
        {
            KFbxCluster* cluster = skin->GetCluster( c );
            
            KFbxNode* link = cluster->GetLink();
            if( !link )
            {
                continue;
            }
            
            const char* linkName = link->GetName();
            
            int  numInfluencedPoints = cluster->GetControlPointIndicesCount();
            if( numInfluencedPoints == 0 )
            {
                continue;
            }
            
            int boneIndex = -1;
            for( int b = 0; b < joints.size() && boneIndex == -1; b++ )
            {
                if( !strcmp( joints[b].name, linkName ) )
                {
                    boneIndex = b;
                }
            }
            
            jointsinternal[boneIndex].animNode = link;
            
            cluster->GetTransformLinkMatrix( jointsinternal[boneIndex].jointMatrix );
        }
    }
    
    for( int i = 0; i < joints.size(); i++ )
    {
        exportJoint_t* exportJoint = &jointsinternal[i];
        KFbxXMatrix jointMatrix = exportJoint->jointMatrix;
        md4AnimJoint_t* joint = &joints[i];
        if( joint->parentnum == -1 )
        {
            VectorCopy( jointMatrix.GetT(), exportJoint->bindpos );
            VectorCopy( jointMatrix.GetQ(), exportJoint->bindrot );
            exportJoint->bindrot[3] = jointMatrix.GetQ()[3];
            continue;
        }
        
        KFbxXMatrix parentJointMatrix = jointsinternal[joint->parentnum].jointMatrix;
        KFbxXMatrix transformMatrix;
        
        transformMatrix = jointMatrix;
        
        VectorCopy( transformMatrix.GetT(), exportJoint->bindpos );
        VectorCopy( transformMatrix.GetQ(), exportJoint->bindrot );
        exportJoint->bindrot[0] = -exportJoint->bindrot[0];
        exportJoint->bindrot[1] = -exportJoint->bindrot[1];
        exportJoint->bindrot[2] = -exportJoint->bindrot[2];
        exportJoint->bindrot[3] = -transformMatrix.GetQ()[3];
    }
}

/*
============
owMD4Mesh::CreateJoint
============
*/
void owMD4Anim::CreateJoint( KFbxNode* skelNode, int parentNum )
{
    md4AnimJoint_t joint;
    KFbxSkeleton*	skeleton;
    exportJoint_t jointData;
    
    // Get the joint from the node.
    skeleton = skelNode->GetSkeleton();
    
    // Set the joint name.
    strcpy( joint.name, skeleton->GetName() );
    
    // Set the joint parent.
    joint.parentnum = parentNum;
    
    // Add the joint to the list.
    joints.push_back( joint );
    
    jointData.animNode = skelNode;
    jointsinternal.push_back( jointData );
}

/*
============
owMD4Mesh::AddJoint_r
============
*/
void owMD4Anim::AddJoint_r( KFbxNode* skelNode, int parentNum )
{
    int jointnum;
    
    if( skelNode->GetSkeleton() == NULL )
    {
        return;
    }
    
    jointnum = joints.size();
    
    if( parentNum != -1 )
    {
        Com_Printf( "Parenting %s to %s\n", skelNode->GetName(), joints[parentNum].name );
    }
    
    // Create the joint that the node is bound to.
    CreateJoint( skelNode, parentNum );
    
    // Add the rest of the child nodes.
    for( int i = 0; i < skelNode->GetChildCount(); i++ )
    {
        AddJoint_r( skelNode->GetChild( i ), jointnum );
    }
}

//Dushan - FIX ME - copy from q_math.c
static void ClearBounds( vec3_t mins, vec3_t maxs )
{
    mins[0] = mins[1] = mins[2] = 99999;
    maxs[0] = maxs[1] = maxs[2] = -99999;
}

//Dushan - FIX ME - copy from q_math.c
static void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs )
{
    if( v[0] < mins[0] )
    {
        mins[0] = v[0];
    }
    if( v[0] > maxs[0] )
    {
        maxs[0] = v[0];
    }
    
    if( v[1] < mins[1] )
    {
        mins[1] = v[1];
    }
    if( v[1] > maxs[1] )
    {
        maxs[1] = v[1];
    }
    
    if( v[2] < mins[2] )
    {
        mins[2] = v[2];
    }
    if( v[2] > maxs[2] )
    {
        maxs[2] = v[2];
    }
}

/*
============
owMD4Mesh::BuildAnim
============
*/
bool owMD4Anim::BuildAnim( int startFrame, int endFrame )
{
    KFbxNode*		skelNode;
    KFbxNode*		meshNode;
    int skelNodeNum = 0, meshNodeNum = 0;
    
    Com_Printf( "Building animation from frame %d to %d\n", startFrame, endFrame );
    
    Com_Printf( "Finding Skel Node...\n" );
    
    // Get the base joint.
    skelNode = globals.fbxManager.FindSkeleton( skelNodeNum );
    if( skelNode == NULL )
    {
        printf( "ERROR: Failed to find skeleton\n" );
        return false;
    }
    
    Com_Printf( "Adding Joints...\n" );
    
    // Recursively add the child joints.
    AddJoint_r( skelNode, -1 );
    if( joints.size() <= 0 )
    {
        Com_Printf( "ERROR: Skeleton has no joints.\n" );
        return false;
    }
    
    Com_Printf( "Building Joint Transforms...\n" );
    meshNode = globals.fbxManager.FindMesh( meshNodeNum );
    KFbxMesh* pMesh = meshNode->GetMesh();
    KFbxAnimEvaluator* lEvaluator = pMesh->GetScene()->GetEvaluator();
    
    SetupJoints( pMesh );
    
    KTime time;    // Defaults to myTime=0
    for( int i = startFrame; i < endFrame; i++ )
    {
        int startTransform = transforms.size();
        for( int j = 0; j < joints.size(); j++ )
        {
            md4JointTransform_t boneTransform;
            KFbxXMatrix transformMatrix;
            KFbxQuaternion quat;
            KFbxVector4 translation;
            
            time.SetGlobalTimeMode( KTime::eCINEMA );
            time.SetTime( 0, 0, 0, i, 0, 0, 0, KTime::eCINEMA );
            
            transformMatrix = lEvaluator->GetNodeGlobalTransform( jointsinternal[j].animNode, time );
            quat = transformMatrix.GetQ();
            
            translation = transformMatrix.GetT();
            
            VectorSet( boneTransform.translate, translation[0], translation[1], translation[2] );
            
            if( j > 0 )
            {
                fbxDouble3 origin = jointsinternal[1].animNode->LclTranslation.Get();
                
                boneTransform.translate[0] = boneTransform.translate[0] - origin[0];
                boneTransform.translate[1] = boneTransform.translate[1] - origin[1];
            }
            
            boneTransform.translate[0] *= MD4MESH_SCALE;
            boneTransform.translate[1] *= MD4MESH_SCALE;
            boneTransform.translate[2] *= MD4MESH_SCALE;
            
            boneTransform.translate[2] -= MD4MESH_ZOFFSET;
            
            VectorSet( boneTransform.rotation, quat[0], quat[1], quat[2] );
            boneTransform.rotation[3] = quat[3];
            
            transforms.push_back( boneTransform );
        }
    }
    
    Com_Printf( "Building Frames..." );
    
    KFbxVector4* verts = pMesh->GetControlPoints();
    frames.clear();
    for( int i = startFrame, d = 0; i < endFrame; i++, d++ )
    {
        md4AnimFrame_t frame;
        
        ClearBounds( frame.mins, frame.maxs );
        
        // TODO: Base this on the actuall animation.
        for( int c = 0; c < pMesh->GetControlPointsCount(); c++ )
        {
            vec3_t v;
            
            VectorSet( v, verts[c][0] * MD4MESH_SCALE, verts[c][1] * MD4MESH_SCALE, verts[c][2] * MD4MESH_SCALE );
            
            v[2] -= MD4MESH_ZOFFSET;
            AddPointToBounds( v, frame.mins, frame.maxs );
        }
        
        frame.baseTransform = ( d * joints.size() );
        
        frames.push_back( frame );
    }
    Com_Printf( "%d\n", frames.size() );
    
    return true;
}

/*
============
owMD4Mesh::WriteAnim
============
*/
void owMD4Anim::WriteAnim( const char* filename )
{
    FILE* file;
    md4AnimHeader_t header;
    
    printf( "Writing Anim...\n" );
    
    file = fopen( filename, "wb" );
    if( !file )
    {
        printf( "ERROR: Failed to open file for writing. \n" );
        return;
    }
    
    // Rename all the joints before we write them out, this is a hack to remove the skeleton_ prefix that
    // maya puts in, and so I don't have to change everything with the animation, I just add tag_ before the
    // joint name.
    for( int i = 0; i < joints.size(); i++ )
    {
        char name[32];
        char* tagname;
        
        tagname = strstr( joints[i].name, "skeleton_" );
        if( tagname != NULL )
        {
            strcpy( name, tagname + strlen( "skeleton_" ) );
            
            for( int c = 0; c < strlen( name ); c++ )
            {
                name[c] = tolower( name[c] );
            }
            memset( joints[i].name, 0, 32 );
            sprintf( joints[i].name, "tag_%s", name );
        }
        else
        {
            strcpy( name, joints[i].name );
        }
    }
    
    header.iden = MD4ANIM_IDENT;
    header.version = MD4ANIM_VERSION;
    header.numJoints = joints.size();
    header.numFrames = frames.size();
    
    Com_Printf( "Num Frames: %d\n", header.numFrames );
    
    // Write the header now we will fix the offsets later.
    fwrite( &header, sizeof( md4AnimHeader_t ), 1, file );
    
    // Write out the joints.
    header.ofsJoints = ftell( file );
    fwrite( &joints[0], sizeof( md4AnimJoint_t ), header.numJoints, file );
    
    // Write out the frames.
    header.ofsFrames = ftell( file );
    fwrite( &frames[0], sizeof( md4AnimFrame_t ), header.numFrames, file );
    
    // Write out the joint transforms.
    header.ofsTransforms = ftell( file );
    fwrite( &transforms[0], sizeof( md4JointTransform_t ), header.numFrames * header.numJoints, file );
    
    // Re-write the header with the proper offsets.
    fseek( file, 0, SEEK_SET );
    fwrite( &header, sizeof( md4AnimHeader_t ), 1, file );
    
    fclose( file );
}
