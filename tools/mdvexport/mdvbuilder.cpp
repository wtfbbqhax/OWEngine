//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2010 JV Software
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
//  File name:   fbxmanager.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "mayaplugin.h"
#include "mdvbuilder.h"

/*
====================
owMDVBUilder::Reader
====================
*/
MStatus owMDVBuilder::reader( const MFileObject& file, const MString& optionsString, FileAccessMode mode )
{
    return MStatus::kFailure;
}

/*
====================
owMDVBuilder::Clear
====================
*/
void owMDVBuilder::Clear()
{
    memset( &header, 0, sizeof( mdvHeader_t ) );
    tags.clear();
    frames.clear();
    surfaces.clear();
    vertexes.clear();
    indexes.clear();
    shaders.clear();
}

/*
====================
owMDVBuilder::WriteHeader
====================
*/
void owMDVBuilder::WriteHeader( FILE* file )
{
    // Set the write position to zero.
    fseek( file, 0, SEEK_SET );
    
    header.iden = MDV_IDENT;
    header.version = MDV_VERSION;
    header.numframes = maxtime - mintime;
    
    // Write the header.
    fwrite( &header, sizeof( mdvHeader_t ), 1, file );
}
/*
====================
GetLocalIndex
====================
*/

//http://ewertb.soundlinker.com/api/api.018.php
MIntArray GetLocalIndex( MIntArray& getVertices, MIntArray& getTriangle )
{
    MIntArray   localIndex;
    unsigned    gv, gt;
    
    for( gt = 0; gt < getTriangle.length(); gt++ )
    {
        for( gv = 0; gv < getVertices.length(); gv++ )
        {
            if( getTriangle[gt] == getVertices[gv] )
            {
                localIndex.append( gv );
                break;
            }
        }
        
        // if nothing was added, add default "no match"
        if( localIndex.length() == gt )
        {
            localIndex.append( -1 );
        }
    }
    
    return localIndex;
}

/*
====================
FixTextureString
====================
*/
MString owMDVBuilder::FixTextureString( MString textureString )
{
    const char* basePath = BASEGAME;
    
    static MString texturePath = strstr( textureString.asChar(), basePath ) + ( strlen( basePath ) + 1 );
    //char *ptr = (char *)texturePath.asChar();
    //ptr[strlen(ptr)-3] = 0;
    
    return texturePath;
}

/*
====================
FindShader
====================
*/
MObject owMDVBuilder::FindShader( MObject& setNode )
{
    MStatus				status;
    MFnDependencyNode	fnNode( setNode );
    MPlug				shaderPlug;
    
    shaderPlug = fnNode.findPlug( "surfaceShader" );
    if( !shaderPlug.isNull() )
    {
        MPlugArray connectedPlugs;
        bool asSrc = false;
        bool asDst = true;
        shaderPlug.connectedTo( connectedPlugs, asDst, asSrc, &status );
        
        if( connectedPlugs.length() != 1 )
        {
            return MObject::kNullObj;
        }
        else
        {
            return connectedPlugs[ 0 ].node();
        }
    }
    
    return MObject::kNullObj;
}

/*
====================
GetShaderForSurface
====================
*/
int owMDVBuilder::GetShaderForSurface( MDagPath path, int instanceNum, MFnMesh& fnMesh )
{
    const char* shadername = GetShaderForMesh( path, instanceNum, fnMesh ).asChar();
    mdvShader_t shader;
    
    for( int i = 0; i < shaders.size(); i++ )
    {
        if( !strcmp( shaders[i].filepath, shadername ) )
        {
            return i;
        }
    }
    
    strcpy( shader.filepath, shadername );
    shaders.push_back( shader );
    
    return shaders.size() - 1;
}

