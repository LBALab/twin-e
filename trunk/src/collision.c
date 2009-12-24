/** @file collision.c
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
#include <stdio.h>

#include "collision.h"
#include "scene.h"
#include "actor.h"
#include "movements.h"
#include "grid.h"
#include "main.h"

/** Check if actor 1 is standing in actor2
	@param actorIdx1 Actor 1 index 
	@param actorIdx2 Actor 2 index */
int32 standing_on_actor(int32 actorIdx1, int32 actorIdx2) { // CheckZvOnZv
	int32 x1Left, y1Left, z1Left, x1Right, y1Right, z1Right;
	int32 x2Left, y2Left, z2Left, x2Right, y2Right, z2Right;
	ActorStruct *actor1;
	ActorStruct *actor2;

	actor1 = &sceneActors[actorIdx1];
	actor2 = &sceneActors[actorIdx2];

	// Current actor (actor 1)
	x1Left = processActorX + actor1->boudingBox.X.bottomLeft;
	x1Right = processActorX + actor1->boudingBox.X.topRight;

	y1Left = processActorZ + actor1->boudingBox.Y.bottomLeft;
	y1Right = processActorZ + actor1->boudingBox.Y.topRight;

	z1Left = processActorY + actor1->boudingBox.Z.bottomLeft;
	z1Right = processActorY + actor1->boudingBox.Z.topRight;

	// Actor 2
	x2Left = actor2->X + actor2->boudingBox.X.bottomLeft;
	x2Right = actor2->X + actor2->boudingBox.X.topRight;

	y2Left = actor2->Y + actor2->boudingBox.Y.bottomLeft;
	y2Right = actor2->Y + actor2->boudingBox.Y.topRight;

	z2Left = actor2->Z + actor2->boudingBox.Z.bottomLeft;
	z2Right = actor2->Z + actor2->boudingBox.Z.topRight;

	if (x1Left >= x2Right)
		return 0; // not standing

	if (x1Right <= x2Left)
		return 0;

	if (y1Left > (y2Right + 1))
		return 0;

	if (y1Left <= (y2Right - 0x100))
		return 0;

	if (y1Right <= y2Left)
		return 0;

	if (z1Left >= z2Right)
		return 0;

	if (z1Right <= z2Left)
		return 0;

	return 1; // standing
}

int32 get_average_value(int32 var0, int32 var1, int32 var2, int32 var3)
{
	if (var3 <= 0) {
		return var0;
	}

	if (var3 >= var2) {
		return var1;
	}

    return ((((var1 - var0) * var3) / var2) + var0);
}

/** Reajust actor position in scene according with brick shape bellow actor
	@param brickShape Shape of brick bellow the actor */
void reajust_actor_position(int32 brickShape) {
	int32 brkX, brkY, brkZ;

	if (!brickShape) {
		return;
	}

	brkX = (collisionX << 9) - 0x100;
	brkY = collisionY << 8;
	brkZ = (collisionZ << 9) - 0x100;

	// double-side stairs
	if (brickShape >= kDoubleSideStairsTop1 && brickShape <= kDoubleSideStairsRight2) {
		switch (brickShape) {
		case kDoubleSideStairsTop1: 
			if (processActorZ - collisionZ <= processActorX - collisionX) {
				brickShape = kStairsTopLeft;
			} else {
				brickShape = kStairsTopRight;
			}
			break;
		case kDoubleSideStairsBottom1: 
			if (processActorZ - collisionZ <= processActorX - collisionX) {
				brickShape = kStairsBottomLeft;
			} else {
				brickShape = kStairsBottomRight;
			}
			break;
		case kDoubleSideStairsLeft1: 
			if (512 - processActorX - collisionX <= processActorZ - collisionZ) {
				brickShape = kStairsTopLeft;
			} else {
				brickShape = kStairsBottomLeft;
			}
			break;
		case kDoubleSideStairsRight1: 
			if (512 - processActorX - collisionX <= processActorZ - collisionZ) {
				brickShape = kStairsTopRight;
			} else {
				brickShape = kStairsBottomRight;
			}
			break;

		case kDoubleSideStairsTop2: 
			if (processActorX - collisionX >= processActorZ - collisionZ) {
				brickShape = kStairsTopRight;
			} else {
				brickShape = kStairsTopLeft;
			}
			break;
		case kDoubleSideStairsBottom2: 
			if (processActorZ - collisionZ <= processActorX - collisionX) {
				brickShape = kStairsBottomRight;
			} else {
				brickShape = kStairsBottomLeft;
			}
			break;
		case kDoubleSideStairsLeft2: 
			if (512 - processActorX - collisionX <= processActorZ - collisionZ) {
				brickShape = kStairsBottomLeft;
			} else {
				brickShape = kStairsTopLeft;
			}
			break;
		case kDoubleSideStairsRight2: 
			if (512 - processActorX - collisionX <= processActorZ - collisionZ) {
				brickShape = kStairsBottomRight;
			} else {
				brickShape = kStairsTopRight;
			}
			break;
		default:
			if (cfgfile.Debug == 1) {
				printf("Brick Shape %d unsupported\n", brickShape);
			}
			break;
		}	
	}

	if (brickShape >= kStairsTopLeft && brickShape <= kStairsBottomRight) {
		switch (brickShape) {
		case kStairsTopLeft: 
			//processActorY = brkY + 
			break;
		default:
			break;
		}
	}
}
