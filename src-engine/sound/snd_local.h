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
//  File name:   snd_local.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: private sound definations
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __SND_LOCAL_H__
#define __SND_LOCAL_H__

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

#define PAINTBUFFER_SIZE        4096                    // this is in samples

#define SND_CHUNK_SIZE          1024                    // samples
#define SND_CHUNK_SIZE_FLOAT    ( SND_CHUNK_SIZE / 2 )      // floats
#define SND_CHUNK_SIZE_BYTE     ( SND_CHUNK_SIZE * 2 )      // floats

#define TALKANIM

typedef struct
{
    int left;           // the final values will be clamped to +/- 0x00ffff00 and shifted down
    int right;
} portable_samplepair_t;

typedef struct adpcm_state
{
    short sample;       /* Previous output value */
    char index;         /* Index into stepsize table */
#if defined( __MACOS__ )
    char pad;           /* //DAJ added pad for alignment */
#endif
} adpcm_state_t;

typedef struct sndBuffer_s
{
    short sndChunk[SND_CHUNK_SIZE];
    struct sndBuffer_s*      next;
    int size;
    adpcm_state_t adpcm;
} sndBuffer;

typedef struct sfx_s
{
    sndBuffer*       soundData;
    bool defaultSound;                  // couldn't be loaded, so use buzz
    bool inMemory;                      // not in Memory
    bool soundCompressed;               // not in Memory
    int soundCompressionMethod;
    int soundLength;
    char soundName[MAX_QPATH];
    int lastTimeUsed;
    struct sfx_s*    next;
} sfx_t;

typedef struct
{
    int channels;
    int samples;                        // mono samples in buffer
    int submission_chunk;               // don't mix less than this #
    int samplebits;
    int speed;
    int samplepos;
    byte*        buffer;
} dma_t;

#define START_SAMPLE_IMMEDIATE  0x7fffffff

typedef struct loopSound_s
{
    vec3_t origin;
    vec3_t velocity;
    float range;            //----(SA)	added
    sfx_t*       sfx;
    int mergeFrame;
    int vol;
    bool loudUnderWater;    // (SA) set if this sound should be played at full vol even when under water (under water loop sound for ex.)
} loopSound_t;

typedef struct
{
    int*         ptr;           //DAJ BUGFIX for freelist/endlist pointer
    int allocTime;
    int startSample;            // START_SAMPLE_IMMEDIATE = set immediately on next mix
    int entnum;                 // to allow overriding a specific sound
    int entchannel;             // to allow overriding a specific sound
    int leftvol;                // 0-255 volume after spatialization
    int rightvol;               // 0-255 volume after spatialization
    int master_vol;             // 0-255 volume before spatialization
    float dopplerScale;
    float oldDopplerScale;
    vec3_t origin;              // only use if fixed_origin is set
    bool fixed_origin;      // use origin instead of fetching entnum's origin
    sfx_t*       thesfx;        // sfx structure
    bool doppler;
    int flags;                  //----(SA)	added
    bool threadReady;
} channel_t;


#define WAV_FORMAT_PCM      1


typedef struct
{
    int format;
    int rate;
    int width;
    int channels;
    int samples;
    int dataofs;                // chunk starts this many bytes from file start
} wavinfo_t;


/*
====================================================================

  SYSTEM SPECIFIC FUNCTIONS

====================================================================
*/

// initializes cycling through a DMA buffer and returns information on it
bool SNDDMA_Init( void );

// gets the current DMA position
int     SNDDMA_GetDMAPos( void );

// shutdown the DMA xfer.
void    SNDDMA_Shutdown( void );

void    SNDDMA_BeginPainting( void );

void    SNDDMA_Submit( void );

//====================================================================

#if defined( __MACOS__ )
#define   MAX_CHANNELS 64
#else
#define MAX_CHANNELS 96
#endif

extern channel_t s_channels[MAX_CHANNELS];
extern channel_t loop_channels[MAX_CHANNELS];
extern int numLoopChannels;

extern int s_paintedtime;
extern vec3_t listener_forward;
extern vec3_t listener_right;
extern vec3_t listener_up;
extern dma_t dma;

#ifdef TALKANIM
extern unsigned char s_entityTalkAmplitude[MAX_CLIENTS];
#endif

//----(SA)	some flags for queued music tracks
#define QUEUED_PLAY_ONCE    -1
#define QUEUED_PLAY_LOOPED  -2
#define QUEUED_PLAY_ONCE_SILENT -3  // when done it goes quiet
//----(SA)	end

// Ridah, streaming sounds
typedef struct
{
    fileHandle_t file;
    wavinfo_t info;
    int samples;
    char name[MAX_QPATH];           //----(SA)	added
    char loop[MAX_QPATH];
    int looped;                 //----(SA)	added
    int entnum;
    int channel;
    int attenuation;
    int kill;           //----(SA)	changed
    
    int fadeStart;              //----(SA)	added
    int fadeEnd;                //----(SA)	added
    float fadeStartVol;         //----(SA)	added
    float fadeTargetVol;        //----(SA)	added
} streamingSound_t;




typedef struct
{
    vec3_t origin;
    bool fixedOrigin;
    int entityNum;
    int entityChannel;
    sfxHandle_t sfx;
    int flags;
} s_pushStack;

#define MAX_PUSHSTACK   64
#define LOOP_HASH       128
#define MAX_LOOP_SOUNDS 128

