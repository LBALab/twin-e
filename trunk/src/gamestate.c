/** @file gamestate.c
	@brief
	This file contains game state routines

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
#include "text.h"
#include "menu.h"
#include "renderer.h"
#include "grid.h"
#include "lbaengine.h"
#include "interface.h"
#include "animations.h"
#include "keyboard.h"
#include "resources.h"
#include "extra.h"
#include "sound.h"
#include "images.h"
#include "music.h"
#include "filereader.h"
#include "menuoptions.h"

#define SAVE_DIR "save//"

int32 magicLevelStrengthOfHit[] = {
	MAGIC_STRENGTH_NONE,
	MAGIC_STRENGTH_YELLOW_BALL,
	MAGIC_STRENGTH_GREEN_BALL,
	MAGIC_STRENGTH_RED_BALL,
	MAGIC_STRENGTH_FIRE_BALL,
	0
};

/** Initialize engine 3D projections */
void init_engine_projections() { // reinitAll1
	set_ortho_projection(311, 240, 512);
	set_base_translation(0, 0, 0);
	set_base_rotation(0, 0, 0);
	set_light_vector(alphaLight, betaLight, 0);
}

/** Initialize variables */
void init_scene_vars() {
	int32 i;

	reset_extras();
	
	for (i = 0; i < OVERLAY_MAX_ENTRIES; i++) {
		overlayList[i].info0 = -1;
	}

	for (i = 0; i < NUM_SCENES_FLAGS; i++) {
		sceneFlags[i] = 0;
	}

	for (i = 0; i < NUM_GAME_FLAGS; i++) {
		gameFlags[i] = 0;
	}

	for (i = 0; i < NUM_INVENTORY_ITEMS; i++) {
		inventoryFlags[i] = 0;
	}

	sampleAmbiance[0] = -1;
	sampleAmbiance[1] = -1;
	sampleAmbiance[2] = -1;
	sampleAmbiance[3] = -1;

	sampleRepeat[0] = 0;
	sampleRepeat[1] = 0;
	sampleRepeat[2] = 0;
	sampleRepeat[3] = 0;

	sampleRound[0] = 0;
	sampleRound[1] = 0;
	sampleRound[2] = 0;
	sampleRound[3] = 0;

	for (i = 0; i < 150; i++) {
		holomapFlags[i] = 0;
	}

	sceneNumActors = 0;
	sceneNumZones  = 0;
	sceneNumTracks = 0;

	currentPositionInBodyPtrTab = 0;
}

void init_hero_vars() { // reinitAll3
	reset_actor(0); // reset Hero

	magicBallIdx = -1;

	inventoryNumLeafsBox = 2;
	inventoryNumLeafs    = 2;
	inventoryNumKashes   = 0;
	inventoryNumKeys     = 0;
	inventoryMagicPoints = 0;

	usingSabre = 0;

	sceneHero->body = 0;
	sceneHero->life = 50;
	sceneHero->talkColor = 4;
}

/** Initialize all engine variables */
void init_engine_vars(int32 save) { // reinitAll
	reset_clip();

	alphaLight = 896;
	betaLight = 950;
	init_engine_projections();
	init_scene_vars();
	init_hero_vars();
	
	newHeroX = 0x2000;
	newHeroY = 0x1800;
	newHeroZ = 0x2000;

	currentSceneIdx = -1;
	needChangeScene = 0;
	quitGame = -1;
	mecaPinguinIdx = -1;
	showCredits = 0;
		
	inventoryNumLeafs = 0;
	inventoryNumLeafsBox = 2;
	inventoryMagicPoints = 0;
	inventoryNumKashes = 0;
	inventoryNumKeys = 0;
	inventoryNumGas = 0;

	magicLevelIdx = 0;
	usingSabre = 0;

	gameChapter = 0;

	currentTextBank = 0;
	currentlyFollowedActor = 0;
	heroBehaviour = 0;
	previousHeroAngle = 0;
	previousHeroBehaviour = 0;

	if (save == -1) {
		load_game();
		if (newHeroX == -1) {
			heroPositionType = POSITION_TYPE_NONE;	
		}
	}
}

