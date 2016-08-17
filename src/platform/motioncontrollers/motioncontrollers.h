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
//  File name:   dllmain.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#if defined (_WIN32)
#ifdef MOTIONCONTROLLER_EXPORTS
#if defined __cplusplus
#define MOTIONCONTROLLER_API extern "C" __declspec(dllexport)
#else
#define MOTIONCONTROLLER_API __declspec(dllexport)
#endif
#else
#if defined __cplusplus
#define MOTIONCONTROLLER_API extern "C" __declspec(dllimport)
#else
#define MOTIONCONTROLLER_API __declspec(dllimport)
#endif
#endif
#else
#define MOTIONCONTROLLER_API
#endif

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

MOTIONCONTROLLER_API int   OculusVR_Init();
MOTIONCONTROLLER_API void  OculusVR_Exit();
MOTIONCONTROLLER_API int   OculusVR_QueryHMD( struct OculusVR_HMDInfo* refHmdInfo );
MOTIONCONTROLLER_API int   OculusVR_Peek( float* yaw, float* pitch, float* roll );
MOTIONCONTROLLER_API int   OculusVR_StereoConfig( int eye, struct OculusVR_StereoCfg* stereoCfg );
MOTIONCONTROLLER_API int   RazorHydra_Peek( int hand, float* joyx, float* joyy, float* pos, float* trigger, float* yaw, float* pitch, float* roll, unsigned int* buttons );