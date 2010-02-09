/** @file animations.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animations.h"
#include "lbaengine.h"
#include "resources.h"
#include "scene.h"
#include "actor.h"
#include "renderer.h"
#include "movements.h"
#include "sound.h"
#include "gamestate.h"
#include "collision.h"
#include "grid.h"
#include "main.h"

enum ActionType {
	kHitting			= 0,
	kSample				= 1,
    kSampleFreq			= 2,
    kThrowExtraBonus	= 3,
	kThrowMagicBall		= 4,
	kSampleRepeat		= 5,
	kActionUnknown6		= 6,
	kActionUnknown7		= 7,
	kSampleStop			= 8,
	kActionUnknown9		= 9, // unused
	kSampleBrick1		= 10,
	kSampleBrick2		= 11,
	kHeroHitting		= 12,
	kActionUnknown13	= 13,
	kActionUnknown14	= 14,
	kActionUnknown15	= 15
};

/** Set animation keyframe
	@param keyframIdx Animation keyframe index
	@param animPtr Pointer to animation
	@param bodyPtr Body model poitner
	@param animTimerDataPtr Animation time data */
int set_anim_at_keyframe(int32 keyframeIdx, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr) {
	int16 numOfKeyframeInAnim;
	int16 numOfBonesInAnim;
	uint8 *ptrToData;
	uint8 *ptrToDataBackup;
	uint8 *ptrToBodyData;
	int16 bodyHeader;
	int16 numOfElementInBody;
	int16 numOfPointInBody;
	int32 i;

	numOfKeyframeInAnim = *(int16 *)(animPtr);

	if (keyframeIdx >= numOfKeyframeInAnim)
		return numOfKeyframeInAnim;

	numOfBonesInAnim = *(int16 *)(animPtr + 2);

	ptrToData = (uint8 *)((numOfBonesInAnim * 8 + 8) * keyframeIdx + animPtr + 8);

	bodyHeader = *(int16 *)(bodyPtr);

	if (!(bodyHeader & 2))
		return 0;

	ptrToBodyData = bodyPtr + 14;

	animTimerDataPtr->ptr = ptrToData;
	animTimerDataPtr->time = lbaTime;

	ptrToBodyData = ptrToBodyData + *(int16 *)(ptrToBodyData) + 2;

	numOfElementInBody = *(int16 *)(ptrToBodyData);

	ptrToBodyData = ptrToBodyData + numOfElementInBody * 6 + 12;

	numOfPointInBody = *(int16 *)(ptrToBodyData - 10); // num elements

	if (numOfBonesInAnim > numOfPointInBody) {
		numOfBonesInAnim = numOfPointInBody;
	}

	ptrToDataBackup = ptrToData;

	ptrToData += 8;

	do {
		for (i = 0; i < 8; i++) {
			*(ptrToBodyData++) = *(ptrToData++);
		}

		ptrToBodyData += 30;

	} while (--numOfBonesInAnim);

	ptrToData = ptrToDataBackup + 2;

	currentStepX = *(int16 *)(ptrToData);
	currentStepY = *(int16 *)(ptrToData + 2);
	currentStepZ = *(int16 *)(ptrToData + 4);

	processRotationByAnim    = *(int16 *)(ptrToData + 6);
	processLastRotationAngle = *(int16 *)(ptrToData + 10);

	return 1;
}

/** Get total number of keyframes in animation
	@param animPtr Pointer to animation */
int32 get_num_keyframes(uint8 *animPtr) {
	return (*(int16 *)(animPtr));
}

/** Get first keyframes in animation
	@param animPtr Pointer to animation */
int32 get_start_keyframe(uint8 *animPtr) {
	return (*(int16 *)(animPtr + 4));
}

/** Apply animation step rotation */
void apply_anim_steprotation(uint8 **ptr, int32 bp, int32 bx) {
	int16 *dest;
	int16 lastAngle;
	int16 newAngle;
	int16 angleDif;
	int16 computedAngle;

	lastAngle = *(int16 *)(lastKeyFramePtr);
	lastKeyFramePtr += 2;

	newAngle = *(int16 *)(keyFramePtr);
	keyFramePtr += 2;

	lastAngle &= 0x3FF;
	newAngle &= 0x3FF;

	angleDif = newAngle - lastAngle;

	if (angleDif) {
		if (angleDif < -0x200) {
			angleDif += 0x400;
		} else if (angleDif > 0x200) {
			angleDif -= 0x400;
		}

		computedAngle = lastAngle + (angleDif * bp) / bx;
	} else {
		computedAngle = lastAngle;
	}

	dest = (int16 *) * (ptr);
	*dest = computedAngle & 0x3FF;
	*(ptr) = *(ptr) + 2;
}

