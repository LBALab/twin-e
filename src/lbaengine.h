/** @file lbaengine.h
	@brief 
	This file contains the main game engine routines
	
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


#ifndef GAMEENGINE_H
#define GAMEENGINE_H

int quitGame;
volatile int lbaTime;
/** Request background redraw */
short int reqBgRedraw;

short int projPosXScreen; // fullRedrawVar1
short int projPosYScreen; // fullRedrawVar2
short int projPosZScreen; // fullRedrawVar3
short int projPosX;
short int projPosY;
short int projPosZ;

short int currentlyFollowedActor;

unsigned char gameFlags[256];

void freeze_time();
void unfreeze_time();

int game_engine_loop();

void init_engine_vars(int save);
void init_engine_projections();

#endif
