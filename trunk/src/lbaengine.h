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

/** Work video buffer */
uint8 *workVideoBuffer;
/** Main game video buffer */
uint8 *frontVideoBuffer;
/** Auxiliar game video buffer */
uint8 *frontVideoBufferbis;

/** temporary screen table */
int32 screenLockupTable[2000];

int16 leftMouse;
int16 rightMouse;

int32 quitGame;
volatile int32 lbaTime;

int16 projPosXScreen; // fullRedrawVar1
int16 projPosYScreen; // fullRedrawVar2
int16 projPosZScreen; // fullRedrawVar3
int16 projPosX;
int16 projPosY;
int16 projPosZ;

int16 currentlyFollowedActor;

int32 loopPressedKey; // mainLoopVar5

void freeze_time();
void unfreeze_time();

int32 game_engine_loop();

void init_engine_vars(int32 save);
void init_engine_projections();

#endif
