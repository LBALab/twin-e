/** @file actor.h
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

#ifndef ACTOR_H
#define ACTOR_H

#include "sys.h"

/** Total number of sprites allowed in the game */
#define NUM_SPRITES 200

/** Total number of bodies allowed in the game */
#define NUM_BODIES 200

/** Normal hehaviour type*/
#define NORMAL			0
/** Athletic hehaviour type*/
#define ATHLETIC		1
/** Aggressive hehaviour type*/
#define AGGRESSIVE		2
/** Discrete hehaviour type*/
#define DISCRETE		3
/** Protopack hehaviour type*/
#define PROTOPACK		4

/** Table with all loaded sprites */
uint8* spriteTable[NUM_SPRITES];
/** Table with all loaded sprite sizes */
uint32   spriteSizeTable[NUM_SPRITES];

/** Actors time structure */
typedef struct TimeStruct
{
	int16 from;
	int16 to;
	int16 numOfStep;
	int32 timeOfChange;
} TimeStruct;

/** Actors zone volumique points structure */
typedef struct ZVPoint
{
	int16 bottomLeft;
	int16 topRight;
} ZVPoint;

/** Actors zone volumique box structure */
typedef struct ZVBox
{
	ZVPoint X;
	ZVPoint Y;
	ZVPoint Z;
} ZVBox;

/** Actors animation timer structure */
typedef struct AnimTimerDataStruct
{
	uint8* ptr;
	int32 time;
} AnimTimerDataStruct;

/** Actors static flags structure */
typedef struct StaticFlagsStruct
{
	uint16 bComputeCollisionWithObj				: 1; // 0x0001
    uint16 bComputeCollisionWithBricks			: 1; // 0x0002
    uint16 bIsZonable							: 1; // 0x0004
    uint16 bUsesClipping						: 1; // 0x0008
    uint16 bCanBePushed							: 1; // 0x0010
    uint16 bComputeLowCollision					: 1; // 0x0020
    uint16 bCanDrown							: 1; // 0x0040
    uint16 bUnk80								: 1; // 0x0080
    uint16 bUnk0100								: 1; // 0x0100
    uint16 bIsHidden							: 1; // 0x0200
    uint16 bIsSpriteActor						: 1; // 0x0400
    uint16 bCanFall								: 1; // 0x0800
    uint16 bDoesntCastShadow					: 1; // 0x1000
    uint16 bIsBackgrounded						: 1; // 0x2000
    uint16 bIsCarrierActor						: 1; // 0x4000
    uint16 bUseMiniZv							: 1; // 0x8000
} StaticFlagsStruct;

//TODO: update field names
/** Actors dynamic flags structure */
typedef struct DynamicFlagsStruct
{
    uint16 bUnk0001								: 1; // 0x0001
    uint16 bUnk0002								: 1; // 0x0002
    uint16 bUnk0004								: 1; // 0x0004
    uint16 bUnk0008								: 1; // 0x0008
    uint16 bIsVisible							: 1; // 0x0010 // bIsVisible
    uint16 bUnk0020								: 1; // 0x0020
    uint16 bIsMoving							: 1; // 0x0040
    uint16 bUnk0080								: 1; // 0x0080
    uint16 bUnk0100								: 1; // 0x0100
    uint16 bUnk0200								: 1; // 0x0200
    uint16 bUnk0400								: 1; // 0x0400
    uint16 bUnk0800								: 1; // 0x0800
    uint16 bUnk1000								: 1; // 0x1000
    uint16 bUnk2000								: 1; // 0x2000
    uint16 bUnk4000								: 1; // 0x4000
    uint16 bUnk8000								: 1; // 0x8000
} DynamicFlagsStruct;

