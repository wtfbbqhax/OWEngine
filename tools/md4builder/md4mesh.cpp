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
//  File name:   md4mesh.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "md4builder.h"
//Dushan - FIX ME
#include "../../src-engine/splines/math_angles.h"

#define DotProduct(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

/*
===============
ConvertToIdSpace
===============
*/
idVec3 ConvertToIdSpace( const idVec3& pos )
{
    idVec3 idpos;
    
    idpos.x = pos.x;
    idpos.y = -pos.z;
    idpos.z = pos.y;
    
    return idpos;
}

/*
============
owMD4Mesh::SetupJoints
============
*/
void owMD4Mesh::SetupJoints( KFbxMesh*	mesh )
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
                //if(meshNodes[b] == link)
                if( !strcmp( joints[b].name, linkName ) )
                    boneIndex = b;
            }
            
            cluster->GetTransformLinkMatrix( jointsinternal[boneIndex].jointMatrix );
        }
    }
    
    for( int i = 0; i < joints.size(); i++ )
    {
        KFbxXMatrix jointMatrix = jointsinternal[i].jointMatrix;
        md4MeshJoint_t* joint = &joints[i];
        if( joint->parentnum == -1 )
        {
            VectorCopy( jointMatrix.GetT(), joint->pos );
            VectorCopy( jointMatrix.GetQ(), joint->rotation );
            joint->rotation[3] = jointMatrix.GetQ()[3];
            continue;
        }
        
        KFbxXMatrix parentJointMatrix = jointsinternal[joint->parentnum].jointMatrix;
        KFbxXMatrix transformMatrix;
        
        transformMatrix = jointMatrix;
        
        VectorCopy( transformMatrix.GetT(), joint->pos );
        VectorCopy( transformMatrix.GetQ(), joint->rotation );
        joint->rotation[0] = joint->rotation[0];
        joint->rotation[1] = joint->rotation[1];
        joint->rotation[2] = joint->rotation[2];
        joint->rotation[3] = transformMatrix.GetQ()[3];
        
        joint->pos[0] *= MD4MESH_SCALE;
        joint->pos[1] *= MD4MESH_SCALE;
        joint->pos[2] *= MD4MESH_SCALE;
        
        joint->pos[1] -= MD4MESH_ZOFFSET;
    }
}

/*
============
owMD4Mesh::CreateJoint
============
*/
void owMD4Mesh::CreateJoint( KFbxNode* skelNode, int parentNum )
{
    md4MeshJoint_t joint;
    KFbxSkeleton*	skeleton;
    fbxDouble3 skelXyz, skelRot, skelScale;
    KFbxVector4 skelPreRotate;
    exportJoint_t jointData;
    
    // Get the joint from the node.
    skeleton = skelNode->GetSkeleton();
    
    if( skeleton == NULL )
    {
        return;
    }
    
    // Set the joint name.
    strcpy( joint.name, skeleton->GetName() );
    
    // Set the joint parent.
    joint.parentnum = parentNum;
    
    // Add the joint to the list.
    joints.push_back( joint );
    
    jointsinternal.push_back( jointData );
}

/*
============
owMD4Mesh::AddJoint_r
============
*/
void owMD4Mesh::AddJoint_r( KFbxNode* skelNode, int parentNum )
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

