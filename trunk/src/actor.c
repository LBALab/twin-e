/** @file actor.c
	@brief
	This file contains scene actor routines

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

#include "actor.h"
#include "lbaengine.h"
#include "scene.h"
#include "hqrdepack.h"
#include "resources.h"
#include "renderer.h"
#include "grid.h"
#include "animations.h"
#include "renderer.h"
#include "movements.h"

/** Actors 3D body table - size of NUM_BODIES */
uint8 *bodyTable[NUM_BODIES];

//TODO: add all the needed content here
/** Restart hero variables while opening new scenes */
void restart_hero_scene() {
	sceneHero->controlMode = 1;
	memset(&sceneHero->dynamicFlags, 0, 2);
	memset(&sceneHero->staticFlags, 0, 2);

	sceneHero->staticFlags.bComputeCollisionWithObj = 1;
	sceneHero->staticFlags.bComputeCollisionWithBricks = 1;
	sceneHero->staticFlags.bIsZonable = 1;
	sceneHero->staticFlags.bCanDrown = 1;
	sceneHero->staticFlags.bCanFall = 1;

	sceneHero->armor = 1;
	sceneHero->positionInMoveScript = -1;
	sceneHero->labelIdx = -1;
	sceneHero->positionInLifeScript = 0;
	sceneHero->zone = -1;
	sceneHero->angle = previousHeroAngle;

	set_actor_angle_safe(sceneHero->angle, sceneHero->angle, 0, &sceneHero->move);
	set_behaviour(previousHeroBehaviour);
}

// TODO: finish this
/** Load hero 3D body and animations */
void load_hero_entities() {
	int32 size;

	size = hqr_getalloc_entry(&heroEntityATHLETIC, HQR_FILE3D_FILE, FILE3DHQR_HEROATHLETIC);
	sceneHero->entityDataPtr = heroEntityATHLETIC;
	heroAnimIdxATHLETIC = get_body_anim_index(0, 0);

	size = hqr_getalloc_entry(&heroEntityAGGRESSIVE, HQR_FILE3D_FILE, FILE3DHQR_HEROAGGRESSIVE);
	sceneHero->entityDataPtr = heroEntityAGGRESSIVE;
	heroAnimIdxAGGRESSIVE = get_body_anim_index(0, 0);

	size = hqr_getalloc_entry(&heroEntityDISCRETE, HQR_FILE3D_FILE, FILE3DHQR_HERODISCRETE);
	sceneHero->entityDataPtr = heroEntityDISCRETE;
	heroAnimIdxDISCRETE = get_body_anim_index(0, 0);

	size = hqr_getalloc_entry(&heroEntityPROTOPACK, HQR_FILE3D_FILE, FILE3DHQR_HEROPROTOPACK);
	sceneHero->entityDataPtr = heroEntityPROTOPACK;
	heroAnimIdxPROTOPACK = get_body_anim_index(0, 0);

	size = hqr_getalloc_entry(&heroEntityNORMAL, HQR_FILE3D_FILE, FILE3DHQR_HERONORMAL);
	sceneHero->entityDataPtr = heroEntityNORMAL;
	heroAnimIdxNORMAL = get_body_anim_index(0, 0);

	sceneHero->animExtraPtr = currentActorAnimExtraPtr;
}

/** Set hero behaviour
	@param behaviour behaviour value to set */
void set_behaviour(int32 behaviour) {
	int32 bodyIdx;

	switch (behaviour) {
	case NORMAL:
		heroBehaviour = NORMAL;
		sceneHero->entityDataPtr = heroEntityNORMAL;
		break;
	case ATHLETIC:
		heroBehaviour = ATHLETIC;
		sceneHero->entityDataPtr = heroEntityATHLETIC;
		break;
	case AGGRESSIVE:
		heroBehaviour = AGGRESSIVE;
		sceneHero->entityDataPtr = heroEntityAGGRESSIVE;
		break;
	case DISCRETE:
		heroBehaviour = DISCRETE;
		sceneHero->entityDataPtr = heroEntityDISCRETE;
		break;
	case PROTOPACK:
		heroBehaviour = PROTOPACK;
		sceneHero->entityDataPtr = heroEntityPROTOPACK;
		break;
	};

	bodyIdx = sceneHero->body;

	sceneHero->entity = -1;
	sceneHero->body = -1;

	init_model_actor(bodyIdx, 0);

	sceneHero->anim = -1;
	sceneHero->animType = 0;

	init_anim(ANIM_STANDING, 0, 255, 0);
}

/** Initialize sprite actor
	@param actorIdx sprite actor index */
