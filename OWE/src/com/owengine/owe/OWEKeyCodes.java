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
//  File name:   OWEKeyCodes.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

package com.owengine.owe;

import java.lang.reflect.Field;

import android.view.KeyEvent;

public class OWEKeyCodes
{

    public static class KeyCodesOW
    {
        public static final int K_TAB = 9;
        public static final int K_ENTER = 13;
        public static final int K_ESCAPE = 27;
        public static final int K_SPACE = 32;
        
        public static final int K_BACKSPACE = 127;
        
        public static final int K_COMMAND = 128;
        public static final int K_CAPSLOCK = 129;
        public static final int K_POWER = 130;
        public static final int K_PAUSE = 131;
        
        public static final int K_UPARROW = 132;
        public static final int K_DOWNARROW = 133;
        public static final int K_LEFTARROW = 134;
        public static final int K_RIGHTARROW = 135;
        
        public static final int K_ALT = 136;
        public static final int K_CTRL = 137;
        public static final int K_SHIFT = 138;
        public static final int K_INS = 139;
        public static final int K_DEL = 140;
        public static final int K_PGDN = 141;
        public static final int K_PGUP = 142;
        public static final int K_HOME = 143;
        public static final int K_END = 144;
        
        public static final int K_F1 = 145;
        public static final int K_F2 = 146;
        public static final int K_F3 = 147;
        public static final int K_F4 = 148;
        public static final int K_F5 = 149;
        public static final int K_F6 = 150;
        public static final int K_F7 = 151;
        public static final int K_F8 = 152;
        public static final int K_F9 = 153;
        public static final int K_F10 = 154;
        public static final int K_F11 = 155;
        public static final int K_F12 = 156;
        public static final int K_F13 = 157;
        public static final int K_F14 = 158;
        public static final int K_F15 = 159;
        
        public static final int K_KP_HOME = 160;
        public static final int K_KP_UPARROW = 161;
        public static final int K_KP_PGUP = 162;
        public static final int K_KP_LEFTARROW = 163;
        public static final int K_KP_5 = 164;
        public static final int K_KP_RIGHTARROW = 165;
        public static final int K_KP_END = 166;
        public static final int K_KP_DOWNARROW = 167;
        public static final int K_KP_PGDN = 168;
        public static final int K_KP_ENTER = 169;
        public static final int K_KP_INS = 170;
        public static final int K_KP_DEL = 171;
        public static final int K_KP_SLASH = 172;
        public static final int K_KP_MINUS = 173;
        public static final int K_KP_PLUS = 174;
        public static final int K_KP_NUMLOCK = 175;
        public static final int K_KP_STAR = 176;
        public static final int K_KP_EQUALS = 177;
        
        public static final int K_MOUSE1 = 178;
        public static final int K_MOUSE2 = 179;
        public static final int K_MOUSE3 = 180;
        public static final int K_MOUSE4 = 181;
        public static final int K_MOUSE5 = 182;
        
        public static final int K_MWHEELDOWN = 183;
        public static final int K_MWHEELUP = 184;
    };
    
    public static class KeyCodesD3
    {
        public static final int K_TAB = 9;
        public static final int K_ENTER = 13;
        public static final int K_ESCAPE = 27;
        public static final int K_SPACE = 32;
        public static final int K_BACKSPACE = 127;
        public static final int K_COMMAND = 128;
        public static final int K_CAPSLOCK = 129;
        public static final int K_SCROLL = 130;
        public static final int K_POWER = 131;
        public static final int K_PAUSE = 132;
        public static final int K_UPARROW = 133;
        public static final int K_DOWNARROW = 134;
        public static final int K_LEFTARROW = 135;
        public static final int K_RIGHTARROW = 136;
        public static final int K_LWIN = 137;
        public static final int K_RWIN = 138;
        public static final int K_MENU = 139;
        public static final int K_ALT = 140;
        public static final int K_CTRL = 141;
        public static final int K_SHIFT = 142;
        public static final int K_INS = 143;
        public static final int K_DEL = 144;
        public static final int K_PGDN = 145;
        public static final int K_PGUP = 146;
        public static final int K_HOME = 147;
        public static final int K_END = 148;
        public static final int K_F1 = 149;
        public static final int K_F2 = 150;
        public static final int K_F3 = 151;
        public static final int K_F4 = 152;
        public static final int K_F5 = 153;
        public static final int K_F6 = 154;
        public static final int K_F7 = 155;
        public static final int K_F8 = 156;
        public static final int K_F9 = 157;
        public static final int K_F10 = 158;
        public static final int K_F11 = 159;
        public static final int K_F12 = 160;
        public static final int K_INVERTED_EXCLAMATION = 161;
        public static final int K_F13 = 162;
        public static final int K_F14 = 163;
        public static final int K_F15 = 164;
        public static final int K_MOUSE1 = 187;
        public static final int K_MOUSE2 = 188;
        public static final int K_MOUSE3 = 189;
        public static final int K_MOUSE4 = 190;
        public static final int K_MOUSE5 = 191;
        public static final int K_MWHEELDOWN = 195;
        public static final int K_MWHEELUP = 196;
    };
    
