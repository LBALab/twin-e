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
#include "text.h"
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
#include "menuoptions.h"

#ifdef GAMEMOD
#include "debug.h"
#endif

int32 isTimeFreezed = 0;
int32 saveFreezedTime = 0;

enum InventoryItems {
	kiHolomap = 0,
	kiMagicBall = 1,
	kiUseSabre = 2,
	kiBookOfBu = 5,
	kiProtoPack = 12,
	kiPinguin = 14,
	kiBonusList = 26,
	kiCloverLeaf = 27
};

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
	read_keys();

	if (needChangeScene > -1) {
		change_scene();
	}

	previousLoopPressedKey = loopPressedKey;
	key  = pressedKey;
	loopPressedKey = skipedKey;
	loopCurrentKey = skipIntro;

#ifdef GAMEMOD
	process_debug(loopCurrentKey);
#endif

	if(showCredits != 0) {
		// TODO: if current music playing != 8, than play_track(8);
		if (skipIntro != 0) {
			return 0;
		}
		if (pressedKey != 0) {
			return 0;
		}
		if (skipedKey != 0) {
			return 0;
		}
	} else {
		// Process give up menu - Press ESC
		if (skipIntro == 1 && sceneHero->life > 0 && sceneHero->entity != -1 && !sceneHero->staticFlags.bIsHidden) {
			freeze_time();
			if (giveup_menu()) {
				unfreeze_time();
				redraw_engine_actions(1);
				freeze_time();
				save_game(); // auto save game
				quitGame = 0;
				cfgfile.Quit = 0;
				unfreeze_time();
				return 0;
			} else {
				unfreeze_time();
				redraw_engine_actions(1);
			}
		}

		// Process options menu - Press F6
		if (loopCurrentKey == 0x40) {
			int tmpLangCD = cfgfile.LanguageCDId;
			freeze_time();
			stop_samples();
			OptionsMenuSettings[5] = 15;
			cfgfile.LanguageCDId = 0;
			init_text_bank(0);
			options_menu();
			cfgfile.LanguageCDId = tmpLangCD;
			init_text_bank(currentTextBank + 3);
			//TODO: play music
			unfreeze_time();
			redraw_engine_actions(1);
		}

		// inventory menu
		loopInventoryItem = -1;
		if (loopCurrentKey == 0x36 && sceneHero->entity != -1 && sceneHero->controlMode == kMANUAL) {
			freeze_time();
			process_inventory_menu();
			
			switch (loopInventoryItem) {
			case kiHolomap:
				printf("Use Inventory [kiHolomap] not implemented!\n");
				break;
			case kiMagicBall:
				if (usingSabre == 1) {
					init_model_actor(0, 0);
				}
				usingSabre = 0;
				break;
			case kiUseSabre:
				if (sceneHero->body == 2) {
					if (heroBehaviour == PROTOPACK) {
						set_behaviour(NORMAL);
					}
					init_model_actor(2, 0);
					init_anim(24, 1, 0, 0);

					usingSabre = 1;
				}
				break;
			case kiBookOfBu: {
				int32 tmpFlagDisplayText;

				fade_2_black(paletteRGBA);
				load_image(RESSHQR_INTROSCREEN1IMG, 1);
				init_text_bank(2);
				newGameVar4 = 0;
				text_clip_full();
				set_font_cross_color(15);
				tmpFlagDisplayText = cfgfile.FlagDisplayText;
				cfgfile.FlagDisplayText = 1;
				draw_text_fullscreen(161);
				cfgfile.FlagDisplayText = tmpFlagDisplayText;
				text_clip_small();
				newGameVar4 = 1;
				init_text_bank(currentTextBank + 3);
				fade_2_black(paletteRGBACustom);
				clear_screen();
				flip();
				set_palette(paletteRGBA);
			}							 
				break;
			case kiProtoPack:
				if (gameFlags[GAMEFLAG_BOOKOFBU]) {
					sceneHero->body = 0;
				} else {
					sceneHero->body = 1;
				}

				if (heroBehaviour == PROTOPACK) {
					set_behaviour(NORMAL);
				} else {
					set_behaviour(PROTOPACK);
				}
				break;
			case kiPinguin:
				printf("Use Inventory [kiPinguin] not implemented!\n");
				break;
			case kiBonusList: {
				int32 tmpLanguageCDIdx;
				tmpLanguageCDIdx = cfgfile.LanguageCDId;
				unfreeze_time();
				redraw_engine_actions(1);
				freeze_time();
				cfgfile.LanguageCDId = 0;
				init_text_bank(2);
				text_clip_full();
				set_font_cross_color(15);
				draw_text_fullscreen(162);
				text_clip_small();
				cfgfile.LanguageCDId = tmpLanguageCDIdx;
				init_text_bank(currentTextBank + 3);
			}
				break;
			case kiCloverLeaf:
				if (sceneHero->life < 50) {
					if (inventoryNumLeafs > 0) {
						sceneHero->life = 50;
						inventoryMagicPoints = magicLevelIdx * 20;
						inventoryNumLeafs--;
						add_overlay(koInventoryItem, 27, 0, 0, 0, koNormal, 3);
					}
				}
				break;
			}


			unfreeze_time();
			redraw_engine_actions(1);
		}

		// Process behaviour menu - Press CTRL
		if ((loopCurrentKey == 0x1D || loopCurrentKey == 0x3B || loopCurrentKey == 0x3C || loopCurrentKey == 0x3D || loopCurrentKey == 0x3E) && sceneHero->entity != -1 && sceneHero->controlMode == kMANUAL) { // F1..F4 Keys
			freeze_time();
			process_behaviour_menu();
			unfreeze_time();
			redraw_engine_actions(1);
		}

		// Process behavior menu - F1..F4 Keys + Interface Style
		if (loopCurrentKey == 0x3B || loopCurrentKey == 0x3C || loopCurrentKey == 0x3D || loopCurrentKey == 0x3E) {
			switch (cfgfile.InterfaceStyle) {
			case 0:
				heroBehaviour = loopCurrentKey - 0x3B;
				freeze_time();
				process_behaviour_menu();
				unfreeze_time();
				redraw_engine_actions(1);
				break;
			case 1:
			case 2: {
			}
				break;
			}
		}
		

		// use Proto-Pack
		if (loopCurrentKey == 0x24 && gameFlags[GAMEFLAG_PROTOPACK] == 1) {
			if (gameFlags[GAMEFLAG_BOOKOFBU]) {
				sceneHero->body = 0;
			} else {
				sceneHero->body = 1;
			}

			if (heroBehaviour == PROTOPACK) {
				set_behaviour(NORMAL);
			} else {
				set_behaviour(PROTOPACK);
			}
		}

		// Press Enter to Recenter Screen
		if ((loopCurrentKey == 0x1C) && disableScreenRecenter == 0) {
			newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
			newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
			newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;
			reqBgRedraw = 1;
		}

		// TODO: draw holomap

		// Process Pause - Press P
		if (loopCurrentKey == 0x19) {
			freeze_time();
			set_font_color(15);
			draw_text(5, 446, "Pause"); // no key for pause in Text Bank
			copy_block_phys(5, 446, 100, 479);
			do {
				read_keys();
				SDL_Delay(10);
			} while (skipIntro != 0x19 && !pressedKey);
			unfreeze_time();
			redraw_engine_actions(1);
		}
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

			if (quitGame != -1) {
				return quitGame;
			}

			if (actor->staticFlags.bCanDrown) {
				int32 brickSound;
				brickSound = get_brick_sound_type(actor->X, actor->Y - 1, actor->Z);
				actor->brickSound = brickSound;

				if ((brickSound & 0xF0) == 0xF0) {
					if ((brickSound & 0xF) == 1) {
						if (a) { // all other actors
							int32 rnd = Rnd(2000) + 3096;
							play_sample(0x25, rnd, 1, actor->X, actor->Y, actor->Z);
							if (actor->bonusParameter & 0x1F0) {
								if (!(actor->bonusParameter & 1)) {
									process_actor_extra_bonus(a);
								}
								actor->life = 0;
							}
						} else { // if Hero
							if (heroBehaviour != 4 || (brickSound & 0x0F) != actor->anim) {
								if (!cropBottomScreen)
								{
									init_anim(ANIM_DRAWN, 4, 0, 0);
									project_position_on_screen(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ);
									cropBottomScreen = projPosY;
								}
								project_position_on_screen(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ);
								actor->controlMode = 0;
								actor->life = -1;
								cropBottomScreen = projPosY;
								actor->staticFlags.bCanDrown |= 0x10;
							}
						}
					}
				}
			}

			if (actor->life <= 0) {
				if (!a) { // if its Hero
					if (actor->dynamicFlags.bAnimEnded) {
						if (inventoryNumLeafs > 0) { // use clover leaf automaticaly
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
							cropBottomScreen = 0;
						} else { // game over
							inventoryNumLeafsBox = 2;
							inventoryNumLeafs = 1;
							inventoryMagicPoints = magicLevelIdx * 20;
							heroBehaviour = previousHeroBehaviour;
							actor->angle  = previousHeroAngle;
							actor->life = 50;

							if (previousSceneIdx != currentSceneIdx) {
								newHeroX = -1;
								newHeroY = -1;
								newHeroZ = -1;
								currentSceneIdx = previousSceneIdx;
							}

							save_game();
							process_gameover_animation();
							quitGame = 0;
							return 0;
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

	// workaround to fix hero redraw after drowning
	if(cropBottomScreen && reqBgRedraw == 1) {
		sceneHero->staticFlags.bIsHidden = 1;
		redraw_engine_actions(1);
		sceneHero->staticFlags.bIsHidden = 0;
	}

	needChangeScene = -1;
	reqBgRedraw = 0;

	return 0;
}

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 game_engine_loop() { // mainLoop
	uint32 start;

	reqBgRedraw = 1;
	// TODO lockPalette = 1;
	set_actor_angle(0, -256, 5, &loopMovePtr);

	while (quitGame == -1) {
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
