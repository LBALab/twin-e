/** @file collision.h
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

#ifndef COLLISION_H
#define COLLISION_H

#include "sys.h"
#include "extra.h"

/** Actor collition X coordinate */
int32 collisionX; // getPosVar1
/** Actor collition Y coordinate */
int32 collisionY; // getPosVar2
/** Actor collition Z coordinate */
int32 collisionZ; // getPosVar3

/** Actor collition X coordinate */
int32 processCollisionX; // processActorVar11
/** Actor collition Y coordinate */
int32 processCollisionY; // processActorVar12
/** Actor collition Z coordinate */
int32 processCollisionZ; // processActorVar13

/** Cause damage in current processed actor */
int32 causeActorDamage; //fieldCauseDamage

/** Check if actor 1 is standing in actor2
	@param actorIdx1 Actor 1 index 
	@param actorIdx2 Actor 2 index */
int32 standingOnActor(int32 actorIdx1, int32 actorIdx2);

int32 getAverageValue(int32 var0, int32 var1, int32 var2, int32 var3);

/** Reajust actor position in scene according with brick shape bellow actor
	@param brickShape Shape of brick bellow the actor */
void reajustActorPosition(int32 brickShape);

/** Check collision with actors
	@param actorIx Current process actor index */
int32 checkCollisionWithActors(int32 actorIdx);

/** Check Hero collision with bricks
	@param X Hero X coordinate 
	@param Y Hero Y coordinate 
	@param Z Hero Z coordinate
	@param damageMask Cause damage mask */
void checkHeroCollisionWithBricks(int32 X, int32 Y, int32 Z, int32 damageMask);

/** Check other actor collision with bricks
	@param X Actor X coordinate 
	@param Y Actor Y coordinate 
	@param Z Actor Z coordinate
	@param damageMask Cause damage mask */
void checkActorCollisionWithBricks(int32 X, int32 Y, int32 Z, int32 damageMask);

/** Make actor to stop falling */
void stopFalling();

/** Check extra collision with actors
	@param extra to process
	@param actorIdx actor to check collision */
int32 checkExtraCollisionWithActors(ExtraListStruct* extra, int32 actorIdx);

/** Check extra collision with bricks */
int32 checkExtraCollisionWithBricks(int32 X, int32 Y, int32 Z, int32 oldX, int32 oldY, int32 oldZ);

/** Check extra collision with another extra
	@param extra to process
	@param extraIdx extra index to check collision */
int32 checkExtraCollisionWithExtra(ExtraListStruct* extra, int32 extraIdx);

#endif
