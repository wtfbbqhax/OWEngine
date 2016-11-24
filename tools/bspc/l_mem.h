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
//  File name:   l_mem.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

//#define MEMDEBUG
#undef MEMDEBUG

#ifndef MEMDEBUG

void* GetClearedMemory( unsigned long size );
void* GetMemory( unsigned long size );

#else

#define GetMemory( size )             GetMemoryDebug( size, # size, __FILE__, __LINE__ );
#define GetClearedMemory( size )  GetClearedMemoryDebug( size, # size, __FILE__, __LINE__ );
//allocate a memory block of the given size
void* GetMemoryDebug( unsigned long size, char* label, char* file, int line );
//allocate a memory block of the given size and clear it
void* GetClearedMemoryDebug( unsigned long size, char* label, char* file, int line );
//
void PrintMemoryLabels( void );
#endif //MEMDEBUG

void FreeMemory( void* ptr );
int MemorySize( void* ptr );
void PrintMemorySize( unsigned long size );
int TotalAllocatedMemory( void );

