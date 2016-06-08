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
//  File name:   OWEInterface.java
//  Version:     v1.00
//  Created:
//  Compilers:   Android Studio 2.1.1
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////


package com.owengine.owe;

public class OWEInterface {
	
	public int UI_SIZE;
	public String[] defaults_table;
	public String[] texture_table;
	public int[] type_table;
	public int[] arg_table; //key,key,key,style or key,canbeheld,style,null	

	public boolean isRTCW=false;
	public boolean isOW=false;
	public boolean isD3=false;
	public boolean isQ1=false;
	
	public String default_path;
	
	public String libname;
	
	//RTCW4A:
	public final int RTCW4A_UI_ACTION=6;
	public final int RTCW4A_UI_KICK=7;
}
