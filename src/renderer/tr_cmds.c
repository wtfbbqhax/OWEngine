//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
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
//  File name:   tr_cmds.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "tr_local.h"

volatile renderCommandList_t*    renderCommandList;

volatile qboolean renderThreadActive;

#if !defined( __ANDROID__ )
extern backEndState_t	backEnd;
extern void RB_SetGL2D( void );
#endif

/*
=====================
R_PerformanceCounters
=====================
*/
void R_PerformanceCounters( void )
{
    if( !r_speeds->integer )
    {
        // clear the counters even if we aren't printing
        memset( &tr.pc, 0, sizeof( tr.pc ) );
        memset( &backEnd.pc, 0, sizeof( backEnd.pc ) );
        return;
    }
    
    if( r_speeds->integer == 1 )
    {
        ri.Printf( PRINT_ALL, "%i/%i shaders/surfs %i leafs %i verts %i/%i tris %.2f mtex %.2f dc\n",
                   backEnd.pc.c_shaders, backEnd.pc.c_surfaces, tr.pc.c_leafs, backEnd.pc.c_vertexes,
                   backEnd.pc.c_indexes / 3, backEnd.pc.c_totalIndexes / 3,
                   R_SumOfUsedImages() / ( 1000000.0f ), backEnd.pc.c_overDraw / ( float )( glConfig.vidWidth * glConfig.vidHeight ) );
    }
    else if( r_speeds->integer == 2 )
    {
        ri.Printf( PRINT_ALL, "(patch) %i sin %i sclip  %i sout %i bin %i bclip %i bout\n",
                   tr.pc.c_sphere_cull_patch_in, tr.pc.c_sphere_cull_patch_clip, tr.pc.c_sphere_cull_patch_out,
                   tr.pc.c_box_cull_patch_in, tr.pc.c_box_cull_patch_clip, tr.pc.c_box_cull_patch_out );
        ri.Printf( PRINT_ALL, "(md3) %i sin %i sclip  %i sout %i bin %i bclip %i bout\n",
                   tr.pc.c_sphere_cull_md3_in, tr.pc.c_sphere_cull_md3_clip, tr.pc.c_sphere_cull_md3_out,
                   tr.pc.c_box_cull_md3_in, tr.pc.c_box_cull_md3_clip, tr.pc.c_box_cull_md3_out );
    }
    else if( r_speeds->integer == 3 )
    {
        ri.Printf( PRINT_ALL, "viewcluster: %i\n", tr.viewCluster );
    }
    else if( r_speeds->integer == 4 )
    {
        if( backEnd.pc.c_dlightVertexes )
        {
            ri.Printf( PRINT_ALL, "dlight srf:%i  culled:%i  verts:%i  tris:%i\n",
                       tr.pc.c_dlightSurfaces, tr.pc.c_dlightSurfacesCulled,
                       backEnd.pc.c_dlightVertexes, backEnd.pc.c_dlightIndexes / 3 );
        }
    }
//----(SA)	this is unnecessary since it will always show 2048.  I moved this to where it is accurate for the world
//	else if (r_speeds->integer == 5 )
//	{
//		ri.Printf( PRINT_ALL, "zFar: %.0f\n", tr.viewParms.zFar );
//	}
    else if( r_speeds->integer == 6 )
    {
        ri.Printf( PRINT_ALL, "flare adds:%i tests:%i renders:%i\n",
                   backEnd.pc.c_flareAdds, backEnd.pc.c_flareTests, backEnd.pc.c_flareRenders );
    }
    
    memset( &tr.pc, 0, sizeof( tr.pc ) );
    memset( &backEnd.pc, 0, sizeof( backEnd.pc ) );
}


/*
====================
R_InitCommandBuffers
====================
*/
void R_InitCommandBuffers( void )
{
    glConfig.smpActive = qfalse;
    if( r_smp->integer )
    {
        ri.Printf( PRINT_ALL, "Trying SMP acceleration...\n" );
        if( GLimp_SpawnRenderThread( RB_RenderThread ) )
        {
            ri.Printf( PRINT_ALL, "...succeeded.\n" );
            glConfig.smpActive = qtrue;
        }
        else
        {
            ri.Printf( PRINT_ALL, "...failed.\n" );
        }
    }
}

