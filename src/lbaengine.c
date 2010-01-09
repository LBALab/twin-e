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
#include "debug.grid.h"
#include "scene.h"
#include "menu.h"
#include "interface.h"
#include "dialogues.h"
#include "redraw.h"
#include "hqrdepack.h"
#include "resources.h"
#include "renderer.h"
#include "animations.h"
#include "movements.h"
#include "keyboard.h"
#include "gamestate.h"
#include "sound.h"
#include "script.life.h"
#include "script.move.h"
#include "extra.h"

#ifdef GAMEMOD
#include "debug.h"
#endif

int32 isTimeFreezed = 0;
int32 saveFreezedTime = 0;

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

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 run_game_engine() { // mainLoopInteration
	int32 a;
	int16 pKey; // current pressed key
	read_keys();

	if (needChangeScene > -1) {
		change_scene();
	}

	previousLoopPressedKey = loopPressedKey;
	key  = pressedKey;
	pKey = skipIntro; // mainLoopVar7
	loopPressedKey = skipedKey;

#ifdef GAMEMOD
	process_debug(pKey);
#endif

	if (process_giveup_menu())
		return 0; // give up

	process_options_menu(pKey);

	// TODO: inventory menu
	
	// Show behaviour menu
	if (loopPressedKey & 4 && sceneHero->entity != -1 && sceneHero->controlMode == kMANUAL) {
		freeze_time();
		process_behaviour_menu();
		redraw_engine_actions(1);
		unfreeze_time();
	}

	// TODO: behaviour menu (LBA2 style)
	// TODO: use Proto-Pack

	// Press Enter to Recenter Screen
	if ((loopPressedKey & 2) && disableScreenRecenter == 0) {
		newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
		newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
		newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;
		reqBgRedraw = 1;
	}

	// TODO: draw holomap

	// Press Pause
	if (pKey == 0x19) {
		freeze_time();
		set_font_color(15);
		display_dialogue_text(5, 446, "Pause"); // no key for pause in Text Bank
		copy_block_phys(5, 446, 100, 479);
		do {
			read_keys();
			SDL_Delay(10);
		} while (skipIntro != 0x19 && !pressedKey);
		copy_screen(workVideoBuffer, frontVideoBuffer);
		flip(workVideoBuffer);
		unfreeze_time();
	}

	loopActorStep = get_real_value(&loopMovePtr);
	if (!loopActorStep) {
		loopActorStep = 1;
	}

	set_actor_angle(0, -256, 5, &loopMovePtr);
	disableScreenRecenter = 0;

	process_environment_sound();

	// Reset HitBy state
	for (a = 0; a < sceneNumActors; a++)
    {
        sceneActors[a].hitBy = -1;
    }

	process_extras();

	for (a = 0; a < sceneNumActors; a++) {
		ActorStruct *actor = &sceneActors[a];

		if (!actor->dynamicFlags.bIsDead) {
			if (actor->life == 0) {
				if (a == 0) { // if its hero who died
					init_anim(ANIM_LANDDEATH, 4, 0, 0);
					actor->controlMode = 0;
				} else {
					play_sample(37, Rnd(2000) + 3096, 1, actor->X, actor->Y, actor->Z);

					if (a == mecaPinguinIdx) {
						// TODO: explode extra	
					}
				}
				
				if (actor->bonusParameter & 0x1F0 && !(actor->bonusParameter & 1)) {
					process_actor_extra_bonus(a);
				}
			}

			process_actor_movements(a);

			actor->collisionX = actor->X;
			actor->collisionY = actor->Y;
			actor->collisionZ = actor->Z;

			if (actor->positionInMoveScript != -1) {
				process_move_script(a);
			}

			process_actor_animations(a);

			if (actor->staticFlags.bIsZonable) {
				process_actor_zones(a);
			}

			if (actor->positionInLifeScript != -1) {
				process_life_script(a);
			}

			if (quitGame == -1) {
				return -1;
			}

			if (actor->staticFlags.bCanDrown) {
				actor->brickSound = get_brick_sound_type(actor->X, actor->Y, actor->Z);
				// TODO process_actor_drown(a)
			}

			if (actor->life <= 0) {
				if (!a) { // if its Hero
					if (actor->dynamicFlags.bAnimEnded) {
						if (inventoryNumLeafsBox > 0) { // use clover leaf automaticaly
							sceneHero->X = newHeroX;
							sceneHero->Y = newHeroY;
							sceneHero->Z = newHeroZ;

							needChangeScene = currentSceneIdx;
							inventoryMagicPoints = magicLevelIdx * 20;

							newCameraX = (sceneHero->X >> 9);
							newCameraY = (sceneHero->Y >> 8);
							newCameraZ = (sceneHero->Z >> 9);

							heroPositionType = POSITION_TYPE_REBORN;

							sceneHero->life = 50;
							reqBgRedraw = 1;
							// TODO: lockPalette = 1;
							inventoryNumLeafs--;
						} else { // game over
							inventoryNumLeafsBox = 2;
							inventoryNumLeafs = 1;
							inventoryMagicPoints = magicLevelIdx * 20; // TODO: recheck this
							heroBehaviour = previousHeroBehaviour;
							actor->angle  = previousHeroAngle;
							actor->life = 50;

							if (previousSceneIdx != currentSceneIdx) {
								newHeroX = -1;
								newHeroY = -1;
								newHeroZ = -1;
								currentSceneIdx = previousSceneIdx;
							}

							// TODO: save game
							// TODO: show game over animation
						}
					}
				} else {
					process_actor_carrier(a);
					actor->dynamicFlags.bIsDead = 1;
					actor->entity = -1;
					actor->zone = -1;
				}
			}

			if (needChangeScene != -1) {
				return 0;
			}
		}
	}

	// recenter screen automatically
	if (!disableScreenRecenter && !useFreeCamera) {
		ActorStruct *actor = &sceneActors[currentlyFollowedActor];
		project_position_on_screen(actor->X - (newCameraX << 9),
								   actor->Y - (newCameraY << 8),
								   actor->Z - (newCameraZ << 9));
		if (projPosX < 80 || projPosX > 539 || projPosY < 80 || projPosY > 429) {
			newCameraX = ((actor->X + 0x100) >> 9) + (((actor->X + 0x100) >> 9) - newCameraX) / 2;
			newCameraY = actor->Y >> 8;
			newCameraZ = ((actor->Z + 0x100) >> 9) + (((actor->Z + 0x100) >> 9) - newCameraZ) / 2;

			if(newCameraX >= 64) {
				newCameraX = 63;
			}

			if(newCameraZ >= 64) {
				newCameraZ = 63;
			}

			reqBgRedraw = 1;
		}
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
	reqBgRedraw = 1;
	// TODO lockPalette = 1;
	set_actor_angle(0, -256, 5, &loopMovePtr);

	while (quitGame) {
		start = SDL_GetTicks();

		while (SDL_GetTicks() < start + cfgfile.Fps) {
			if (run_game_engine())
				return 1;
			SDL_Delay(1);
		}
		lbaTime++;
	}
	return 0;
}
