////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
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
//  File name:   OWEMain.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

package com.owengine.owe;

import java.io.File;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.os.Build;
import android.os.Bundle;
import android.os.PowerManager;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

public class OWEMain extends Activity
{
    public static OWECallbackObj mAudio;
    public static OWEView mGLSurfaceView;
    public static String datadir;
    
    public void ShowMessage( String s )
    {
        Toast.makeText( this, s, Toast.LENGTH_LONG ).show();
    }
    
    public boolean checkGameFiles()
    {
        if( !new File( datadir ).exists() )
        {
            ShowMessage( "Game files weren't found: put game files to " + datadir );
            this.finish();
            return false;
        }
        
        return true;
    }
    
    @Override
    public void onCreate( Bundle savedInstanceState )
    {
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                              WindowManager.LayoutParams.FLAG_FULLSCREEN );
        getWindow().addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON );
        requestWindowFeature( Window.FEATURE_NO_TITLE );
        
        if( Build.VERSION.SDK_INT >= 9 )
            setRequestedOrientation( ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE );
            
        super.onCreate( savedInstanceState );
        
        datadir = PreferenceManager.getDefaultSharedPreferences( this ).getString( OWEUtils.pref_datapath, OWEUtils.owei.default_path );
        
        if( checkGameFiles() )
        {
            if( mGLSurfaceView == null )
                mGLSurfaceView = new OWEView( this );
            setContentView( mGLSurfaceView );
            mGLSurfaceView.requestFocus();
            if( mAudio == null )
            {
                mAudio = new OWECallbackObj();
                mAudio.vw = mGLSurfaceView;
            }
            OWEJNI.setCallbackObject( mAudio );
        }
        else
        {
            finish();
        }
    }
    
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
        
        if( mAudio != null )
        {
            mAudio.pause();
        }
        
        if( mGLSurfaceView != null )
        {
            mGLSurfaceView.onPause();
        }
    }
    
    @Override
    protected void onResume()
    {
        super.onResume();
        
        if( mGLSurfaceView != null )
        {
            mGLSurfaceView.onResume();
        }
        
        if( mAudio != null )
        {
            mAudio.resume();
        }
    }
}