/*
====================
R_ShutdownCommandBuffers
====================
*/
void R_ShutdownCommandBuffers( void )
{
    // kill the rendering thread
    if( glConfig.smpActive )
    {
        GLimp_WakeRenderer( ( void* )0xdead );
        glConfig.smpActive = qfalse;
    }
}

/*
====================
R_IssueRenderCommands
====================
*/
int c_blockedOnRender;
int c_blockedOnMain;

void R_IssueRenderCommands( qboolean runPerformanceCounters )
{
    renderCommandList_t* cmdList;
    
    cmdList = &backEndData[tr.smpFrame]->commands;
    assert( cmdList ); // bk001205
    // add an end-of-list command
    *( int* )( cmdList->cmds + cmdList->used ) = RC_END_OF_LIST;
    
    // clear it out, in case this is a sync and not a buffer flip
    cmdList->used = 0;
    
    if( glConfig.smpActive )
    {
        // if the render thread is not idle, wait for it
        if( renderThreadActive )
        {
            c_blockedOnRender++;
            if( r_showSmp->integer )
            {
                ri.Printf( PRINT_ALL, "R" );
            }
        }
        else
        {
            c_blockedOnMain++;
            if( r_showSmp->integer )
            {
                ri.Printf( PRINT_ALL, "." );
            }
        }
        
        // sleep until the renderer has completed
        GLimp_FrontEndSleep();
    }
    
    // at this point, the back end thread is idle, so it is ok
    // to look at it's performance counters
    if( runPerformanceCounters )
    {
        R_PerformanceCounters();
    }
    
    // actually start the commands going
    if( !r_skipBackEnd->integer )
    {
        // let it start on the new batch
        if( !glConfig.smpActive )
        {
            RB_ExecuteRenderCommands( cmdList->cmds );
        }
        else
        {
            GLimp_WakeRenderer( cmdList );
        }
    }
}


/*
====================
R_SyncRenderThread

Issue any pending commands and wait for them to complete.
After exiting, the render thread will have completed its work
and will remain idle and the main thread is free to issue
OpenGL calls until R_IssueRenderCommands is called.
====================
*/
void R_SyncRenderThread( void )
{
    if( !tr.registered )
    {
        return;
    }
    R_IssueRenderCommands( qfalse );
    
    if( !glConfig.smpActive )
    {
        return;
    }
    GLimp_FrontEndSleep();
}

/*
============
R_GetCommandBuffer

make sure there is enough command space, waiting on the
render thread if needed.
============
*/
void* R_GetCommandBuffer( int bytes )
{
    renderCommandList_t* cmdList;
    
    cmdList = &backEndData[tr.smpFrame]->commands;
    
    // always leave room for the end of list command
    if( cmdList->used + bytes + 4 > MAX_RENDER_COMMANDS )
    {
        if( bytes > MAX_RENDER_COMMANDS - 4 )
        {
            ri.Error( ERR_FATAL, "R_GetCommandBuffer: bad size %i", bytes );
        }
        // if we run out of room, just start dropping commands
        return NULL;
    }
    
    cmdList->used += bytes;
    
    return cmdList->cmds + cmdList->used - bytes;
}


/*
=============
R_AddDrawSurfCmd

=============
*/
void    R_AddDrawSurfCmd( drawSurf_t* drawSurfs, int numDrawSurfs )
{
    drawSurfsCommand_t*  cmd;
    
    cmd = R_GetCommandBuffer( sizeof( *cmd ) );
    if( !cmd )
    {
        return;
    }
    cmd->commandId = RC_DRAW_SURFS;
    
    cmd->drawSurfs = drawSurfs;
    cmd->numDrawSurfs = numDrawSurfs;
    
    cmd->refdef = tr.refdef;
    cmd->viewParms = tr.viewParms;
}


/*
=============
RE_SetColor

Passing NULL will set the color to white
=============
*/
void    RE_SetColor( const float* rgba )
{
    setColorCommand_t*   cmd;
    
    cmd = R_GetCommandBuffer( sizeof( *cmd ) );
    if( !cmd )
    {
        return;
    }
    cmd->commandId = RC_SET_COLOR;
    if( !rgba )
    {
        static float colorWhite[4] = { 1, 1, 1, 1 };
        
        rgba = colorWhite;
    }
    
    cmd->color[0] = rgba[0];
    cmd->color[1] = rgba[1];
    cmd->color[2] = rgba[2];
    cmd->color[3] = rgba[3];
}