/** Actors structure */
typedef struct ActorStruct
{
	StaticFlagsStruct staticFlags;
	DynamicFlagsStruct dynamicFlags;

	int32 entity; // costumeIndex
	int32 body;
	int32 anim;
	int32 animExtra; //field_2
	int32 field_3;
	uint8 *animExtraData;
	int32 sprite; // field_8
   	uint8 *entityDataPtr;

	int32 X; // field_20
	int32 Y; // field_22
	int32 Z; // field_24
	int32 strengthOfHit; // field_66
	int32 hitBy;
	int32 bonusParameter; // field_10
	int32 angle;
	int32 speed;
	int32 comportement;
	int32 info0; // cropLeft
	int32 info1; // cropTop
	int32 info2; // cropRight
	int32 info3; // cropBottom
	int32 followedActor; // same as info3
	int32 bonusAmount; // field_12
	int32 talkColor;
	int32 armor; // field_14
	int32 life;

	int32 positionInMoveScript;
	uint8 *moveScript;

	int32 positionInActorScript;
	uint8 *lifeScript;

	int32 labelIdx;			// script label index
	int32 currentLabelPtr;	// pointer to LABEL offset
	int32 pausedTrackPtr;

	//int costumeIndex;
	int32 collision;
	int32 standPosition;
	int32 standOn;
	int32 zone;
	
	int32 lastRotationSpeed;
	int32 lastX;
	int32 lastZ;
	int32 lastY;
	int32 previousAnimIdx;
	int32 doorStatus;
	int32 animPosition;
	int32 field_78;

	ZVBox boudingBox;
	TimeStruct time;
	AnimTimerDataStruct animTimerData;
} ActorStruct;

/** Actor shadow X coordinate */
int32 shadowX;
/** Actor shadow Y coordinate */
int32 shadowY;
/** Actor shadow Z coordinate */
int32 shadowZ;
/** Actor shadow collition type */
int8 shadowCollisionType; // shadowVar

/** Process actor X coordinate */
int16 processActorX;
/** Process actor Y coordinate */
int16 processActorY;
/** Process actor Z coordinate */
int16 processActorZ;

/** Actor collition X coordinate */
int32 collisionX; // getPosVar1
/** Actor collition Y coordinate */
int32 collisionY; // getPosVar2
/** Actor collition Z coordinate */
int32 collisionZ; // getPosVar3

int16 currentX;
int16 currentY;
int16 currentZ;

uint8 *currentActorAnimExtraData;


/** Hero behaviour */
int16 heroBehaviour;

/** Hero 3D entity for normal behaviour */
uint8 *heroEntityNORMAL;	 // file3D0
/** Hero 3D entity for athletic behaviour */
uint8 *heroEntityATHLETIC;	 // file3D1
/** Hero 3D entity for aggressive behaviour */
uint8 *heroEntityAGGRESSIVE; // file3D2
/** Hero 3D entity for discrete behaviour */
uint8 *heroEntityDISCRETE;	 // file3D3
/** Hero 3D entity for protopack behaviour */
uint8 *heroEntityPROTOPACK;  // file3D4

/** Actors 3D body table - size of NUM_BODIES */
extern uint8 *bodyTable[NUM_BODIES];

/** Current position in body table */
int32 currentPositionInBodyPtrTab;

/** Actor bounding box bottom left X coordinate */
int16 bottomLeftX; // loadCostumeVar
/** Actor bounding box bottom left Y coordinate */
int16 bottomLeftY; // loadCostumeVar2
/** Actor bounding box bottom left Z coordinate */
int16 bottomLeftZ; // loadCostumeVar3
/** Actor bounding box top left X coordinate */
int16 topRightX;   // loadCostumeVar4
/** Actor bounding box top left Y coordinate */
int16 topRightY;   // loadCostumeVar5
/** Actor bounding box top left Z coordinate */
int16 topRightZ;   // loadCostumeVar6

/** Restart hero variables while opening new scenes */
void restart_hero_scene();

/** Load hero 3D body and animations */
void load_hero_entities();

/** Set hero behaviour
	@param behaviour behaviour value to set */
void set_behaviour(int32 behaviour);

/** Preload all sprites */
void preload_sprites();

/** Initialize 3D actor
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
void init_model_actor(int32 bodyIdx, int16 actorIdx);

/** Initialize actors
	@param actorIdx actor index to init */
void init_actor(int16 actorIdx);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle_safe(int16 startAngle, int16 endAngle, int16 stepAngle, TimeStruct * timePtr);

/** Clear actors safe angle
	@param actorPtr actor pointer */
void clear_real_angle(ActorStruct * actorPtr);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle(int16 startAngle, int16 endAngle, int16 stepAngle, TimeStruct * timePtr);

/** Get actor real angle
	@param angleData time pointer to process */
int32 get_real_angle(TimeStruct * angleData);

/** Get actor angle
	@param angleData time pointer to process */
int32 get_real_value(TimeStruct * angleData);

#endif