/** Apply animation step */
void apply_anim_step(uint8 **ptr, int32 bp, int32 bx) {
	int16 *dest;
	int16 lastAngle;
	int16 newAngle;
	int16 angleDif;
	int16 computedAngle;

	lastAngle = *(int16 *) lastKeyFramePtr;
	lastKeyFramePtr += 2;

	newAngle = *(int16 *) keyFramePtr;
	keyFramePtr += 2;

	angleDif = newAngle - lastAngle;

	if (angleDif) {
		computedAngle = lastAngle + (angleDif * bp) / bx;
	} else {
		computedAngle = lastAngle;
	}

	dest = (int16 *) * (ptr);
	*dest = computedAngle;
	*(ptr) = *(ptr) + 2;
}

/** Get animation mode */
int32 get_anim_mode(uint8 **ptr) {
	int16 *lptr;
	int16 opcode;

	lptr = (int16 *) * ptr;

	opcode = *(int16 *)(keyFramePtr);
	*(int16 *)(lptr) = opcode;

	keyFramePtr += 2;
	*(ptr) = *(ptr) + 2;
	lastKeyFramePtr += 2;

	return opcode;
}

/** Set new body animation
	@param animIdx Animation index
	@param animPtr Animation pointer
	@param bodyPtr Body model poitner 
	@param animTimerDataPtr Animation time data */