    public static class KeyCodesQ1
    {
        public static final int K_TAB = 9;
        public static final int K_ENTER = 13;
        public static final int K_ESCAPE = 27;
        public static final int K_SPACE	= 32;
        public static final int K_BACKSPACE	= 127;
        public static final int K_UPARROW = 128;
        public static final int K_DOWNARROW = 129;
        public static final int K_LEFTARROW = 130;
        public static final int K_RIGHTARROW = 131;
        public static final int K_ALT = 132;
        public static final int K_CTRL = 133;
        public static final int K_SHIFT = 134;
        public static final int K_F1 = 135;
        public static final int K_F2 = 136;
        public static final int K_F3 = 137;
        public static final int K_F4 = 138;
        public static final int K_F5 = 139;
        public static final int K_F6 = 140;
        public static final int K_F7 = 141;
        public static final int K_F8 = 142;
        public static final int K_F9 = 143;
        public static final int K_F10 = 144;
        public static final int K_F11 = 145;
        public static final int K_F12 = 146;
        public static final int K_INS = 147;
        public static final int K_DEL = 148;
        public static final int K_PGDN = 149;
        public static final int K_PGUP = 150;
        public static final int K_HOME = 151;
        public static final int K_END = 152;
        public static final int K_PAUSE = 153;
        public static final int K_NUMLOCK = 154;
        public static final int K_CAPSLOCK = 155;
        public static final int K_SCROLLOCK = 156;
        public static final int K_MOUSE1 = 512;
        public static final int K_MOUSE2 = 513;
        public static final int K_MOUSE3 = 514;
        public static final int K_MWHEELUP = 515;
        public static final int K_MWHEELDOWN = 516;
        public static final int K_MOUSE4 = 517;
        public static final int K_MOUSE5 = 518;
    };
    
    
    
    public static class KeyCodes
    {
        public static int K_TAB;
        public static int K_ENTER;
        public static int K_ESCAPE;
        public static int K_SPACE;
        public static int K_BACKSPACE;
        public static int K_CAPSLOCK;
        public static int K_PAUSE;
        public static int K_UPARROW;
        public static int K_DOWNARROW;
        public static int K_LEFTARROW;
        public static int K_RIGHTARROW;
        public static int K_ALT;
        public static int K_CTRL;
        public static int K_SHIFT;
        public static int K_INS;
        public static int K_DEL;
        public static int K_PGDN;
        public static int K_PGUP;
        public static int K_HOME;
        public static int K_END;
        public static int K_F1;
        public static int K_F2;
        public static int K_F3;
        public static int K_F4;
        public static int K_F5;
        public static int K_F6;
        public static int K_F7;
        public static int K_F8;
        public static int K_F9;
        public static int K_F10;
        public static int K_F11;
        public static int K_F12;
        public static int K_MOUSE1;
        public static int K_MOUSE2;
        public static int K_MOUSE3;
        public static int K_MOUSE4;
        public static int K_MOUSE5;
        public static int K_MWHEELDOWN;
        public static int K_MWHEELUP;
    };
    
    public static void InitOWKeycodes()
    {
        try
        {
            for( Field f: KeyCodes.class.getFields() )
                f.set( null, KeyCodesOW.class.getField( f.getName() ).get( null ) );
        }
        catch( Exception e ) {};
    }
    
