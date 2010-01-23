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
#include "text.h"
#include "resources.h"
#include "music.h"
#include "renderer.h"
#include "gamestate.h"
#include "redraw.h"
#include "movements.h"
#include "sound.h"
#include "animations.h"
#include "extra.h"

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

	sampleAmbiance[0] = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat[0] = *((uint16*)localScene);
	localScene += 2;
	sampleRound[0] = *((uint16*)localScene);
	localScene += 2;

	sampleAmbiance[1] = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat[1] = *((uint16*)localScene);
	localScene += 2;
	sampleRound[1] = *((uint16*)localScene);
	localScene += 2;

	sampleAmbiance[2] = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat[2] = *((uint16*)localScene);
	localScene += 2;
	sampleRound[2] = *((uint16*)localScene);
	localScene += 2;

	sampleAmbiance[3] = *((uint16*)localScene);
	localScene += 2;
	sampleRepeat[3] = *((uint16*)localScene);
	localScene += 2;
	sampleRound[3] = *((uint16*)localScene);
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
		uint16 staticFlags;
		
		reset_actor(i);

		staticFlags = *((uint16*)localScene);
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
		sceneActors[i].collisionX = sceneActors[i].X;
		localScene += 2;
		sceneActors[i].Y = *((uint16*)localScene);
		sceneActors[i].collisionY = sceneActors[i].Y;
		localScene += 2;
		sceneActors[i].Z = *((uint16*)localScene);
		sceneActors[i].collisionZ = sceneActors[i].Z;
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
		sceneActors[i].info0 = *((int16*)localScene);
		localScene += 2;
		sceneActors[i].info1 = *((int16*)localScene);
		localScene += 2;
		sceneActors[i].info2 = *((int16*)localScene);
		localScene += 2;
		sceneActors[i].info3 = *((int16*)localScene);
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

		sceneZones[i].infoData.generic.info0 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].infoData.generic.info1 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].infoData.generic.info2 = *((uint16*)localScene);
		localScene += 2;
		sceneZones[i].infoData.generic.info3 = *((uint16*)localScene);
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

/** Initialize new scene */
int32 init_scene(int32 index) {
	int32 sceneSize;

	// load scene from file
	sceneSize = hqr_getalloc_entry(&currentScene, HQR_SCENE_FILE, index);

	load_scene();

	return 1;
}


/** Reset scene */
void reset_scene() {
	int32 i;

	reset_extras();

	for (i = 0; i < NUM_SCENES_FLAGS; i++) {
		sceneFlags[i] = 0;
	}

	for (i = 0; i < OVERLAY_MAX_ENTRIES; i++) {
		overlayList[i].info0 = -1;
	}

	currentPositionInBodyPtrTab = 0;
	// useAlternatePalette = 0;
}

/** Change to another scene */
void change_scene() {
	int32 a;
	
	// change twinsen house destroyed hard-coded
	if (needChangeScene == 4 && gameFlags[30] != 0)
		needChangeScene = 118;

	// local backup previous scene
	previousSceneIdx = currentSceneIdx;
	currentSceneIdx = needChangeScene;

	stop_samples();

	reset_scene();
	load_hero_entities();

	sceneHero->controlMode = 1;
	sceneHero->zone = -1;
	sceneHero->positionInLifeScript = 0;
	sceneHero->positionInMoveScript = -1;
	sceneHero->labelIdx = -1;

	init_scene(needChangeScene);

	//TODO: treat holomap trajectories

	if (needChangeScene == 116 || needChangeScene == 117)
		currentTextBank = 10;

	init_text_bank(currentTextBank + 3);
	init_grid(needChangeScene);

	if (heroPositionType == POSITION_TYPE_ZONE) {
		newHeroX = zoneHeroX;
		newHeroY = zoneHeroY;
		newHeroZ = zoneHeroZ;
	}

	if (heroPositionType == POSITION_TYPE_SCENE || heroPositionType == POSITION_TYPE_NONE) {
		newHeroX = sceneHeroX;
		newHeroY = sceneHeroY;
		newHeroZ = sceneHeroZ;
	}

	sceneHero->X = newHeroX;
	sceneHero->Y = heroYBeforeFall = newHeroY;
	sceneHero->Z = newHeroZ;

	set_light_vector(alphaLight, betaLight, 0);

	if (previousSceneIdx != needChangeScene) {
		previousHeroBehaviour = heroBehaviour;
		previousHeroAngle = sceneHero->angle;
		save_game();
	}

	restart_hero_scene();

	for (a = 1; a < sceneNumActors; a++) {
		init_actor(a);
	}

	inventoryNumKeys = 0;
	disableScreenRecenter = 0;
	heroPositionType = POSITION_TYPE_NONE;
	sampleAmbienceTime = 0;

	newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
	newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
	newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;

	magicBallIdx = -1;
	heroMoved = 1;
	useCellingGrid = -1;
	cellingGridIdx = -1;
	reqBgRedraw = 1;
	// TODO: lockpalette = 0;

	needChangeScene = -1;
	changeRoomVar10 = 1;
	changeRoomVar11 = 14;

	set_light_vector(alphaLight, betaLight, 0);

	if (sceneMusic != -1) {
		play_midi_music(sceneMusic, -1);
	}
}