/*
=============
RE_StretchPic
=============
*/
void RE_StretchPic( float x, float y, float w, float h,
                    float s1, float t1, float s2, float t2, qhandle_t hShader )
{
    stretchPicCommand_t* cmd;
    
    cmd = R_GetCommandBuffer( sizeof( *cmd ) );
    if( !cmd )
    {
        return;
    }
    cmd->commandId = RC_STRETCH_PIC;
    cmd->shader = R_GetShaderByHandle( hShader );
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->s1 = s1;
    cmd->t1 = t1;
    cmd->s2 = s2;
    cmd->t2 = t2;
}


//----(SA)	added
/*
==============
RE_StretchPicGradient
==============
*/
void RE_StretchPicGradient( float x, float y, float w, float h,
                            float s1, float t1, float s2, float t2, qhandle_t hShader, const float* gradientColor, int gradientType )
{
    stretchPicCommand_t* cmd;
    
    cmd = R_GetCommandBuffer( sizeof( *cmd ) );
    if( !cmd )
    {
        return;
    }
    cmd->commandId = RC_STRETCH_PIC_GRADIENT;
    cmd->shader = R_GetShaderByHandle( hShader );
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->s1 = s1;
    cmd->t1 = t1;
    cmd->s2 = s2;
    cmd->t2 = t2;
    
    if( !gradientColor )
    {
        static float colorWhite[4] = { 1, 1, 1, 1 };
        
        gradientColor = colorWhite;
    }
    
    cmd->gradientColor[0] = gradientColor[0] * 255;
    cmd->gradientColor[1] = gradientColor[1] * 255;
    cmd->gradientColor[2] = gradientColor[2] * 255;
    cmd->gradientColor[3] = gradientColor[3] * 255;
    cmd->gradientType = gradientType;
}
//----(SA)	end


/*
====================
RE_BeginFrame

If running in stereo, RE_BeginFrame will be called twice
for each RE_EndFrame
====================
*/
void RE_BeginFrame( stereoFrame_t stereoFrame )
{
    drawBufferCommand_t* cmd = NULL;
    
#if !defined( __ANDROID__ )
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, glConfig.oculusFBL );
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, glConfig.oculusDepthRenderBufferLeft );
    
    glViewport( 0, 0, glConfig.vidWidth, glConfig.vidHeight );
    
    if( ovr_warpingShader->integer )
    {
        if( stereoFrame == STEREO_LEFT )
        {
            glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
        }
        else
        {
            glDrawBuffer( GL_COLOR_ATTACHMENT1_EXT );
        }
    }