int32 set_model_animation(int32 animState, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr) {
	int16 animOpcode;

	int16 bodyHeader;

	uint8 *edi;
	uint8 *ebx;
	int32 ebp;
	int32 eax;
	int32 keyFrameLength;
	int32 numOfPointInBody;
	int32 numOfPointInAnim;
	uint8 *keyFramePtrOld;

	numOfPointInAnim = *(int16 *)(animPtr + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animState) + animPtr + 8;

	keyFrameLength = *(int16 *)(keyFramePtr);

	bodyHeader = *(int16 *)(bodyPtr);

	if (!(bodyHeader & 2)) {
		return 0;
	}

	edi = bodyPtr + 16;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx) {
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}

	lastKeyFramePtr = ebx;

	eax = *(int16 *)(edi - 2);
	edi += eax;

	eax = *(int16 *)(edi);
	eax = eax + eax * 2;
	edi = edi + eax * 2 + 12;

	numOfPointInBody = *(int16 *)(edi - 10);

	if (numOfPointInAnim > numOfPointInBody) {
		numOfPointInAnim = numOfPointInBody;
	}

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength) {
		int32 *destPtr; // keyFrame
		int32 *sourcePtr;

		sourcePtr = (int32 *)(keyFramePtr + 8);
		destPtr = (int32 *) edi;

		do {
			*(destPtr++) = *(sourcePtr++);
			*(destPtr++) = *(sourcePtr++);
			destPtr = (int32 *)(((int8 *)destPtr) + 30);
		} while (--numOfPointInAnim);

		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentStepX = *(int16 *)(keyFramePtr + 2);
		currentStepY = *(int16 *)(keyFramePtr + 4);
		currentStepZ = *(int16 *)(keyFramePtr + 6);

		processRotationByAnim    = *(int16 *)(keyFramePtr + 8);
		processLastRotationAngle = *(int16 *)(keyFramePtr + 12);

		return 1;
	} else {
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processRotationByAnim    = *(int16 *)(keyFramePtr);
		processLastRotationAngle = (*(int16 *)(keyFramePtr + 4) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		edi += 38;

		if (--numOfPointInAnim) {
			int16 tmpNumOfPoints = numOfPointInAnim;

			do {
				animOpcode = get_anim_mode(&edi);

				switch (animOpcode) {
				case 0: {	// allow global rotate
					apply_anim_steprotation(&edi, eax, keyFrameLength);
					apply_anim_steprotation(&edi, eax, keyFrameLength);
					apply_anim_steprotation(&edi, eax, keyFrameLength);
					break;
				}
				case 1: {	// dissallow global rotate
					apply_anim_step(&edi, eax, keyFrameLength);
					apply_anim_step(&edi, eax, keyFrameLength);
					apply_anim_step(&edi, eax, keyFrameLength);
					break;
				}
				case 2: {	// dissallow global rotate + hide
					apply_anim_step(&edi, eax, keyFrameLength);
					apply_anim_step(&edi, eax, keyFrameLength);
					apply_anim_step(&edi, eax, keyFrameLength);
					break;
				}
				default: {
					printf("Unsupported animation rotation mode %d!\n", animOpcode);
					exit(1);
				}
				}

				edi += 30;
			} while (--tmpNumOfPoints);
		}

		currentStepX = (*(int16 *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentStepY = (*(int16 *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentStepZ = (*(int16 *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return 0;
}

/** Get entity anim index (This is taken from File3D entities)
	@param anim Entity animation index
	@param actorIdx Actor index */
int32 get_body_anim_index(int32 animIdx, int16 actorIdx) {
	int8 type;
	uint16 realAnimIdx;
	uint8 *bodyPtr;
	uint8 *ptr, *ptr2;
	uint8 *costumePtr = NULL;
	ActorStruct *actor;

	actor = &sceneActors[actorIdx];
	bodyPtr = actor->entityDataPtr;

	do {
		type = *(bodyPtr++);

		if (type == -1) {
			currentActorAnimExtraPtr = NULL;
			return -1;
		}

		ptr = (bodyPtr + 1);

		if (type == 3) {
			if (animIdx == *bodyPtr) {
				ptr++;
				realAnimIdx = *(int16 *)(ptr);
				ptr += 2;
				ptr2 = ptr;
				ptr++;
				if (*ptr2 != 0) {
					costumePtr = ptr - 1;
				}
				currentActorAnimExtraPtr = costumePtr;
				return realAnimIdx;
			}
		}

		bodyPtr = *ptr + ptr;

	} while (1);

	return 0;
}

/** Stock animation - copy the next keyFrame from a different buffer
	@param animPtr Animation pointer
	@param bodyPtr Body model poitner 
	@param animTimerDataPtr Animation time data */
int32 stock_animation(uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr) {
	int32 playAnim;
	uint8 *ptr;
	int32 *edi;
	int32 *esi;
	int32 var0;
	int32 var1;
	int32 var2;
	int32 counter;

	playAnim = *(int16 *)(bodyPtr);

	if (playAnim & 2) {
		ptr = (bodyPtr + 0x10);

		animTimerDataPtr->time = lbaTime;
		animTimerDataPtr->ptr = animPtr;

		var0 = *(int16 *)(ptr - 2);
		ptr = ptr + var0;

		var1 = *(int16 *)(ptr);
		var1 = var1 + var1 * 2;

		ptr = ptr + var1 * 2 + 2;

		var2 = *(int16 *)(ptr);
		counter = var2;
		var2 = (var2 * 8) + 8;

		edi = (int32 *)(animPtr + 8);
		esi = (int32 *)(ptr + 10);

		do {
			*(edi++) = *(esi++);
			*(edi++) = *(esi++);

			esi = (int32 *)(((int8 *) esi) + 30);
		} while (counter--);

		return var2;
	}
	return 0;
}

/** Verify animation at keyframe
	@param animIdx Animation index
	@param animPtr Animation pointer
	@param bodyPtr Body model poitner 
	@param animTimerDataPtr Animation time data */
int32 verify_anim_at_keyframe(int32 animIdx, uint8 *animPtr, uint8 *bodyPtr, AnimTimerDataStruct* animTimerDataPtr) {
	int16 bodyHeader;

	uint8 *edi;
	uint8 *ebx;
	int32 ebp;
	int32 eax;
	int32 keyFrameLength;
	int32 numOfPointInAnim = -1;
	uint8 *keyFramePtrOld;

	numOfPointInAnim = *(int16 *)(animPtr + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animIdx) + animPtr + 8;

	keyFrameLength = *(int16 *)(keyFramePtr);

	bodyHeader = *(int16 *)(bodyPtr);

	if (!(bodyHeader & 2)) {
		return 0;
	}

	edi = bodyPtr + 16;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx) {
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}

	lastKeyFramePtr = ebx;

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength) {
		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentStepX = *(int16 *)(keyFramePtr + 2);
		currentStepY = *(int16 *)(keyFramePtr + 4);
		currentStepZ = *(int16 *)(keyFramePtr + 6);

		processRotationByAnim    = *(int16 *)(keyFramePtr + 8);
		processLastRotationAngle = *(int16 *)(keyFramePtr + 12);

		return 1;
	} else {
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processRotationByAnim    = *(int16 *)(keyFramePtr);
		processLastRotationAngle = (*(int16 *)(keyFramePtr + 4) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		currentStepX = (*(int16 *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentStepY = (*(int16 *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentStepZ = (*(int16 *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return 0;
}

/** Process acotr animation actions
	@param actorIdx Actor index */
void process_anim_actions(int16 actorIdx) {
	int32 startAnimEntityIdx, endAnimEntityIdx, actionType, animPos;
	uint8 *data;
	ActorStruct *actor;

	actor = &sceneActors[actorIdx];
	data = actor->animExtraPtr;

	startAnimEntityIdx = 0;
	endAnimEntityIdx = *(data++);

	while (startAnimEntityIdx < endAnimEntityIdx) {
		actionType = *(data++);

		actionType -= 5;
		if (actionType > 16) {
			return;
		}

		switch (actionType) {
		case kHitting: {
			animPos = *(data++);
			animPos--;
			if (animPos == actor->animPosition) {
				actor->strengthOfHit = *(data);
				actor->dynamicFlags.bIsHitting = 1;
			}
			data++;
		}
			break;
		case kSample: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int16 sampleIdx = *((int16 *)data);
				play_sample(sampleIdx, 0x1000, 1, actor->X, actor->Y, actor->Z);
			}
			data+=2;
		}
			break;
		case kSampleFreq: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int16 sampleIdx, frequency;
				sampleIdx = *((int16 *)data); data+=2;
				frequency = *((int16 *)data); data+=2;
				frequency = Rnd(frequency) + 0x1000 - (Abs(frequency) >> 1);
				play_sample(sampleIdx, frequency, 1, actor->X, actor->Y, actor->Z);
			} else {
				data+=4;
			}
		}
			break;
		case kThrowExtraBonus: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 yHeight, var_C, var_24, var_14, cx, dx, var;

				yHeight = *((int16 *)data);
				data+=2;
				var_C = *(data++);
				cx = *((int16 *)data);
				data+=2;
				dx = actor->angle + *((int16 *)data);
				data+=2;
				var_24 = *((int16 *)data);
				data+=2;
				var_14 = *(data++);
				var = *(data++);

				add_extra_throw(actorIdx, actor->X, actor->Y + yHeight, actor->Z, var_C, cx, dx, var_24, var_14, var);
			} else {
				data += 11;
			}
		}
			break;
		case kThrowMagicBall: {
			if (magicBallIdx == -1) {
				animPos = *(data++);
				if (animPos == actor->animPosition) {
					int32 var_8, dx, var_24, var_14;

					var_8 = *((int16 *)data);
					data += 2;
					dx = *((int16 *)data);
					data += 2;
					var_24 = *((int16 *)data);
					data += 2;
					var_14 = *(data++);

					add_extra_throw_magicball(actor->X, actor->Y + var_8, actor->Z, dx, actor->angle, var_24, var_14);
				} else {
					data += 7;
				}
			} else {
				data += 8;
			}
		}
			break;
		case kSampleRepeat: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int16 sampleIdx, repeat;
				sampleIdx = *((int16 *)data); data+=2;
				repeat = *((int16 *)data); data+=2;
				play_sample(sampleIdx, 0x1000, repeat, actor->X, actor->Y, actor->Z);
			} else {
				data+=4;
			}
		}
			break;
		case kActionUnknown6: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 var_8, var_C, dx, var_24, temp;

				var_8 = *((int16 *)data);
				data += 2;
				var_C = *(data++);
				dx = *(data++);
				var_24 = *((int16 *)data);
				data += 2;
				temp = *(data++);

				add_extra_aiming(actorIdx, actor->X, actor->Y + var_8, actor->Z, var_C, dx, var_24, temp);
			} else {
				data+=6;
			}
		}
			break;
		case kActionUnknown7: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 yHeight, var_C, var_24, var_14, cx, dx, distance, angle, var;

				distance = get_distance_2D(actor->X, actor->Z, sceneHero->X, sceneHero->Z);
				angle = get_angle(actor->Y, 0, sceneHero->Y, distance);

				yHeight = *((int16 *)data);
				data+=2;
				var_C = *(data++);
				dx = *((int16 *)data);
				data+=2;
				cx = actor->angle + *((int16 *)data);
				data+=2;
				var_24 = *((int16 *)data);
				data+=2;
				var_14 = *(data++);
				var = *(data++);

				add_extra_throw(actorIdx, actor->X, actor->Y + yHeight, actor->Z, var_C, dx, cx, var_24, var_14, var);
			} else {
				data += 11;
			}
		}
			break;
		case kSampleStop: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				stop_samples();
			}
			data += 2;
		}
			break;
		case kSampleBrick1: {
			animPos = *(data++);
			if (animPos == actor->animPosition && (actor->brickSound & 0x0F0) != 0x0F0) {
				int16 sampleIdx = (actor->brickSound & 0x0F) + 126;
				play_sample(sampleIdx, Rnd(1000) + 3596, 1, actor->X, actor->Y, actor->Z);
			}
		}
			break;
		case kSampleBrick2: {
			animPos = *(data++);
			if (animPos == actor->animPosition && (actor->brickSound & 0x0F0) != 0x0F0) {
				int16 sampleIdx = (actor->brickSound & 0x0F) + 126;
				play_sample(sampleIdx, Rnd(1000) + 3596, 1, actor->X, actor->Y, actor->Z);
			}
		}
			break;
		case kHeroHitting: {
			animPos = *(data++);
			animPos--;
			if (animPos == actor->animPosition) {
				actor->strengthOfHit = magicLevelStrengthOfHit[magicLevelIdx];
				actor->dynamicFlags.bIsHitting = 1;
			}
		}
			break;
		case kActionUnknown13: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 throwX, throwY, throwZ;
				int32 distanceX, distanceY, distanceZ;
				int32 spriteIdx, strength;
				int32 param1, param2, param3, param4;

				distanceX = *((int16 *)data);
				data += 2;
				distanceY = *((int16 *)data);
				data += 2;
				distanceZ = *((int16 *)data);
				data += 2;

				rotate_actor(distanceX, distanceZ, actor->angle);

				throwX = destX + actor->X;
				throwY = distanceY + actor->Y;
				throwZ = destZ + actor->Z;

				spriteIdx = *(data++);

				param1 = *((int16 *)data);
				data += 2;
				param2 = *((int16 *)data) + actor->angle;
				data += 2;
				param3 = *((int16 *)data);
				data += 2;
				param4 = *(data++);

				strength = *(data++);

				add_extra_throw(actorIdx, throwX, throwY, throwZ, spriteIdx, param1, param2, param3, param4, strength);
			} else {
				data += 15;
			}
		}
			break;
		case kActionUnknown14: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 newAngle, throwX, throwY, throwZ;
				int32 distanceX, distanceY, distanceZ;
				int32 spriteIdx, strength;
				int32 param1, param2, param3, param4;

				newAngle = get_angle(actor->Y, 0, sceneHero->Y, get_distance_2D(actor->X, actor->Z, sceneHero->X, sceneHero->Z));

				distanceX = *((int16 *)data);
				data += 2;
				distanceY = *((int16 *)data);
				data += 2;
				distanceZ = *((int16 *)data);
				data += 2;

				rotate_actor(distanceX, distanceZ, actor->angle);

				throwX = destX + actor->X;
				throwY = distanceY + actor->Y;
				throwZ = destZ + actor->Z;

				spriteIdx = *(data++);

				param1 = *((int16 *)data) + newAngle;
				data += 2;
				param2 = *((int16 *)data) + actor->angle;
				data += 2;
				param3 = *((int16 *)data);
				data += 2;
				param4 = *(data++);

				strength = *(data++);

				add_extra_throw(actorIdx, throwX, throwY, throwZ, spriteIdx, param1, param2, param3, param4, strength);
			} else {
				data += 15;
			}
		}
			break;
		case kActionUnknown15: {
			animPos = *(data++);
			if (animPos == actor->animPosition) {
				int32 distanceX, distanceY, distanceZ;
				int32 spriteIdx, targetActor, param3, param4;

				distanceX = *((int16 *)data);
				data += 2;
				distanceY = *((int16 *)data);
				data += 2;
				distanceZ = *((int16 *)data);
				data += 2;

				rotate_actor( distanceX, distanceZ, actor->angle);

				spriteIdx = *(data++);
				targetActor = *(data++);
				param3 = *((int16 *)data);
				data += 2;
				param4 = *(data++);

				add_extra_aiming(actorIdx, actor->X + destX, actor->Y + distanceY, actor->Z + distanceZ, spriteIdx, targetActor, param3, param4);
			} else {
				data+=11;
			}
		}
			break;
		case kActionUnknown9:
		default:
			break;
		}
		startAnimEntityIdx++;
	}
}

