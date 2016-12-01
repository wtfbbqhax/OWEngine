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
//  File name:   snd_public.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __SND_PUBLIC_H__
#define __SND_PUBLIC_H__

//
// idSoundSystem
//
class idSoundSystem
{
public:
    virtual void Init( void ) = 0;
    virtual void Shutdown( void ) = 0;
    virtual void UpdateThread( void ) = 0;
    
    // if origin is NULL, the sound will be dynamically sourced from the entity
    virtual void StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx ) = 0;
    virtual void StartSoundEx( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int flags ) = 0;
    virtual void StartLocalSound( sfxHandle_t sfx, int channelNum ) = 0;
    
    virtual void StartBackgroundTrack( const char* intro, const char* loop, int fadeupTime ) = 0;
    virtual void StopBackgroundTrack( void ) = 0;
    virtual void FadeStreamingSound( float targetvol, int time, int ssNum ) = 0;  //----(SA)	added
    virtual void FadeAllSounds( float targetvol, int time ) = 0;    //----(SA)	added
    
    virtual void StartStreamingSound( const char* intro, const char* loop, int entnum, int channel, int attenuation ) = 0;
    virtual void StopStreamingSound( int index ) = 0;
    virtual void StopEntStreamingSound( int entNum ) = 0; //----(SA)	added
    
    // cinematics and voice-over-network will send raw samples
    // 1.0 volume will be direct output of source samples
    virtual void RawSamples( int samples, int rate, int width, int channels, const byte* data, float lvol, float rvol, int streamingIndex ) = 0;
    
    // stop all sounds and the background track
    virtual void StopAllSounds( void ) = 0;
    
    // all continuous looping sounds must be added before calling Update
    virtual void ClearLoopingSounds( void ) = 0;
    virtual void ClearSounds( bool clearStreaming, bool clearMusic ) = 0; //----(SA)	modified
    virtual void AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, const int range, sfxHandle_t sfxHandle, int volume ) = 0;
    
    // recompute the reletive volumes for all running sounds
    // reletive to the given entityNum / orientation
    virtual void Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater ) = 0;
    
    // let the sound system know where an entity currently is
    virtual void UpdateEntityPosition( int entityNum, const vec3_t origin ) = 0;
    
    virtual void Update( void ) = 0;
    
    virtual void DisableSounds( void ) = 0;
    
    virtual void BeginRegistration( void ) = 0;
    
    // RegisterSound will allways return a valid sample, even if it
    // has to create a placeholder.  This prevents continuous filesystem
    // checks for missing files
    virtual sfxHandle_t RegisterSound( const char* sample ) = 0;
    
    
    virtual void DisplayFreeMemory( void ) = 0;
    
    //
    virtual int GetVoiceAmplitude( int entityNum ) = 0;
};

extern idSoundSystem* soundSystem;

#endif // !__SND_PUBLIC_H_

