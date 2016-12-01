//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
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
//  File name:   r_model_md4mesh.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: model loading and caching
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "r_local.h"

#define X 0
#define Y 1
#define Z 2
#define W 3

void Quat_normalize( vec4_t q )
{
    /* Compute magnitude of the quaternion */
    float mag = sqrt( ( q[X] * q[X] ) + ( q[Y] * q[Y] )
                      + ( q[Z] * q[Z] ) + ( q[W] * q[W] ) );
                      
    /* Check for bogus length, to protect against divide by zero */
    if( mag > 0.0f )
    {
        /* Normalize it */
        float oneOverMag = 1.0f / mag;
        
        q[W] *= oneOverMag;
        q[X] *= oneOverMag;
        q[Y] *= oneOverMag;
        q[Z] *= oneOverMag;
    }
}

void Quat_multQuat( const vec4_t qa, const vec4_t qb, vec4_t out )
{
    out[W] = ( qa[W] * qb[W] ) - ( qa[X] * qb[X] ) - ( qa[Y] * qb[Y] ) - ( qa[Z] * qb[Z] );
    out[X] = ( qa[X] * qb[W] ) + ( qa[W] * qb[X] ) + ( qa[Y] * qb[Z] ) - ( qa[Z] * qb[Y] );
    out[Y] = ( qa[Y] * qb[W] ) + ( qa[W] * qb[Y] ) + ( qa[Z] * qb[X] ) - ( qa[X] * qb[Z] );
    out[Z] = ( qa[Z] * qb[W] ) + ( qa[W] * qb[Z] ) + ( qa[X] * qb[Y] ) - ( qa[Y] * qb[X] );
}

void Quat_multVec( const vec4_t q, const vec3_t v, vec4_t out )
{
    out[W] = -( q[X] * v[X] ) - ( q[Y] * v[Y] ) - ( q[Z] * v[Z] );
    out[X] = ( q[W] * v[X] ) + ( q[Y] * v[Z] ) - ( q[Z] * v[Y] );
    out[Y] = ( q[W] * v[Y] ) + ( q[Z] * v[X] ) - ( q[X] * v[Z] );
    out[Z] = ( q[W] * v[Z] ) + ( q[X] * v[Y] ) - ( q[Y] * v[X] );
}

