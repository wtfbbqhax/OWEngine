//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
//  Copyright (C) 2010 Roderick Colenbrander
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
//  File name:   android_snd.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../../client/snd_local.h"

static int buf_size = 0;
static int bytes_per_sample = 0;
static int chunkSizeBytes = 0;
static int dmapos = 0;

extern void ( *setState )( int shown );
void ( *initAudio )( void* buffer, int size );
void ( *writeAudio )( int offset, int length );

void OWE_SetCallbacks( void* init_audio, void* write_audio, void* set_state )
{
    setState = set_state;
    writeAudio = write_audio;
    initAudio = init_audio;
}

qboolean SNDDMA_Init( void )
{
    Com_Printf( "Initializing Android Sound subsystem\n" );
    
    /* For now hardcode this all :) */
    dma.channels = 2;
    dma.samples = 32768;
    dma.samplebits = 16;
    
    dma.submission_chunk = 4096; /* This is in single samples, so this would equal 2048 frames (assuming stereo) in Android terminology */
    dma.speed = 44100; /* This is the native sample frequency of the Milestone */
    
    bytes_per_sample = dma.samplebits / 8;
    buf_size = dma.samples * bytes_per_sample;
    dma.buffer = calloc( 1, buf_size );
    
    chunkSizeBytes = dma.submission_chunk * bytes_per_sample;
    
    initAudio( dma.buffer, buf_size );
    
    return qtrue;
}


int SNDDMA_GetDMAPos( void )
{
    return dmapos;
}

void SNDDMA_Shutdown( void )
{
    Com_Printf( "SNDDMA_ShutDown\n" );
}

void SNDDMA_BeginPainting( void )
{
}

void OWE_GetAudio( void )
{
    int offset = ( dmapos * bytes_per_sample ) & ( buf_size - 1 );
    writeAudio( offset, chunkSizeBytes );
    dmapos += dma.submission_chunk;
}

void SNDDMA_Submit( void )
{
}

