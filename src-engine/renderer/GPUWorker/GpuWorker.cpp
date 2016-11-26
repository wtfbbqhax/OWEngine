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
//  File name:   GpuWorker.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description: functions that are not called every frame
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../r_local.h"
#include "../../splines/util_list.h"
#include "../../splines/util_str.h"
#include "GpuWorker_Local.h"

#ifdef _WIN32
#include <GL/wglew.h>
#elif __LINUX__
#include <GL/glxew.h>
#elif __ANDROID__
#include <GL/eglew.h>
#else
// Mac - I Have no idea
#endif

owGpuWorkerLocal		gpuWorkerLocal;
owGpuWorker*			gpuWorker = &gpuWorkerLocal;

cl_int					owGpuWorkerLocal::clError;

/*
=============
owGpuWorker::Init
=============
*/
void owGpuWorkerLocal::Init( void )
{
    ri.Printf( PRINT_ALL, "----------- R_GPUWorker_Init ----------\n" );
    ri.Printf( PRINT_ALL, "Init OpenCL...\n" );
    ri.Printf( PRINT_ALL, "...Getting Device Platform ID.\n" );
    ID_GPUWORKER_CALLAPI( clGetPlatformIDs( 1, &platform, NULL ) );
    
    // Get all the device ids
    ri.Printf( PRINT_ALL, "...Getting Device ID.\n" );
    ID_GPUWORKER_CALLAPI( clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL ) );
    
    cl_context_properties properties[] =
    {
#ifdef _WIN32
        CL_GL_CONTEXT_KHR, ( cl_context_properties ) wglGetCurrentContext(),
        CL_WGL_HDC_KHR, ( cl_context_properties ) wglGetCurrentDC(),
#elif __LINUX__
        //Dushan - this should be for Linux
        CL_GL_CONTEXT_KHR, ( cl_context_properties ) glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, ( cl_context_properties ) glXGetCurrentDisplay(),
#elif __ANDROID__
        //Dushan - also fot the Android
        CL_GL_CONTEXT_KHR, ( cl_context_properties ) eglGetCurrentDisplay(),
        CL_EGL_DISPLAY_KHR, ( cl_context_properties ) eglGetCurrentContext(),
#else
        //Dushan - I have no idea for Mac
#endif
        CL_CONTEXT_PLATFORM, ( cl_context_properties ) platform,
        0
    };
    
    // Create the OpenCL device context
    ri.Printf( PRINT_ALL, "...Creating Device Context.\n" );
    context = clCreateContext( properties, 1, &device, NULL, NULL, &clError );
    
    // Create the opencl command queue.
    ri.Printf( PRINT_ALL, "...Creating Device Command Queue.\n" );
    queue = clCreateCommandQueue( context, device, 0, &clError );
    
    ri.Printf( PRINT_ALL, "---------------------------------------\n" );
}

/*
=============
owGpuWorkerLocal::FindProgram
=============
*/
owGPUWorkerProgram* owGpuWorkerLocal::FindProgram( const char* path )
{
    for( int i = 0; i < programPool.Num(); i++ )
    {
        if( !strcmp( programPool[i]->Name(), path ) )
        {
            return programPool[i];
        }
    }
    
    return NULL;
}

/*
=============
owGpuWorkerLocal::Shutdown
=============
*/
void owGpuWorkerLocal::Shutdown( void )
{
    ri.Printf( PRINT_ALL, "----------- R_GPUWorker_Shutdown ----------\n" );
    
    ri.Printf( PRINT_ALL, "Clearing render program workers...\n" );
    for( int i = 0; i < programPool.Num(); i++ )
    {
        programPool[i]->Free();
        delete programPool[i];
    }
    
    programPool.Clear();
    
    ri.Printf( PRINT_ALL, "Releasing command queue\n" );
    clReleaseCommandQueue( queue );
    
    ri.Printf( PRINT_ALL, "Releasing OpenCL context\n" );
    clReleaseContext( context );
}
