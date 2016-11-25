//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2011-2012 Justin Marshall <justinmarshall20@gmail.com>
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
//  File name:   GpuWorker_Program.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __GPUWORKER_PROGGRAM_H__
#define __GPUWORKER_PROGGRAM_H__

#include "../tr_local.h"
#include "../../splines/util_str.h"
#include <cl/cl_platform.h>


#define GPUWORKER_FOLDER "renderWorkerProgs"
#define GPUWORKER_EXT ".renderworkerprog"

// ----------------------------------
typedef void* 			gpuWorkerProgramHandle_t;
typedef void* 			gpuWorkerMemoryPtr_t;
typedef void* 			gpuWorkerKernelHandle_t;
// ----------------------------------

//
// owGPUWorkerProgram
//
class owGPUWorkerProgram
{
public:
    // Loads in the worker program.
    virtual void					LoadWorkerProgram( const char* path );
    // Frees the render program handles.
    virtual void					Free( void );
    // Returns the name of the current worker program.
    const char* 					Name()
    {
        return name.c_str();
    };
    // strings in case of the error.
    const char*                     clErrorString( cl_int err );
protected:
    gpuWorkerProgramHandle_t		deviceHandle;		// Handle to the gpu program device handle.
    // Uploads memory to the GPU.
    virtual void					UploadMemory( gpuWorkerMemoryPtr_t memhandle, void* data, int size );
    // Creates a kernel handle(a kernel a function were going to execute at some point).
    virtual gpuWorkerKernelHandle_t	CreateKernel( const char* kernelName );
private:
    idStr							name;
};

#endif // !__GPUWORKER_PROGGRAM_H__
