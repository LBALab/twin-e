/** @file redraw.h
	@brief
	This file contains engine redraw actions routines

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

#ifndef REDRAW_H
#define REDRAW_H

#include "sys.h"

#define OVERLAY_MAX_ENTRIES		10

/** Auxiliar object render left position on screen */
int32 renderLeft;
/** Auxiliar object render right position on screen */
int32 renderRight;
/** Auxiliar object render top position on screen */
int32 renderTop;
/** Auxiliar object render bottom position on screen */
int32 renderBottom;

//short int drawInGameTransBox;

/** Request background redraw */
int16 reqBgRedraw;

enum OverlayType {
	koSprite = 0,
	koNumber = 1,
	koNumberRange = 2,
	ko3Dmodel = 3,
	koText = 4
};

enum OverlayPosType {
	koNormal = 0,
	koFollowActor = 1
};

/** Overlay list structure */
typedef struct OverlayListStruct {
	int16 type;
	int16 info0; // sprite/3d model entry | number | number range
	int16 X;
	int16 Y;
	int16 info1; // followed actor | total coins
	int16 posType;
	int16 lifeTime;
} OverlayListStruct;

OverlayListStruct overlayList[OVERLAY_MAX_ENTRIES];

/** This is responsible for the entire game screen redraw
	@param bgRedraw true if we want to redraw background grid, false if we want to update certain screen areas */
void redraw_engine_actions(int32 bgRedraw);

#endif
