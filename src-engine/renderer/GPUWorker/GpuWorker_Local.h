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
//  File name:   GpuWorker_Local.h
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2015
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __GPUWORKER_LOCAL_H__
#define __GPUWORKER_LOCAL_H__

#include <cl/Opencl.h>
#include "../../splines/util_list.h"

//
// owGpuWorkerLocal
//
class owGpuWorkerLocal : public owGpuWorker
{
public:
    virtual void					Init( void );
    virtual void					Shutdown( void );
    
    virtual owGPUWorkerProgram*		FindProgram( const char* path );
    virtual void					AppendProgram( owGPUWorkerProgram* program )
    {
        programPool.Append( program );
    };
    
    cl_context						GetDeviceContext( void )
    {
        return context;
    }
    cl_device_id					GetDeviceId( void )
    {
        return ( cl_device_id )device;
    }
    cl_command_queue				GetCommandQueue( void )
    {
        return queue;
    }
    
    static cl_int					clError;
private:

    cl_platform_id*                 platform;
    cl_context                      context;
    cl_command_queue                queue;
    cl_device_id                    device;
    
    idList<owGPUWorkerProgram*>     programPool;
};

extern owGpuWorkerLocal		        gpuWorkerLocal;

#endif // !__GPUWORKER_LOCAL_H__