void Quat_rotatePoint( const vec4_t q, const vec3_t in, vec3_t out )
{
    vec4_t tmp, inv, final;
    
    inv[X] = -q[X];
    inv[Y] = -q[Y];
    inv[Z] = -q[Z];
    inv[W] = q[W];
    
    Quat_normalize( inv );
    
    Quat_multVec( q, in, tmp );
    Quat_multQuat( tmp, inv, final );
    
    out[X] = final[X];
    out[Y] = final[Y];
    out[Z] = final[Z];
}
/*
=================
RB_SurfaceMD4Mesh
=================
*/
void RB_SurfaceMD4Mesh( md4MeshSurface_t* surface )
{
    md4MeshVertex_t* vert;
    md4MeshHeader_t* header;
    md4MeshWeight_t* weight;
    md4MeshJoint_t*  bones;
    short* indexes;
    int i, w;//, j;
    
    header = ( md4MeshHeader_t* )surface->header;
    
    vert = ( md4MeshVertex_t* )( ( byte* )header + header->ofsVetexes + ( surface->startVertex * sizeof( md4MeshVertex_t ) ) );
    indexes = ( short* )( ( byte* )header + header->ofsIndexes + ( surface->startIndex * sizeof( short ) ) );
    bones = ( md4MeshJoint_t* )( ( byte* )header + header->ofsJoints + ( 0 * sizeof( md4MeshJoint_t ) ) );
    
    if( backEnd.currentEntity->e.animHandle <= 0 && tr.models[backEnd.currentEntity->e.hModel]->md4Anims[0] != NULL )
    {
        backEnd.currentEntity->e.animHandle = 1; // Default to a base animation if present.
    }
    
    // This will probably give me some kind of headache later on but its faster and more efficient just to do this.
    if( backEnd.currentEntity->e.animHandle > 0 )
    {
        int animHandle;
        md4AnimFrame_t* frame;
        md4AnimHeader_t* anim;
        md4JointTransform_t* jointTransform;
        
        animHandle = backEnd.currentEntity->e.animHandle - 1;
        
        anim = tr.models[backEnd.currentEntity->e.hModel]->md4Anims[animHandle];
        
        frame = ( md4AnimFrame_t* )( ( byte* )anim + anim->ofsFrames + ( backEnd.currentEntity->e.frame * sizeof( md4AnimFrame_t ) ) );
        jointTransform = ( md4JointTransform_t* )( ( byte* )anim + anim->ofsTransforms + ( frame->baseTransform * sizeof( md4JointTransform_t ) ) );
        
        for( i = 0; i < anim->numJoints; i++, jointTransform++ )
        {
            VectorCopy( jointTransform->translate, bones[i].pos );
            VectorCopy( jointTransform->rotation, bones[i].rotation );
            bones[i].rotation[3] = jointTransform->rotation[3];
        }
    }
    
    for( i = 0; i < surface->numIndexes; i++ )
    {
        tess.indexes[tess.numIndexes + i] = indexes[i];
    }
    
    if( r_bonesDebug->integer )
    {
        if( 1 )
        {
            vec3_t vec;
            // DEBUG: show the bones as a stick figure with axis at each bone
            
            for( i = 0; i < header->numJoints; i++ )
            {
                GL_Bind( tr.whiteImage );
                glPointSize( 10 );
                if( bones[i].parentnum == -1 )
                {
                    glColor3f( 1, 0, 0 );
                }
                else
                {
                    glColor3f( 0, 0, 1 );
                }
                glBegin( GL_POINTS );
                vec[0] = bones[i].pos[0];
                vec[1] = bones[i].pos[1];
                vec[2] = bones[i].pos[2];
                glVertex3fv( vec );
                glEnd();
                glPointSize( 1 );
                
                if( bones[i].parentnum == -1 )
                {
                    continue;
                }
                
                GL_Bind( tr.whiteImage );
                glLineWidth( 2 );
                glBegin( GL_LINES );
                glColor3f( 1, 1, 1 );
                vec[0] = bones[i].pos[0];
                vec[1] = bones[i].pos[1];
                vec[2] = bones[i].pos[2];
                glVertex3fv( vec );
                
                glColor3f( 0, 1, 0 );
                vec[0] = bones[bones[i].parentnum].pos[0];
                vec[1] = bones[bones[i].parentnum].pos[1];
                vec[2] = bones[bones[i].parentnum].pos[2];
                glVertex3fv( vec );
                
                vec[0] = bones[i].pos[0] + 2;
                vec[1] = bones[i].pos[1] + 2;
                vec[2] = bones[i].pos[2] + 2;
                glVertex3fv( vec );
                glEnd();
                
                glLineWidth( 1 );
            }
        }
    }
    
    for( i = 0; i < surface->numVertexes; i++ )
    {
        VectorClear( tess.xyz[tess.numVertexes + i] );
        
        weight = ( md4MeshWeight_t* )( ( byte* )header + header->ofsWeights + ( vert[i].startweight * sizeof( md4MeshWeight_t ) ) );
        for( w = 0; w < vert[i].numWeights; w++, weight++ )
        {
            vec3_t wv;
            bones = ( md4MeshJoint_t* )( ( byte* )header + header->ofsJoints + ( weight->jointNum * sizeof( md4MeshJoint_t ) ) );
            
            //LocalVectorMA(tess.xyz[tess.numVertexes + i], weight->jointWeight, weight->offset, tess.xyz[tess.numVertexes + i]);
            Quat_rotatePoint( bones->rotation, weight->offset, wv );
            
            tess.xyz[tess.numVertexes + i][0] += ( bones->pos[0] + wv[0] ) * weight->jointWeight;
            tess.xyz[tess.numVertexes + i][1] += ( bones->pos[1] + wv[1] ) * weight->jointWeight;
            tess.xyz[tess.numVertexes + i][2] += ( bones->pos[2] + wv[2] ) * weight->jointWeight;
        }
        
        tess.texCoords[tess.numVertexes + i][0][0] = vert[i].texCoords[0];
        tess.texCoords[tess.numVertexes + i][0][1] = vert[i].texCoords[1];
    }
    
    tess.numIndexes += surface->numIndexes;
    tess.numVertexes += surface->numVertexes;
}

