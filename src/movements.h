/** @file movements.h
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

#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include "sys.h"
#include "actor.h"

/** Hero moved */
int16 heroMoved; // twinsenMove
/** Hero Action */
int16 heroAction; // action

/** Process actor X coordinate */
int16 processActorX;
/** Process actor Y coordinate */
int16 processActorY;
/** Process actor Z coordinate */
int16 processActorZ;

/** Previous process actor X coordinate */
int16 previousActorX; // processActorVar2
/** Previous process actor Y coordinate */
int16 previousActorY; // processActorVar3
/** Previous process actor Z coordinate */
int16 previousActorZ; // processActorVar4


/** Get shadow position
	@param X Shadow X coordinate
	@param Y Shadow Y coordinate
	@param Z Shadow Z coordinate */
void get_shadow_position(int32 X, int32 Y, int32 Z);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle
	@param stepAngle number of steps
	@param movePtr time pointer to update */
void set_actor_angle_safe(int16 startAngle, int16 endAngle, int16 stepAngle, ActorMoveStruct * movePtr);

/** Clear actors safe angle
	@param actorPtr actor pointer */
void clear_real_angle(ActorStruct * actorPtr);

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle
	@param stepAngle number of steps
	@param movePtr time pointer to update */
void set_actor_angle(int16 startAngle, int16 endAngle, int16 stepAngle, ActorMoveStruct * movePtr);

/** Get actor real angle
	@param movePtr time pointer to process */
int32 get_real_angle(ActorMoveStruct * movePtr);

/** Get actor angle
	@param movePtr time pointer to process */
int32 get_real_value(ActorMoveStruct * movePtr);

/** Rotate actor with a given angle
	@param X Actor current X coordinate
	@param Z Actor current Z coordinate
	@param angle Actor angle to rotate */
void rotate_actor(int32 X, int32 Z, int32 angle);

/** Move actor around the scene
	@param angleFrom Current actor angle
	@param angleTo Angle to rotate
	@param speed Rotate speed
	@param movePtr Pointer to process movements */
void move_actor(int32 angleFrom, int32 angleTo, int32 speed, ActorMoveStruct *movePtr);

void process_actor_movements(int32 actorIdx);

#endif
