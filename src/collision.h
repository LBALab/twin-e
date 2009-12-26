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
int32 standing_on_actor(int32 actorIdx1, int32 actorIdx2);

/** Reajust actor position in scene according with brick shape bellow actor
	@param brickShape Shape of brick bellow the actor */
void reajust_actor_position(int32 brickShape);

/** Check collision with actors
	@param actorIx Current process actor index */
int32 check_collision_with_actors(int32 actorIdx);

#endif