/*
====================
GetShaderForMesh
====================
*/
MString owMDVBuilder::GetShaderForMesh( MDagPath path, int instanceNum, MFnMesh& fnMesh )
{
    MObjectArray sets;
    MObjectArray comps;
    MStatus status;
    
    status = fnMesh.getConnectedSetsAndMembers( instanceNum, sets, comps, true );
    
    if( !status )
    {
        return "surfaceTextureError";
    }
    
    // Loop through all the sets.  If the set is a polygonal set, find the
    // shader attached to the and print out the texture file name for the
    // set along with the polygons in the set.
    //
    for( int i = 0; i < ( int )sets.length(); i++ )
    {
        MObject set = sets[i];
        MObject comp = comps[i];
        
        MFnSet fnSet( set, &status );
        
        // Make sure the set is a polygonal set.  If not, continue.
        MItMeshPolygon piter( path, comp, &status );
        if( status == MS::kFailure )
        {
            continue;
        }
        
        // Find the texture that is applied to this set.  First, get the
        // shading node connected to the set.  Then, if there is an input
        // attribute called "color", search upstream from it for a texture
        // file node.
        //
        MObject shaderNode = FindShader( set );
        if( shaderNode == MObject::kNullObj )
        {
            continue;
        }
        
        MPlug colorPlug = MFnDependencyNode( shaderNode ).findPlug( "color", &status );
        if( status == MS::kFailure )
        {
            continue;
        }
        
        MItDependencyGraph dgIt( colorPlug, MFn::kFileTexture,
                                 MItDependencyGraph::kUpstream,
                                 MItDependencyGraph::kBreadthFirst,
                                 MItDependencyGraph::kNodeLevel,
                                 &status );
                                 
        if( status == MS::kFailure )
        {
            continue;
        }
        
        dgIt.disablePruningOnFilter();
        
        // If no texture file node was found, just continue.
        //
        if( dgIt.isDone() )
        {
            continue;
        }
        
        // Print out the texture node name and texture file that it references.
        //
        MObject textureNode = dgIt.thisNode();
        MPlug filenamePlug = MFnDependencyNode( textureNode ).findPlug( "fileTextureName" );
        MString textureName;
        filenamePlug.getValue( textureName );
        
        // remove the OS path and save it in the mesh
        return FixTextureString( textureName );
    }
    
    return "";
}

/*
====================
owMDVBuilder::BuildSurface
====================
*/
void owMDVBuilder::BuildSurface( MDagPath meshDag, MFnMesh& fnMesh, mdvSurface_t& surface, int framenum )
{
    MStringArray  UVSets;
    MPointArray vertexList;
    MFloatVectorArray normals;
    MFloatArray   u, v;
    MStatus		  status;
    mdvFrame_t	  frame;
    Bounds		  bounds;
    
    fnMesh.getPoints( vertexList, MSpace::kWorld );
    fnMesh.getNormals( normals, MSpace::kWorld );
    
    fnMesh.getUVSetNames( UVSets );
    fnMesh.getUVs( u, v, &UVSets[0] );
    
//	if(framenum == 0)
//	{
    surface.startVertex = vertexes.size();
    surface.startIndex = indexes.size();
    surface.numIndexes = 0;
    surface.numVertexes = vertexList.length();
//	}

    surface.baseframenum = frames.size();
    
    for( int i = 0; i < surface.numVertexes; i++ )
    {
        mdvVertex_t vert;
        
        VectorSet( vert.xyz, vertexList[i][0], vertexList[i][2], -vertexList[i][1] );
        
        bounds.AddPoint( &vert.xyz[0] );
        vertexes.push_back( vert );
    }
    VectorCopy( bounds.b[0], frame.mins );
    VectorCopy( bounds.b[1], frame.maxs );
    frames.push_back( frame );
    
    MItMeshPolygon  itPolygon( meshDag, MObject::kNullObj );
    for( ; !itPolygon.isDone(); itPolygon.next() )
    {
        // Get object-relative indices for the vertices in this face.
        MIntArray                           polygonVertices;
        itPolygon.getVertices( polygonVertices );
        
        // Get triangulation of this poly.
        int numTriangles = 0;
        itPolygon.numTriangles( numTriangles );
        
        while( numTriangles-- )
        {
            MPointArray                     nonTweaked;
            // object-relative vertex indices for each triangle
            MIntArray                       triangleVertices;
            // face-relative vertex indices for each triangle
            MIntArray                       localIndex;
            
            status = itPolygon.getTriangle( numTriangles,
                                            nonTweaked,
                                            triangleVertices,
                                            MSpace::kWorld );
                                            
            if( status == MS::kSuccess )
            {
                int uvID[3];
                
                // Get face-relative vertex indices for this triangle
                localIndex = GetLocalIndex( polygonVertices, triangleVertices );
                
                // Get UV values for each vertex within this polygon
                for( int vtxInPolygon = 0; vtxInPolygon < 3; vtxInPolygon++ )
                {
                    itPolygon.getUVIndex( localIndex[vtxInPolygon], uvID[vtxInPolygon], &UVSets[0] );
                }
                
                
                // Add the indexes, uv's and normals to the surface pools.
                for( int i = 0; i < 3; i++ )
                {
                    int index = triangleVertices[i];
                    int uvindex = uvID[i];
                    
                    vertexes[surface.startVertex + index].st[0] = u[uvindex];
                    vertexes[surface.startVertex + index].st[1] = v[uvindex];
                    
                    if( framenum == 0 )
                    {
                        indexes.push_back( index );
                        surface.numIndexes++;
                    }
                }
            }
        }
    }
}