#endif
    
    if( !tr.registered )
    {
        return;
    }
    glState.finishCalled = qfalse;
    
    tr.frameCount++;
    tr.frameSceneNum = 0;
    
    //
    // do overdraw measurement
    //
    if( r_measureOverdraw->integer )
    {
        if( glConfig.stencilBits < 4 )
        {
            ri.Printf( PRINT_ALL, "Warning: not enough stencil bits to measure overdraw: %d\n", glConfig.stencilBits );
            ri.Cvar_Set( "r_measureOverdraw", "0" );
            r_measureOverdraw->modified = qfalse;
        }
        else if( r_shadows->integer == 2 )
        {
            ri.Printf( PRINT_ALL, "Warning: stencil shadows and overdraw measurement are mutually exclusive\n" );
            ri.Cvar_Set( "r_measureOverdraw", "0" );
            r_measureOverdraw->modified = qfalse;
        }
        else
        {
            R_SyncRenderThread();
            glEnable( GL_STENCIL_TEST );
            glStencilMask( ~0U );
            glClearStencil( 0U );
            glStencilFunc( GL_ALWAYS, 0U, ~0U );
            glStencilOp( GL_KEEP, GL_INCR, GL_INCR );
        }
        r_measureOverdraw->modified = qfalse;
    }
    else
    {
        // this is only reached if it was on and is now off
        if( r_measureOverdraw->modified )
        {
            R_SyncRenderThread();
            glDisable( GL_STENCIL_TEST );
        }
        r_measureOverdraw->modified = qfalse;
    }
    
    //
    // texturemode stuff
    //
    if( r_textureMode->modified )
    {
        R_SyncRenderThread();
        GL_TextureMode( r_textureMode->string );
        r_textureMode->modified = qfalse;
    }
    
    //
    // ATI stuff
    //
    
    
    //
    // NVidia stuff
    //
    
    
    //
    // gamma stuff
    //
    if( r_gamma->modified )
    {
        r_gamma->modified = qfalse;
        
        R_SyncRenderThread();
        R_SetColorMappings();
    }
    
    // check for errors
    if( !r_ignoreGLErrors->integer )
    {
        int err;
        
        R_SyncRenderThread();
        if( ( err = glGetError() ) != GL_NO_ERROR )
        {
            ri.Error( ERR_FATAL, "RE_BeginFrame() - glGetError() failed (0x%x)!\n", err );
        }
    }
    
    //
    // draw buffer stuff
    //
    if( !( cmd = R_GetCommandBuffer( sizeof( *cmd ) ) ) )
        return;
        
    cmd->commandId = RC_DRAW_BUFFER;
    
    if( stereoFrame == STEREO_LEFT )
    {
        cmd->buffer = ( int )GL_BACK_LEFT;
    }
    else if( stereoFrame == STEREO_RIGHT )
    {
        cmd->buffer = ( int )GL_BACK_RIGHT;
    }
    else
    {
        ri.Error( ERR_FATAL, "RE_BeginFrame: Stereo is enabled, but stereoFrame was %i", stereoFrame );
    }
    tr.refdef.stereoFrame = stereoFrame;
}

#if !defined( __ANDROID__ )
void SetupShaderDistortion( int eye, float VPX, float VPY, float VPW, float VPH )
{
    float  as, x, y, w, h;
    struct OculusVR_StereoCfg stereoCfg;
    GLuint lenscenter = glGetUniformLocation( glConfig.oculusProgId, "LensCenter" );
    GLuint screencenter = glGetUniformLocation( glConfig.oculusProgId, "ScreenCenter" );
    GLuint uscale = glGetUniformLocation( glConfig.oculusProgId, "Scale" );
    GLuint uscalein = glGetUniformLocation( glConfig.oculusProgId, "ScaleIn" );
    GLuint uhmdwarp = glGetUniformLocation( glConfig.oculusProgId, "HmdWarpParam" );
    GLuint offset = glGetUniformLocation( glConfig.oculusProgId, "Offset" );
    
    stereoCfg.x = VPX;
    stereoCfg.y = VPY;
    stereoCfg.w = VPW * 0.5f;
    stereoCfg.h = VPH;
    
    as = ( VPW * 0.5f ) / VPH;
    x = VPX / ( float )( glConfig.vidWidth );
    y = VPY / ( float )( glConfig.vidHeight );
    w = VPW / ( float )( glConfig.vidWidth );
    h = VPH / ( float )( glConfig.vidHeight );
    
    if( OculusVRDetected )
    {
        ri.OculusVR_StereoConfig( eye, &stereoCfg );
    }
    else
    {
        stereoCfg.distscale = 1.701516f;
        stereoCfg.XCenterOffset = 0.145299f;
        if( eye == 1 )
        {
            stereoCfg.XCenterOffset *= -1;
        }
        stereoCfg.K[0] = 1.00f;
        stereoCfg.K[1] = 0.22f;
        stereoCfg.K[2] = 0.24f;
        stereoCfg.K[3] = 0.00f;
    }
    
    glUniform2f( lenscenter, x + ( w + stereoCfg.XCenterOffset * ovr_lenseoffset->value ) * 0.5f, y + h * 0.5f );
    glUniform2f( screencenter, x + w * 0.5f, y + h * 0.5f );
    if( eye == 1 )
        glUniform2f( offset, ovr_viewofsx->value, ovr_viewofsy->value );
    else
        glUniform2f( offset, -ovr_viewofsx->value, ovr_viewofsy->value );
        
    stereoCfg.distscale = 1.0f / stereoCfg.distscale;
    
    glUniform2f( uscale, ( w / 2 ) * stereoCfg.distscale, ( h / 2 ) * stereoCfg.distscale * as );
    glUniform2f( uscalein, ( 2 / w ), ( 2 / h ) / as );
    glUniform4fv( uhmdwarp, 1, stereoCfg.K );
    
    if( ovr_ipd->value != 0.0 )
    {
        HMD.InterpupillaryDistance = ovr_ipd->value;
    }
    
}
#endif