/** Initialize animation
	@param newAnim animation to init
	@param animType animation type
	@param animExtra animation actions extra data
	@param actorIdx actor index */
int32 init_anim(int8 newAnim, int16 animType, uint8 animExtra, int16 actorIdx) {
	ActorStruct *actor;
	int32 animIndex;

	actor = &sceneActors[actorIdx];

	if (actor->entity == -1)
		return 0;

	if (actor->staticFlags.bIsSpriteActor)
		return 0;

	if (newAnim == actor->anim && actor->previousAnimIdx != -1)
		return 1;

	if (animExtra == 255 && actor->animType != 2)
		animExtra = actor->anim;

	animIndex = get_body_anim_index(newAnim, actorIdx);

	if (animIndex == -1)
		animIndex = get_body_anim_index(0, actorIdx);

	if (animType != 4 && actor->animType == 2) {
		actor->animExtra = newAnim;
		return 0;
	}

	if (animType == 3) {
		animType = 2;

		animExtra = actor->anim;

		if (animExtra == 15 || animExtra == 7 || animExtra == 8 || animExtra == 9) {
			animExtra = 0;
		}
	}

	if (animType == 4)
		animType = 2;

	if (actor->previousAnimIdx == -1) {	// if no previous animation
		set_anim_at_keyframe(0, animTable[animIndex], bodyTable[actor->entity], &actor->animTimerData);
	} else { // interpolation between animations
		animBuffer2 += stock_animation(animBuffer2, bodyTable[actor->entity], &actor->animTimerData);
		if (animBuffer1 + 4488 < animBuffer2)
			animBuffer2 = animBuffer1;
	}

	actor->previousAnimIdx = animIndex;
	actor->anim = newAnim;
	actor->animExtra = animExtra;
	actor->animExtraPtr = currentActorAnimExtraPtr;
	actor->animType = animType;
	actor->animPosition = 0;
	actor->dynamicFlags.bIsHitting = 0;
	actor->dynamicFlags.bAnimEnded = 0;
	actor->dynamicFlags.bAnimFrameReached = 1;

	if (actor->animExtraPtr) {
		process_anim_actions(actorIdx);
	}

	actor->lastRotationAngle = 0;
	actor->lastX = 0;
	actor->lastY = 0;
	actor->lastZ = 0;

	return 1;
}