/*
====================
BuildMesh
====================
*/
void owMDVBuilder::BuildMesh( int framenum )
{
    MStatus stat;
    MItDag dagIter( MItDag::kBreadthFirst, MFn::kInvalid, &stat );
    
    // Go through all the nodes in the scene.
    for( ; !dagIter.isDone(); dagIter.next() )
    {
        MDagPath dagPath;
        stat = dagIter.getPath( dagPath );
        
        if( stat )
        {
            MFnDagNode dagNode( dagPath, &stat );
            
            // Always skip intermediate objects.
            if( !dagNode.isIntermediateObject() )
            {
                if( dagPath.hasFn( MFn::kMesh ) && !dagPath.hasFn( MFn::kTransform ) )
                {
                    MFnMesh fnMesh( dagPath );
                    mdvSurface_t surface;
                    
                    BuildSurface( dagPath, fnMesh, surface, framenum );
                    strcpy( surface.surfacename, fnMesh.name().asChar() );
                    
                    if( framenum == 0 )
                    {
                        surface.shadernum = GetShaderForSurface( dagPath, dagPath.instanceNumber(), fnMesh );
                        surfaces.push_back( surface );
                    }
                }
                else if( dagPath.hasFn( MFn::kJoint ) )
                {
                    MFnIkJoint joint( dagPath );
                    const char* jointname = joint.name().asChar();
                    
                    // Only include joints that have been marked as a tag.
                    if( strstr( jointname, "tag_" ) )
                    {
                        mdvTag_t tag;
                        MEulerRotation rotation;
                        MVector xyz;
                        MMatrix axis;
                        
                        xyz = joint.getTranslation( MSpace::kWorld );
                        joint.getRotation( rotation );
                        axis = rotation.asMatrix();
                        
                        
                        strcpy( tag.name, jointname );
                        VectorSet( tag.origin, xyz.x, xyz.y, xyz.z );
                        for( int i = 0; i < 3; i++ )
                        {
                            VectorCopy( axis.matrix[i], tag.axis[i] );
                        }
                        tags.push_back( tag );
                    }
                }
            }
        }
    }
}

/*
====================
WriteLump
====================
*/
void owMDVBuilder::WriteLump( FILE* file, mdvLump_t& lumptable, void* lump, int numLumps, int size )
{
    lumptable.offset = ftell( file );
    lumptable.numlumps = numLumps;
    fwrite( lump, size, numLumps, file );
}

