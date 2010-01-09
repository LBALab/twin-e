/** @file gamestate.c
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

#include "gamestate.h"
#include "scene.h"
#include "redraw.h"
#include "dialogues.h"
#include "menu.h"
#include "renderer.h"
#include "grid.h"
#include "lbaengine.h"
#include "interface.h"
#include "animations.h"
#include "keyboard.h"
#include "resources.h"
#include "extra.h"

int32 magicLevelStrengthOfHit[] = {
	MAGIC_STRENGTH_NONE,
	MAGIC_STRENGTH_YELLOW_BALL,
	MAGIC_STRENGTH_GREEN_BALL,
	MAGIC_STRENGTH_RED_BALL,
	MAGIC_STRENGTH_FIRE_BALL,
	0
};

/** Initialize engine 3D projections */
void init_engine_projections() { // reinitAll1
	set_ortho_projection(311, 240, 512);
	set_base_translation(0, 0, 0);
	set_base_rotation(0, 0, 0);
	set_light_vector(alphaLight, betaLight, 0);
}

/** Initialize variables */
void init_scene_vars() {
	int32 i;

	reset_extras();
	
	for (i = 0; i < OVERLAY_MAX_ENTRIES; i++) {
		overlayList[i].info0 = -1;
	}

	for (i = 0; i < NUM_SCENES_FLAGS; i++) {
		sceneFlags[i] = 0;
	}

	for (i = 0; i < NUM_GAME_FLAGS; i++) {
		gameFlags[i] = 0;
	}

	for (i = 0; i < NUM_INVENTORY_ITEMS; i++) {
		inventoryFlags[i] = 0;
	}

	sampleAmbiance[0] = -1;
	sampleAmbiance[1] = -1;
	sampleAmbiance[2] = -1;
	sampleAmbiance[3] = -1;

	sampleRepeat[0] = 0;
	sampleRepeat[1] = 0;
	sampleRepeat[2] = 0;
	sampleRepeat[3] = 0;

	sampleRound[0] = 0;
	sampleRound[1] = 0;
	sampleRound[2] = 0;
	sampleRound[3] = 0;

	for (i = 0; i < 150; i++) {
		holomapFlags[i] = 0;
	}

	sceneNumActors = 0;
	sceneNumZones  = 0;
	sceneNumTracks = 0;

	currentPositionInBodyPtrTab = 0;
}

void init_hero_vars() { // reinitAll3
	reset_actor(0); // reset Hero

	magicBallIdx = -1;

	inventoryNumLeafsBox = 2;
	inventoryNumLeafs    = 2;
	inventoryNumKashes   = 0;
	inventoryNumKeys     = 0;
	inventoryMagicPoints = 0;

	usingSabre = 0;

	sceneHero->body = 0;
	sceneHero->life = 50;
	sceneHero->talkColor = 4;
}

/** Initialize all engine variables */
void init_engine_vars(int32 save) { // reinitAll
	reset_clip();

	alphaLight = 896;
	betaLight = 950;
	init_engine_projections();
	init_scene_vars();
	init_hero_vars();
	
	newHeroX = 0x2000;
	newHeroY = 0x1800;
	newHeroZ = 0x2000;

	currentSceneIdx = -1;
	needChangeScene = 0;
	//brutalExit = -1;
	mecaPinguinIdx = -1;
		
	inventoryNumLeafs = 0;
	inventoryNumLeafsBox = 2;
	inventoryMagicPoints = 0;
	inventoryNumKashes = 0;
	inventoryNumKeys = 0;
	inventoryNumGas = 0;

	magicLevelIdx = 0;
	usingSabre = 0;

	gameChapter = 0;

	currentTextBank = 0;
	currentlyFollowedActor = 0;
	heroBehaviour = 0;
	previousHeroAngle = 0;
	previousHeroBehaviour = 0;

	if (save == -1) {
		// TODO: load game
		if (newHeroX == -1) {
			heroPositionType = POSITION_TYPE_NONE;	
		}
	}
}

