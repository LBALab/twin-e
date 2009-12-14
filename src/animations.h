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

#include "actor.h"

/** Total number of animations allowed in the game */
#define NUM_ANIMS 600

#define ANIM_STANDING 0

/** Table with all loaded animations */
unsigned char* animTable[NUM_ANIMS];
/** Table with all loaded animations sizes */
unsigned int   animSizeTable[NUM_ANIMS];

ActorStruct *processActorVar1; // actor pointer
short int processActorVar2; // X
short int processActorVar3; // Y
short int processActorVar4; // Z
short int processActorVar5;
short int processActorVar6;
short int processActorSub2Var0;
short int processActorSub2Var1;
short int currentlyProcessedActorNum;

unsigned char *keyFramePtr;
unsigned char *lastKeyFramePtr;

unsigned char *animVar1;
short int animVar4;

unsigned char *animBuffer1;
unsigned char *animBuffer2;


void preload_animations();
int set_anim_at_keyframe(int keyframeIdx, unsigned char *anim, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr);
int get_num_keyframes(unsigned char *ptr);
int get_start_keyframe(unsigned char *ptr);
int set_model_animation(int animState, unsigned char *animData, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr);
int get_body_anim_index(int anim, short int actorNumber);
int init_anim(char newAnim, short int arg_4, unsigned char arg_8, short int actorNum);
int verify_anim_at_keyframe(int animPos, unsigned char *animData, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr);
void process_actor_animations(int actorIdx);

#endif
