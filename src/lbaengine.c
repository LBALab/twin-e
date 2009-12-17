/** @file lbaengine.c
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

#include "lbaengine.h"
#include "main.h"
#include "sdlengine.h"
#include "images.h"
#include "grid.h"
#include "scene.h"
#include "menu.h"
#include "interface.h"
#include "dialogues.h"
#include "redraw.h"
#include "hqrdepack.h"
#include "resources.h"
#include "renderer.h"
#include "animations.h"

#ifdef GAMEMOD
#include "debug.h"
#endif

int32 isTimeFreezed = 0;
int32 saveFreezedTime = 0;

int32 heroPressedKey;
int32 heroPressedKey2;

int32 loopPressedKey; // mainLoopVar5


void freeze_time() {
	if (!isTimeFreezed)
		saveFreezedTime = lbaTime;
	isTimeFreezed++;
}

void unfreeze_time() {
	--isTimeFreezed;
	if (isTimeFreezed == 0)
		lbaTime = saveFreezedTime;
}

void process_control_mode(int32 actorIdx) {
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

		move_actor(actor->angle, actor->angle + tempAngle, actor->speed, &actor->time);

		heroPressedKey = key;
	} else {
		if (!actor->staticFlags.bIsSpriteActor) {
			if (actor->controlMode != 1) {
				actor->angle = get_real_angle(&actor->time);
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
							actor->angle = get_real_angle(&actor->time);
						}
					}
				}

				if (key & 8) { // turn right
					heroMoved = 1;
					if (actor->anim == 0) {
						init_anim(ANIM_TURNRIGHT, 0, 255, actorIdx);
					} else {
						if (!actor->dynamicFlags.bIsRotationByAnim) {
							actor->angle = get_real_angle(&actor->time);
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

				move_actor(actor->angle, actor->angle + tempAngle, actor->speed, &actor->time);

				heroPressedKey  = key;
				heroPressedKey2 = loopPressedKey;

			}
			break;
			/*default:
				printf("Unimplemented control mode %d\n", actor->controlMode);*/
		}
	}
}

void recenter_screen() {
	if ((loopPressedKey & 2)) { //  && disableScreenRecenter == 0 recenter screen
		newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
		newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
		newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;
		reqBgRedraw = 1;
		//needChangeRoom = 119;
		//needChangeRoom=currentRoom+1;
	}
}

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 run_game_engine() { // mainLoopInteration
	int32 a;
	int16 pKey; // current pressed key
	read_keys();

	key  = pressedKey;
	pKey = skipIntro; // mainLoopVar67
	loopPressedKey = skipedKey;

	if (process_giveup_menu())
		return 0; // give up

	process_options_menu(pKey);

	recenter_screen();

#ifdef GAMEMOD
	process_debug(pKey);
#endif

	if (needChangeScene > -1)
		change_scene();

	for (a = 0; a < sceneNumActors; a++) {
		ActorStruct *actor = &sceneActors[a];

		process_control_mode(a);
		// TODO: process_track_script(a);
		process_actor_animations(a);
	}

	redraw_engine_actions(reqBgRedraw);

	needChangeScene = -1;
	reqBgRedraw = 0;

	return 0;
}

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 game_engine_loop() { // mainLoop
	uint32 start;

	quitGame = 1;
	//requestBackgroundRedraw = 1;
	//lockPalette = 1;
	//setActorAngle(0, -256, 5, &mainLoopVar1);

	while (quitGame) {
		start = SDL_GetTicks();

		while (SDL_GetTicks() < start + cfgfile.Fps) {
			if (run_game_engine())
				return 1;
			SDL_Delay(cfgfile.Fps);
		}
		lbaTime++;
	}
	return 0;
}

void init_engine_vars(int32 save) { // reinitAll
	alphaLight = 896;
	betaLight = 950;
	init_engine_projections();
}

void init_engine_projections() {
	set_ortho_projection(311, 240, 512);
	set_base_translation(0, 0, 0);
	set_base_rotation(0, 0, 0);
	set_light_vector(alphaLight, betaLight, 0);
}

