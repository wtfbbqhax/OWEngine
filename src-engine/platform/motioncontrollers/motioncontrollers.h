//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 2013 Frederic Lauzon
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
//  File name:   motioncontrollers.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __MOTIONCONTROLLERS_H__
#define __MOTIONCONTROLLERS_H__

#define MOTLIB_API_VERSION 1

struct OculusVR_HMDInfo
{
    unsigned      HResolution;
    unsigned      VResolution;
    float         HScreenSize;
    float         VScreenSize;
    float         VScreenCenter;
    float         EyeToScreenDistance;
    float         LensSeparationDistance;
    float         InterpupillaryDistance;
    float         DistortionK[4];
    int           DesktopX;
    int           DesktopY;
    char          DisplayDeviceName[32];
};

struct OculusVR_StereoCfg
{
    int   x;
    int   y;
    int   w;
    int   h;
    float renderScale;
    float XCenterOffset;
    float distscale;
    float K[4];
};

#define SIXENSE_BUTTON_BUMPER   (0x01<<7)
#define SIXENSE_BUTTON_JOYSTICK (0x01<<8)
#define SIXENSE_BUTTON_1        (0x01<<5)
#define SIXENSE_BUTTON_2        (0x01<<6)
#define SIXENSE_BUTTON_3        (0x01<<3)
#define SIXENSE_BUTTON_4        (0x01<<4)
#define SIXENSE_BUTTON_START    (0x01<<0)

typedef struct motcontr_import_s
{
    //print messages from the bot library
    void ( *Print )( const char* fmt, ... );
    //Dushan
    void ( *Error )( int errorLevel, const char* fmt, ... );
} motcontr_import_t;

//bot AI library imported functions
typedef struct motcontr_export_s
{
    int ( *OculusVR_Init )();
    void ( *OculusVR_Exit )();
    int ( *OculusVR_QueryHMD )( struct OculusVR_HMDInfo* refHmdInfo );
    int ( *OculusVR_Peek )( float* yaw, float* pitch, float* roll );
    int ( *OculusVR_StereoConfig )( int eye, struct OculusVR_StereoCfg* stereoCfg );
    int ( *RazerHydra_Init )();
    int ( *RazerHydra_Peek )( int hand, float* joyx, float* joyy, float* pos, float* trigger, float* yaw, float* pitch, float* roll, unsigned int* buttons );
} motcontr_export_t;

//linking library
typedef motcontr_export_t* ( *GetMotContrLibAPI_t )( int apiVersion, motcontr_import_t* rimp );

#endif // !__MOTIONCONTROLLERS_H__