void init_sprite_actor(int32 actorIdx) {
	ActorStruct *localActor = &sceneActors[actorIdx];

	if (localActor->staticFlags.bIsSpriteActor && localActor->sprite != -1 && localActor->entity != localActor->sprite) {
		int16 *ptr = (int16 *)(spriteBoundingBoxPtr + localActor->sprite * 16 + 4);

		localActor->entity = localActor->sprite;
		localActor->boudingBox.X.bottomLeft = *(ptr++);
		localActor->boudingBox.X.topRight = *(ptr++);
		localActor->boudingBox.Y.bottomLeft = *(ptr++);
		localActor->boudingBox.Y.topRight = *(ptr++);
		localActor->boudingBox.Z.bottomLeft = *(ptr++);
		localActor->boudingBox.Z.topRight = *(ptr++);
	}
}

/** Initialize 3D actor body
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
int32 init_body(int32 bodyIdx, int32 actorIdx) {
	ActorStruct *localActor;
	uint8 *bodyPtr;
	uint8 var1;
	uint8 var2;
	uint8 *bodyPtr2;
	uint8 *bodyPtr3;
	uint8 *bodyPtr4;
	int16 *bodyPtr5;
	int16 flag;
	int32 index;

	localActor = &sceneActors[actorIdx];
	bodyPtr = localActor->entityDataPtr;

	do {
		var1 = *(bodyPtr++);

		if (var1 == 0xFF)
			return (-1);

		bodyPtr2 = bodyPtr + 1;

		if (var1 == 1) {
			var2 = *(bodyPtr);

			if (var2 == bodyIdx) {
				bodyPtr3 = bodyPtr2 + 1;
				flag = *((uint16*)bodyPtr3);

				if (!(flag & 0x8000)) {
					hqr_getalloc_entry(&bodyTable[currentPositionInBodyPtrTab], HQR_BODY_FILE, flag & 0xFFFF);

					if (!bodyTable[currentPositionInBodyPtrTab]) {
						printf("HQR ERROR: Loading body entities\n");
						exit(1);
					}
					prepare_iso_model(bodyTable[currentPositionInBodyPtrTab]);
					*((uint16*)bodyPtr3) = currentPositionInBodyPtrTab + 0x8000;
					index = currentPositionInBodyPtrTab;
					currentPositionInBodyPtrTab++;
				} else {
					flag &= 0x7FFF;
					index = flag;
				}

				bodyPtr3 += 2;
				bottomLeftX = -32000;

				bodyPtr4 = bodyPtr3;
				bodyPtr3++;

				if (!*bodyPtr4)
					return (index);

				bodyPtr4 = bodyPtr3;
				bodyPtr3++;

				if (*bodyPtr4 != 14)
					return (index);

				bodyPtr5 = (int16 *) bodyPtr3;

				bottomLeftX = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;
				bottomLeftY = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;
				bottomLeftZ = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;

				topRightX = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;
				topRightY = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;
				topRightZ = *((uint16*)bodyPtr3);
				bodyPtr3 += 2;

				return (index);
			}
		}

		bodyPtr = *bodyPtr2 + bodyPtr2;
	} while (1);
}

/** Initialize 3D actor
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
void init_model_actor(int32 bodyIdx, int16 actorIdx) {
	ActorStruct *localActor;
	int32  entityIdx;
	int32  entityIdxOld;
	int    currentIndex;
	uint16 *ptr;
	int16  var1, var2, var3, var4;

	int32 result, result1, result2;

	result = 0;

	localActor = &sceneActors[actorIdx];

	if (localActor->staticFlags.bIsSpriteActor)
		return;

	if (actorIdx == 0 && heroBehaviour == PROTOPACK && localActor->armor != 0 && localActor->armor != 1) { // if hero
		set_behaviour(NORMAL);
	}

	if (bodyIdx != -1) {
		entityIdx = init_body(bodyIdx, actorIdx);
	} else {
		entityIdx = -1;
	}

	if (entityIdx != -1) {
		if (localActor->entity == entityIdx)
			return;

		entityIdxOld = localActor->entity;
		localActor->entity = entityIdx;
		localActor->body = bodyIdx;
		currentIndex = localActor->entity;

		if (bottomLeftX == -32000) {
			ptr = (uint16 *) bodyTable[localActor->entity];
			ptr++;

			var1 = *((int16 *)ptr++);
			var2 = *((int16 *)ptr++);
			localActor->boudingBox.Y.bottomLeft = *((int16 *)ptr++);
			localActor->boudingBox.Y.topRight = *((int16 *)ptr++);
			var3 = *((int16 *)ptr++);
			var4 = *((int16 *)ptr++);

			if (localActor->staticFlags.bUseMiniZv) {
				result1 = var2 - var1; // take smaller for bound
				result2 = var4 - var3;

				if (result1 < result2)
					result = result1;
				else
					result = result2;

				result = abs(result);
				result >>= 1;
			} else {
				result1 = var2 - var1; // take average for bound
				result2 = var4 - var3;

				result = result2 + result1;
				result = abs(result);
				result >>= 2;
			}

			localActor->boudingBox.X.bottomLeft = -result;
			localActor->boudingBox.X.topRight = result;
			localActor->boudingBox.Z.bottomLeft = -result;
			localActor->boudingBox.Z.topRight = result;
		} else {
			localActor->boudingBox.X.bottomLeft = bottomLeftX;
			localActor->boudingBox.X.topRight = topRightX;
			localActor->boudingBox.Y.bottomLeft = bottomLeftY;
			localActor->boudingBox.Y.topRight = topRightY;
			localActor->boudingBox.Z.bottomLeft = bottomLeftZ;
			localActor->boudingBox.Z.topRight = topRightZ;
		}

		if (currentIndex == -1)
			return;

		if (localActor->previousAnimIdx == -1)
			return;

		copy_actor_intern_anim(bodyTable[currentIndex], bodyTable[localActor->entity]);

		return;
	}

	localActor->body = -1;
	localActor->entity = -1;

	localActor->boudingBox.X.bottomLeft = 0;
	localActor->boudingBox.X.topRight = 0;
	localActor->boudingBox.Y.bottomLeft = 0;
	localActor->boudingBox.Y.topRight = 0;
	localActor->boudingBox.Z.bottomLeft = 0;
	localActor->boudingBox.Z.topRight = 0;
}

/** Initialize actors
	@param actorIdx actor index to init */
