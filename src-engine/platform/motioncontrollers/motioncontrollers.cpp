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
//  File name:   motioncontrollers.cpp
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "../../qcommon/q_shared.h"
#include "motioncontrollers.h"
#include <OVR.h>
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

motcontr_export_t mcexport;
motcontr_import_t mcimport;

using namespace OVR;

// Ptr<> AddRef'ed, AutoCleaned
bool                         bInited = false;
HMDInfo                      pDevInfo;
Ptr<DeviceManager>           pManager;
Ptr<HMDDevice>               pHMD;
Ptr<SensorDevice>            pSensor;
SensorFusion                 pSensorFusion;
Util::Render::StereoConfig   pStereo;

// flags that the controller manager system can set to tell the graphics system to draw the instructions
// for the player
//static bool controller_manager_screen_visible = true;
//std::string controller_manager_text_string;

static void controller_manager_setup_callback( sixenseUtils::ControllerManager::setup_step step )
{
    if( sixenseUtils::getTheControllerManager()->isMenuVisible() )
    {
        //controller_manager_screen_visible = true;
        //controller_manager_text_string = sixenseUtils::getTheControllerManager()->getStepString();
        
        // We could also load the supplied controllermanager textures using the filename: sixenseUtils::getTheControllerManager()->getTextureFileName();
    }
    else
    {
        // We're done with the setup, so hide the instruction screen.
        //controller_manager_screen_visible = false;
    }
}

/*
=====================
OculusVR_Init
=====================
*/
int OculusVR_Init()
{
    bInited = false;
    
    System::Init( Log::ConfigureDefaultLog( OVR::LogMask_All ) );
    
    if( System::IsInitialized() )
    {
        int stage = -1;
        while( ++stage > -1 && !bInited )
        {
            switch( stage )
            {
                case 0:
                    pManager = *DeviceManager::Create();
                    if( pManager == NULL )
                        return bInited;
                    break;
                case 1:
                    pHMD     = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
                    if( pHMD == NULL )
                        return bInited;
                    break;
                case 2:
                    pSensor = *pHMD->GetSensor();
                    if( pSensor == NULL )
                        return bInited;
                    break;
                case 3:
                    pHMD->GetDeviceInfo( &pDevInfo );
                    break;
                default:
                    bInited = true;
                    break;
            };
        }
    }
    
    pSensorFusion.AttachToSensor( pSensor );
    
    return ( bInited ? 1 : 0 );
}

/*
=====================
OculusVR_Exit
=====================
*/
void OculusVR_Exit()
{
    pSensor.Clear();
    pHMD.Clear();
    pManager.Clear();
    System::Destroy();
    sixenseExit();
    bInited = false;
}

/*
=====================
OculusVR_QueryHMD
=====================
*/
int OculusVR_QueryHMD( OculusVR_HMDInfo* refHmdInfo )
{
    if( !bInited )
    {
        return 0;
    }
    
    HMDInfo src;
    if( pHMD->GetDeviceInfo( &src ) )
    {
        refHmdInfo->HResolution             = src.HResolution;
        refHmdInfo->VResolution             = src.VResolution;
        refHmdInfo->HScreenSize             = src.HScreenSize;
        refHmdInfo->VScreenSize             = src.VScreenSize;
        refHmdInfo->VScreenCenter           = src.VScreenCenter;
        refHmdInfo->EyeToScreenDistance     = src.EyeToScreenDistance;
        refHmdInfo->LensSeparationDistance  = src.LensSeparationDistance;
        refHmdInfo->InterpupillaryDistance  = src.InterpupillaryDistance;
        refHmdInfo->DistortionK[0]          = src.DistortionK[0];
        refHmdInfo->DistortionK[1]          = src.DistortionK[1];
        refHmdInfo->DistortionK[2]          = src.DistortionK[2];
        refHmdInfo->DistortionK[3]          = src.DistortionK[3];
        refHmdInfo->DesktopX                = src.DesktopX;
        refHmdInfo->DesktopY                = src.DesktopY;
        memcpy( refHmdInfo->DisplayDeviceName, src.DisplayDeviceName, sizeof( refHmdInfo->DisplayDeviceName ) );
    }
    
    return 1;
}

/*
=====================
OculusVR_Peek
=====================
*/
int OculusVR_Peek( float* yaw, float* pitch, float* roll )
{
    if( !bInited )
    {
        return 0;
    }
    
    Quatf hmdOrient = pSensorFusion.GetOrientation();
    hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>( yaw, pitch, roll );
    
    return 1;
}