/** Process scene environment sound */
void process_environment_sound() {
	int16 s, currentAmb, decal, repeat;
	int16 sampleIdx = -1;

	if (lbaTime >= sampleAmbienceTime) {
		currentAmb = Rnd(4); // random ambiance

		for(s = 0; s < 4; s++) {
			if(!(samplePlayed & (1 << currentAmb))) { // if not already played
				samplePlayed |= (1 << currentAmb); // make sample played

				if(samplePlayed == 15) { // reset if all samples played
					samplePlayed = 0;
				}

				sampleIdx = sampleAmbiance[currentAmb];
				if(sampleIdx != -1) {
					decal = sampleRound[currentAmb];
					repeat = sampleRepeat[currentAmb];

					play_sample(sampleIdx, (0x1000+Rnd(decal)-(decal/2)), repeat, 110, -1, 110);
					break ;
				}
			}

			currentAmb++;    // try next ambiance
			currentAmb &= 3; // loop in all 4 ambiances
		}

		// compute next ambiance timer
		sampleAmbienceTime = lbaTime + (Rnd(sampleMinDelayRnd) + sampleMinDelay) * 50;
	}
}

/** Process zone extra bonus */
void process_zone_extra_bonus(ZoneStruct *zone) {
	int32 a, numBonus;
	int8 bonusTable[8], currentBonus;

	numBonus = 0;

	// bonus not used yet
	if (!zone->infoData.generic.info3) {
		for (a = 0; a < 5; a++) {
			if (zone->infoData.generic.info1 & (1 << (a + 4))) {
				bonusTable[numBonus++] = a;
			}
		}

		if (numBonus) {
			int32 angle, index;
			currentBonus = bonusTable[Rnd(numBonus)];

			// if bonus is magic an no magic level yet, then give life points
			if (!magicLevelIdx && currentBonus == 2) {
				currentBonus = 1;
			}

			angle = get_angle(Abs(zone->topRight.X + zone->bottomLeft.X)/2, Abs(zone->topRight.Z + zone->bottomLeft.Z)/2, sceneHero->X, sceneHero->Z);
			index = add_extra_bonus(Abs(zone->topRight.X + zone->bottomLeft.X)/2, zone->topRight.Y, Abs(zone->topRight.Z + zone->bottomLeft.Z)/2, 180, angle, currentBonus + 3, zone->infoData.generic.info2);
			
			if (index != -1) {
				extraList[index].type |= 0x400;
				zone->infoData.generic.info3 = 1; // set as used
			}
		}
	}
}


/** Process actor zones
	@param actorIdx Process actor index */
