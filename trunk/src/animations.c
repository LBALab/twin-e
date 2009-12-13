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

/** Preload all animations */
void preload_animations()
{
	int i;
	int numEntries = hqr_num_entries(HQR_ANIM_FILE)-1;

	for(i=0; i<numEntries; i++)
	{
		animSizeTable[i] = hqr_getalloc_entry(&animTable[i],HQR_ANIM_FILE,i);
	}
}

int set_anim_at_keyframe(int keyframeIdx, unsigned char *anim, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr)
{
	short int numOfKeyframeInAnim;
	short int numOfBonesInAnim;
	unsigned char *ptrToData;
	unsigned char *ptrToDataBackup;
	unsigned char *ptrToBodyData;
	short int bodyHeader;
	short int numOfElementInBody;
	short int numOfPointInBody;
	int i;

	numOfKeyframeInAnim = *(short int *)(anim);

	if (keyframeIdx >= numOfKeyframeInAnim)
		return (numOfKeyframeInAnim);

	numOfBonesInAnim = *(short int *) (anim + 2);

	ptrToData = (unsigned char *) ((numOfBonesInAnim * 8 + 8) * keyframeIdx + anim + 8);

	bodyHeader = *(short int *)(body);

	if (!(bodyHeader & 2))
		return (0);

	ptrToBodyData = body + 14;

	animTimerDataPtr->ptr = ptrToData;
	animTimerDataPtr->time = lbaTime;

	ptrToBodyData = ptrToBodyData + *(short int *)(ptrToBodyData) + 2; 

	numOfElementInBody = *(short int *)(ptrToBodyData);

	ptrToBodyData = ptrToBodyData + numOfElementInBody * 6 + 12; 

	numOfPointInBody = *(short int *)(ptrToBodyData - 10); // num elements

	if (numOfBonesInAnim > numOfPointInBody)
	{
		numOfBonesInAnim = numOfPointInBody;
	}

	ptrToDataBackup = ptrToData;

	ptrToData += 8;

	do
	{
		for (i = 0; i < 8; i++)
		{
			*(ptrToBodyData++) = *(ptrToData++);
		}

		ptrToBodyData += 30;

	}while (--numOfBonesInAnim);

	ptrToData = ptrToDataBackup + 2;

	currentX = *(short int *)(ptrToData);
	currentY = *(short int *)(ptrToData + 2);
	currentZ = *(short int *)(ptrToData + 4);

	processActorVar5 = *(short int *)(ptrToData + 6);
	processActorSub2Var0 = *(short int *)(ptrToData + 8);
	processActorVar6 = *(short int *)(ptrToData + 10);
	processActorSub2Var1 = *(short int *)(ptrToData + 12);

	return (1);
}

int get_num_keyframes(unsigned char *ptr)
{
  return (*(short int *)(ptr));
}

int get_start_keyframe(unsigned char *ptr)
{
  return (*(short int *)(ptr + 4));
}

void apply_anim_steprotation(unsigned char **ptr, int bp, int bx)
{
	short int *dest;
	short int lastAngle;
	short int newAngle;
	short int angleDif;
	short int computedAngle;

	lastAngle = *(short int *)(lastKeyFramePtr);
	lastKeyFramePtr += 2;

	newAngle = *(short int *)(keyFramePtr);
	keyFramePtr += 2;

	lastAngle &= 0x3FF;
	newAngle &= 0x3FF;

	angleDif = newAngle - lastAngle;

	if (angleDif)
	{
		if (angleDif < -0x200)
		{
			angleDif += 0x400;
		}
		else if (angleDif > 0x200)
		{
			angleDif -= 0x400;
		}

		computedAngle = lastAngle + (angleDif * bp) / bx;
	}
	else
	{
		computedAngle = lastAngle;
	}

	dest = (short int *) *(ptr);
	*dest = computedAngle & 0x3FF;
	*(ptr) = *(ptr) + 2;
}

void apply_anim_step(unsigned char **ptr, int bp, int bx)
{
	short int *dest;
	short int lastAngle;
	short int newAngle;
	short int angleDif;
	short int computedAngle;

	lastAngle = *(short int *) lastKeyFramePtr;
	lastKeyFramePtr += 2;

	newAngle = *(short int *) keyFramePtr;
	keyFramePtr += 2;

	angleDif = newAngle - lastAngle;

	if (angleDif)
	{
		computedAngle = lastAngle + (angleDif * bp) / bx;
	}
	else
	{
		computedAngle = lastAngle;
	}

	dest = (short int *) *(ptr);
	*dest = computedAngle;
	*(ptr) = *(ptr) + 2;
}

