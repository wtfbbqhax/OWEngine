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
//  File name:   GameLauncher.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

package com.owengine.rtcw4a;

import com.owengine.owe.OWEInterface;
import com.owengine.owe.OWEJNI;
import com.owengine.owe.OWEKeyCodes;
import com.owengine.owe.OWEMain;
import com.owengine.owe.OWEUiConfig;
import com.owengine.owe.OWEUtils;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Display;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioGroup;
import android.widget.TabHost;

public class GameLauncher extends Activity
{

    final String default_gamedata = "/sdcard/rtcw4a";
    
    final int UI_JOYSTICK = 0;
    final int UI_SHOOT = 1;
    final int UI_JUMP = 2;
    final int UI_CROUCH = 3;
    final int UI_RELOADBAR = 4;
    final int UI_ALT = 5;
    final int UI_ACTION = 6;
    final int UI_KICK = 7;
    final int UI_SAVE = 8;
    final int UI_BINOCULAR = 9;
    final int UI_NOTEPAD = 10;
    final int UI_1 = 11;
    final int UI_2 = 12;
    final int UI_3 = 13;
    final int UI_KBD = 14;
    final int UI_SIZE = UI_KBD + 1;
    
    public void InitOWE( Context cnt, int width, int height )
    {
        OWEKeyCodes.InitOWKeycodes();
        OWEInterface owei = new OWEInterface();
        owei.isRTCW = true;
        owei.UI_SIZE = UI_SIZE;
        
        int r = OWEUtils.dip2px( cnt, 75 );
        int rightoffset = r * 3 / 4;
        int sliders_width = OWEUtils.dip2px( cnt, 125 );
        
        owei.defaults_table = new String[UI_SIZE];
        owei.defaults_table[UI_JOYSTICK] = ( r * 4 / 3 ) + " " + ( height - r * 4 / 3 ) + " " + r + " " + 30;
        owei.defaults_table[UI_SHOOT]    = ( width - r / 2 - rightoffset ) + " " + ( height - r / 2 - rightoffset ) + " " + r * 3 / 2 + " " + 30;
        owei.defaults_table[UI_JUMP]     = ( width - r / 2 ) + " " + ( height - r - 2 * rightoffset ) + " " + r + " " + 30;
        owei.defaults_table[UI_CROUCH]   = ( width - r / 2 ) + " " + ( height - r / 2 ) + " " + r + " " + 30;
        owei.defaults_table[UI_RELOADBAR] = ( width - sliders_width / 2 - rightoffset / 3 ) + " " + ( sliders_width * 3 / 8 ) + " " + sliders_width + " " + 30;
        owei.defaults_table[UI_ALT]      = ( width - sliders_width / 4 - rightoffset / 3 ) + " " + ( sliders_width * 3 / 4 ) + " " + sliders_width / 2 + " " + 30;
        owei.defaults_table[UI_ACTION]   = ( width - r - 2 * rightoffset ) + " " + ( height - r / 2 ) + " " + r + " " + 30;
        owei.defaults_table[UI_KICK]     = ( width - r / 2 - 4 * rightoffset ) + " " + ( height - r / 2 ) + " " + r + " " + 30;
        owei.defaults_table[UI_SAVE]     = sliders_width / 2 + " " + sliders_width / 2 + " " + sliders_width + " " + 30;
        
        for( int i = UI_SAVE + 1; i < UI_SIZE; i++ )
            owei.defaults_table[i] = ( r / 2 + r * ( i - UI_SAVE - 1 ) ) + " " + ( height + r / 2 * 3 / 2 ) + " " + r + " " + 30;
            
        owei.arg_table = new int[UI_SIZE * 4];
        owei.type_table = new int[UI_SIZE];
        
        owei.type_table[UI_JOYSTICK] = OWEUtils.TYPE_JOYSTICK;
        owei.type_table[UI_SHOOT] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_JUMP] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_CROUCH] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_RELOADBAR] = OWEUtils.TYPE_SLIDER;
        owei.type_table[UI_ALT] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_ACTION] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_KICK] = OWEUtils.TYPE_BUTTON;
        owei.type_table[UI_SAVE] = OWEUtils.TYPE_SLIDER;
        for( int i = UI_SAVE + 1; i < UI_SIZE; i++ )
            owei.type_table[i] = OWEUtils.TYPE_BUTTON;
            
        owei.arg_table[UI_SHOOT * 4] = OWEKeyCodes.KeyCodes.K_MOUSE1;
        owei.arg_table[UI_SHOOT * 4 + 1] = 0;
        owei.arg_table[UI_SHOOT * 4 + 2] = 0;
        owei.arg_table[UI_SHOOT * 4 + 3] = 0;
        
        
        owei.arg_table[UI_JUMP * 4] = OWEKeyCodes.KeyCodes.K_SPACE;
        owei.arg_table[UI_JUMP * 4 + 1] = 0;
        owei.arg_table[UI_JUMP * 4 + 2] = 0;
        owei.arg_table[UI_JUMP * 4 + 3] = 0;
        
        owei.arg_table[UI_CROUCH * 4] = 99;
        owei.arg_table[UI_CROUCH * 4 + 1] = 1;
        owei.arg_table[UI_CROUCH * 4 + 2] = 1;
        owei.arg_table[UI_CROUCH * 4 + 3] = 0;
        
        owei.arg_table[UI_RELOADBAR * 4] = 91;
        owei.arg_table[UI_RELOADBAR * 4 + 1] = 114;
        owei.arg_table[UI_RELOADBAR * 4 + 2] = 93;
        owei.arg_table[UI_RELOADBAR * 4 + 3] = 0;
        
        owei.arg_table[UI_ALT * 4] = OWEKeyCodes.KeyCodes.K_MOUSE3;
        owei.arg_table[UI_ALT * 4 + 1] = 0;
        owei.arg_table[UI_ALT * 4 + 2] = 2;
        owei.arg_table[UI_ALT * 4 + 3] = 0;
        
        owei.arg_table[UI_ACTION * 4] = 102;
        owei.arg_table[UI_ACTION * 4 + 1] = 0;
        owei.arg_table[UI_ACTION * 4 + 2] = 0;
        owei.arg_table[UI_ACTION * 4 + 3] = 0;
        
        owei.arg_table[UI_KICK * 4] = 103;
        owei.arg_table[UI_KICK * 4 + 1] = 0;
        owei.arg_table[UI_KICK * 4 + 2] = 0;
        owei.arg_table[UI_KICK * 4 + 3] = 0;
        
        owei.arg_table[UI_SAVE * 4] = 149;
        owei.arg_table[UI_SAVE * 4 + 1] = 27;
        owei.arg_table[UI_SAVE * 4 + 2] = 153;
        owei.arg_table[UI_SAVE * 4 + 3] = 1;
        
        owei.arg_table[UI_BINOCULAR * 4] = 98;
        owei.arg_table[UI_BINOCULAR * 4 + 1] = 0;
        owei.arg_table[UI_BINOCULAR * 4 + 2] = 0;
        owei.arg_table[UI_BINOCULAR * 4 + 3] = 0;
        
        owei.arg_table[UI_NOTEPAD * 4] = 110;
        owei.arg_table[UI_NOTEPAD * 4 + 1] = 0;
        owei.arg_table[UI_NOTEPAD * 4 + 2] = 0;
        owei.arg_table[UI_NOTEPAD * 4 + 3] = 0;
        
        owei.arg_table[UI_1 * 4] = 49;
        owei.arg_table[UI_1 * 4 + 1] = 0;
        owei.arg_table[UI_1 * 4 + 2] = 0;
        owei.arg_table[UI_1 * 4 + 3] = 0;
        
        owei.arg_table[UI_2 * 4] = 50;
        owei.arg_table[UI_2 * 4 + 1] = 0;
        owei.arg_table[UI_2 * 4 + 2] = 0;
        owei.arg_table[UI_2 * 4 + 3] = 0;
        
        owei.arg_table[UI_3 * 4] = 51;
        owei.arg_table[UI_3 * 4 + 1] = 0;
        owei.arg_table[UI_3 * 4 + 2] = 0;
        owei.arg_table[UI_3 * 4 + 3] = 0;
        
        owei.arg_table[UI_KBD * 4] = OWEUtils.K_VKBD;
        owei.arg_table[UI_KBD * 4 + 1] = 0;
        owei.arg_table[UI_KBD * 4 + 2] = 0;
        owei.arg_table[UI_KBD * 4 + 3] = 0;
        
        owei.default_path = default_gamedata;
        if( OWEJNI.detectNeon() )
            owei.libname = "libwolfsp_neon.so";
        else
            owei.libname = "libwolfsp.so";
        owei.texture_table = new String[UI_SIZE];
        owei.texture_table[UI_JOYSTICK] = "";
        owei.texture_table[UI_SHOOT] = "btn_sht.png";
        owei.texture_table[UI_JUMP] = "btn_jump.png";
        owei.texture_table[UI_CROUCH] = "btn_crouch.png";
        owei.texture_table[UI_RELOADBAR] = "btn_reload.png";
        owei.texture_table[UI_ALT] = "btn_altfire.png";
        owei.texture_table[UI_ACTION] = "btn_activate.png";
        owei.texture_table[UI_KICK] = "btn_kick.png";
        owei.texture_table[UI_SAVE] = "btn_pause.png";
        owei.texture_table[UI_BINOCULAR] = "btn_binocular.png";
        owei.texture_table[UI_NOTEPAD] = "btn_notepad.png";
        owei.texture_table[UI_1] = "btn_1.png";
        owei.texture_table[UI_2] = "btn_2.png";
        owei.texture_table[UI_3] = "btn_3.png";
        owei.texture_table[UI_KBD] = "btn_keyboard.png";
        
        OWEUtils.owei = owei;
    }
    
    public void DonateDialog()
    {
        AlertDialog.Builder bldr = new AlertDialog.Builder( this );
        bldr.setTitle( "Do you want to support the developer?" );
        bldr.setPositiveButton( "Donate by PayPal", new AlertDialog.OnClickListener()
        {
            @Override
            public void onClick( DialogInterface dialog, int which )
            {
                Intent ppIntent = new Intent( Intent.ACTION_VIEW, Uri.parse( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=thedushantm%40gmail%2ecom&lc=US&item_name=owengine&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHostedGuest" ) );
                ppIntent.addFlags( Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET );
                startActivity( ppIntent );
                dialog.dismiss();
            }
        } );
        bldr.setNegativeButton( "Don't ask", new AlertDialog.OnClickListener()
        {
            @Override
            public void onClick( DialogInterface dialog, int which )
            {
                dialog.dismiss();
            }
        } );
        bldr.setNeutralButton( "Paid apps by owengine", new AlertDialog.OnClickListener()
        {
            @Override
            public void onClick( DialogInterface dialog, int which )
            {
                Intent marketIntent = new Intent( Intent.ACTION_VIEW, Uri.parse( "market://search?q=pub:owengine" ) );
                marketIntent.addFlags( Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET );
                startActivity( marketIntent );
                dialog.dismiss();
            }
        } );
        AlertDialog dl = bldr.create();
        dl.setCancelable( false );
        dl.show();
    }
    
    public void support( View vw )
    {
        DonateDialog();
    }
    
    public void UpdateMouseMenu( boolean show )
    {
        ( ( LinearLayout )findViewById( R.id.layout_mouseconfig ) ).setVisibility( show ? LinearLayout.VISIBLE : LinearLayout.GONE );
    }
    
    public void UpdateMouseManualMenu( boolean show )
    {
        ( ( LinearLayout )findViewById( R.id.layout_manualmouseconfig ) ).setVisibility( show ? LinearLayout.VISIBLE : LinearLayout.GONE );
    }
    
    public void SelectCheckbox( int cbid, int id )
    {
        RadioGroup rg = ( RadioGroup )findViewById( cbid );
        rg.check( rg.getChildAt( id ).getId() );
    }
    
    public int GetIdCheckbox( int cbid )
    {
        RadioGroup rg = ( RadioGroup )findViewById( cbid );
        return rg.indexOfChild( findViewById( rg.getCheckedRadioButtonId() ) );
    }
    
    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.main );
        
        Display display = getWindowManager().getDefaultDisplay();
        int width = Math.max( display.getWidth(), display.getHeight() );
        int height = Math.min( display.getWidth(), display.getHeight() );
        
        InitOWE( this, width, height );
        
        TabHost th = ( TabHost )findViewById( R.id.tabhost );
        th.setup();
        th.addTab( th.newTabSpec( "tab1" ).setIndicator( "General" ).setContent( R.id.launcher_tab1 ) );
        th.addTab( th.newTabSpec( "tab2" ).setIndicator( "Controls" ).setContent( R.id.launcher_tab2 ) );
        th.addTab( th.newTabSpec( "tab3" ).setIndicator( "Graphics" ).setContent( R.id.launcher_tab3 ) );
        SharedPreferences mPrefs = PreferenceManager.getDefaultSharedPreferences( this );
        
        int tmp = mPrefs.getInt( "launchcount", 0 );
        if( tmp == 4 )
            DonateDialog();
        mPrefs.edit().putInt( "launchcount", tmp + 1 ).commit();
        
        ( ( EditText )findViewById( R.id.edt_cmdline ) ).setText( mPrefs.getString( OWEUtils.pref_params, "wolfsp.arm" ) );
        ( ( EditText )findViewById( R.id.edt_mouse ) ).setText( mPrefs.getString( OWEUtils.pref_eventdev, "/dev/input/event???" ) );
        ( ( EditText )findViewById( R.id.edt_path ) ).setText( mPrefs.getString( OWEUtils.pref_datapath, default_gamedata ) );
        ( ( CheckBox )findViewById( R.id.hideonscr ) ).setOnCheckedChangeListener( new OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged( CompoundButton buttonView, boolean isChecked )
            {
                UpdateMouseMenu( isChecked );
            }
        } );
        ( ( CheckBox )findViewById( R.id.hideonscr ) ).setChecked( mPrefs.getBoolean( OWEUtils.pref_hideonscr, false ) );
        
        UpdateMouseMenu( ( ( CheckBox )findViewById( R.id.hideonscr ) ).isChecked() );
        
        ( ( CheckBox )findViewById( R.id.gfx_32bit ) ).setChecked( mPrefs.getBoolean( OWEUtils.pref_32bit, false ) );
        ( ( CheckBox )findViewById( R.id.mapvol ) ).setChecked( mPrefs.getBoolean( OWEUtils.pref_mapvol, false ) );
        ( ( CheckBox )findViewById( R.id.detectmouse ) ).setOnCheckedChangeListener( new OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged( CompoundButton buttonView, boolean isChecked )
            {
                UpdateMouseManualMenu( !isChecked );
            }
        } );
        ( ( CheckBox )findViewById( R.id.detectmouse ) ).setChecked( mPrefs.getBoolean( OWEUtils.pref_detectmouse, true ) );
        
        UpdateMouseManualMenu( !( ( CheckBox )findViewById( R.id.detectmouse ) ).isChecked() );
        
        SelectCheckbox( R.id.rg_curpos, mPrefs.getInt( OWEUtils.pref_mousepos, 3 ) );
        SelectCheckbox( R.id.rg_scrres, mPrefs.getInt( OWEUtils.pref_scrres, 0 ) );
        SelectCheckbox( R.id.rg_msaa, mPrefs.getInt( OWEUtils.pref_msaa, 0 ) );
        
        ( ( EditText )findViewById( R.id.res_x ) ).setText( mPrefs.getString( OWEUtils.pref_resx, "640" ) );
        ( ( EditText )findViewById( R.id.res_y ) ).setText( mPrefs.getString( OWEUtils.pref_resy, "480" ) );
    }
    
    public void start( View vw )
    {
        Editor mEdtr = PreferenceManager.getDefaultSharedPreferences( this ).edit();
        mEdtr.putString( OWEUtils.pref_params, ( ( EditText )findViewById( R.id.edt_cmdline ) ).getText().toString() );
        mEdtr.putString( OWEUtils.pref_eventdev, ( ( EditText )findViewById( R.id.edt_mouse ) ).getText().toString() );
        mEdtr.putString( OWEUtils.pref_datapath, ( ( EditText )findViewById( R.id.edt_path ) ).getText().toString() );
        mEdtr.putBoolean( OWEUtils.pref_hideonscr, ( ( CheckBox )findViewById( R.id.hideonscr ) ).isChecked() );
        mEdtr.putBoolean( OWEUtils.pref_32bit, ( ( CheckBox )findViewById( R.id.gfx_32bit ) ).isChecked() );
        mEdtr.putBoolean( OWEUtils.pref_mapvol, ( ( CheckBox )findViewById( R.id.mapvol ) ).isChecked() );
        mEdtr.putBoolean( OWEUtils.pref_detectmouse, ( ( CheckBox )findViewById( R.id.detectmouse ) ).isChecked() );
        mEdtr.putInt( OWEUtils.pref_mousepos, GetIdCheckbox( R.id.rg_curpos ) );
        mEdtr.putInt( OWEUtils.pref_scrres, GetIdCheckbox( R.id.rg_scrres ) );
        mEdtr.putInt( OWEUtils.pref_msaa, GetIdCheckbox( R.id.rg_msaa ) );
        mEdtr.putString( OWEUtils.pref_resx, ( ( EditText )findViewById( R.id.res_x ) ).getText().toString() );
        mEdtr.putString( OWEUtils.pref_resy, ( ( EditText )findViewById( R.id.res_y ) ).getText().toString() );
        mEdtr.commit();
        startActivity( new Intent( this, OWEMain.class ) );
        finish();
    }
    
    public void resetcontrols( View vw )
    {
        Editor mEdtr = PreferenceManager.getDefaultSharedPreferences( this ).edit();
        for( int i = 0; i < UI_SIZE; i++ )
            mEdtr.putString( OWEUtils.pref_controlprefix + i, null );
        mEdtr.commit();
    }
    
    public void controls( View vw )
    {
        startActivity( new Intent( this, OWEUiConfig.class ) );
    }
}
