//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2011-2012 Justin Marshall <jjustinmarshall20@gmail.com>
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
//  File name:   GpuWorker.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description: functions that are not called every frame
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __GPUWORKER_H__
#define __GPUWORKER_H__

#include "GpuWorker_Program.h"

/*
=======================================
GPU Workrer
GPU Wokerer uses OpenCL to pass off a lot of work of various processing intensive operations to the GPU.
This is great for stuff like texture readback, upload of various texture blobs that do not conform with
the standard less-than-par OpenGL texture formats.
=======================================
*/

//
// owGpuWorkerHelper
//
class owGpuWorkerHelper
{
public:
    template <class T>
    static owGPUWorkerProgram*		LoadProgram( const char* path );
};

//
// owGpuWorker
//
class owGpuWorker
{
    friend class owGpuWorkerHelper;
public:
    virtual void					Init( void ) = 0;
    virtual void					Shutdown( void ) = 0;
    
    virtual owGPUWorkerProgram*		FindProgram( const char* path ) = 0;
protected:
    virtual void					AppendProgram( owGPUWorkerProgram* program ) = 0;
};

extern owGpuWorker*				gpuWorker;

/*
======================
owGpuWorkerHelper::LoadProgram
======================
*/
template <class T>
ID_INLINE owGPUWorkerProgram* owGpuWorkerHelper::LoadProgram( const char* path )
{
    T* program = NULL;
    
    // Check to see if the program is already loaded.
    program = ( T* )gpuWorker->FindProgram( path );
    if( program != NULL )
    {
        return program;
    }
    
    // Create the new program object.
    program = new T();
    
    // Load in the worker program(if this fails its a fatal error).
    program->LoadWorkerProgram( path );
    
    // Call the protected function to append the program.
    gpuWorker->AppendProgram( program );
    
    return program;
}

#endif // !__GPUWORKER_H__
