/** @file movements.c
	@brief
	This file contains movies routines

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

#include <math.h>

#include "movements.h"
#include "actor.h"
#include "lbaengine.h"
#include "renderer.h"
#include "grid.h"
#include "scene.h"
#include "keyboard.h"
#include "animations.h"
#include "collision.h"

/** Get shadow position
	@param X Shadow X coordinate
	@param Y Shadow Y coordinate
	@param Z Shadow Z coordinate */
void get_shadow_position(int32 X, int32 Y, int32 Z) {
	int32 tempX;
	int32 tempY;
	int32 tempZ;
	uint8* ptr;

	tempX = (X + 0x100) >> 9;
	tempY = Y >> 8;
	tempZ = (Z + 0x100) >> 9;

	ptr = blockBuffer + tempY * 2 + tempX * 25 * 2 + (tempZ << 6) * 25 * 2;

	while (tempY) { // search down until either ground is found or lower border of the cube is reached
		if (*(int16*)ptr) // found the ground
			break;

		tempY--;
		ptr -= 2;
	}

	shadowCollisionType = 0;

	collisionX = tempX;
	collisionY = tempY;
	collisionZ = tempZ;

	processActorX = X;
	processActorY = (tempY + 1) << 8;
	processActorZ = Y;

	if (*ptr) {
		uint8* tempPtr;

		tempPtr = get_block_library(*(ptr++) - 1) + 3;

		shadowCollisionType = *(tempPtr + *(ptr) * 4);

		//ReajustPos(shadowCollisionType);
	}

	shadowX = processActorX;
	shadowY = processActorY;
	shadowZ = processActorZ;
}

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle
	@param stepAngle number of steps
	@param movePtr Pointer to process movements */
void set_actor_angle_safe(int16 startAngle, int16 endAngle, int16 stepAngle, ActorMoveStruct * movePtr) {
	movePtr->from = startAngle & 0x3FF;
	movePtr->to = endAngle & 0x3FF;
	movePtr->numOfStep = stepAngle & 0x3FF;
	movePtr->timeOfChange = lbaTime;
}

/** Clear actors safe angle
	@param actorPtr actor pointer */
void clear_real_angle(ActorStruct * actorPtr) {
	set_actor_angle_safe(actorPtr->angle, actorPtr->angle, 0, &actorPtr->move);
}

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle
	@param stepAngle number of steps
	@param movePtr Pointer to process movements */
void set_actor_angle(int16 startAngle, int16 endAngle, int16 stepAngle, ActorMoveStruct * movePtr) {
	movePtr->from = startAngle;
	movePtr->to = endAngle;
	movePtr->numOfStep = stepAngle;
	movePtr->timeOfChange = lbaTime;
}

/** Get actor angle
	@param x1 Actor 1 X
	@param z1 Actor 1 Z
	@param x2 Actor 2 X
	@param z2 Actor 2 Z */
int32 get_angle(int32 x1, int32 z1, int32 x2, int32 z2) {
	int32 newX, newZ, difX, difZ, tmpX, tmpZ, tmpEx, flag, destAngle, startAngle, stopAngle, finalAngle;

	difZ = tmpZ = z2 - z1;
	newZ = tmpZ * tmpZ;

	difX = tmpX = x2 - x1;
	newX = tmpX * tmpX;

	// Exchange X and Z
	if (newX < newZ) {
		tmpEx = difX;
		difX = difZ;
		difZ = tmpEx;

		flag = 1;
	} else {
		flag = 0;
	}

	moveAngle = (int32)sqrt(newX + newZ);

	if (!moveAngle) {
		return 0;
	}

	destAngle = (difZ << 14) / moveAngle;

	startAngle = 0;
	stopAngle  = 0x100;

	while (shadeAngleTab3[startAngle] > destAngle) {
		startAngle++;
	}

	if (shadeAngleTab3[startAngle] != destAngle) {
		if ((shadeAngleTab3[startAngle - 1] + shadeAngleTab3[startAngle]) / 2 <= destAngle) {
			startAngle--;
		}
	}

	finalAngle = 128 + startAngle;

	if (difX <= 0) {
		finalAngle = -finalAngle;
	}

	if (flag & 1) {
		finalAngle = -finalAngle + 0x100;
	}

	return finalAngle & 0x3FF;
}

/** Get actor real angle
	@param movePtr Pointer to process movements */
int32 get_real_angle(ActorMoveStruct * movePtr) {
	int32 timePassed;
	int32 remainingAngle;

	if (movePtr->numOfStep) {
		timePassed = lbaTime - movePtr->timeOfChange;

		if (timePassed >= movePtr->numOfStep) {	// rotation is finished
			movePtr->numOfStep = 0;
			return (movePtr->to);
		}

		remainingAngle = movePtr->to - movePtr->from;

		if (remainingAngle < -0x200) {
			remainingAngle += 0x400;
		} else if (remainingAngle > 0x200) {
			remainingAngle -= 0x400;
		}

		remainingAngle *= timePassed;
		remainingAngle /= movePtr->numOfStep;
		remainingAngle += movePtr->from;

		return (remainingAngle);
	}

	return (movePtr->to);
}

/** Get actor step
	@param movePtr Pointer to process movements */
