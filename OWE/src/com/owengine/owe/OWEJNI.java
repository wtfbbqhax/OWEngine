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
//  File name:   OWEJNI.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

package com.owengine.owe;

import java.io.BufferedReader;
import java.io.FileReader;

public class OWEJNI {	
	public static native void setCallbackObject(Object obj);	
	public static native void init(String LibPath,int width, int height, String GameDir, String Args);	
	public static native void drawFrame();
	public static native void sendKeyEvent(int state,int key,int character);
	public static native void sendMotionEvent(float x, float y);	
	public static native void requestAudioData();
	public static native void vidRestart();
	
	public static boolean detectNeon()
	{
		try
		{
		BufferedReader br=new BufferedReader(new FileReader("/proc/cpuinfo"));
		String l;
		while ((l=br.readLine())!=null)
			if ((l.contains("Features")) && (l.contains("neon"))) 
				{
				br.close();
				return true;		
				}
		br.close();
		return false;
		}
		catch (Exception e){return false;}
	}

	static {
		System.loadLibrary("oweloader");
	}
}

