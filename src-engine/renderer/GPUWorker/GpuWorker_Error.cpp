//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
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
//  File name:   GpuWorker_Error.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description: functions that are not called every frame
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../r_local.h"
#include <cl/cl.h>

const char* owGPUWorkerProgram::clErrorString( cl_int err )
{
    const char* errString = NULL;
    
    switch( err )
    {
        case CL_SUCCESS:
            errString = "Success";
            break;
            
        case CL_DEVICE_NOT_FOUND:
            errString = "Device not found";
            break;
            
        case CL_DEVICE_NOT_AVAILABLE:
            errString = "Device not available";
            break;
            
        case CL_COMPILER_NOT_AVAILABLE:
            errString = "Compiler not available";
            break;
            
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            errString = "Memory object allocation failure";
            break;
            
        case CL_OUT_OF_RESOURCES:
            errString = "Out of resources";
            break;
            
        case CL_OUT_OF_HOST_MEMORY:
            errString = "Out of host memory";
            break;
            
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            errString = "Profiling info not available";
            break;
            
        case CL_MEM_COPY_OVERLAP:
            errString = "Memory copy overlap";
            break;
            
        case CL_IMAGE_FORMAT_MISMATCH:
            errString = "Image format mismatch";
            break;
            
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            errString = "Image format not supported";
            break;
            
        case CL_BUILD_PROGRAM_FAILURE:
            errString = "Build program failure";
            break;
            
        case CL_MAP_FAILURE:
            errString = "Map failure";
            break;
            
        case CL_INVALID_VALUE:
            errString = "Invalid value";
            break;
            
        case CL_INVALID_DEVICE_TYPE:
            errString = "Invalid device type";
            break;
            
        case CL_INVALID_PLATFORM:
            errString = "Invalid platform";
            break;
            
        case CL_INVALID_DEVICE:
            errString = "Invalid device";
            break;
            
        case CL_INVALID_CONTEXT:
            errString = "Invalid context";
            break;
            
        case CL_INVALID_QUEUE_PROPERTIES:
            errString = "Invalid queue properties";
            break;
            
        case CL_INVALID_COMMAND_QUEUE:
            errString = "Invalid command queue";
            break;
            
        case CL_INVALID_HOST_PTR:
            errString = "Invalid host pointer";
            break;
            
        case CL_INVALID_MEM_OBJECT:
            errString = "Invalid memory object";
            break;
            
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            errString = "Invalid image format descriptor";
            break;
            
        case CL_INVALID_IMAGE_SIZE:
            errString = "Invalid image size";
            break;
            
        case CL_INVALID_SAMPLER:
            errString = "Invalid sampler";
            break;
            
        case CL_INVALID_BINARY:
            errString = "Invalid binary";
            break;
            
        case CL_INVALID_BUILD_OPTIONS:
            errString = "Invalid build options";
            break;
            
        case CL_INVALID_PROGRAM:
            errString = "Invalid program";
            break;
            
        case CL_INVALID_PROGRAM_EXECUTABLE:
            errString = "Invalid program executable";
            break;
            
        case CL_INVALID_KERNEL_NAME:
            errString = "Invalid kernel name";
            break;
            
        case CL_INVALID_KERNEL_DEFINITION:
            errString = "Invalid kernel definition";
            break;
            
        case CL_INVALID_KERNEL:
            errString = "Invalid kernel";
            break;
            
        case CL_INVALID_ARG_INDEX:
            errString = "Invalid argument index";
            break;
            
        case CL_INVALID_ARG_VALUE:
            errString = "Invalid argument value";
            break;
            
        case CL_INVALID_ARG_SIZE:
            errString = "Invalid argument size";
            break;
            
        case CL_INVALID_KERNEL_ARGS:
            errString = "Invalid kernel arguments";
            break;
            
        case CL_INVALID_WORK_DIMENSION:
            errString = "Invalid work dimension";
            break;
            
        case CL_INVALID_WORK_GROUP_SIZE:
            errString = "Invalid work group size";
            break;
            
        case CL_INVALID_WORK_ITEM_SIZE:
            errString = "invalid work item size";
            break;
            
        case CL_INVALID_GLOBAL_OFFSET:
            errString = "Invalid global offset";
            break;
            
        case CL_INVALID_EVENT_WAIT_LIST:
            errString = "Invalid event wait list";
            break;
            
        case CL_INVALID_EVENT:
            errString = "Invalid event";
            break;
            
        case CL_INVALID_OPERATION:
            errString = "Invalid operation";
            break;
            
        case CL_INVALID_GL_OBJECT:
            errString = "Invalid OpenGL object";
            break;
            
        case CL_INVALID_BUFFER_SIZE:
            errString = "Invalid buffer size";
            break;
            
        case CL_INVALID_MIP_LEVEL:
            errString = "Invalid MIP level";
            break;
    }
    
    return errString;
}
