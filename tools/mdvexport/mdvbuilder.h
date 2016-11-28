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

#ifndef __MDVBUILDER_H__
#define __MDVBUILDER_H__

/*
====================
owMDVBuilder
====================
*/
class owMDVBuilder : public MPxFileTranslator
{
public:
    // Alloc
    static void* Alloc()
    {
        return new owMDVBuilder();
    }
    
    MStatus         reader( const MFileObject& file, const MString& optionsString, FileAccessMode mode );
    MStatus         writer( const MFileObject& file, const MString& optionsString, FileAccessMode mode );
    
    bool            haveReadMethod() const;
    bool            haveWriteMethod() const;
    MString         defaultExtension() const;
    MFileKind       identifyFile( const MFileObject& fileName,
                                  const char* buffer,
                                  short size ) const;
private:
    void			Clear();
    void			WriteHeader( FILE* file );
    void			BuildMesh( int framenum );
    void			BuildSurface( MDagPath meshDag, MFnMesh& fnMesh, mdvSurface_t& surface, int framenum );
    MString			GetShaderForMesh( MDagPath path, int instanceNum, MFnMesh& fnMesh );
    MString			FixTextureString( MString textureString );
    MObject			FindShader( MObject& setNode );
    int				GetShaderForSurface( MDagPath path, int instanceNum, MFnMesh& fnMesh );
    void			WriteLump( FILE* file, mdvLump_t& lumptable, void* lump, int numLumps, int size );
protected:
    mdvHeader_t		header;
    int				mintime;
    int				maxtime;
    
    vector<mdvTag_t>	  tags;
    vector<mdvFrame_t>	  frames;
    vector<mdvShader_t>   shaders;
    vector<mdvSurface_t>  surfaces;
    vector<mdvVertex_t>   vertexes;
    vector<short>	      indexes;
};

#endif // !__MDVBUILDER_H__