/*
=============
RE_EndFrame

Returns the number of msec spent in the back end
=============
*/
void RE_EndFrame( int* frontEndMsec, int* backEndMsec )
{
    swapBuffersCommand_t*    cmd;
#if !defined( __ANDROID__ )
    float x;
    float y;
    float w;
    float h;
    GLuint texID;
    static int b = 0;
#endif
    
    if( !tr.registered )
    {
        return;
    }
    cmd = R_GetCommandBuffer( sizeof( *cmd ) );
    if( !cmd )
    {
        return;
    }
    cmd->commandId = RC_SWAP_BUFFERS;
    
    R_IssueRenderCommands( qtrue );
    
#if !defined( __ANDROID__ )
    //*** Rift post processing
    if( ovr_warpingShader->integer )
    {
        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
        glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
        
        if( !backEnd.projection2D )
        {
            RB_SetGL2D();
        }
        
        glViewport( 0, 0, glConfig.vidWidth, glConfig.vidHeight ); // Render on the whole framebuffer, complete from the lower left corner to the upper right
        
        // Use our shader
        glUseProgram( glConfig.oculusProgId );
        
        
        glEnable( GL_TEXTURE_2D );
        
        
        {
            float VPX = 0.0f;
            float VPY = 0.0f;
            float VPW = glConfig.vidWidth; // ViewPort Width
            float VPH = glConfig.vidHeight;
            
            SetupShaderDistortion( 0, VPX, VPY, VPW, VPH ); // Left Eye
        }
        
        // Set our "renderedTexture" sampler to user Texture Unit 0
        texID = glGetUniformLocation( glConfig.oculusProgId, "texid" );
        glUniform1i( texID, 0 );
        
        
        glColor3f( tr.identityLight, tr.identityLight, tr.identityLight );
        
        //	if (stereoFrame == STEREO_LEFT)
        {
        
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_2D, glConfig.oculusRenderTargetLeft );
            
            x = 0.0f;
            y = 0.0f;
            w = glConfig.vidWidth;
            h = glConfig.vidHeight;
            
            
            glBegin( GL_QUADS );
            glTexCoord2f( 0, 1 );
            glVertex2f( x, y );
            glTexCoord2f( 1, 1 );
            glVertex2f( x + w / 2, y );
            glTexCoord2f( 1, 0 );
            glVertex2f( x + w / 2, y + h );
            glTexCoord2f( 0, 0 );
            glVertex2f( x, y + h );
            glEnd();
        }
        //else
        {
        
        
            {
                float VPX = 0;
                float VPY = 0.0f;
                float VPW = glConfig.vidWidth; // ViewPort Width
                float VPH = glConfig.vidHeight;
                
                SetupShaderDistortion( 1, VPX, VPY, VPW, VPH ); // Right Eye
            }
            
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_2D, glConfig.oculusRenderTargetRight );
            
            
            x = glConfig.vidWidth * 0.5f;
            y = 0.0f;
            w = glConfig.vidWidth;
            h = glConfig.vidHeight;
            
            
            glBegin( GL_QUADS );
            glTexCoord2f( 0, 1 );
            glVertex2f( x, y );
            glTexCoord2f( 1, 1 );
            glVertex2f( x + w / 2, y );
            glTexCoord2f( 1, 0 );
            glVertex2f( x + w / 2, y + h );
            glTexCoord2f( 0, 0 );
            glVertex2f( x, y + h );
            glEnd();
        }
        // unbind the GLSL program
        // this means that from here the OpenGL fixed functionality is used
        glUseProgram( 0 );
    }
#endif
    
    // use the other buffers next frame, because another CPU
    // may still be rendering into the current ones
    R_ToggleSmpFrame();
    
    if( frontEndMsec )
    {
        *frontEndMsec = tr.frontEndMsec;
    }
    tr.frontEndMsec = 0;
    if( backEndMsec )
    {
        *backEndMsec = backEnd.pc.msec;
    }
    backEnd.pc.msec = 0;
}

