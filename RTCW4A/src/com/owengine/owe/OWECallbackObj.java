////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  This file contains some code from kwaak3:
//  Copyright (C) 2012 n0n3m4
//  Copyright (C) 2016 Dusan Jocic <dusanjocic@msn.com>
//
//  OWEngine source code is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  OWEngine source code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA,
//  or simply visit <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------------
//  File name:   OWECallbackObj.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

package com.owengine.owe;

import java.nio.ByteBuffer;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
//import android.support.v4.util.LongSparseArray;
//import android.util.Log;

public class OWECallbackObj
{
    public OWEAudioTrack mAudioTrack;
    byte[] mAudioData;
    public static boolean reqThreadrunning = true;
    public OWEView vw;
    
    public void setState( int newstate )
    {
        vw.setState( newstate );
    }
    
    public void init( int size )
    {
        if( mAudioTrack != null ) return;
        if( ( OWEUtils.owei.isOW ) || ( OWEUtils.owei.isRTCW ) || ( OWEUtils.owei.isQ1 ) ) size /= 8;
        
        mAudioData = new byte[size];
        int sampleFreq = 44100;
        int bufferSize = 2 * AudioTrack.getMinBufferSize( sampleFreq, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT );
        mAudioTrack = new OWEAudioTrack( AudioManager.STREAM_MUSIC, sampleFreq, AudioFormat.CHANNEL_CONFIGURATION_STEREO,
                                         AudioFormat.ENCODING_PCM_16BIT, bufferSize, AudioTrack.MODE_STREAM );
        mAudioTrack.play();
        
        long sleeptime = ( size * 1000000000l ) / ( 2 * 2 * sampleFreq );
        ScheduledThreadPoolExecutor stpe = new ScheduledThreadPoolExecutor( 5 );
        stpe.scheduleAtFixedRate( new Runnable()
        {
            @Override
            public void run()
            {
                if( reqThreadrunning )
                {
                    OWEJNI.requestAudioData();
                }
            }
        }, 0, sleeptime, TimeUnit.NANOSECONDS );
    }
    
    int sync = 0;
    
    public void writeAudio( ByteBuffer audioData, int offset, int len )
    {
        if( mAudioTrack == null )
            return;
        audioData.position( offset );
        audioData.get( mAudioData, 0, len );
        if( sync++ % 128 == 0 )
            mAudioTrack.flush();
        mAudioTrack.write( mAudioData, 0, len );
    }
    
    public void pause()
    {
        if( mAudioTrack == null )
            return;
        mAudioTrack.pause();
        reqThreadrunning = false;
    }
    
    public void resume()
    {
        if( mAudioTrack == null )
            return;
        mAudioTrack.play();
        reqThreadrunning = true;
    }
}

class OWEAudioTrack extends AudioTrack
{
    private int mFrameSize;
    public OWEAudioTrack( int streamType, int sampleRateInHz,
                          int channelConfig, int audioFormat, int bufferSizeInBytes, int mode )
    throws IllegalStateException
    {
        super( streamType, sampleRateInHz, channelConfig, audioFormat,
               bufferSizeInBytes, mode );
               
        if( audioFormat == AudioFormat.ENCODING_PCM_16BIT )
            mFrameSize = 2 * this.getChannelCount();
        else
            mFrameSize = this.getChannelCount();
    }
    
    @Override
    public void play() throws IllegalStateException
    {
        flush();
        super.play();
    }
    
    public int getFrameSize()
    {
        return mFrameSize;
    }
}