//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2010 JV Software
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
//  File name:   fbxmanager.cpp
//  Version:     v1.00
//  Created:
//  Compilers:   Visual Studio 2010
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4996)

#include "../../src-engine/qcommon/q_shared.h"
#include "../../src-engine/splines/splines.h"
#include "../../src-engine/qcommon/qfiles.h"
#include "maya.h"

#include <vector>

/*
===============
TimeForFrame
===============
*/
_inline MTime TimeForFrame( int num )
{
    MTime	time;
    
    // set time unit to 24 frames per second
    time.setUnit( MTime::kFilm );
    time.setValue( num );
    return time.as( MTime::kSeconds );
}

/*
===============
SetFrame
===============
*/
_inline void SetFrame( int num )
{
    MTime	time;
    int		frameNum;
    
    frameNum = num;
    
    // set time unit to 24 frames per second
    time.setUnit( MTime::kFilm );
    time.setValue( frameNum );
    MGlobal::viewFrame( time );
}

