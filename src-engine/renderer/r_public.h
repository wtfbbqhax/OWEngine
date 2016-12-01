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
//  File name:   r_public.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __R_PUBLIC_H__
#define __R_PUBLIC_H__

#include "r_types.h"
#include "../qcommon/surfaceflags.h"

#define GLYPH_START 0
#define GLYPH_END 255
#define GLYPH_CHARSTART 32
#define GLYPH_CHAREND 127
#define GLYPHS_PER_FONT GLYPH_END - GLYPH_START + 1
typedef struct
{
    int height;     // number of scan lines
    int top;        // top of glyph in buffer
    int bottom;     // bottom of glyph in buffer
    int pitch;      // width for copying
    int xSkip;      // x adjustment
    int imageWidth; // width of actual image
    int imageHeight; // height of actual image
    float s;        // x offset in image where glyph starts
    float t;        // y offset in image where glyph starts
    float s2;
    float t2;
    qhandle_t glyph; // handle to the shader with the glyph
    char shaderName[32];
} glyphInfo_t;

typedef struct
{
    glyphInfo_t glyphs [GLYPHS_PER_FONT];
    float glyphScale;
    char name[MAX_QPATH];
} fontInfo_t;


//
// idRenderSystem
//
class idRenderSystem
{
public:
    // called before the library is unloaded
    // if the system is just reconfiguring, pass destroyWindow = qfalse,
    // which will keep the screen from flashing to the desktop.
    virtual void Shutdown( bool destroyWindow ) = 0;
    
    // All data that will be used in a level should be
    // registered before rendering any frames to prevent disk hits,
    // but they can still be registered at a later time
    // if necessary.
    //
    // Init makes any existing media pointers invalid
    // and returns the current gl configuration, including screen width
    // and height, which can be used by the client to intelligently
    // size display elements
    virtual void Init( glconfig_t* config ) = 0;
    virtual qhandle_t RegisterModel( const char* name ) = 0;
    virtual qhandle_t RegisterSkin( const char* name ) = 0;
    virtual qhandle_t RegisterShader( const char* name ) = 0;
    virtual qhandle_t RegisterShaderNoMip( const char* name ) = 0;
    virtual void LoadWorld( const char* name ) = 0;
    virtual bool GetSkinModel( qhandle_t skinid, const char* type, char* name ) = 0;     //----(SA)	added
    virtual qhandle_t GetShaderFromModel( qhandle_t modelid, int surfnum, int withlightmap ) = 0;                 //----(SA)	added
    virtual qhandle_t RegisterShaderLightMap( const char* name, int lightmapIndex ) = 0;
    
    // the vis data is a large enough block of data that we go to the trouble
    // of sharing it with the clipmodel subsystem
    virtual void SetWorldVisData( const byte* vis ) = 0;
    
    // a scene is built up by calls to R_ClearScene and the various R_Add functions.
    // Nothing is drawn until R_RenderScene is called.
    virtual void ClearScene( void ) = 0;
    virtual void AddRefEntityToScene( const refEntity_t* re ) = 0;
    virtual int LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir ) = 0;
    virtual void AddPolyToScene( qhandle_t hShader, int numVerts, const polyVert_t* verts ) = 0;
    // Ridah
    virtual void AddPolysToScene( qhandle_t hShader, int numVerts, const polyVert_t* verts, int numPolys ) = 0;
    // done.
    virtual void AddLightToScene( const vec3_t org, float intensity, float r, float g, float b, int overdraw ) = 0;
//----(SA)
    virtual void AddCoronaToScene( const vec3_t org, float r, float g, float b, float scale, int id, int flags ) = 0;
    virtual void SetFog( int fogvar, int var1, int var2, float r, float g, float b, float density ) = 0;
//----(SA)
    virtual void RenderScene( const refdef_t* fd ) = 0;
    
    virtual void SetColor( const float* rgba ) = 0;     // NULL = 1,1,1,1
    virtual void DrawStretchPic( float x, float y, float w, float h,
                                 float s1, float t1, float s2, float t2, qhandle_t hShader ) = 0; // 0 = white
    virtual void DrawStretchPicGradient( float x, float y, float w, float h,
                                         float s1, float t1, float s2, float t2, qhandle_t hShader, const float* gradientColor, int gradientType ) = 0;
                                         
    // Draw images for cinematic rendering, pass as 32 bit rgba
    virtual void DrawStretchRaw( int x, int y, int w, int h, int cols, int rows, const byte* data, int client, bool dirty ) = 0;
    virtual void UploadCinematic( int w, int h, int cols, int rows, const byte* data, int client, bool dirty ) = 0;
    
    virtual void BeginFrame( stereoFrame_t stereoFrame ) = 0;
    
    // if the pointers are not NULL, timing info will be returned
    virtual void EndFrame( int* frontEndMsec, int* backEndMsec ) = 0;
    
    
    virtual int MarkFragments( int numPoints, const vec3_t* points, const vec3_t projection,
                               int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t* fragmentBuffer ) = 0;
                               
    virtual int LerpTag( orientation_t* tag,  const refEntity_t* refent, const char* tagName, int startIndex ) = 0;
    virtual void ModelBounds( qhandle_t model, vec3_t mins, vec3_t maxs ) = 0;
    
    virtual void RegisterFont( const char* fontName, int pointSize, fontInfo_t* font ) = 0;
    virtual void RemapShader( const char* oldShader, const char* newShader, const char* offsetTime ) = 0;
    // RF
    virtual void ZombieFXAddNewHit( int entityNum, const vec3_t hitPos, const vec3_t hitDir ) = 0;
    
    virtual bool GetEntityToken( char* buffer, int size ) = 0;
    
    virtual qhandle_t LoadAnim( qhandle_t modelHandle, const char* name ) = 0;
    
    virtual void DeleteTextures( void ) = 0;
};

extern idRenderSystem* renderSystem;

#endif // !__R_PUBLIC_H__