void init_actor(int16 actorIdx) {
	ActorStruct *actor = &sceneActors[actorIdx];

	if (actor->staticFlags.bIsSpriteActor) { // if sprite actor
		if (actor->strengthOfHit != 0) {
			actor->dynamicFlags.bIsHitting = 1;
		}

		actor->entity = -1;

		init_sprite_actor(actorIdx);

		set_actor_angle_safe(0, 0, 0, &actor->move);

		if (actor->staticFlags.bUsesClipping) {
			actor->lastX = actor->X;
			actor->lastY = actor->Y;
			actor->lastZ = actor->Z;
		}

	} else {
		actor->entity = -1;

		init_model_actor(actor->body, actorIdx);

		actor->previousAnimIdx = -1;
		actor->animType = 0;

		if (actor->entity != -1) {
			init_anim(actor->anim, 0, 255, actorIdx);
		}

		set_actor_angle_safe(actor->angle, actor->angle, 0, &actor->move);
	}

	actor->positionInMoveScript = -1;
	actor->labelIdx = -1;
	actor->positionInLifeScript = 0;
}

/** Reset actor
	@param actorIdx actor index to init */
void reset_actor(int16 actorIdx) {
	ActorStruct *actor = &sceneActors[actorIdx];

	actor->body = 0;
	actor->anim = 0;
	actor->X = 0;
	actor->Y = -1;
	actor->Z = 0;

	actor->boudingBox.X.bottomLeft = 0;
	actor->boudingBox.X.topRight = 0;
	actor->boudingBox.Y.bottomLeft = 0;
	actor->boudingBox.Y.topRight = 0;
	actor->boudingBox.Z.bottomLeft = 0;
	actor->boudingBox.Z.topRight = 0;

	actor->angle = 0;
	actor->speed = 40;
	actor->controlMode = 0;

	actor->info0 = 0;
	actor->info1 = 0;
	actor->info2 = 0;
	actor->info3 = 0;

	actor->brickShape = 0;
	actor->collision = -1;
	actor->standOn = -1;
	actor->zone = -1;
	
	memset(&actor->staticFlags,0,2);
	memset(&actor->dynamicFlags,0,2);

	actor->life = 50;
	actor->armor = 1;
	actor->hitBy = -1;
	actor->lastRotationAngle = 0;
	actor->lastX = 0;
	actor->lastY = 0;
	actor->lastZ = 0;
	actor->entity = -1;
	actor->previousAnimIdx = -1;
	actor->animType = 0;
	actor->animPosition = 0;

	set_actor_angle_safe(0, 0, 0, &actor->move);

	actor->positionInMoveScript = -1;
	actor->positionInLifeScript = 0;
}
