/** @file state.h
	@brief
	This file contains movies routines

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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sys.h"

#define NUM_GAME_FLAGS			255
#define NUM_INVENTORY_ITEMS		28

#define GAMEFLAG_HAS_MAGICBALL			1
#define GAMEFLAG_HAS_SABRE				2
#define GAMEFLAG_TUNIC					4
#define GAMEFLAG_INVENTORY_DISABLED		70

#define MAGIC_STRENGTH_NONE			2
#define MAGIC_STRENGTH_YELLOW_BALL	3
#define MAGIC_STRENGTH_GREEN_BALL	4
#define MAGIC_STRENGTH_RED_BALL		6
#define MAGIC_STRENGTH_FIRE_BALL	8

/** LBA engine game flags to save quest states */
uint8 gameFlags[256];

/** LBA engine chapter */
int16 gameChapter;

extern int32 magicLevelStrengthOfHit[];

/** Magic ball type index */
int16 magicBallIdx;
/** Magic ball num bounce */
int16 magicBallNumBounce;
/** Magic ball auxiliar bounce number */
int16 magicBallAuxBounce; // magicBallParam
/** Magic level index */
int16 magicLevelIdx;

/** Inventory used flags */
uint8 inventoryFlags[NUM_INVENTORY_ITEMS];

/** Inventory used flags */
uint8 holomapFlags[150]; // GV14

/** Store the number of inventory keys */
int16 inventoryNumKeys;
/** Store the number of inventory kashes */
int16 inventoryNumKashes;
/** Store the number of inventory clover leafs boxes */
int16 inventoryNumLeafsBox;
/** Store the number of inventory clover leafs */
int16 inventoryNumLeafs;
/** Store the number of inventory magic points */
int16 inventoryMagicPoints;
/** Store the number of gas */
int16 inventoryNumGas;

/** Its using FunFrock Sabre */
int16 usingSabre;


/** Initialize all engine variables */
void init_engine_vars(int32 save);

/** Initialize engine 3D projections */
void init_engine_projections();

#endif
