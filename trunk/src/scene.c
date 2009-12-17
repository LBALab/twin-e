/** @file scene.c
	@brief
	This file contains main scenario routines

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scene.h"
#include "actor.h"
#include "grid.h"
#include "main.h"
#include "lbaengine.h"
#include "dialogues.h"
#include "resources.h"
#include "music.h"
#include "renderer.h"
#include "gamestate.h"

uint8* currentScene;


void set_actor_staticflags(int32 actorIdx, uint16 staticFlags) {
	if (staticFlags & 0x1) {
		sceneActors[actorIdx].staticFlags.bComputeCollisionWithObj = 1;
	}
	if (staticFlags & 0x2) {
		sceneActors[actorIdx].staticFlags.bComputeCollisionWithBricks = 1;
	}
	if (staticFlags & 0x4) {
		sceneActors[actorIdx].staticFlags.bIsZonable = 1;
	}
	if (staticFlags & 0x8) {
		sceneActors[actorIdx].staticFlags.bUsesClipping = 1;
	}
	if (staticFlags & 0x10) {
		sceneActors[actorIdx].staticFlags.bCanBePushed = 1;
	}
	if (staticFlags & 0x20) {
		sceneActors[actorIdx].staticFlags.bComputeLowCollision = 1;
	}
	if (staticFlags & 0x40) {
		sceneActors[actorIdx].staticFlags.bCanDrown = 1;
	}
	if (staticFlags & 0x80) {
		sceneActors[actorIdx].staticFlags.bUnk80 = 1;
	}

	if (staticFlags & 0x100) {
		sceneActors[actorIdx].staticFlags.bUnk0100 = 1;
	}
	if (staticFlags & 0x200) {
		sceneActors[actorIdx].staticFlags.bIsHidden = 1;
	}
	if (staticFlags & 0x400) {
		sceneActors[actorIdx].staticFlags.bIsSpriteActor = 1;
	}
	if (staticFlags & 0x800) {
		sceneActors[actorIdx].staticFlags.bCanFall = 1;
	}
	if (staticFlags & 0x1000) {
		sceneActors[actorIdx].staticFlags.bDoesntCastShadow = 1;
	}
	if (staticFlags & 0x2000) {
		//sceneActors[actorIdx].staticFlags.bIsBackgrounded = 1;
	}
	if (staticFlags & 0x4000) {
		sceneActors[actorIdx].staticFlags.bIsCarrierActor = 1;
	}
	if (staticFlags & 0x8000) {
		sceneActors[actorIdx].staticFlags.bUseMiniZv = 1;
	}
}

void load_scene() {
	int32 i;
	int32 scriptSize = 0;
	uint8* localScene = currentScene;

	// load scene ambience properties
	currentTextBank = *(localScene++);
	currentGameOverScene = *(localScene++);
	localScene += 4;

	alphaLight = *((uint16*)localScene);
	localScene += 2;
	betaLight = *((uint16*)localScene);
	localScene += 2;

	sampleAmbience.info0 = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat.info0 = *((uint16*)localScene);
	localScene += 2;
	sampleRound.info0 = *((uint16*)localScene);
	localScene += 2;

	sampleAmbience.info1 = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat.info1 = *((uint16*)localScene);
	localScene += 2;
	sampleRound.info1 = *((uint16*)localScene);
	localScene += 2;

	sampleAmbience.info2 = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat.info2 = *((uint16*)localScene);
	localScene += 2;
	sampleRound.info2 = *((uint16*)localScene);
	localScene += 2;

	sampleAmbience.info3 = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat.info3 = *((uint16*)localScene);
	localScene += 2;
	sampleRound.info3 = *((uint16*)localScene);
	localScene += 2;

	sampleMinDelay = *((uint16*)localScene);
	localScene += 2;
	sampleMinDelayRnd = *((uint16*)localScene);
	localScene += 2;

	sceneMusic = *(localScene++);

	// load hero properties
	sceneHeroX = *((uint16*)localScene);
	localScene += 2;
	sceneHeroY = *((uint16*)localScene);
	localScene += 2;
	sceneHeroZ = *((uint16*)localScene);
	localScene += 2;

	scriptSize = *((uint16*)localScene);
	localScene += 2;
	sceneHero->moveScript = localScene;
	localScene += scriptSize;

	scriptSize = *((uint16*)localScene);
	localScene += 2;
	sceneHero->lifeScript = localScene;
	localScene += scriptSize;

	sceneNumActors = *((uint16*)localScene);
	localScene += 2;

	for (i = 1; i < sceneNumActors; i++) {
		uint16 staticFlags = *((uint16*)localScene);
		localScene += 2;
		set_actor_staticflags(i, staticFlags);

		sceneActors[i].entity = *((uint16*)localScene);
		localScene += 2;

		if (!sceneActors[i].staticFlags.bIsSpriteActor) {
			hqr_getalloc_entry(&sceneActors[i].entityDataPtr, HQR_FILE3D_FILE, sceneActors[i].entity);
		}

		sceneActors[i].body = *(localScene++);
		sceneActors[i].anim = *(localScene++);
		sceneActors[i].sprite = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].X = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].Y = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].Z = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].strengthOfHit = *(localScene++);
		sceneActors[i].bonusParameter = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].bonusParameter &= 0xFE;
		sceneActors[i].angle = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].speed = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].controlMode = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].info0 = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].info1 = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].info2 = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].info3 = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].followedActor = sceneActors[i].info3;
		sceneActors[i].bonusAmount = *(localScene++);
		sceneActors[i].talkColor = *(localScene++);
		sceneActors[i].armor = *(localScene++);
		sceneActors[i].life = *(localScene++);

		scriptSize = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].moveScript = localScene;
		localScene += scriptSize;

		scriptSize = *((uint16*)localScene);
		localScene += 2;
		sceneActors[i].lifeScript = localScene;
		localScene += scriptSize;
	}

	sceneNumZones = *((uint16*)localScene);
	localScene += 2;

	for (i = 0; i < sceneNumZones; i++) {
		sceneZones[i].bottomLeft.X = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].bottomLeft.Y = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].bottomLeft.Z = *((uint16*)localScene);
		localScene += 2;

		sceneZones[i].topRight.X = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].topRight.Y = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].topRight.Z = *((uint16*)localScene);
		localScene += 2;

		sceneZones[i].type = *((uint16*)localScene);
		localScene += 2;

		sceneZones[i].info0 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].info1 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].info2 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].info3 = *((uint16*)localScene);
		localScene += 2;

		sceneZones[i].snap = *((uint16*)localScene);
		localScene += 2;
	}

	sceneNumTracks = *((uint16*)localScene);
	localScene += 2;

	for (i = 0; i < sceneNumTracks; i++) {
		sceneTracks[i].X = *((uint16*)localScene);
		localScene += 2;
		sceneTracks[i].Y = *((uint16*)localScene);
		localScene += 2;
		sceneTracks[i].Z = *((uint16*)localScene);
		localScene += 2;
	}
}


int32 init_scene(int32 index) {
	int32 sceneSize;

	// load scene from file
	sceneSize = hqr_getalloc_entry(&currentScene, HQR_SCENE_FILE, index);

	load_scene();

	return 1;
}

void change_scene() {
	int32 previousSceneIdx, a;
	// change twinsen house destroyed hard-coded
	if (needChangeScene == 4 && gameFlags[30] != 0)
		needChangeScene = 118;

	// local backup previous scene
	previousSceneIdx = currentSceneIdx;

	currentSceneIdx = needChangeScene;

	//TODO: stop_samples
	currentPositionInBodyPtrTab = 0;
	//TODO: clear_scene
	load_hero_entities();
	//TODO: reset twinsen variables

	init_scene(needChangeScene);

	//TODO: treat holomap trajectories

	if (needChangeScene == 116 || needChangeScene == 117)
		currentTextBank = 10;

	init_dialogue_bank(currentTextBank + 3);

	init_grid(needChangeScene);

	// TODO: set Hero position

	sceneHero->X = sceneHeroX;
	sceneHero->Y = sceneHeroY;
	sceneHero->Z = sceneHeroZ;

	newCameraX = sceneHero->X >> 9;
	newCameraY = sceneHero->Y >> 8;
	newCameraZ = sceneHero->X >> 9;

	set_light_vector(alphaLight, betaLight, 0); // TODO: set light vector

	// TODO: backup previous scene vars and save game

	restart_hero_scene(); //TODO: RestartPerso()

	// TODO: reset Hero vars

	// TODO: load scene actors
	for (a = 1; a < sceneNumActors; a++) {
		init_actor(a);
	}

	// TODO: reset current scene global variables

	newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
	newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
	newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;

	// TODO: play midi music (scene music)
	play_midi_music(sceneMusic, -1);

	// TODO: all the other change scene definitions
}
