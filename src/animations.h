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

/** Standing anim value */
#define ANIM_STANDING		0
/** Forward anim value */
#define ANIM_FORWARD		1
/** Backward anim value */
#define ANIM_BACKWARD		2
/** Turn left anim value */
#define ANIM_TURNLEFT		3
/** Turn right anim value */
#define ANIM_TURNRIGHT		4
/** Hit anim value */
#define ANIM_HIT			5
/** Big hit anim value */
#define ANIM_BIG_HIT		6
/** Fall anim value */
#define ANIM_FALL			7
/** Landing anim value */
#define ANIM_LANDING		8
/** Landing hit anim value */
#define ANIM_LANDING_HIT	9
/** Dead anim value */
#define ANIM_LANDDEATH		10
/** Hit anim value */
#define ANIM_HIT2			22

/** Table with all loaded animations */
uint8* animTable[NUM_ANIMS];
/** Table with all loaded animations sizes */
uint32 animSizeTable[NUM_ANIMS];

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
/** Current actor anim extra pointer */
uint8 *currentActorAnimExtraPtr;

/** Pointer to current animation keyframe */
uint8 *keyFramePtr;
/** Pointer to last animation keyframe */
uint8 *lastKeyFramePtr;

uint8 *animBuffer1;
uint8 *animBuffer2;

/** Set animation keyframe
	@param keyframIdx Animation keyframe index
	@param animPtr Pointer to animation
	@param bodyPtr Body model poitner
	@param animTimerDataPtr Animation time data */
int32 set_anim_at_keyframe(int32 keyframeIdx, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr);

/** Get total number of keyframes in animation
	@param animPtr Pointer to animation */
int32 get_num_keyframes(uint8 *animPtr);

/** Get first keyframes in animation
	@param animPtr Pointer to animation */
int32 get_start_keyframe(uint8 *animPtr);

/** Set new body animation
	@param animIdx Animation index
	@param animPtr Animation pointer
	@param bodyPtr Body model poitner 
	@param animTimerDataPtr Animation time data */
int32 set_model_animation(int32 animIdx, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr);

/** Get entity anim index (This is taken from File3D entities)
	@param animIdx Entity animation index
	@param actorIdx Actor index */
int32 get_body_anim_index(int32 animIdx, int16 actorIdx);

/** Verify animation at keyframe
	@param animIdx Animation index
	@param animPtr Animation pointer
	@param bodyPtr Body model poitner 
	@param animTimerDataPtr Animation time data */
int32 verify_anim_at_keyframe(int32 animPos, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr);

/** Initialize animation
	@param newAnim animation to init
	@param animType animation type
	@param animExtra animation actions extra data
	@param actorIdx actor index */
int32 init_anim(int8 newAnim, int16 animType, uint8 animExtra, int16 actorIdx);

/** Process acotr animation actions
	@param actorIdx Actor index */
void process_anim_actions(int16 actorIdx);

/** Process main loop actor animations
	@param actorIdx Actor index */
void process_actor_animations(int32 actorIdx);

#endif