/*
=====================
OculusVR_StereoConfig
=====================
*/
int OculusVR_StereoConfig( int eye, struct OculusVR_StereoCfg* stereoCfg )
{
    /*if (!bInited)
    {
    	return 0;
    }*/
    
    using namespace OVR::Util::Render;
    StereoEyeParams seye = pStereo.GetEyeRenderParams( ( eye == 0 ? StereoEye_Left : StereoEye_Right ) );
    
    pStereo.SetFullViewport( Viewport( stereoCfg->x, stereoCfg->y, stereoCfg->w, stereoCfg->h ) );
    pStereo.SetStereoMode( Stereo_LeftRight_Multipass );
    pStereo.SetHMDInfo( pDevInfo );
    
    if( pDevInfo.HScreenSize > 0.0f )
    {
        if( pDevInfo.HScreenSize > 0.140f ) // 7"
            pStereo.SetDistortionFitPointVP( -1.0f, 0.0f );
        else
            pStereo.SetDistortionFitPointVP( 0.0f, 1.0f );
    }
    
    stereoCfg->renderScale = pStereo.GetDistortionScale();
    
    DistortionConfig distCfg = pStereo.GetDistortionConfig();
    
    // Right Eye ?
    if( eye == 1 )
    {
        distCfg.XCenterOffset = -distCfg.XCenterOffset;
    }
    
    stereoCfg->x = seye.VP.x;
    stereoCfg->y = seye.VP.y;
    stereoCfg->w = seye.VP.w;
    stereoCfg->h = seye.VP.h;
    stereoCfg->XCenterOffset = distCfg.XCenterOffset;
    stereoCfg->distscale     = distCfg.Scale;
    stereoCfg->K[0] = distCfg.K[0];
    stereoCfg->K[1] = distCfg.K[1];
    stereoCfg->K[2] = distCfg.K[2];
    stereoCfg->K[3] = distCfg.K[3];
    
    return 1;
}

/*
=====================
RazerHydra_Init
=====================
*/
int RazerHydra_Init()
{
    if( sixenseInit() != SIXENSE_SUCCESS )
    {
        return 0;
    }
    
    sixenseUtils::getTheControllerManager()->setGameType( sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER );
    sixenseUtils::getTheControllerManager()->registerSetupCallback( controller_manager_setup_callback );
    ///sixenseSetFilterParams(0.0f,  1.0f, 0.5f, 0.5f);
    
    sixenseSetFilterEnabled( 0 );
    
    return 1;
}

/*
=====================
RazerHydra_Peek
=====================
*/
int RazerHydra_Peek( int hand, float* joyx, float* joyy, float* pos, float* trigger, float* yaw, float* pitch, float* roll, unsigned int* buttons )
{
    const  sixenseUtils::IControllerManager::controller_desc HandIndex[] =
    {
        sixenseUtils::ControllerManager::P1L, sixenseUtils::ControllerManager::P1R
    };
    static sixenseAllControllerData   acd;
    static sixenseUtils::ButtonStates btnState;
    
    sixenseSetActiveBase( 0 );
    sixenseGetAllNewestData( &acd );
    sixenseUtils::getTheControllerManager()->update( &acd );
    
    int idx = sixenseUtils::getTheControllerManager()->getIndex( HandIndex[hand] );
    
    sixenseControllerData& ref = acd.controllers[idx];
    btnState.update( &ref );
    
    *buttons = ref.buttons;
    
    if( !ref.enabled )
    {
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        *joyx  = 0.0f;
        *joyy  = 0.0f;
        *trigger = 0.0f;
        *yaw     = 0.0f;
        *pitch   = 0.0f;
        *roll    = 0.0f;
        return 0;
    }
    
    pos[0] = ref.pos[0];
    pos[1] = ref.pos[1];
    pos[2] = ref.pos[2];
    
    *joyx  = ref.joystick_x;
    *joyy  = ref.joystick_y;
    
    *trigger = ref.trigger;
    
    Quatf orient( ref.rot_quat[0], ref.rot_quat[1], ref.rot_quat[2], ref.rot_quat[3] );
    orient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>( yaw, pitch, roll );
    return 1;
}

/*
============
GetMotContrLibAPI
============
*/
motcontr_export_t* GetMotContrLibAPI( int apiVersion, motcontr_import_t* import )
{
    mcimport = *import;
    
    memset( &mcexport, 0, sizeof( mcexport ) );
    
    if( apiVersion != MOTLIB_API_VERSION )
    {
        Com_Printf( "Mismatched BOTLIB_API_VERSION: expected %i, got %i\n", MOTLIB_API_VERSION, apiVersion );
        return NULL;
    }
    
    mcexport.OculusVR_Init = OculusVR_Init;
    mcexport.OculusVR_Exit = OculusVR_Exit;
    mcexport.OculusVR_QueryHMD = OculusVR_QueryHMD;
    mcexport.OculusVR_Peek = OculusVR_Peek;
    mcexport.OculusVR_StereoConfig = OculusVR_StereoConfig;
    mcexport.RazerHydra_Init = RazerHydra_Init;
    mcexport.RazerHydra_Peek = RazerHydra_Peek;
    
    return &mcexport;
}

void QDECL Com_Printf( const char* msg, ... )
{
    va_list         argptr;
    char            text[1024];
    
    va_start( argptr, msg );
    _vsnprintf( text, sizeof( text ), msg, argptr );
    va_end( argptr );
    
    mcimport.Print( "%s", text );
}

void QDECL Com_Error( int level, const char* error, ... )
{
    va_list         argptr;
    char            text[1024];
    
    va_start( argptr, error );
    _vsnprintf( text, sizeof( text ), error, argptr );
    va_end( argptr );
    
    mcimport.Error( level, "%s", text );
}