/*
============
owMD4Mesh::GetWeightForVertex
============
*/
bool owMD4Mesh::GetWeightForVertex( md4MeshVertex_t& vertex, KFbxVector4 vertexXyz, int vertexNum, KFbxMesh*	mesh )
{
    KFbxLayerElementArrayTemplate<KFbxVector2>* uvValues = NULL;
    
    int lPolygonCount, lPolyVertexCount, lVertexCount;
    
    lPolygonCount = mesh->GetPolygonCount();
    if( lPolygonCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no polygons\n" );
        return false;
    }
    
    lPolyVertexCount = mesh->GetPolygonVertexCount();
    if( lPolyVertexCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no poly verts\n" );
        return false;
    }
    
    lVertexCount = mesh->GetControlPointsCount();
    if( lVertexCount == 0 )
    {
        Com_Printf( "ERROR: Layer with no vertexs\n" );
        return false;
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
            
            int* weightIndices = cluster->GetControlPointIndices();
            double* weightValues = cluster->GetControlPointWeights();
            for( int w = 0; w < numInfluencedPoints; w++ )
            {
                if( weightIndices[w] == vertexNum && weightValues[w] > 0.05f )
                {
                    vec3_t xyz;
                    md4MeshWeight_t weight;
                    
                    if( vertex.startweight == -1 )
                    {
                        vertex.startweight = weights.size();
                        vertex.numWeights = 0;
                    }
                    
                    weight.jointNum = boneIndex;
                    weight.jointWeight = weightValues[w];
                    
                    VectorSubtract( vertexXyz, joints[boneIndex].pos, xyz );
                    
                    weight.offset[0] = DotProduct( xyz, jointsinternal[boneIndex].jointMatrix[0] );
                    weight.offset[1] = DotProduct( xyz, jointsinternal[boneIndex].jointMatrix[1] );
                    weight.offset[2] = DotProduct( xyz, jointsinternal[boneIndex].jointMatrix[2] );
                    
                    if( vertex.numWeights > 3 )
                    {
                    
                        for( int f = 1; f <= vertex.numWeights; f++ )
                        {
                            if( f == vertex.numWeights )
                            {
                                Com_Printf( "WARNING: Too many weights on vertex. \n" );
                                break;
                            }
                            if( weights[weights.size() - f].jointWeight < weight.jointWeight )
                            {
                                totalWeight -= weights[weights.size() - f].jointWeight;
                                weights[weights.size() - f] = weight;
                                totalWeight += weight.jointWeight;
                                break;
                            }
                        }
                    }
                    else
                    {
                        weights.push_back( weight );
                        totalWeight += weight.jointWeight;
                        vertex.numWeights++;
                    }
                }
            }
        }
    }
    
    if( totalWeight < 0.98f )
    {
        //	Com_Printf("WARNING: Vertex weight invalid %f\n", totalWeight);
    }
    
    for( int c = 0; c < vertex.numWeights; c++ )
    {
        weights[ vertex.startweight + c ].jointWeight /= totalWeight;
    }
    
    return true;
}

/*
============
owMD4Mesh::AddSurface
============
*/
bool owMD4Mesh::AddSurface( KFbxNode* pNode )
{
    md4MeshSurface_t	surface;
    KFbxMesh*			mesh;
    KFbxLayerElementArrayTemplate<KFbxVector2>* uvValues = NULL;
    KFbxVector4*			xyz;
    
    strcpy( surface.name, pNode->GetName() );
    surface.startVertex = vertexes.size();
    surface.startIndex = indexes.size();
    surface.shadernum = shaders.size();
    
    Com_Printf( "Processing Surface %s \n", surface.name );
    
    mesh = pNode->GetMesh();
    
    if( mesh->GetLayerCount() == 0 )
    {
        Com_Printf( "ERROR: Mesh with no layers. \n" );
        return false;
    }
    
    if( pNode->GetMaterialCount() > 0 )
    {
        KFbxSurfaceMaterial* mat = pNode->GetMaterial( 0 );
        KFbxProperty props = NULL;
        props = mat->FindProperty( KFbxSurfaceMaterial::sDiffuse );
        
        if( props.IsValid() )
        {
            KFbxTexture* texture = KFbxCast <KFbxTexture>( props.GetSrcObject( KFbxTexture::ClassId, 0 ) );
            
            if( texture != NULL )
            {
                md4MeshShader_t	shader;
                const char* name = texture->GetFileName();
                strcpy( shader.shadername, strstr( name, BASEGAME ) + ( strlen( BASEGAME ) + 1 ) );
                
                int len = strlen( shader.shadername );
                shader.shadername[len - 0] = 0;
                shader.shadername[len - 1] = 0;
                shader.shadername[len - 2] = 0;
                shader.shadername[len - 3] = 0;
                shader.shadername[len - 4] = 0;
                
                shaders.push_back( shader );
            }
        }
    }
    else
    {
        Com_Printf( "ERROR: No Material Found. \n" );
        return false;
    }
    
    surface.numIndexes = mesh->GetPolygonVertexCount();
    
    int* ind = mesh->GetPolygonVertices();
    for( int i = 0; i < surface.numIndexes; i++ )
    {
        indexes.push_back( ind[i] );
    }
    
    KFbxLayerElementUV* uvs = mesh->GetLayer( 0 )->GetUVs();
    xyz = mesh->GetControlPoints();
    
    surface.numVertexes = mesh->GetControlPointsCount();
    
    Com_Printf( "Polygon Count: %d\n", mesh->GetPolygonCount() );
    
    for( int i = 0; i < surface.numVertexes; i++ )
    {
        md4MeshVertex_t v;
        int uvid = i;
        
        v.startweight = -1;
        
        if( uvs->GetMappingMode() == KFbxLayerElement::eBY_CONTROL_POINT )
        {
            if( uvs->GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT )
            {
                uvid = uvs->GetIndexArray().GetAt( i );
            }
            
            v.texCoords[0] = uvValues->GetAt( uvid )[0];
            v.texCoords[1] = uvValues->GetAt( uvid )[1];
        }
        
        if( !GetWeightForVertex( v, xyz[i], surface.startVertex + i, mesh ) )
        {
            return false;
        }
        
        if( v.startweight == -1 )
        {
            Com_Printf( "WARNING: Vertex %d with no weight. \n", i );
        }
        
        vertexes.push_back( v );
    }
    
    if( uvs->GetMappingMode() == KFbxLayerElement::eBY_POLYGON_VERTEX )
    {
        for( int i = 0; i < mesh->GetPolygonCount(); i++ )
        {
            int numVerts = mesh->GetPolygonSize( i );
            for( int c = 0; c < numVerts; c++ )
            {
                int uvid = uvid = mesh->GetTextureUVIndex( i, c );
                int index = mesh->GetPolygonVertex( i, c );
                
                vertexes[index].texCoords[0] = uvs->GetDirectArray().GetAt( uvid )[0];
                vertexes[index].texCoords[1] = 1.0f - uvs->GetDirectArray().GetAt( uvid )[1];
            }
        }
    }
    
    surfaces.push_back( surface );
    return true;
}