// removed many statics into a common sound struct
typedef struct
{
    sfx_t*       sfxHash[LOOP_HASH];
    int numLoopSounds;
    loopSound_t loopSounds[MAX_LOOP_SOUNDS];
    
    float volTarget;
    float volStart;
    int volTime1;
    int volTime2;
    float volFadeFrac;
    float volCurrent;
    
    channel_t*   freelist;
    channel_t*   endflist;
    
    int s_numSfx;
    
    s_pushStack pushPop[MAX_PUSHSTACK];
    int tart;
    
    bool s_soundPainted;
    int s_clearSoundBuffer;
    
    int s_soundStarted;
//	bool	s_soundMute;
    int s_soundMute;                // 0 - not muted, 1 - muted, 2 - no new sounds, but play out remaining sounds (so they can die if necessary)
    
    vec3_t entityPositions[MAX_GENTITIES];
    
    char nextMusicTrack[MAX_QPATH];         // extracted from CS_MUSIC_QUEUE //----(SA)	added
    int nextMusicTrackType;
} snd_t;

extern snd_t snd;   // globals for sound



#define MAX_STREAMING_SOUNDS    12  // need to keep it low, or the rawsamples will get too big
#define MAX_RAW_SAMPLES         16384

extern streamingSound_t streamingSounds[MAX_STREAMING_SOUNDS];
extern int s_rawend[MAX_STREAMING_SOUNDS];
extern portable_samplepair_t s_rawsamples[MAX_STREAMING_SOUNDS][MAX_RAW_SAMPLES];
extern portable_samplepair_t s_rawVolume[MAX_STREAMING_SOUNDS];


extern cvar_t*   s_volume;
extern cvar_t*   s_nosound;
extern cvar_t*   s_khz;
extern cvar_t*   s_show;
extern cvar_t*   s_mixahead;
extern cvar_t*   s_mute;

extern cvar_t*   s_testsound;
extern cvar_t*   s_separation;
extern cvar_t*   s_currentMusic;    //----(SA)	added
extern cvar_t*   s_debugMusic;      //----(SA)	added

bool S_LoadSound( sfx_t* sfx );

void        SND_free( sndBuffer* v );
sndBuffer*  SND_malloc();
void        SND_setup();

void S_PaintChannels( int endtime );

void S_memoryLoad( sfx_t* sfx );
portable_samplepair_t* S_GetRawSamplePointer();

// spatializes a channel
void S_Spatialize( channel_t* ch );

// adpcm functions
int  S_AdpcmMemoryNeeded( const wavinfo_t* info );
void S_AdpcmEncodeSound( sfx_t* sfx, short* samples );
void S_AdpcmGetSamples( sndBuffer* chunk, short* to );

// wavelet function

#define SENTINEL_MULAW_ZERO_RUN 127
#define SENTINEL_MULAW_FOUR_BIT_RUN 126

void S_FreeOldestSound();

#define NXStream byte

void encodeWavelet( sfx_t* sfx, short* packets );
void decodeWavelet( sndBuffer* stream, short* packets );

void encodeMuLaw( sfx_t* sfx, short* packets );
extern short mulawToShort[256];

extern short* sfxScratchBuffer;
extern const sfx_t* sfxScratchPointer;
extern int sfxScratchIndex;

extern unsigned char s_entityTalkAmplitude[MAX_CLIENTS];

extern float S_GetStreamingFade( streamingSound_t* ss );    //----(SA)	added

//
// idSoundSystemLocal
//
class idSoundSystemLocal : public idSoundSystem
{
public:
    virtual void Init( void ) ;
    virtual void Shutdown( void ) ;
    virtual void UpdateThread( void ) ;
    
    // if origin is NULL, the sound will be dynamically sourced from the entity
    virtual void StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx ) ;
    virtual void StartSoundEx( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int flags ) ;
    virtual void StartLocalSound( sfxHandle_t sfx, int channelNum ) ;
    
    virtual void StartBackgroundTrack( const char* intro, const char* loop, int fadeupTime ) ;
    virtual void StopBackgroundTrack( void ) ;
    virtual void FadeStreamingSound( float targetvol, int time, int ssNum ) ;  //----(SA)	added
    virtual void FadeAllSounds( float targetvol, int time ) ;    //----(SA)	added
    
    virtual void StartStreamingSound( const char* intro, const char* loop, int entnum, int channel, int attenuation ) ;
    virtual void StopStreamingSound( int index ) ;
    virtual void StopEntStreamingSound( int entNum ) ; //----(SA)	added
    
    // cinematics and voice-over-network will send raw samples
    // 1.0 volume will be direct output of source samples
    virtual void RawSamples( int samples, int rate, int width, int channels, const byte* data, float lvol, float rvol, int streamingIndex ) ;
    
    // stop all sounds and the background track
    virtual void StopAllSounds( void ) ;
    
    // all continuous looping sounds must be added before calling Update
    virtual void ClearLoopingSounds( void ) ;
    virtual void ClearSounds( bool clearStreaming, bool clearMusic ) ; //----(SA)	modified
    virtual void AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, const int range, sfxHandle_t sfxHandle, int volume ) ;
    
    // recompute the reletive volumes for all running sounds
    // reletive to the given entityNum / orientation
    virtual void Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater ) ;
    
    // let the sound system know where an entity currently is
    virtual void UpdateEntityPosition( int entityNum, const vec3_t origin ) ;
    
    virtual void Update( void ) ;
    
    virtual void DisableSounds( void ) ;
    
    virtual void BeginRegistration( void ) ;
    
    // RegisterSound will allways return a valid sample, even if it
    // has to create a placeholder.  This prevents continuous filesystem
    // checks for missing files
    virtual sfxHandle_t RegisterSound( const char* sample ) ;
    
    
    virtual void DisplayFreeMemory( void ) ;
    
    //
    virtual int GetVoiceAmplitude( int entityNum ) ;
};

extern idSoundSystemLocal soundSystemLocal;

#endif // !__SND_LOCAL_H__