/** @file gamestate.h
	@brief
	This file contains game state routines

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team
	Copyright (C) 2008-2013 Prequengine team
	Copyright (C) 2002-2007 The TwinEngine team

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sys.h"

#define NUM_GAME_FLAGS			255
#define NUM_INVENTORY_ITEMS		28

#define GAMEFLAG_HAS_HOLOMAP			0
#define GAMEFLAG_HAS_MAGICBALL			1
#define GAMEFLAG_HAS_SABRE				2
#define GAMEFLAG_TUNIC					4
#define GAMEFLAG_BOOKOFBU				6
#define GAMEFLAG_PROTOPACK				12
#define GAMEFLAG_MECA_PINGUIN			14
#define GAMEFLAG_HAS_CLOVER_LEAF		27
#define GAMEFLAG_INVENTORY_DISABLED		70

/** Magicball strength*/
enum MagicballStrengthType {
	kNoBallStrenght			= 2,
	kYellowBallStrenght		= 3,
	kGreenBallStrenght		= 4,
	kRedBallStrenght		= 6,
	kFireBallStrength		= 8
};

/** LBA engine game flags to save quest states */
extern uint8 gameFlags[256];

/** LBA engine chapter */
extern int16 gameChapter;

/** Magic ball type index */
extern int16 magicBallIdx;
/** Magic ball num bounce */
extern int16 magicBallNumBounce;
/** Magic ball auxiliar bounce number */
extern int16 magicBallAuxBounce; // magicBallParam
/** Magic level index */
extern int16 magicLevelIdx;

/** Store the number of inventory keys */
extern int16 inventoryNumKeys;
/** Store the number of inventory kashes */
extern int16 inventoryNumKashes;
/** Store the number of inventory clover leafs boxes */
extern int16 inventoryNumLeafsBox;
/** Store the number of inventory clover leafs */
extern int16 inventoryNumLeafs;
/** Store the number of inventory magic points */
extern int16 inventoryMagicPoints;
/** Store the number of gas */
extern int16 inventoryNumGas;

/** Its using FunFrock Sabre */
extern int16 usingSabre;

/** Inventory used flags */
extern uint8 inventoryFlags[NUM_INVENTORY_ITEMS];

/** Inventory used flags */
extern uint8 holomapFlags[150]; // GV14

extern int8 savePlayerName[30]; // playerName

extern int32 gameChoices[10]; // inGameMenuData
extern int32 numChoices;      // numOfOptionsInChoice
extern int16 gameChoicesSettings[18]; // choiceTab -  same structure as menu settings
extern int32 choiceAnswer; // inGameMenuAnswer

extern int32 magicLevelStrengthOfHit[];

/** Initialize all engine variables */
void initEngineVars(int32 save);

/** Initialize engine 3D projections */
void initEngineProjections();

void processFoundItem(int32 item);

void loadGame();
void saveGame();

void processGameChoices(int32 choiceIdx);

void processGameoverAnimation();

#endif
