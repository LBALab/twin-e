/** @file lbaengine.h
	@brief
	This file contains the main game engine routines

	Prequengine: a Little Big Adventure engine

	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$URL$
	$Id$
*/


#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "sys.h"
#include "actor.h"

int32 quitGame;
volatile int32 lbaTime;

int16 leftMouse;
int16 rightMouse;

/** Work video buffer */
uint8 *workVideoBuffer;
/** Main game video buffer */
uint8 *frontVideoBuffer;
/** Auxiliar game video buffer */
uint8 *frontVideoBufferbis;

/** temporary screen table */
int32 screenLockupTable[2000];

ActorMoveStruct loopMovePtr; // mainLoopVar1

int32 loopPressedKey;         // mainLoopVar5
int32 previousLoopPressedKey; // mainLoopVar6
int32 loopCurrentKey;    // mainLoopVar7
int32 loopInventoryItem; // mainLoopVar9

int32 loopActorStep; // mainLoopVar17

/** Disable screen recenter */
int16 disableScreenRecenter;



void freeze_time();
void unfreeze_time();

int32 game_engine_loop();

#endif
