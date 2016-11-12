//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
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
//  File name:   ai_dmnet.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: Quake3 bot AI
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#define MAX_NODESWITCHES    50

void AIEnter_Intermission( bot_state_t* bs );
void AIEnter_Observer( bot_state_t* bs );
void AIEnter_Respawn( bot_state_t* bs );
void AIEnter_Stand( bot_state_t* bs );
void AIEnter_Seek_ActivateEntity( bot_state_t* bs );
void AIEnter_Seek_NBG( bot_state_t* bs );
void AIEnter_Seek_LTG( bot_state_t* bs );
void AIEnter_Seek_Camp( bot_state_t* bs );
void AIEnter_Battle_Fight( bot_state_t* bs );
void AIEnter_Battle_Chase( bot_state_t* bs );
void AIEnter_Battle_Retreat( bot_state_t* bs );
void AIEnter_Battle_NBG( bot_state_t* bs );
int AINode_Intermission( bot_state_t* bs );
int AINode_Observer( bot_state_t* bs );
int AINode_Respawn( bot_state_t* bs );
int AINode_Stand( bot_state_t* bs );
int AINode_Seek_ActivateEntity( bot_state_t* bs );
int AINode_Seek_NBG( bot_state_t* bs );
int AINode_Seek_LTG( bot_state_t* bs );
int AINode_Battle_Fight( bot_state_t* bs );
int AINode_Battle_Chase( bot_state_t* bs );
int AINode_Battle_Retreat( bot_state_t* bs );
int AINode_Battle_NBG( bot_state_t* bs );

void BotResetNodeSwitches( void );
void BotDumpNodeSwitches( bot_state_t* bs );

