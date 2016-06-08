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
//  File name:   idSound.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//  Created by zaphod on Thu Mar 08 2001.
//
//////////////////////////////////////////////////////////////////////////////////////

#include "idAudio.h"

void idSound::start()
{
}

void idSound::finish()
{
}

idSound::idSound( const char* sname )
{
    int hr;
    name = new idStr( sname );                            // give me a name
    
    lastTimeUsed = -1;
    
    pSound = NULL;
    
    // Load the wave file into a DirectSound buffer
    hr = idSpeak->hw->Create( &pSound, name->c_str(), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL );
    if( hr != E_FAIL )
    {
        // Get the 3D buffer from the secondary buffer
        pSound->Get3DBufferInterfaces( idSpeak->getMinDistance(), idSpeak->getMaxDistance() );
        flags.set( LOADED );
    }
    else
    {
        SAFE_DELETE( pSound );
    }
}

idSound::~idSound()
{
}

void idSound::play( const int dwFlags )
{
    if( !flags.test( LOADED ) )
    {
        int hr;
        pSound = NULL;
        hr = idSpeak->hw->Create( &pSound, name->c_str(), 0, GUID_NULL );
        if( hr != E_FAIL )
        {
            flags.set( LOADED );
        }
        else
        {
            SAFE_DELETE( pSound );
        }
    }
    if( pSound )
    {
        bIndex = pSound->GetFreeIndex();
        if( bIndex != -1 )
        {
            pSound->Play( bIndex, 0, dwFlags );
        }
    }
}

void idSound::stop()
{
    if( pSound )
    {
        pSound->Stop( bIndex );
    }
}


void idSound::update3D( const idVec3& pos, const idVec3& vel )
{
    if( flags.test( LOADED ) )
    {
        pSound->SetPosition( bIndex, pos[0], pos[1], pos[2] );
        pSound->SetVelocity( bIndex, vel[0], vel[1], vel[2] );
    }
}

void idSound::play3D( const int dwFlags, const idVec3& pos, const idVec3& vel )
{
    if( !flags.test( LOADED ) )
    {
        int hr;
        pSound = NULL;
        // Load the wave file into a DirectSound buffer
        hr = idSpeak->hw->Create( &pSound, name->c_str(), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL );
        if( hr != E_FAIL )
        {
            pSound->Get3DBufferInterfaces( idSpeak->getMinDistance(), idSpeak->getMaxDistance() );
            flags.set( LOADED );
        }
        else
        {
            SAFE_DELETE( pSound );
        }
    }
    if( pSound )
    {
        bIndex = pSound->GetFreeIndex();
        if( bIndex != -1 )
        {
            pSound->SetPosition( bIndex, pos[0], pos[1], pos[2] );
            pSound->SetVelocity( bIndex, vel[0], vel[1], vel[2] );
            pSound->Play( bIndex, 0, dwFlags );
        }
    }
}
