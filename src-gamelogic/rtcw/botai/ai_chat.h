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
//  File name:   ai_chat.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description: Quake3 bot AI
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __AI_CHAT_H__
#define __AI_CHAT_H__

//
int BotChat_EnterGame( bot_state_t* bs );
//
int BotChat_ExitGame( bot_state_t* bs );
//
int BotChat_StartLevel( bot_state_t* bs );
//
int BotChat_EndLevel( bot_state_t* bs );
//
int BotChat_HitTalking( bot_state_t* bs );
//
int BotChat_HitNoDeath( bot_state_t* bs );
//
int BotChat_HitNoKill( bot_state_t* bs );
//
int BotChat_Death( bot_state_t* bs );
//
int BotChat_Kill( bot_state_t* bs );
//
int BotChat_EnemySuicide( bot_state_t* bs );
//
int BotChat_Random( bot_state_t* bs );
// time the selected chat takes to type in
float BotChatTime( bot_state_t* bs );
// returns true if the bot can chat at the current position
int BotValidChatPosition( bot_state_t* bs );
// test the initial bot chats
void BotChatTest( bot_state_t* bs );

#endif // !__AI_CHAT_H__
