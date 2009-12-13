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
*/

#ifndef ACTOR_H
#define ACTOR_H

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
unsigned char* spriteTable[NUM_SPRITES];
/** Table with all loaded sprite sizes */
unsigned int   spriteSizeTable[NUM_SPRITES];

/** Actors time structure */
typedef struct TimeStruct
{
	short int from;
	short int to;
	short int numOfStep;
	int timeOfChange;
} TimeStruct;

/** Actors zone volumique points structure */
typedef struct ZVPoint
{
	short int bottomLeft;
	short int topRight;
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
	unsigned char* ptr;
	int time;
} AnimTimerDataStruct;

/** Actors static flags structure */
typedef struct StaticFlagsStruct
{
	unsigned short int bComputeCollisionWithObj				: 1; // 0x0001
    unsigned short int bComputeCollisionWithBricks			: 1; // 0x0002
    unsigned short int bIsZonable							: 1; // 0x0004
    unsigned short int bUsesClipping						: 1; // 0x0008
    unsigned short int bCanBePushed							: 1; // 0x0010
    unsigned short int bComputeLowCollision					: 1; // 0x0020
    unsigned short int bCanDrown							: 1; // 0x0040
    unsigned short int bUnk80								: 1; // 0x0080
    unsigned short int bUnk0100								: 1; // 0x0100
    unsigned short int bIsHidden							: 1; // 0x0200
    unsigned short int bIsSpriteActor						: 1; // 0x0400
    unsigned short int bCanFall								: 1; // 0x0800
    unsigned short int bDoesntCastShadow					: 1; // 0x1000
    unsigned short int bIsBackgrounded						: 1; // 0x2000
    unsigned short int bIsCarrierActor						: 1; // 0x4000
    unsigned short int bUseMiniZv							: 1; // 0x8000
} StaticFlagsStruct;

//TODO: update field names
/** Actors dynamic flags structure */
typedef struct DynamicFlagsStruct
{
    unsigned short int bUnk0001								: 1; // 0x0001
    unsigned short int bUnk0002								: 1; // 0x0002
    unsigned short int bUnk0004								: 1; // 0x0004
    unsigned short int bUnk0008								: 1; // 0x0008
    unsigned short int bIsVisible							: 1; // 0x0010 // bIsVisible
    unsigned short int bUnk0020								: 1; // 0x0020
    unsigned short int bIsMoving							: 1; // 0x0040
    unsigned short int bUnk0080								: 1; // 0x0080
    unsigned short int bUnk0100								: 1; // 0x0100
    unsigned short int bUnk0200								: 1; // 0x0200
    unsigned short int bUnk0400								: 1; // 0x0400
    unsigned short int bUnk0800								: 1; // 0x0800
    unsigned short int bUnk1000								: 1; // 0x1000
    unsigned short int bUnk2000								: 1; // 0x2000
    unsigned short int bUnk4000								: 1; // 0x4000
    unsigned short int bUnk8000								: 1; // 0x8000
} DynamicFlagsStruct;

/** Actors structure */
typedef struct ActorStruct
{
	StaticFlagsStruct staticFlags;
	DynamicFlagsStruct dynamicFlags;

	int entity; // costumeIndex
	int body;
	int anim;
	int animExtra; //field_2
	int field_3;
	unsigned char *animExtraData;
	int sprite; // field_8
   	unsigned char *entityDataPtr;

	int X; // field_20
	int Y; // field_22
	int Z; // field_24
	int strengthOfHit; // field_66
	int hitBy;
	int bonusParameter; // field_10
	int angle;
	int speed;
	int comportement;
	int info0; // cropLeft
	int info1; // cropTop
	int info2; // cropRight
	int info3; // cropBottom
	int followedActor; // same as info3
	int bonusAmount; // field_12
	int talkColor;
	int armor; // field_14
	int life;

	int positionInMoveScript;
	unsigned char *moveScript;

	int positionInActorScript;
	unsigned char *lifeScript;

	int labelIdx;			// script label index
	int currentLabelPtr;	// pointer to LABEL offset
	int pausedTrackPtr;

	//int costumeIndex;
	int collision;
	int standPosition;
	int standOn;
	int zone;
	
	int lastRotationSpeed;
	int lastX;
	int lastZ;
	int lastY;
	int previousAnimIdx;
	int doorStatus;
	int animPosition;
	int field_78;

	ZVBox boudingBox;
	TimeStruct time;
	AnimTimerDataStruct animTimerData;
} ActorStruct;

/** Actor shadow X coordinate */
int shadowX;
/** Actor shadow Y coordinate */
int shadowY;
/** Actor shadow Z coordinate */
int shadowZ;
/** Actor shadow collition type */
char shadowCollisionType; // shadowVar

/** Process actor X coordinate */
short int processActorX;
/** Process actor Y coordinate */
short int processActorY;
/** Process actor Z coordinate */
short int processActorZ;

/** Actor collition X coordinate */
int collisionX; // getPosVar1
/** Actor collition Y coordinate */
int collisionY; // getPosVar2
/** Actor collition Z coordinate */
int collisionZ; // getPosVar3

short int currentX;
short int currentY;
short int currentZ;

unsigned char *currentActorAnimExtraData;


/** Hero behaviour */
short int heroBehaviour;

/** Hero 3D entity for normal behaviour */
unsigned char *heroEntityNORMAL;	 // file3D0
/** Hero 3D entity for athletic behaviour */
unsigned char *heroEntityATHLETIC;	 // file3D1
/** Hero 3D entity for aggressive behaviour */
unsigned char *heroEntityAGGRESSIVE; // file3D2
/** Hero 3D entity for discrete behaviour */
unsigned char *heroEntityDISCRETE;	 // file3D3
/** Hero 3D entity for protopack behaviour */
unsigned char *heroEntityPROTOPACK;  // file3D4

/** Actors 3D body table - size of NUM_BODIES */
extern unsigned char *bodyTable[NUM_BODIES];

/** Current position in body table */
int currentPositionInBodyPtrTab;

/** Actor bounding box bottom left X coordinate */
short int bottomLeftX; // loadCostumeVar
/** Actor bounding box bottom left Y coordinate */
short int bottomLeftY; // loadCostumeVar2
/** Actor bounding box bottom left Z coordinate */
short int bottomLeftZ; // loadCostumeVar3
/** Actor bounding box top left X coordinate */
short int topRightX;   // loadCostumeVar4
/** Actor bounding box top left Y coordinate */
short int topRightY;   // loadCostumeVar5
/** Actor bounding box top left Z coordinate */
short int topRightZ;   // loadCostumeVar6

/** Restart hero variables while opening new scenes */
void restart_hero_scene();

/** Load hero 3D body and animations */
void load_hero_entities();

/** Set hero behaviour
	@param behaviour behaviour value to set */
void set_behaviour(int behaviour);

/** Preload all sprites */
void preload_sprites();

/** Initialize 3D actor
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
void init_model_actor(int bodyIdx, short int actorIdx);

/** Initialize actors
	@param actorIdx actor index to init */
void init_actor(short int actorIdx);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle_safe(short int startAngle, short int endAngle, short int stepAngle, TimeStruct * timePtr);

/** Clear actors safe angle
	@param actorPtr actor pointer */
void clear_real_angle(ActorStruct * actorPtr);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle(short int startAngle, short int endAngle, short int stepAngle, TimeStruct * timePtr);

/** Get actor real angle
	@param angleData time pointer to process */
int get_real_angle(TimeStruct * angleData);

/** Get actor angle
	@param angleData time pointer to process */
int get_real_value(TimeStruct * angleData);

#endif