/*
====================
writer
====================
*/
MStatus owMDVBuilder::writer( const MFileObject& file, const MString& optionsString, FileAccessMode mode )
{
    FILE* mdvmodelfile;
    MStatus status;
    
    // Ensure everything is cleared.
    Clear();
    
    MGlobal::viewFrame( MAnimControl::maxTime() );
    MGlobal::viewFrame( MAnimControl::minTime() );
    
    mintime = MAnimControl::minTime().as( MTime::kFilm );
    maxtime = MAnimControl::maxTime().as( MTime::kFilm );
    
    // Open the output file for writing.
    mdvmodelfile = fopen( file.fullName().asChar(), "wb" );
    if( !mdvmodelfile )
    {
        MGlobal::displayInfo( "Unable to open output file for writing\n" );
        return MStatus::kFailure;
    }
    
    // Write the header now, we will go back in and rewrite it later with the right values.
    WriteHeader( mdvmodelfile );
    
    // Build our mesh.
    for( int i = mintime, a = 0; i < mintime + maxtime; i++, a++ )
    {
        SetFrame( i );
        BuildMesh( a );
    }
    
    // Write the lumps
    for( int i = 0; i < NUMMDVLUMPS; i++ )
    {
        void* lumpptr;
        int lumpsize = -1;
        int numlumps = -1;
        
        switch( i )
        {
            case MDVLUMP_SHADERS:
                if( shaders.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &shaders[0];
                lumpsize = sizeof( mdvShader_t );
                numlumps = shaders.size();
                break;
                
            case MDVLUMP_SURFACES:
                if( surfaces.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &surfaces[0];
                lumpsize = sizeof( mdvSurface_t );
                numlumps = surfaces.size();
                break;
                
            case MDVLUMP_VERTS:
                if( vertexes.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &vertexes[0];
                lumpsize = sizeof( mdvVertex_t );
                numlumps = vertexes.size();
                break;
                
            case MDVLUMP_TAGS:
                if( tags.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &tags[0];
                lumpsize = sizeof( mdvTag_t );
                numlumps = tags.size();
                break;
                
            case MDVLUMP_FRAMES:
                if( frames.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &frames[0];
                lumpsize = sizeof( mdvFrame_t );
                numlumps = frames.size();
                break;
                
            case MDVLUMP_INDEXES:
                if( indexes.size() <= 0 )
                {
                    continue;
                }
                
                lumpptr = &indexes[0];
                lumpsize = sizeof( short );
                numlumps = indexes.size();
                break;
            default:
                MGlobal::displayInfo( "Unknown lump to write" );
                break;
        }
        
        WriteLump( mdvmodelfile, header.lumps[i], lumpptr, numlumps, lumpsize );
    }
    
    // Rewrite the header with the updated lump information.
    WriteHeader( mdvmodelfile );
    
    // Close the mdv file
    fclose( mdvmodelfile );
    
    // Clear up any used memory.
    Clear();
    
    return MStatus::kSuccess;
}

/*
====================
owMDVBuilder::haveReadMethod
====================
*/
bool owMDVBuilder::haveReadMethod() const
{
    return false;
}

/*
====================
owMDVBuilder::haveReadMethod
====================
*/
bool owMDVBuilder::haveWriteMethod() const
{
    return true;
}

/*
====================
owMDVBuilder::defaultExtension
====================
*/
MString owMDVBuilder::defaultExtension() const
{
    return "mdv";
}

/*
====================
owMDVBuilder::identifyFile
====================
*/
MPxFileTranslator::MFileKind owMDVBuilder::identifyFile(
    const MFileObject& fileName,
    const char* buffer,
    short size ) const
{
    const char* name = fileName.name().asChar();
    int   nameLength = strlen( name );
    
    if( ( nameLength > 4 ) && !stricmp( name + nameLength - 4, ".mdv" ) )
    {
        return kCouldBeMyFileType;
    }
    else
    {
        return kNotMyFileType;
    }
}