    public static void InitD3Keycodes()
    {
        try
        {
            for( Field f: KeyCodes.class.getFields() )
                f.set( null, KeyCodesD3.class.getField( f.getName() ).get( null ) );
        }
        catch( Exception e ) {};
    }
    
    public static void InitQ1Keycodes()
    {
        try
        {
            for( Field f: KeyCodes.class.getFields() )
                f.set( null, KeyCodesQ1.class.getField( f.getName() ).get( null ) );
        }
        catch( Exception e ) {};
    }
    
    
    public static int convertKeyCode( int keyCode, KeyEvent event )
    {
        switch( keyCode )
        {
            case KeyEvent.KEYCODE_FOCUS:
                return KeyCodes.K_F1;
            case KeyEvent.KEYCODE_VOLUME_DOWN:
                return KeyCodes.K_F2;
            case KeyEvent.KEYCODE_VOLUME_UP:
                return KeyCodes.K_F3;
            case KeyEvent.KEYCODE_DPAD_UP:
                return KeyCodes.K_UPARROW;
            case KeyEvent.KEYCODE_DPAD_DOWN:
                return KeyCodes.K_DOWNARROW;
            case KeyEvent.KEYCODE_DPAD_LEFT:
                return KeyCodes.K_LEFTARROW;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                return KeyCodes.K_RIGHTARROW;
            case KeyEvent.KEYCODE_DPAD_CENTER:
                return KeyCodes.K_CTRL;
            case KeyEvent.KEYCODE_ENTER:
                return KeyCodes.K_ENTER;
            case KeyEvent.KEYCODE_BACK:
                return KeyCodes.K_ESCAPE;
            case KeyEvent.KEYCODE_DEL:
                return KeyCodes.K_DEL;
            case KeyEvent.KEYCODE_ALT_LEFT:
            case KeyEvent.KEYCODE_ALT_RIGHT:
                return KeyCodes.K_ALT;
            case KeyEvent.KEYCODE_SHIFT_LEFT:
            case KeyEvent.KEYCODE_SHIFT_RIGHT:
                return KeyCodes.K_SHIFT;
            case KeyEvent.KEYCODE_CTRL_LEFT:
            case KeyEvent.KEYCODE_CTRL_RIGHT:
                return KeyCodes.K_CTRL;
            case KeyEvent.KEYCODE_INSERT:
                return KeyCodes.K_INS;
            case 122:
                return KeyCodes.K_HOME;
            case KeyEvent.KEYCODE_FORWARD_DEL:
                return KeyCodes.K_DEL;
            case 123:
                return KeyCodes.K_END;
            case KeyEvent.KEYCODE_ESCAPE:
                return KeyCodes.K_ESCAPE;
            case KeyEvent.KEYCODE_TAB:
                return KeyCodes.K_TAB;
            case KeyEvent.KEYCODE_F1:
                return KeyCodes.K_F1;
            case KeyEvent.KEYCODE_F2:
                return KeyCodes.K_F2;
            case KeyEvent.KEYCODE_F3:
                return KeyCodes.K_F3;
            case KeyEvent.KEYCODE_F4:
                return KeyCodes.K_F4;
            case KeyEvent.KEYCODE_F5:
                return KeyCodes.K_F5;
            case KeyEvent.KEYCODE_F6:
                return KeyCodes.K_F6;
            case KeyEvent.KEYCODE_F7:
                return KeyCodes.K_F7;
            case KeyEvent.KEYCODE_F8:
                return KeyCodes.K_F8;
            case KeyEvent.KEYCODE_F9:
                return KeyCodes.K_F9;
            case KeyEvent.KEYCODE_F10:
                return KeyCodes.K_F10;
            case KeyEvent.KEYCODE_F11:
                return KeyCodes.K_F11;
            case KeyEvent.KEYCODE_F12:
                return KeyCodes.K_F12;
            case KeyEvent.KEYCODE_CAPS_LOCK:
                return KeyCodes.K_CAPSLOCK;
            case KeyEvent.KEYCODE_PAGE_DOWN:
                return KeyCodes.K_PGDN;
            case KeyEvent.KEYCODE_PAGE_UP:
                return KeyCodes.K_PGUP;
                
        }
        int uchar = event.getUnicodeChar( 0 );
        if( uchar < 127 )
            return uchar;
        return 0;
    }
    
}
