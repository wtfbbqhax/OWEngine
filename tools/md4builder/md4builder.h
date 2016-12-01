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
//  File name:   md4builder.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4996)

#include <vector>
using namespace std;

//Dushan - FIX ME
#include "../../src-engine/qcommon/q_shared.h"
#include "../../src-engine/splines/splines.h"
#include "../../src-engine/splines/math_matrix.h"
#include "../../src-engine/splines/math_quaternion.h"
#include "../../src-engine/qcommon/qfiles.h"

#undef DotProduct
#include <fbxsdk.h>
#include <fbxfilesdk/fbxfilesdk_nsuse.h>

struct exportJoint_t
{
    KFbxXMatrix jointMatrix;
    KFbxNode* animNode;
    vec3_t bindpos;
    vec3_t bindrot;
};

void Com_Printf( const char* fmt, ... );

//
// owMD4Anim
//
class owMD4Anim
{
public:
    bool			BuildAnim( int startFrame, int endFrame );
    void			WriteAnim( const char* filename );
private:
    void			CreateJoint( KFbxNode* skelNode, int parentNum );
    void			AddJoint_r( KFbxNode* skelNode, int parentNum );
    void			SetupJoints( KFbxMesh*	mesh );
    
    vector<md4AnimJoint_t> joints;
    vector<exportJoint_t> jointsinternal;
    vector<md4JointTransform_t> transforms;
    vector<md4AnimFrame_t> frames;
};

//
// owMD4Mesh
//
class owMD4Mesh
{
public:
    bool			BuildMesh();
    void			ExportMesh( const char* filename );
private:
    void			AddJoint_r( KFbxNode* skelNode, int parentNum );
    void			CreateJoint( KFbxNode* skelNode, int parentNum );
    bool			AddSurface( KFbxNode* surfaceNode );
    bool			GetWeightForVertex( md4MeshVertex_t& vertex, KFbxVector4 vertexXyz, int vertexNum, KFbxMesh*	mesh );
    void			SetupJoints( KFbxMesh*	mesh );
protected:
    vector<short> indexes;
    vector<md4MeshJoint_t> joints;
    vector<exportJoint_t> jointsinternal;
    vector<md4MeshSurface_t> surfaces;
    vector<md4MeshVertex_t> vertexes;
    vector<md4MeshWeight_t> weights;
    vector<md4MeshShader_t> shaders;
};

//
// owFBXManager
//
class owFBXManager
{
public:
    void			Init();
    bool			LoadFBXFromFile( const char* filePath );
    KFbxNode*		FindSkeleton( int& nodeNum );
    KFbxNode*       FindMesh( int& nodeNum );
protected:
    KFbxSdkManager*	fbxSdkManager;
    KFbxImporter*	importer;
    KFbxScene*		scene;
    KFbxNode*		rootNode;
};

//
// globals_t
//
struct globals_t
{
    owFBXManager	fbxManager;
};

extern globals_t globals;
