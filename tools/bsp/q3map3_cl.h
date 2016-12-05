/*
Copyright (C) 1999-2007 id Software, Inc. and contributors.
For a list of contributors, see the accompanying CONTRIBUTORS file.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-------------------------------------------------------------------------------

This code has been altered significantly from its original form, to support
several games based on the Quake III Arena engine, in the form of "Q3Map2."

------------------------------------------------------------------------------- */

#ifndef Q3MAP3_CL_H
#define Q3MAP3_CL_H

#include "CL/cl.h"
#include "q3map2.h"

#define PLATFORM_IDS_SIZE 1

/*kernels are compiled at run-time from strings. Each index will contain a series of kernels to be compiled.
  The following is a "dynamic array" so we don't have to load every single file in one go.*/
typedef struct
{
    int num; /*number of files loaded in to the source string*/
    int size;
    char* source; /* Each file as string stored here */
} t_source;

/* The "array" we will work with for kernel shenanigans */
t_source* kernel_source;

cl_int           clerror; /*opencl error handling*/
cl_platform_id   clplatform;
cl_int           num_clplatforms; /*number of ocl platforms detected*/
cl_context       clcontext;
cl_command_queue clqueue;
cl_device_id     cldevice;
cl_event         clevent;

cl_program       clprogram; /*ocl program which consists of all loaded kernels*/

/***********************************
* HARD-CODED MEMORY BUFFERS AND KERNEL OBJECTS
* Replace with flexible solution after testing
************************************/
static const int MAGIC_SIZE = 16384;
size_t local_ws;
size_t global_ws;
size_t awesome;
#define MEM_SIZE (sizeof(cl_float4)*MAGIC_SIZE) /*4 * 4096 bytes of memory per argument... max size of 4096 for float arrays*/
/*host memory*/
cl_float4 src_a_h[16384];
cl_float4 src_b_h[16384];
cl_float4 res_h[16384];
/* device memory*/
cl_mem src_a;
cl_mem src_b;
cl_mem res;
cl_kernel magic;
int stupid;
cl_ulong start, end;
float total;


typedef struct
{
    cl_mem mem[10]; /*memory buffer array for kernel object*/
    
    size_t local_ws; /*local work size*/
    size_t global_ws; /*global work size*/
    
    cl_kernel kernel; /*the function to be executed*/
} t_kernel;

t_kernel kernels[256]; /*An array of *COMPILED* and ready-to-be-executed OpenCL functions.*/
/* ^ Allows for up to 256 functions. Increase as needed.*/



/*OpenCL Helper Functions to keep the clutter out of q3map2's source*/
qboolean InitOpenCL( void );
qboolean GetPlatform( cl_platform_id* platform ); /*We don't even need to bother passing cl_platform_id as an argument*/
qboolean GetDevice( void );
qboolean CreateContext( void );
qboolean CreateCommandQueue( void );

void CleanOpenCL( void ); /*clean up memory and release ocl objects*/

qboolean LoadProgramSource( char* filename ); /*Load a .cl file to a single string. Return value stored in kernels[]*/
void BuildAllProgramSource( void ); /*builds all loaded .cl files in to executable code*/

void PushKernel( char* kernel, unsigned int* length ); /*push a kernel on to the array*/
void createCLVectorGPU( void );

#endif