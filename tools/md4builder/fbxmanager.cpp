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
//  File name:   fbxmanager.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "md4builder.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(fbxSdkManager->GetIOSettings()))
#endif

/*
============
owFBXManager::Init
============
*/
void owFBXManager::Init()
{
    Com_Printf( "------- owFBXManager::Init ----------\n" );
    
    //Create FBK SDK
    fbxSdkManager = KFbxSdkManager::Create();
    
    // create an IOSettings object
    KFbxIOSettings* ios = KFbxIOSettings::Create( fbxSdkManager, IOSROOT );
    fbxSdkManager->SetIOSettings( ios );
    
    
}

/*
============
owFBXManager::FindSkeleton
============
*/
KFbxNode* owFBXManager::FindSkeleton( int& nodeNum )
{
    KFbxNodeAttribute::EAttributeType lAttributeType;
    for( int i = nodeNum; i < rootNode->GetChildCount(); i++, nodeNum++ )
    {
        KFbxNode* pNode = rootNode->GetChild( i );
        if( pNode->GetNodeAttribute() )
        {
            lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
        }
        else
        {
            lAttributeType = KFbxNodeAttribute::eNULL;
        }
        
        Com_Printf( "Created node: %s\n", pNode->GetName() );
        
        switch( lAttributeType )
        {
            case KFbxNodeAttribute::eSKELETON:
                nodeNum++;
                return pNode;
        }
    }
    
    return NULL;
}

/*
============
owFBXManager::FindMesh
============
*/
KFbxNode* owFBXManager::FindMesh( int& nodeNum )
{
    KFbxNodeAttribute::EAttributeType lAttributeType;
    for( int i = nodeNum; i < rootNode->GetChildCount(); i++, nodeNum++ )
    {
        KFbxNode* pNode = rootNode->GetChild( i );
        if( pNode->GetNodeAttribute() )
        {
            lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
        }
        else
        {
            lAttributeType = KFbxNodeAttribute::eNULL;
        }
        
        switch( lAttributeType )
        {
            case KFbxNodeAttribute::eMESH:
                nodeNum++;
                return pNode;
        }
    }
    
    return NULL;
}

/*
============
owFBXManager::LoadFBXFromFile
============
*/
bool owFBXManager::LoadFBXFromFile( const char* filepath )
{
    Com_Printf( "------- owFBXManager::LoadFBXFromFile ----------\n" );
    Com_Printf( "Loading %s\n", filepath );
    importer = KFbxImporter::Create( fbxSdkManager, "" );
    
    // Create a blank scene to import the fbx into.
    scene = KFbxScene::Create( fbxSdkManager, "" );
    
    // Try to load the FBX file.
    if( !importer->Initialize( filepath, -1, fbxSdkManager->GetIOSettings() ) )
    {
        Com_Printf( "ERROR: Failed to Init FBX file. Ensure the import path is correct. \n" );
        return false;
    }
    
    IOS_REF.SetBoolProp( IMP_FBX_MATERIAL,        true );
    IOS_REF.SetBoolProp( IMP_FBX_TEXTURE,         true );
    IOS_REF.SetBoolProp( IMP_FBX_LINK,            true );
    IOS_REF.SetBoolProp( IMP_FBX_SHAPE,           true );
    IOS_REF.SetBoolProp( IMP_FBX_GOBO,            true );
    IOS_REF.SetBoolProp( IMP_FBX_ANIMATION,       true );
    IOS_REF.SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );
    
    if( !importer->IsFBX() )
    {
        Com_Printf( "ERROR: Import file isn't a valid FBX. \n" );
        return false;
    }
    
    // Impor the FBX into the scene.
    if( !importer->Import( scene ) )
    {
        Com_Printf( "ERROR: Failed to import FBX file. \n" );
        return false;
    }
    
    // Get the root node for the scene.
    rootNode = scene->GetRootNode();
    
    return true;
}