/*
============
owMD4Mesh::BuildMesh
============
*/
bool owMD4Mesh::BuildMesh()
{
    KFbxNode*		skelNode;
    KFbxNode*		meshNode;
    int skelNodeNum = 0, meshNodeNum = 0;
    
    Com_Printf( "Finding Skel Node...\n" );
    
    // Get the base joint.
    skelNode = globals.fbxManager.FindSkeleton( skelNodeNum );
    if( skelNode == NULL )
    {
        Com_Printf( "ERROR: Failed to find skeleton\n" );
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
    meshNode = globals.fbxManager.FindMesh( meshNodeNum );
    SetupJoints( meshNode->GetMesh() );
    
    Com_Printf( "Adding Model Surfaces...\n" );
    
    // Add all the surfaces for the model.
    while( meshNode != NULL )
    {
        if( !AddSurface( meshNode ) )
        {
            return false;
        }
        meshNode = globals.fbxManager.FindMesh( meshNodeNum );
    }
    
    if( surfaces.size() <= 0 )
    {
        Com_Printf( "ERROR: Model with no geometry\n" );
        return false;
    }
    
    return true;
}

/*
============
owMD4Mesh::ExportMesh
============
*/
void owMD4Mesh::ExportMesh( const char* filename )
{
    FILE* file;
    md4MeshHeader_t header;
    
    Com_Printf( "Writing Mesh...\n" );
    
    file = fopen( filename, "wb" );
    if( !file )
    {
        Com_Printf( "ERROR: Failed to open file for writing. \n" );
        return;
    }
    
    // Rename all the joints before we write them out, this is a hack to remove the skeleton_ prefix that
    // maya puts in, and so I don't have to change everything with the animation, I just add tag_ before the
    // joint name.
    for( int i = 0; i < joints.size(); i++ )
    {
        char name[32];
        
        strcpy( name, strstr( joints[i].name, "skeleton_" ) + strlen( "skeleton_" ) );
        for( int c = 0; c < strlen( name ); c++ )
        {
            name[c] = tolower( name[c] );
        }
        memset( joints[i].name, 0, 32 );
        sprintf( joints[i].name, "tag_%s", name );
    }
    
    header.iden = MD4MESH_IDENT;
    header.version = MD4MESH_VERSION;
    header.numJoints = joints.size();
    header.numShaders = shaders.size();
    header.numSurfaces = surfaces.size();
    header.numVertexes = vertexes.size();
    header.numIndexes = indexes.size();
    header.numWeights = weights.size();
    
    // Write the header now we will fix the offsets later.
    fwrite( &header, sizeof( md4MeshHeader_t ), 1, file );
    
    // Write out the joints.
    header.ofsJoints = ftell( file );
    fwrite( &joints[0], sizeof( md4MeshJoint_t ), header.numJoints, file );
    
    // Write the shaders.
    header.ofsShaders = ftell( file );
    fwrite( &shaders[0], sizeof( md4MeshShader_t ), header.numShaders, file );
    
    // Write out the surfaces
    header.ofsSurface = ftell( file );
    fwrite( &surfaces[0], sizeof( md4MeshSurface_t ), header.numSurfaces, file );
    
    // Write out the vertexes.
    header.ofsVetexes = ftell( file );
    fwrite( &vertexes[0], sizeof( md4MeshVertex_t ), header.numVertexes, file );
    
    // Write out the indexes.
    header.ofsIndexes = ftell( file );
    fwrite( &indexes[0], sizeof( short ), header.numIndexes, file );
    
    // Write out the weights
    header.ofsWeights = ftell( file );
    fwrite( &weights[0], sizeof( md4MeshWeight_t ), header.numWeights, file );
    
    // Write the header with the proper offsets.
    fseek( file, 0, SEEK_SET );
    fwrite( &header, sizeof( md4MeshHeader_t ), 1, file );
    
    fclose( file );
}