void process_actor_zones(int32 actorIdx) {
	int32 currentX, currentY, currentZ, z, tmpCellingGrid;
	ActorStruct *actor;

	actor = &sceneActors[actorIdx];

	currentX = actor->X;
	currentY = actor->Y;
	currentZ = actor->Z;

	actor->zone = -1;
	tmpCellingGrid = 0;

	if (!actorIdx) {
		currentActorInZone = actorIdx;
	}

	for (z = 0; z < sceneNumZones; z++) {
		ZoneStruct *zone = &sceneZones[z];

		// check if actor is in zone
		if ((currentX >= zone->bottomLeft.X && currentX <= zone->topRight.X) &&
			(currentY >= zone->bottomLeft.Y && currentY <= zone->topRight.Y) &&
			(currentZ >= zone->bottomLeft.Z && currentZ <= zone->topRight.Z)) {
			switch (zone->type) {
			case kCube:
				if (!actorIdx && actor->life > 0) {
					needChangeScene = zone->infoData.ChangeScene.newSceneIdx;
					zoneHeroX = actor->X - zone->bottomLeft.X + zone->infoData.ChangeScene.X;
					zoneHeroY = actor->Y - zone->bottomLeft.Y + zone->infoData.ChangeScene.Y;
					zoneHeroZ = actor->Z - zone->bottomLeft.Z + zone->infoData.ChangeScene.Z;
					heroPositionType = POSITION_TYPE_ZONE;
				}
				break;
			case kCamera:
				if (currentlyFollowedActor == actorIdx) {
					disableScreenRecenter = 1;
					if (newCameraX != zone->infoData.CameraView.X || newCameraY != zone->infoData.CameraView.Y || newCameraZ != zone->infoData.CameraView.Z) {
						newCameraX = zone->infoData.CameraView.X;
						newCameraY = zone->infoData.CameraView.Y;
						newCameraZ = zone->infoData.CameraView.Z;
						reqBgRedraw = 1;
					}
				}
				break;
			case kSceneric:
				actor->zone = zone->infoData.Sceneric.zoneIdx;
				break;
			case kGrid:
				if (currentlyFollowedActor == actorIdx) {
					tmpCellingGrid = 1;
					if (useCellingGrid != zone->infoData.CeillingGrid.newGrid) {
						if (zone->infoData.CeillingGrid.newGrid != -1) {
							create_grid_map();
						}

						useCellingGrid = zone->infoData.CeillingGrid.newGrid;
						cellingGridIdx = z;
						freeze_time();
						init_celling_grid(useCellingGrid);
						unfreeze_time();
					}
				}
				break;
			case kObject:
				if (!actorIdx && heroAction != 0) {
					init_anim(ANIM_ACTION, 1, 0, 0);
					process_zone_extra_bonus(zone);
				}
				break;
			case kText:
				if (!actorIdx && heroAction != 0) {
					freeze_time();
					set_font_cross_color(zone->infoData.DisplayText.textColor);
					talkingActor = actorIdx;
					draw_text_fullscreen(zone->infoData.DisplayText.textIdx);
					unfreeze_time();
					redraw_engine_actions(1);
				}
				break;
			case kLadder:
				if (!actorIdx && heroBehaviour != PROTOPACK && (actor->anim == ANIM_FORWARD || actor->anim == ANIM_TOP_LADDER || actor->anim == ANIM_CLIMB_LADDER)) {
					rotate_actor(actor->boudingBox.X.bottomLeft, actor->boudingBox.Z.bottomLeft, actor->angle + 0x580);
					destX += processActorX;
					destZ += processActorZ;

					if (destX >= 0 && destZ >= 0 && destX <= 0x7E00 && destZ <= 0x7E00) {
						if (get_brick_shape(destX, actor->Y + 0x100, destZ)) {
							currentActorInZone = 1;
							if (actor->Y >= Abs(zone->bottomLeft.Y + zone->topRight.Y) / 2) {
								init_anim(ANIM_TOP_LADDER, 2, 0, actorIdx); // reached end of ladder
							} else {
								init_anim(ANIM_CLIMB_LADDER, 0, 255, actorIdx); // go up in ladder
							}
						}
					}
				}
				break;
			}
		}
	}

	if (!tmpCellingGrid && actorIdx == currentlyFollowedActor && useCellingGrid != -1) {
		useCellingGrid = -1;
		cellingGridIdx = -1;
		create_grid_map();
		reqBgRedraw = 1;
	}
}