void load_game() {
	FileReader fr;
	uint8 data;
	int8* namePtr;

	if (!fropen(&fr, SAVE_DIR "S9999.LBA", "rb")) {
		printf("Can't load S9999.LBA saved game!\n");
		return;
	}

	namePtr = savePlayerName;

	frread(&fr, &data, 1); // save game id

	do {
		frread(&fr, &data, 1); // get save player name characters
		*(namePtr++) = data;
	} while (data);

	frread(&fr, &data, 1); // number of game flags, always 0xFF
	frread(&fr, gameFlags, data);
	frread(&fr, &needChangeScene, 1); // scene index
	frread(&fr, &gameChapter, 1);

	frread(&fr, &heroBehaviour, 1);
	previousHeroBehaviour = heroBehaviour;
	frread(&fr, &sceneHero->life, 1);
	frread(&fr, &inventoryNumKashes, 2);
	frread(&fr, &magicLevelIdx, 1);
	frread(&fr, &inventoryMagicPoints, 1);
	frread(&fr, &inventoryNumLeafsBox, 1);
	frread(&fr, &newHeroX, 2);
	frread(&fr, &newHeroY, 2);
	frread(&fr, &newHeroZ, 2);
	frread(&fr, &sceneHero->angle, 2);
	previousHeroAngle = sceneHero->angle;
	frread(&fr, &sceneHero->body, 1);

	frread(&fr, &data, 1); // number of holomap locations, always 0x96
	frread(&fr, holomapFlags, data);

	frread(&fr, &inventoryNumGas, 1);

	frread(&fr, &data, 1); // number of used inventory items, always 0x1C
	frread(&fr, inventoryFlags, data);

	frread(&fr, &inventoryNumLeafs, 1);
	frread(&fr, &usingSabre, 1);

	frclose(&fr);

	currentSceneIdx = -1;
	heroPositionType = POSITION_TYPE_REBORN;
}

void save_game() {
	FileReader fr;
	int8 data;

	if (!fropen(&fr, SAVE_DIR "S9999.LBA", "wb+")) {
		printf("Can't save S9999.LBA saved game!\n");
		return;
	}

	data = 0x03;
	frwrite(&fr, &data, 1, 1);

	data = 0x00;
	frwrite(&fr, "PrequengineSave", 16, 1);

	data = 0xFF; // number of game flags
	frwrite(&fr, &data, 1, 1);
	frwrite(&fr, gameFlags, 255, 1);

	frwrite(&fr, &currentSceneIdx, 1, 1);
	frwrite(&fr, &gameChapter, 1, 1);
	frwrite(&fr, &heroBehaviour, 1, 1);
	frwrite(&fr, &sceneHero->life, 1, 1);
	frwrite(&fr, &inventoryNumKashes, 2, 1);
	frwrite(&fr, &magicLevelIdx, 1, 1);
	frwrite(&fr, &inventoryMagicPoints, 1, 1);
	frwrite(&fr, &inventoryNumLeafsBox, 1, 1);
	frwrite(&fr, &newHeroX, 2, 1);
	frwrite(&fr, &newHeroY, 2, 1);
	frwrite(&fr, &newHeroZ, 2, 1);
	frwrite(&fr, &sceneHero->angle, 2, 1);
	frwrite(&fr, &sceneHero->body, 1, 1);

	data = 0x96; // number of holomap locations
	frwrite(&fr, &data, 1, 1);
	frwrite(&fr, holomapFlags, 150, 1);

	frwrite(&fr, &inventoryNumGas, 1, 1);

	data = 0x1C; // number of inventory items
	frwrite(&fr, &data, 1, 1);
	frwrite(&fr, inventoryFlags, 28, 1);

	frwrite(&fr, &inventoryNumLeafs, 1, 1);
	frwrite(&fr, &usingSabre, 1, 1);

	frclose(&fr);
}

