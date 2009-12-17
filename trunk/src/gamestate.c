/** @file gamestate.c
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

#include "gamestate.h"
#include "scene.h"
#include "redraw.h"
#include "dialogues.h"
#include "menu.h"
#include "renderer.h"
#include "grid.h"
#include "lbaengine.h"
#include "interface.h"
#include "animations.h"
#include "keyboard.h"
#include "resources.h"

/** Initialize main engine variables */
void init_vars() {
	set_camera_position(40, 40, 128, 200, 200);

	needChangeScene = 0;
	reqBgRedraw = 1;
	currentSceneIdx = 0;
	currentTextBank = -1;
	currMenuTextIndex = -1;
	currMenuTextBank = -1;
	isUsingOrhoProjection = 1;

	sceneHero = &sceneActors[0];

	useCellingGrid = -1;
	cellingGridIdx = 0;

	currentlyFollowedActor = 0;

	/*setSomethingVar1 = 320;
	setSomethingVar2 = 200;
	setSomethingVar3 = 500;*/
	// TODO: configure ortho
	orthoProjX = 311;
	orthoProjY = 240;
	orthoProjZ = 512;

	renderLeft = 0;
	renderTop = 0;
	renderRight = SCREEN_TEXTLIMIT_RIGHT;
	renderBottom = SCREEN_TEXTLIMIT_BOTTOM;

	rightMouse = 0;
	leftMouse = 0;

	currentPositionInBodyPtrTab = 0; // TODO: put this in scene.c file under clear_scene()
}

void init_hero_vars() { // TODO: get rid of this here
	//TODO: reset actor 0
	sceneHero->speed = 40;
	// TODO: init in-game hero status var
}

/** Initialize all needed stuffs at first time running engine */
void init_all() {
	blockBuffer = (uint8 *)malloc(204800);  // 204800 = 64*64*25*2

	animBuffer1 = animBuffer2 = (uint8 *)malloc(5000);

	// Init engine keymap
	init_keymap();
	init_resources();
	init_vars();
	init_hero_vars();
}
