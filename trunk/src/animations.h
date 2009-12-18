/** @file animations.h
	@brief
	This file contains 3D actors animations routines

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

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "sys.h"
#include "actor.h"

/** Total number of animations allowed in the game */
#define NUM_ANIMS 600

#define ANIM_STANDING  0
#define ANIM_FORWARD   1
#define ANIM_BACKWARD  2
#define ANIM_TURNLEFT  3
#define ANIM_TURNRIGHT 4

/** Table with all loaded animations */
uint8* animTable[NUM_ANIMS];
/** Table with all loaded animations sizes */
uint32 animSizeTable[NUM_ANIMS];

ActorStruct *processActorPtr; // actor pointer

/** Rotation by anim and not by engine */
int16 processRotationByAnim;    // processActorVar5
/** Last rotation angle */
int16 processLastRotationAngle; // processActorVar6
/** Current process actor index */
int16 currentlyProcessedActorIdx;

/** Current step X coornidate */
int16 currentStepX;
/** Current step Y coornidate */
int16 currentStepY;
/** Current step Z coornidate */
int16 currentStepZ;

uint8 *currentActorAnimExtraPtr;

uint8 *keyFramePtr;
uint8 *lastKeyFramePtr;

uint8 *animVar1;
int16 animVar4;

uint8 *animBuffer1;
uint8 *animBuffer2;


int32 set_anim_at_keyframe(int32 keyframeIdx, uint8 *anim, uint8 *body, AnimTimerDataStruct* animTimerDataPtr);
int32 get_num_keyframes(uint8 *ptr);
int32 get_start_keyframe(uint8 *ptr);
int32 set_model_animation(int32 animState, uint8 *animData, uint8 *body, AnimTimerDataStruct* animTimerDataPtr);
int32 get_body_anim_index(int32 anim, int16 actorNumber);
int32 verify_anim_at_keyframe(int32 animPos, uint8 *animData, uint8 *body, AnimTimerDataStruct* animTimerDataPtr);

/**  */
int32 init_anim(int8 newAnim, int16 animType, uint8 animExtra, int16 actorIdx);

/** Process main loop actor animations
	@param actorIdx Actor index */
void process_actor_animations(int32 actorIdx);

#endif