int get_anim_mode(unsigned char **ptr)
{
	short int *lptr;
	short int opcode;

	lptr = (short int *) *ptr;

	opcode = *(short int *)(keyFramePtr);
    *(short int *)(lptr) = opcode;

	keyFramePtr += 2;
	*(ptr) = *(ptr) + 2;
	lastKeyFramePtr += 2;

	return (opcode);
}

int set_model_animation(int animState, unsigned char *animData, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr)
{
	short int animOpcode;

	short int bodyHeader;

	unsigned char *edi;
	unsigned char *ebx;
	int ebp;
	int eax;
	int keyFrameLength;
	int numOfPointInBody;
	int numOfPointInAnim;
	unsigned char *keyFramePtrOld;

	numOfPointInAnim = *(short int *)(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animState) + animData + 8;

	keyFrameLength = *(short int *)(keyFramePtr);

	bodyHeader = *(short int *)(body);

	if (!(bodyHeader & 2))
	{
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx)
	{
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}
/*	else
	{
		assert_ptr(ebx);
	}*/

	lastKeyFramePtr = ebx;

	eax = *(short int *)(edi - 2);
	edi += eax;

	eax = *(short int *)(edi);
	eax = eax + eax * 2;
	edi = edi + eax * 2 + 12;

	numOfPointInBody = *(short int *)(edi - 10);

	if (numOfPointInAnim > numOfPointInBody)
	{
		numOfPointInAnim = numOfPointInBody;
	}

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength)
	{
		int *destPtr; // keyFrame
		int *sourcePtr;

		sourcePtr = (int *) (keyFramePtr + 8);
		destPtr = (int *) edi;

		do
		{
			*(destPtr++) = *(sourcePtr++);
			*(destPtr++) = *(sourcePtr++);
			destPtr = (int *) (((char *)destPtr) + 30);
		}while (--numOfPointInAnim);

		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentX = *(short int *)(keyFramePtr + 2);
		currentY = *(short int *)(keyFramePtr + 4);
		currentZ = *(short int *)(keyFramePtr + 6);

		processActorVar5 = *(short int *)(keyFramePtr + 8);
		processActorSub2Var0 = *(short int *)(keyFramePtr + 10);
		processActorVar6 = *(short int *)(keyFramePtr + 12);
		processActorSub2Var1 = *(short int *)(keyFramePtr + 14);

		return (1);
	}
	else
	{
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5 = *(short int *)(keyFramePtr);
		processActorSub2Var0 = (*(short int *)(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6 = (*(short int *)(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (*(short int *)(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		edi += 38;

		if (--numOfPointInAnim)
		{
			animVar4 = numOfPointInAnim;

			do
			{
				animOpcode = get_anim_mode(&edi);

				switch (animOpcode)
				{
					case 0:	// allow global rotate
					{
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						break;
					}
					case 1:	// dissallow global rotate
					{
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						break;
					}
					case 2:	// dissallow global rotate + hide
					{
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						break;
					}
					default:
					{
						printf("Unsupported animation rotation mode %d!\n", animOpcode);
						exit(1);
					}
				}

				edi += 30;
			}while (--animVar4);
		}

		currentX = ( *(short int *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentY = ( *(short int *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentZ = ( *(short int *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

int get_body_anim_index(int anim, short int actorNumber)
{
	char type;
	unsigned short int var1;
	unsigned char *bodyPtr;
	unsigned char *ptr, *ptr2;
	unsigned char *costumePtr = NULL;
	ActorStruct *localActor;

	localActor = &sceneActors[actorNumber];
	bodyPtr = localActor->entityDataPtr;

	do
	{
		type = *(bodyPtr++);

		if (type == -1)
		{
			currentActorAnimExtraData = NULL;
			return (-1);
		}

		ptr = (bodyPtr + 1);

		if (type == 3)
		{
			if (anim == *bodyPtr)
			{
				ptr++;
				var1 = *(short int *)(ptr);
				ptr += 2;
				ptr2 = ptr;
				ptr++;
				if (*ptr2 != 0)
				{
					costumePtr = ptr - 1;
				}
				currentActorAnimExtraData = costumePtr;
				return (var1);
			}
		}

		bodyPtr = *ptr + ptr;

	}while (1);

	return (0);
}


int stock_animation(unsigned char *lBufAnim, unsigned char *lBody, AnimTimerDataStruct* animTimerDataPtr)	// copy the next keyFrame from a different buffer
{
	int temp;
	unsigned char *ptr;
	int *edi;
	int *esi;
	int var0;
	int var1;
	int var2;
	int counter;

	//assert_ptr(lBufAnim);
	//assert_ptr(lBody);

	temp = *(short int *)(lBody);

	if (temp & 2)
	{
		ptr = (lBody + 0x10);

		animTimerDataPtr->time = lbaTime;
		animTimerDataPtr->ptr = lBufAnim;

		var0 = *(short int *)(ptr - 2);
		ptr = ptr + var0;

		var1 = *(short int *)(ptr);
		var1 = var1 + var1 * 2;

		ptr = ptr + var1 * 2 + 2;

		var2 = *(short int *)(ptr);
		counter = var2;
		var2 = (var2 * 8) + 8;

		edi = (int *) (lBufAnim + 8);
		esi = (int *) (ptr + 10);

		do
		{
			*(edi++) = *(esi++);
			*(edi++) = *(esi++);

			esi = (int *) (((char *) esi) + 30);
		}while (--counter);

		return (var2);
	}
	return (0);
}

int init_anim(char newAnim, short int arg_4, unsigned char arg_8, short int actorNum)
{
	ActorStruct *localActor;
	int animIndex;

	localActor = &sceneActors[actorNum];

	if (localActor->entity == -1)
		return (0);

	if (localActor->staticFlags.bIsSpriteActor) // si c'est un sprite
		return (0);

	if (newAnim == localActor->anim && localActor->previousAnimIdx != -1) // le costume est deja loade
		return (1);

	if (arg_8 == 255 && localActor->field_78 != 2)
		arg_8 = localActor->anim;

	animIndex = get_body_anim_index(newAnim, actorNum);

	if (animIndex == -1)
		animIndex = get_body_anim_index(0, actorNum);

	if (arg_4 != 4 && localActor->field_78 == 2)
	{
		localActor->animExtra = newAnim;
		return (0);
	}

	if (arg_4 == 3)
	{
		arg_4 = 2;

		arg_8 = localActor->anim;

		if (arg_8 == 15 || arg_8 == 7 || arg_8 == 8 || arg_8 == 9)
		{
			arg_8 = 0;
		}
	}

	if (arg_4 == 4)
		arg_4 = 2;

	if (localActor->previousAnimIdx == -1)	// if no previous animation
	{
		//setAnimAtKeyFrame(0, HQR_Get(HQR_Anims, animIndex), bodyTable[lactor->costumeIndex], &lactor->animTimerData);	// set animation directly to first keyFrame
	}
	else // interpolation between animations
	{
		animBuffer2 += stock_animation(animBuffer2, bodyTable[localActor->entity], &localActor->animTimerData);
		if (animBuffer1 + 4488 > animBuffer2)
			animBuffer2 = animBuffer1;
	}

	localActor->previousAnimIdx = animIndex;
	localActor->anim = newAnim;
	localActor->animExtra = arg_8;
	localActor->animExtraData = currentActorAnimExtraData;
	localActor->field_78 = arg_4;
	localActor->animPosition = 0;
	localActor->dynamicFlags.bUnk0002 = 0;
	localActor->dynamicFlags.bUnk0004 = 0;
	localActor->dynamicFlags.bUnk0008 = 1;

	if (localActor->animExtraData)
	{
		//GereAnimAction(lactor, actorNum);
	}

	localActor->lastRotationSpeed = 0;
	localActor->lastX = 0;
	localActor->lastY = 0;
	localActor->lastZ = 0;

	return (1);
}

int verify_anim_at_keyframe(int animPos, unsigned char *animData, unsigned char *body, AnimTimerDataStruct* animTimerDataPtr)
{
	short int bodyHeader;

	unsigned char *edi;
	unsigned char *ebx;
	int ebp;
	int eax;
	int keyFrameLength;
	int numOfPointInAnim = -1;
	unsigned char *keyFramePtrOld;

	numOfPointInAnim = *(short int *)(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animPos) + animData + 8;

	keyFrameLength = *(short int *)(keyFramePtr);

	bodyHeader = *(short int *)(body);

	if (!(bodyHeader & 2))
	{
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx)
	{
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}

	lastKeyFramePtr = ebx;

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength)
	{
		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentX = *(short int *)(keyFramePtr + 2);
		currentY = *(short int *)(keyFramePtr + 4);
		currentZ = *(short int *)(keyFramePtr + 6);

		processActorVar5 = *(short int *)(keyFramePtr + 8);
		processActorSub2Var0 = *(short int *)(keyFramePtr + 10);
		processActorVar6 = *(short int *)(keyFramePtr + 12);
		processActorSub2Var1 = *(short int *)(keyFramePtr + 14);

		return (1);
	}
	else
	{
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5 = *(short int *)(keyFramePtr);
		processActorSub2Var0 = (*(short int *)(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6 = (*(short int *)(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (*(short int *)(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		currentX = ( *(short int *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentY = ( *(short int *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentZ = ( *(short int *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

/** Process main loop actor animations */
void process_actor_animations(int actorIdx) // DoAnim
{
	short int numKeyframe;
	char *animPtr;
	ActorStruct *actor; 

	actor = &sceneActors[actorIdx];

    currentlyProcessedActorNum = actorIdx;
    processActorVar1 = actor;

    if (actor->entity == -1)
		return;

	processActorVar2 = actor->X;
    processActorVar3 = actor->Y;
    processActorVar4 = actor->Z;

	if (actor->staticFlags.bIsSpriteActor) // is sprite actor
    {
		// TODO: update sprite actors
	}
	else // 3D actor
	{
		if (actor->previousAnimIdx != -1)
		{
			int keyFramePassed;
			animPtr = animTable[actor->previousAnimIdx];
      
			keyFramePassed = verify_anim_at_keyframe(actor->animPosition, animPtr,(char*)bodyTable[actor->entity], &actor->animTimerData); 

			if (processActorVar5)
			{
				actor->dynamicFlags.bUnk0080 = 1;
			}
			else
			{
				actor->dynamicFlags.bUnk0080 = 0;
			}

			actor->angle = (actor->angle + processActorVar6 - actor->lastRotationSpeed) & 0x3FF;
			actor->lastRotationSpeed = processActorVar6;

			//Rotate(currentX, currentZ, lactor->angle);

			currentX = destX; // dest
			currentZ = destZ;

			processActorX = actor->X + currentX - actor->lastX;
			processActorY = actor->Y + currentY - actor->lastY;
			processActorZ = actor->Z + currentZ - actor->lastZ;

			actor->lastX = currentX;
			actor->lastY = currentY;
			actor->lastZ = currentZ;

			actor->dynamicFlags.bUnk0004 = 0;
			actor->dynamicFlags.bUnk0008 = 0;

			if(keyFramePassed)  // if keyFrame
			{
				actor->animPosition++;

				if (actor->animExtraData) // if actor have animation actions to process
				{
					//TODO process_anim_actions - GereAnimAction(actor, actorIdx);
				}

				numKeyframe = actor->animPosition;
				if (numKeyframe == get_num_keyframes(animPtr)) // last anim keyframe
				{
					actor->dynamicFlags.bUnk0002 = 0;

					if (actor->field_78 == 0)
					{
						actor->animPosition = get_start_keyframe(animPtr);
					}
					else
					{
						actor->anim = actor->animExtra;
						actor->previousAnimIdx = get_body_anim_index(actor->anim, actorIdx);

						if (actor->previousAnimIdx == -1)
						{
							actor->previousAnimIdx = get_body_anim_index(0, actorIdx);
							actor->anim = 0;
						}

						actor->animExtraData = currentActorAnimExtraData;

						actor->field_78 = 0;
						actor->animPosition = 0;
						actor->strengthOfHit = 0;
					}

					if (actor->animExtraData)
					{
						//TODO process_anim_actions - GereAnimAction(actor, actorIdx);
					}

					actor->dynamicFlags.bUnk0004 = 1;
				}

				actor->lastRotationSpeed = 0;

				actor->lastX = 0;
				actor->lastY = 0;
				actor->lastZ = 0;
			}
		}
	}

	// TODO: actor standing of object
    // TODO: actor falling Y speed.
	// TODO: actor collisions
	// TODO: check actor damage
	// TODO: check actor bounding position
}