int32 get_real_value(ActorMoveStruct * movePtr) {
	int32 tempStep;

	if (!movePtr->numOfStep)
		return (movePtr->to);

	if (!(lbaTime - movePtr->timeOfChange < movePtr->numOfStep)) {
		movePtr->numOfStep = 0;
		return (movePtr->to);
	}

	tempStep = movePtr->to - movePtr->from;
	tempStep *= lbaTime - movePtr->timeOfChange;
	tempStep /= movePtr->numOfStep;

	return (tempStep + movePtr->from);
}

/** Rotate actor with a given angle
	@param X Actor current X coordinate
	@param Z Actor current Z coordinate
	@param angle Actor angle to rotate */
void rotate_actor(int32 X, int32 Z, int32 angle) {
	int32 angle1;
	int32 angle2;

	if (!angle) {
		destX = X;
		destZ = Z;
	} else {
		angle1 = shadeAngleTab1[angle & 0x3FF];
		angle2 = shadeAngleTab1[(angle + 0x100) & 0x3FF];

		destX = (X * angle2 + Z * angle1) >> 14;
		destZ = (Z * angle2 - X * angle1) >> 14;
	}
}

/** Get distance value in 2D
	@param x1 Actor 1 X coordinate
	@param z1 Actor 1 Z coordinate
	@param x2 Actor 2 X coordinate
	@param z2 Actor 2 Z coordinate */
int32 get_distance_2D(int32 x1, int32 z1, int32 x2, int32 z2) {
	int32 newX;
	int32 newZ;

	newX = x2 - x1;
	newX *= newX;

	newZ = z2 - z1;
	newZ *= newZ;

	return (int32)sqrt(newX + newZ);
}

/** Move actor around the scene
	@param angleFrom Current actor angle
	@param angleTo Angle to rotate
	@param speed Rotate speed
	@param movePtr Pointer to process movements */
void move_actor(int32 angleFrom, int32 angleTo, int32 speed, ActorMoveStruct *movePtr) { // ManualRealAngle
	int32 numOfStepInt;
	int16 numOfStep;
	int16 from;
	int16 to;

	from = angleFrom & 0x3FF;
	to   = angleTo & 0x3FF;

	movePtr->from = from;
	movePtr->to   = to;

	numOfStep = (from - to) << 6;

	if (numOfStep < 0) {
		numOfStepInt = -numOfStep;
	} else {
		numOfStepInt = numOfStep;
	}

	numOfStepInt >>= 6;

	numOfStepInt *= speed;
	numOfStepInt >>= 8;

	movePtr->numOfStep = (int16)numOfStepInt;
	movePtr->timeOfChange = lbaTime;
}

void process_actor_movements(int32 actorIdx) {
	ActorStruct *actor = &sceneActors[actorIdx];

	if (actor->entity == -1)
		return;

	if (actor->dynamicFlags.bIsFalling) {
		int16 tempAngle = 0;

		if (actor->controlMode != 1)
			return;

		if (key & 4)
			tempAngle = 0x100;

		if (key & 8)
			tempAngle = -0x100;

		move_actor(actor->angle, actor->angle + tempAngle, actor->speed, &actor->move);

		heroPressedKey = key;
	} else {
		if (!actor->staticFlags.bIsSpriteActor) {
			if (actor->controlMode != 1) {
				actor->angle = get_real_angle(&actor->move);
			}
		}

		switch (actor->controlMode) {
		case 0: // NO_MOVE
			break;
		case 1: // MOVE_MANUAL
			if (!actorIdx) {
				heroAction = 0;

				//TODO: add action key like in LBA2

				//TODO: do behaviour actions
			}

			if (loopPressedKey == 0 || heroAction != 0) {
				int16 tempAngle;

				if (key & 3)  // if continue walking
					heroMoved = 0; // don't break animation

				if (key != heroPressedKey || loopPressedKey != heroPressedKey2) {
					if (heroMoved != 0) {
						init_anim(ANIM_STANDING, 0, 255, actorIdx);
					}
				}

				heroMoved = 0;

				if (key & 1) { // walk forward
					//if (currentActorInZoneProcess == 0) // TODO
					{
						init_anim(ANIM_FORWARD, 0, 255, actorIdx);
					}
					heroMoved = 1;
				}

				if (key & 2 && !(key & 1)) { // walk backward
					init_anim(ANIM_BACKWARD, 0, 255, actorIdx);
					heroMoved = 1;
				}

				if (key & 4) { // turn left
					heroMoved = 1;
					if (actor->anim == 0) {
						init_anim(ANIM_TURNLEFT, 0, 255, actorIdx);
					} else {
						if (!actor->dynamicFlags.bIsRotationByAnim) {
							actor->angle = get_real_angle(&actor->move);
						}
					}
				}

				if (key & 8) { // turn right
					heroMoved = 1;
					if (actor->anim == 0) {
						init_anim(ANIM_TURNRIGHT, 0, 255, actorIdx);
					} else {
						if (!actor->dynamicFlags.bIsRotationByAnim) {
							actor->angle = get_real_angle(&actor->move);
						}
					}
				}

				tempAngle = 0;

				if (key & 4) {
					tempAngle = 0x100;
				}

				if (key & 8) {
					tempAngle = -0x100;
				}

				move_actor(actor->angle, actor->angle + tempAngle, actor->speed, &actor->move);

				heroPressedKey  = key;
				heroPressedKey2 = loopPressedKey;

			}
			break;
			/*default:
				printf("Unimplemented control mode %d\n", actor->controlMode);*/
		}
	}
}