/** Process main loop actor animations
	@param actorIdx Actor index */
void process_actor_animations(int32 actorIdx) { // DoAnim
	int16 numKeyframe;
	int8 *animPtr;
	ActorStruct *actor;

	actor = &sceneActors[actorIdx];

	currentlyProcessedActorIdx = actorIdx;
	processActorPtr = actor;

	if (actor->entity == -1)
		return;

	previousActorX = actor->collisionX;
	previousActorY = actor->collisionY;
	previousActorZ = actor->collisionZ;

	if (actor->staticFlags.bIsSpriteActor) { // is sprite actor
		if (actor->strengthOfHit) {
			actor->dynamicFlags.bIsHitting = 1;
		}

		processActorX = actor->X;
		processActorY = actor->Y;
		processActorZ = actor->Z;

		if (!actor->dynamicFlags.bIsFalling) {
			if (actor->speed) {
				int32 angle = get_real_value(&actor->move);
				if (!angle) {
					if (actor->move.to > 0) {
						angle = 1;
					} else {
						angle = -1;
					}
				}
				
				rotate_actor(angle, 0, actor->animType);

				processActorY = actor->Y - destZ;

				rotate_actor(0, destX, actor->angle);

				processActorX = actor->X + destX;
				processActorZ = actor->Z + destZ;

				set_actor_angle(0, actor->speed, 50, &actor->move);

				if (actor->dynamicFlags.bIsSpriteMoving) {
					if (actor->doorStatus) { // open door
						if (get_distance_2D(processActorX, processActorZ, actor->lastX, actor->lastZ) >= actor->doorStatus) {
							if (actor->angle == 0) {
								processActorZ = actor->lastZ + actor->doorStatus;
							} else if (actor->angle == 0x100) {
								processActorX = actor->lastX + actor->doorStatus;
							} else if (actor->angle == 0x200) {
								processActorZ = actor->lastZ - actor->doorStatus;
							} else if (actor->angle == 0x300) {
								processActorX = actor->lastX - actor->doorStatus;
							}

							actor->dynamicFlags.bIsSpriteMoving = 0;
							actor->speed = 0;
						}
					} else { // close door
						int16 updatePos = 0;

						if (actor->angle == 0) {
							if (processActorZ <= actor->lastZ) {
								updatePos = 1;
							}
						} else if (actor->angle == 0x100) {
							if (processActorX <= actor->lastX) {
								updatePos = 1;
							}
						} else if (actor->angle == 0x200) {
							if (processActorZ >= actor->lastZ) {
								updatePos = 1;
							}
						} else if (actor->angle == 0x300) {
							if (processActorX >= actor->lastX) {
								updatePos = 1;
							}
						}

						if (updatePos) {
							processActorX = actor->lastX;
							processActorY = actor->lastY;
							processActorZ = actor->lastZ;

							actor->dynamicFlags.bIsSpriteMoving = 0;
							actor->speed = 0;
						}
					}
				}
			}

			if (actor->staticFlags.bCanBePushed) {
				processActorX += actor->lastX;
				processActorY += actor->lastY;
				processActorZ += actor->lastZ;

				if (actor->staticFlags.bUseMiniZv) {
					processActorX = ((processActorX / 128) * 128);
					processActorZ = ((processActorZ / 128) * 128);
				}

				actor->lastX = 0;
				actor->lastY = 0;
				actor->lastZ = 0;
			}
		}
	} else { // 3D actor
		if (actor->previousAnimIdx != -1) {
			int32 keyFramePassed;
			animPtr = animTable[actor->previousAnimIdx];

			keyFramePassed = verify_anim_at_keyframe(actor->animPosition, animPtr, (int8*)bodyTable[actor->entity], &actor->animTimerData);

			if (processRotationByAnim) {
				actor->dynamicFlags.bIsRotationByAnim = 1;
			} else {
				actor->dynamicFlags.bIsRotationByAnim = 0;
			}

			actor->angle = (actor->angle + processLastRotationAngle - actor->lastRotationAngle) & 0x3FF;
			actor->lastRotationAngle = processLastRotationAngle;

			rotate_actor(currentStepX, currentStepZ, actor->angle);

			currentStepX = destX;
			currentStepZ = destZ;

			processActorX = actor->X + currentStepX - actor->lastX;
			processActorY = actor->Y + currentStepY - actor->lastY;
			processActorZ = actor->Z + currentStepZ - actor->lastZ;

			actor->lastX = currentStepX;
			actor->lastY = currentStepY;
			actor->lastZ = currentStepZ;

			actor->dynamicFlags.bAnimEnded = 0;
			actor->dynamicFlags.bAnimFrameReached = 0;

			if (keyFramePassed) {
				actor->animPosition++;

				// if actor have animation actions to process
				if (actor->animExtraPtr) {
					process_anim_actions(actorIdx);
				}

				numKeyframe = actor->animPosition;
				if (numKeyframe == get_num_keyframes(animPtr)) {
					actor->dynamicFlags.bIsHitting = 0;

					if (actor->animType == 0) {
						actor->animPosition = get_start_keyframe(animPtr);
					} else {
						actor->anim = actor->animExtra;
						actor->previousAnimIdx = get_body_anim_index(actor->anim, actorIdx);

						if (actor->previousAnimIdx == -1) {
							actor->previousAnimIdx = get_body_anim_index(0, actorIdx);
							actor->anim = 0;
						}

						actor->animExtraPtr = currentActorAnimExtraPtr;

						actor->animType = 0;
						actor->animPosition = 0;
						actor->strengthOfHit = 0;
					}

					if (actor->animExtraPtr) {
						process_anim_actions(actorIdx);
					}

					actor->dynamicFlags.bAnimEnded = 1;
				}

				actor->lastRotationAngle = 0;

				actor->lastX = 0;
				actor->lastY = 0;
				actor->lastZ = 0;
			}
		}
	}

	// actor standing on another actor
	if (actor->standOn != -1) {
		processActorX -= sceneActors[actor->standOn].collisionX;
		processActorY -= sceneActors[actor->standOn].collisionY;
		processActorZ -= sceneActors[actor->standOn].collisionZ;

		processActorX += sceneActors[actor->standOn].X;
		processActorY += sceneActors[actor->standOn].Y;
		processActorZ += sceneActors[actor->standOn].Z;

		if (!standing_on_actor(actorIdx, actor->standOn)) {
			actor->standOn = -1; // no longer standing on other actor
		}
	}

	// actor falling Y speed
	if (actor->dynamicFlags.bIsFalling) {
		processActorX = previousActorX;
		processActorY = previousActorY + loopActorStep; // add step to fall
		processActorZ = previousActorZ;
	}

	// actor collisions with bricks
	if (actor->staticFlags.bComputeCollisionWithBricks) {
		int32 brickShape;
		collisionY = 0;
		
		brickShape = get_brick_shape(previousActorX, previousActorY, previousActorZ);

		if (brickShape) {
			if (brickShape != kSolid) {
				reajust_actor_position(brickShape);
			} /*else { // this shouldn't happen (collision should avoid it)
				actor->Y = processActorY = (processActorY / 256) * 256 + 256; // go upper
			}*/
		}

		if (actor->staticFlags.bComputeCollisionWithObj) {
			check_collision_with_actors(actorIdx);
		}

		if (actor->standOn != -1 && actor->dynamicFlags.bIsFalling) {
			stop_falling();
		}

		causeActorDamage = 0;

		processCollisionX = processActorX;
		processCollisionY = processActorY;
		processCollisionZ = processActorZ;

		if (!actorIdx && !actor->staticFlags.bComputeLowCollision) {
			// check hero collisions with bricks
			check_hero_collision_with_bricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 1);
			check_hero_collision_with_bricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 2);
			check_hero_collision_with_bricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   4);
			check_hero_collision_with_bricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   8);
		} else {
			// check other actors collisions with bricks
			check_actor_collision_with_bricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 1);
			check_actor_collision_with_bricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 2);
			check_actor_collision_with_bricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   4);
			check_actor_collision_with_bricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   8);
		}

		// process wall hit while running
		if (causeActorDamage && !actor->dynamicFlags.bIsFalling && !currentlyProcessedActorIdx && heroBehaviour == ATHLETIC && actor->anim == ANIM_FORWARD) {
			rotate_actor(actor->boudingBox.X.bottomLeft, actor->boudingBox.Z.bottomLeft, actor->angle + 0x580);

			destX += processActorX;
			destZ += processActorZ;

			if (destX >= 0 && destZ >= 0 && destX <= 0x7E00 && destZ <= 0x7E00) {
				if (get_brick_shape(destX, processActorY + 0x100, destZ)) {
					add_extra_special(actor->X, actor->Y + 1000, actor->Z, kHitStars);
					init_anim(ANIM_BIG_HIT, 2, 0, currentlyProcessedActorIdx);

					if (currentlyProcessedActorIdx == 0) {
						heroMoved = 1;
					}
					
					// cause wall damage
					actor->life--;

					/*if (cfgfile.Debug == 1) {
						printf("Wall hit - Type: running\n");
					}*/
				}
			}
		}

		brickShape = get_brick_shape(processActorX, processActorY, processActorZ);
		actor->brickShape = brickShape;

		if (brickShape) {
			if (brickShape == kSolid) {
				if (actor->dynamicFlags.bIsFalling) {
					stop_falling();
					processActorY = (collisionY << 8) + 0x100;
				} else {
					if (!actorIdx && heroBehaviour == ATHLETIC && actor->anim == brickShape) {
						add_extra_special(actor->X, actor->Y + 1000, actor->Z, kHitStars);
						init_anim(ANIM_BIG_HIT, 2, 0, currentlyProcessedActorIdx);	

						if (!actorIdx) {
							heroMoved = 1;
						}
						
						// cause wall damage
						actor->life--;

						/*if (cfgfile.Debug == 1) {
							printf("Wall hit - Type: running 2\n");
						}*/
					}

					// no Z coordinate issue
					if (!get_brick_shape(processActorX, processActorY, previousActorZ)) {
						processActorZ = previousActorZ;
					}

					// no X coordinate issue
					if (!get_brick_shape(previousActorX, processActorY, processActorZ)) {
						processActorX = previousActorX;
					}

					// X and Z with issue, no move
					if (get_brick_shape(processActorX, processActorY, previousActorZ) && get_brick_shape(previousActorX, processActorY, processActorZ)) {
						return;
					}
				}
			} else {
				if (actor->dynamicFlags.bIsFalling) {
					stop_falling();
				}

				reajust_actor_position(brickShape);
			}

			actor->dynamicFlags.bIsFalling = 0;
		} else {
			if (actor->staticFlags.bCanFall && actor->standOn == -1) {
				brickShape = get_brick_shape(processActorX, processActorY - 1, processActorZ);

				if (brickShape) {
					if (actor->dynamicFlags.bIsFalling) {
						stop_falling();
					}

					reajust_actor_position(brickShape);
				} else {
					if (!actor->dynamicFlags.bIsRotationByAnim) {
						actor->dynamicFlags.bIsFalling = 1;

						if (!actorIdx && heroYBeforeFall == 0) {
							heroYBeforeFall = processActorY;
						}

						init_anim(ANIM_FALL, 0, 255, actorIdx);
					}
				}
			}
		}

		// if under the map, than die
		if (collisionY == -1) {
			actor->life = 0;
		}
	} else {
		if (actor->staticFlags.bComputeCollisionWithObj) {
			check_collision_with_actors(actorIdx);
		}
	}

	if (causeActorDamage) {
		actor->brickShape |= 0x80;
	}

	// check and fix actor bounding position
	if (processActorX < 0) {
		processActorX = 0;
	}

	if (processActorY < 0) {
		processActorY = 0;
	}

	if (processActorZ < 0) {
		processActorZ = 0;
	}

	if (processActorX > 0x7E00) {
		processActorX = 0x7E00;
	}

	if (processActorZ > 0x7E00) {
		processActorZ = 0x7E00;
	}

	actor->X = processActorX;
	actor->Y = processActorY;
	actor->Z = processActorZ;
}