void process_found_item(int32 item) {
	int32 itemCameraX, itemCameraY, itemCameraZ; // objectXYZ
	int32 itemX, itemY, itemZ; // object2XYZ
	int32 boxTopLeftX, boxTopLeftY, boxBottomRightX, boxBottomRightY;
	int32 textState, quitItem, currentAnimState;
	uint8 *currentAnim;
	AnimTimerDataStruct tmpAnimTimer;

	newCameraX = (sceneHero->X + 0x100) >> 9;
	newCameraY = (sceneHero->Y + 0x100) >> 8;
	newCameraZ = (sceneHero->Z + 0x100) >> 9;

	// Hide hero in scene
	sceneHero->staticFlags.bIsHidden = 1;
	redraw_engine_actions(1);
	sceneHero->staticFlags.bIsHidden = 0;

	copy_screen(frontVideoBuffer, workVideoBuffer);

	itemCameraX = newCameraX << 9;
	itemCameraY = newCameraY << 8;
	itemCameraZ = newCameraZ << 9;

	render_iso_model(sceneHero->X - itemCameraX, sceneHero->Y - itemCameraY, sceneHero->Z - itemCameraZ, 0, 0x80, 0, bodyTable[sceneHero->entity]);
	set_clip(renderLeft, renderTop, renderRight, renderBottom);

	itemX = (sceneHero->X + 0x100) >> 9;
	itemY = sceneHero->Y >> 8;
	if (sceneHero->brickShape & 0x7F) {
		itemY++;
	}
	itemZ = (sceneHero->Z + 0x100) >> 9;

	draw_over_model_actor(itemX, itemY, itemZ);
	flip();

	project_position_on_screen(sceneHero->X - itemCameraX, sceneHero->Y - itemCameraY, sceneHero->Z - itemCameraZ);
	projPosY -= 150;

	boxTopLeftX = projPosX - 65;
	boxTopLeftY = projPosY - 65;

	boxBottomRightX = projPosX + 65;
	boxBottomRightY = projPosY + 65;

	play_sample(41, 0x1000, 1, 0x80, 0x80, 0x80);

	// TODO: process vox play
	{
		int32 tmpLanguageCDId;
		stop_music();
		tmpLanguageCDId = cfgfile.LanguageCDId;
		cfgfile.LanguageCDId = 0;
		init_text_bank(2);
		cfgfile.LanguageCDId = tmpLanguageCDId;
	}

	reset_clip();
	init_text(item);
	init_dialogue_box();

	textState = 1;
	quitItem = 0;

	// TODO: process vox play

	currentAnim = animTable[get_body_anim_index(ANIM_FOUND_ITEM, 0)];

	tmpAnimTimer = sceneHero->animTimerData;
	
	animBuffer2 += stock_animation(animBuffer2, bodyTable[sceneHero->entity], &sceneHero->animTimerData);
	if (animBuffer1 + 4488 < animBuffer2) {
		animBuffer2 = animBuffer1;
	}

	currentAnimState = 0;

	prepare_iso_model(inventoryTable[item]);
	numOfRedrawBox = 0;

	while (!quitItem) {
		reset_clip();
		currNumOfRedrawBox = 0;
		blit_background_areas();
		draw_transparent_box(boxTopLeftX, boxTopLeftY, boxBottomRightX, boxBottomRightY, 4);

		set_clip(boxTopLeftX, boxTopLeftY, boxBottomRightX, boxBottomRightY);

		itemAngle[item] += 8;

		render_inventory_item(projPosX, projPosY, inventoryTable[item], itemAngle[item], 10000);

		draw_box(boxTopLeftX, boxTopLeftY, boxBottomRightX, boxBottomRightY);
		add_redraw_area(boxTopLeftX, boxTopLeftY, boxBottomRightX, boxBottomRightY);
		reset_clip();
		init_engine_projections();

		if (set_model_animation(currentAnimState, currentAnim, bodyTable[sceneHero->entity], &sceneHero->animTimerData)) {
			currentAnimState++; // keyframe
			if (currentAnimState >= get_num_keyframes(currentAnim)) {
				currentAnimState = get_start_keyframe(currentAnim);
			}
		}

		render_iso_model(sceneHero->X - itemCameraX, sceneHero->Y - itemCameraY, sceneHero->Z - itemCameraZ, 0, 0x80, 0, bodyTable[sceneHero->entity]);
		set_clip(renderLeft, renderTop, renderRight, renderBottom);
		draw_over_model_actor(itemX, itemY, itemZ);
		add_redraw_area(renderLeft, renderTop, renderRight, renderBottom);

		if (textState) {
			reset_clip();
			textState = printText10();
		}

		if (textState == 0 || textState == 2) {
			delay(15);
		}

		flip_redraw_areas();
		
		read_keys();
		if (skipedKey) {
			if (!textState) {
				quitItem = 1;
			}

			if (textState == 2) {
				textState = 1;
			}
		}

		lbaTime++;
	}

	// TODO: process vox play
	/*{
		while (printText11()) {
			read_keys();
			if (skipIntro == 1) {
				break;
			}
			delay(1);
		}
	}*/

	init_engine_projections();
	init_text_bank(currentTextBank + 3);

	/*do {
		read_keys();
		delay(1);
	} while (!skipIntro);*/

	sceneHero->animTimerData = tmpAnimTimer;
}

void process_game_choices(int32 choiceIdx) {
	int32 i;
	copy_screen(frontVideoBuffer, workVideoBuffer);

	gameChoicesSettings[0] = 0;	// Current loaded button (button number)
	gameChoicesSettings[1] = numChoices; // Num of buttons
	gameChoicesSettings[2] = 0; // Buttons box height
	gameChoicesSettings[3] = currentTextBank + 3;

	if (numChoices > 0) {
		for(i = 0; i < numChoices; i++) {
			gameChoicesSettings[i * 2 + 4] = 0;
			gameChoicesSettings[i * 2 + 5] = gameChoices[i];
		}
	}

	draw_ask_question(choiceIdx);

	process_menu(gameChoicesSettings);
	choiceAnswer = gameChoices[gameChoicesSettings[0]];

	// TODO: process vox play
}