/*
=================
R_AddMD4MeshSurfaces
=================
*/
void R_AddMD4MeshSurfaces( trRefEntity_t* ent )
{
    md4MeshHeader_t* header;
    md4MeshSurface_t* surface;
    shader_t*	shader;
    int i, fogNum, j;//,cull;
    bool personalModel;
    
    fogNum = 0;
    
    // don't add third_person objects if not in a portal
    personalModel = ( ent->e.renderfx & RF_THIRD_PERSON ) && !tr.viewParms.isPortal;
    
    if( ent->e.renderfx & RF_WRAP_FRAMES )
    {
        //ent->e.frame %= tr.currentModel->md4mesh->numframes;
        //	ent->e.oldframe %= tr.currentModel->mdv->numframes;
    }
    
    //
    // Validate the frames so there is no chance of a crash.
    // This will write directly into the entity structure, so
    // when the surfaces are rendered, they don't need to be
    // range checked again.
    //
    /*
    if ( ( ent->e.frame >= tr.currentModel->mdv->numframes )
    || ( ent->e.frame < 0 )
    || ( ent->e.oldframe >= tr.currentModel->mdv->numframes)
    || ( ent->e.oldframe < 0 ) ) {
    Com_DPrintf( "R_AddMD3Surfaces: no such frame %d to %d for '%s'\n",
    ent->e.oldframe, ent->e.frame,
    tr.currentModel->name );
    ent->e.frame = 0;
    ent->e.oldframe = 0;
    }
    */
    header = tr.currentModel->md4mesh;
    
    //
    // cull the entire model if merged bounding box of both frames
    // is outside the view frustum.
    //
    //cull = R_CullModel( header, ent );
    //if ( cull == CULL_OUT ) {
    //	return;
    //}
    
    //
    // set up lighting now that we know we aren't culled
    //
    if( !personalModel || r_shadows->integer > 1 )
    {
        R_SetupEntityLighting( &tr.refdef, ent );
    }
    
    surface = ( md4MeshSurface_t* )( ( byte* )header + header->ofsSurface );
    for( i = 0; i < header->numSurfaces; i++, surface++ )
    {
        if( ent->e.customShader )
        {
            shader = R_GetShaderByHandle( ent->e.customShader );
        }
        else if( ent->e.customSkin > 0 && ent->e.customSkin < tr.numSkins )
        {
            skin_t* skin;
            
            skin = R_GetSkinByHandle( ent->e.customSkin );
            
            // match the surface name to something in the skin file
            shader = tr.defaultShader;
            for( j = 0; j < skin->numSurfaces; j++ )
            {
                // the names have both been lowercased
                if( !strcmp( skin->surfaces[j]->name, surface->name ) )
                {
                    shader = skin->surfaces[j]->shader;
                    break;
                }
            }
            
            if( shader == tr.defaultShader )
            {
                Com_Printf( "WARNING: no shader for surface %s in skin %s\n", surface->name, skin->name );
            }
            else if( shader->defaultShader )
            {
                Com_Printf( "WARNING: shader %s in skin %s not found\n", shader->name, skin->name );
            }
        }
        else
        {
            shader = R_GetShaderByHandle( surface->shadernum );
        }
        
        // stencil shadows can't do personal models unless I polyhedron clip
        if( !personalModel
                && r_shadows->integer == 2
                && fogNum == 0
                && !( ent->e.renderfx & ( RF_NOSHADOW | RF_DEPTHHACK ) )
                && shader->sort == SS_OPAQUE )
        {
            // GR - tessellate according to model capabilities
            R_AddDrawSurf( ( surfaceType_t* )surface, tr.shadowShader, 0, false, tr.currentModel->ATI_tess );
        }
        
        // don't add third_person objects if not viewing through a portal
        if( !personalModel )
        {
            // GR - tessellate according to model capabilities
            R_AddDrawSurf( ( surfaceType_t* )surface, shader, fogNum, false, tr.currentModel->ATI_tess );
        }
    }
}
