/** @file menu.c
	@brief
	This file contains main menu create and processing routines.

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

#include "sys.h"
#include "main.h"
#include "menu.h"
#include "menuoptions.h"
#include "resources.h"
#include "music.h"
#include "sound.h"
#include "images.h"
#include "sdlengine.h"
#include "hqrdepack.h"
#include "lbaengine.h"
#include "dialogues.h"
#include "interface.h"
#include "redraw.h"
#include "keyboard.h"
#include "scene.h"
#include "animations.h"
#include "actor.h"
#include "movements.h"
#include "gamestate.h"
#include "renderer.h"
#include "grid.h"
#include "gamestate.h"

/** Plasma effect file size: RESS.hqr:51 */
#define PLASMA_EFFECT_FILESIZE	262176

/** Main menu new game option key */
#define MAINMENU_NEWGAME		20
/** Main menu continue game option key */
#define MAINMENU_CONTINUEGAME	21
/** Main menu option key */
#define MAINMENU_OPTIONS		23
/** Main menu quit key */
#define MAINMENU_QUIT			22

/** Give up menu continue game key */
#define GIVEUPMENU_CONTINUE		28
/** give up menu quit game key */
#define GIVEUPMENU_QUIT			27

/** options menu return game key */
#define OPTIONSMENU_RETURNGAME		15
/** options menu return menu key */
#define OPTIONSMENU_RETURNMENU		26
/** options menu volume key */
#define OPTIONSMENU_VOLUME			30
/** options menu save game management key */
#define OPTIONSMENU_SAVEMANAGE		46
/** options menu advanced options key */
#define OPTIONSMENU_ADVOPTIONS		47

/** Main menu background image number

	Used when returning from credit sequence to redraw the main menu background image */
#define MAINMENU_BKGIMAGE		9999
/** Menu buttons width */
#define MAINMENU_BUTTONWIDTH	320
/** Used to calculate the spanning between button and screen */
#define MAINMENU_BUTTONSPAN		550

/** Music volume button key */
#define MUSICVOLUME				1
/** Sound effects volume button key */
#define SOUNDVOLUME				2
/** CD volume button key */
#define CDVOLUME				3
/** Line-in volume button key */
#define LINEVOLUME				4
/** Master volume button key */
#define MASTERVOLUME			5

/** Main Menu Settings

	Used to create the game main menu. */
int16 MainMenuSettings[] = {
	0,          // Current loaded button (button number)
	4,          // Num of buttons
	200,        // Buttons box height ( is used to calc the height where the first button will appear )
	0,          // unused
	0,
	20,         // new game
	0,
	21,         // continue game
	0,
	23,         // options
	0,
	22,         // quit
};

/** Give Up Menu Settings

	Used to create the in-game menu. */
int16 GiveUpMenuSettings[] = {
	0,			// Current loaded button (button number)
	2,			// Num of buttons
	240,		// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	28,			// continue game
	0,
	27,			// quit game
};

/** Give Up Menu Settings

	Used to create the in-game menu. This menu have one extra item to save the game */
int16 GiveUpMenuSettingsWithSave[] = {
	0,			// Current loaded button (button number)
	3,			// Num of buttons
	240,		// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	28,			// continue game
	0,
	16,			// save game
	0,
	27,			// quit game
};

/** Options Menu Settings

	Used to create the options menu. */
int16 OptionsMenuSettings[] = {
	0,			// Current loaded button (button number)
	4,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	24,			// return to previous menu
	0,
	30,			// volume settings
	0,
	46,			// save game management
	0,
	47,			// advanced options
};

/** Advanced Options Menu Settings

	Used to create the advanced options menu. */
int16 AdvOptionsMenuSettings[] = {
	0,			// Current loaded button (button number)
	5,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	0,
	4,			// aggressive mode (manual|auto)
	6,
	31,			// Polygon detail (full|medium|low)
	7,
	32,			// Shadows (all|character|no)
	8,
	33,			// scenary zoon (on|off)
};

/** Save Game Management Menu Settings

	Used to create the save game management menu. */
int16 SaveManageMenuSettings[] = {
	0,			// Current loaded button (button number)
	3,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	0,
	41,			// copy saved game
	0,
	45,			// delete saved game
};

/** Volume Menu Settings

	Used to create the volume menu. */
int16 VolumeMenuSettings[] = {
	0,			// Current loaded button (button number)
	7,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	1,
	10,			// music volume
	2,
	11,			// sfx volume
	3,
	12,			// cd volume
	4,
	13,			// line-in volume
	5,
	14,			// master volume
	0,
	16,			// save parameters
};


/** Plasma Effect pointer to file content: RESS.HQR:51 */
uint8 *plasmaEffectPtr;
/** Plasma Effect variables 1 */
uint8 *plasmaEffectVar1;
/** Plasma Effect variables 2 */
uint8 *plasmaEffectVar2;

/** Hero behaviour menu entity */
uint8 *behaviourEntity;
/** Behaviour menu anim state */
int16 behaviourAnimState[4]; // winTab
/** Behaviour menu anim data pointer */
AnimTimerDataStruct behaviourAnimData[4];

/** Plasma Effect Initialization */
void plasma_effect_init() {
	uint8  *temp1 = 0;
	uint8  *temp2 = 0;
	int8   *temp2bis = 0;
	int16  temp4 = 0;
	int32  i = 0;
	uint32 *copy1 = 0;
	uint32 *copy2 = 0;

	plasmaEffectVar1 = plasmaEffectPtr;
	plasmaEffectVar2 = plasmaEffectPtr + 16000;

	temp1 = plasmaEffectVar1 + 321; // where start first line
	temp2 = plasmaEffectVar2 + 321; // where start first line

	for (i = 0; i < 15358; i++) { // for all the lines except the last one
		temp4 = 0;

		temp4 += temp1[-1];
		temp4 += temp1[-320];
		temp4 += temp1[-319];
		temp4 += temp1[-321];
		temp4 += temp1[+1];
		temp4 += temp1[+320];
		temp4 += temp1[+319];
		temp4 += temp1[+321];

		temp4 = (temp4 >> 3) | ((temp4 & 0x0003) << 13);

		if (!(temp4 & 0x6500)) {
			if (temp2 >= (plasmaEffectVar2 + 14720)) {
				temp4--;
			} else if (temp4 > 0) {
				temp4--;
			}
		}

		temp1++;
		*temp2 = (uint8) temp4;
		temp2++;
	}

	copy1 = (uint32 *) plasmaEffectVar1;
	copy2 = (uint32 *)(plasmaEffectVar2 + 320);   // pass to the next line

	for (i = 0; i < 3840; i++)
		*(copy1++) = *(copy2++);  // copy current frame buffer with 1 line

	temp2bis = (int8 *) plasmaEffectVar2 + 12480; // pointer with 11 last lines

	for (i = 1600; i >= 0; i--) { // prepare next frame
		temp4 = *temp2bis;
		if (temp4 <= 15) {
			*temp2bis = -(temp4 - 11);
		}
		temp2++;
	}
}

/** Process the plasma effect
	@param top top height where the effect will be draw in the front buffer
	@param color plasma effect start color */
void process_plasma_effect(int32 top, int32 color) {
	uint8 *temp;
	uint8 *out;
	int32 i, j;
	uint8 temp3;
	uint8 bh, bl;

	plasma_effect_init();

	temp = plasmaEffectVar1 + 1600;
	out = frontVideoBuffer + screenLockupTable[top];

	bl = color;
	bh = bl + 15;

	for (i = 25; i >= 0; i--) {
		for (j = MAINMENU_BUTTONWIDTH; j >= 0; j--) {
			temp3 = *temp;
			temp3 = temp3 >> 1;
			temp3 += bl;
			if (temp3 > bh)
				temp3 = bh;

			out[0] = temp3;
			out[1] = temp3;
			out[640] = temp3;
			out[641] = temp3;

			temp++;
			out += 2;
		}
		out += 640; //SCREEN_WIDTH;
	}
}

/** Draw the entire button box
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button */
void draw_box(int32 left, int32 top, int32 right, int32 bottom) {
	draw_line(left, top, right, top, 79);			// top line
	draw_line(left, top, left, bottom, 79);			// left line
	draw_line(right, ++top, right, bottom, 73);		// right line
	draw_line(++left, bottom, right, bottom, 73);	// bottom line
}

/** Draws main menu button
	@param width menu button width
	@param topheight is the height between the top of the screen and the first button
	@param id current button identification from menu settings
	@param value current button key pressed value
	@param mode flag to know if should draw as a hover button or not */
void draw_button_gfx(int32 width, int32 topheight, int32 id, int32 value, int32 mode) {
	int32 right;
	int32 top;
	int32 left;
	int32 bottom2;
	int32 bottom;
	/*
	 * int CDvolumeRemaped; int musicVolumeRemaped; int masterVolumeRemaped; int lineVolumeRemaped;
	 * int waveVolumeRemaped;
	 */

	int8 dialText[256];
	int32 textSize;

	left = width - MAINMENU_BUTTONSPAN / 2;
	right = width + MAINMENU_BUTTONSPAN / 2;

	// topheigh is the center Y pos of the button
	top = topheight - 25;		// this makes the button be 50 height
	bottom = bottom2 = topheight + 25;	// ||

	if (mode != 0) {
		if (id <= 5 && id >= 1) {
			int32 newWidth = 0;

			switch (id) {
			case 1: {
				if (cfgfile.MusicVolume > 255)
					cfgfile.MusicVolume = 255;
				if (cfgfile.MusicVolume < 0)
					cfgfile.MusicVolume = 0;
				newWidth = cross_dot(left, right, 255, cfgfile.MusicVolume);
				break;
			}
			case 2: {
				if (cfgfile.WaveVolume > 255)
					cfgfile.WaveVolume = 255;
				if (cfgfile.WaveVolume < 0)
					cfgfile.WaveVolume = 0;
				newWidth = cross_dot(left, right, 255, cfgfile.WaveVolume);
				break;
			}
			case 3: {
				if (cfgfile.CDVolume > 255)
					cfgfile.CDVolume = 255;
				if (cfgfile.CDVolume < 0)
					cfgfile.CDVolume = 0;
				newWidth = cross_dot(left, right, 255, cfgfile.CDVolume);
				break;
			}
			case 4: {
				if (cfgfile.LineVolume > 255)
					cfgfile.LineVolume = 255;
				if (cfgfile.LineVolume < 0)
					cfgfile.LineVolume = 0;
				newWidth = cross_dot(left, right, 255, cfgfile.LineVolume);
				break;
			}
			case 5: {
				if (cfgfile.MasterVolume > 255)
					cfgfile.MasterVolume = 255;
				if (cfgfile.MasterVolume < 0)
					cfgfile.MasterVolume = 0;
				newWidth = cross_dot(left, right, 255, cfgfile.MasterVolume);
				break;
			}
			};

			process_plasma_effect(top, 80);
			if (!(rand() % 5)) {
				plasmaEffectPtr[rand() % 140 * 10 + 1900] = 255;
			}
			draw_splitted_box(newWidth, top, right, bottom, 68);
		} else {
			process_plasma_effect(top, 64);
			if (!(rand() % 5)) {
				plasmaEffectPtr[rand() % 320 * 10 + 6400] = 255;
			}
		}

		if (id <= 5 && id >= 1) {
			// implement this
		}
	} else {
		blit_box(left, top, right, bottom, (int8 *) workVideoBuffer, left, top, (int8 *) frontVideoBuffer);
		draw_transparent_box(left, top, right, bottom2, 4);
	}

	draw_box(left, top, right, bottom);

	set_font_color(15);
	get_menu_text(value, dialText);
	textSize = dialogue_text_size(dialText);
	display_dialogue_text(width - (textSize / 2), topheight - 18, dialText);

	// TODO: make volume buttons

	copy_block_phys(left, top, right, bottom);
}

/** Process the menu button draw
	@param data menu settings array
	@param mode flag to know if should draw as a hover button or not */
void draw_button(int16 *menuSettings, int32 mode) {
	int32  buttonNumber;
	int32  maxButton;
	int16  *localData = menuSettings;
	int32  topHeight;
	uint8  menuItemId;
	uint16 menuItemValue; // applicable for sound menus, to save the volume/sound bar
	int8   currentButton;

	buttonNumber = *localData;
	localData += 1;
	maxButton = *localData;
	localData += 1;
	topHeight = *localData;
	localData += 2;

	if (topHeight == 0) {
		topHeight = 35;
	} else {
		topHeight = topHeight - (((maxButton - 1) * 6) + ((maxButton) * 50)) / 2;
	}

	if (maxButton <= 0) {
		return;
	}

	currentButton = 0;

	do {
		// get menu item settings
		menuItemId = (uint8) * localData;
		localData += 1;
		menuItemValue = *localData;
		localData += 1;
		if (mode != 0) {
			if (currentButton == buttonNumber) {
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 1);
			}
		} else {
			if (currentButton == buttonNumber) {
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 1);
			} else {
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 0);
			}
		}

		currentButton++;
		topHeight += 56; // increase button top height

		// slow down the CPU
		fps_cycles(1000);
	} while (currentButton < maxButton);
}

/** Where the main menu options are processed
	@param menuSettings menu settings array with the information to build the menu options
	@return pressed menu button identification */
int process_menu(int16 * menuSettings) {
	int32 localTime;
	int32 numEntry;
	int32 buttonNeedRedraw;
	int32 maxButton;
	int16 *localData = menuSettings;
	int16 currentButton;
	int16 id;
	int32 musicChanged;
	int32 buttonReleased = 1;

	musicChanged = 0;

	buttonNeedRedraw = 1;

	numEntry = localData[1];
	currentButton = localData[0];
	localTime = lbaTime;
	maxButton = numEntry - 1;

	read_keys();

	do {
		// if its on main menu
		if (localData == MainMenuSettings) {
			if (lbaTime - localTime <= 11650) {
				if (skipIntro == 46)
					if (skipedKey != 32)
						return MAINMENU_BKGIMAGE;
			} else {
				return MAINMENU_BKGIMAGE;
			}
		}

		if (pressedKey == 0) {
			buttonReleased = 1;
		}

		if (buttonReleased) {
			key = pressedKey;

			if (((uint8) key & 2)) { // on arrow key down
				currentButton++;
				if (currentButton == numEntry) { // if current button is the last, than next button is the first
					currentButton = 0;
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			if (((uint8) key & 1)) { // on arrow key up
				currentButton--;
				if (currentButton < 0) { // if current button is the first, than previous button is the last
					currentButton = maxButton;
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			if (*(localData + 8) <= 5) { // if its a volume button
				id = *(localData + currentButton * 2 + 4); // get button parameters from settings array

				switch (id) {
				case MUSICVOLUME: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.MusicVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.MusicVolume += 4;
					}
					music_volume(cfgfile.MusicVolume);
					break;
				}
				case SOUNDVOLUME: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.WaveVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.WaveVolume += 4;
					}
					sample_volume(-1, cfgfile.WaveVolume);
					break;
				}
				case CDVOLUME: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.CDVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.CDVolume += 4;
					}
					break;
				}
				case LINEVOLUME: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.LineVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.LineVolume += 4;
					}
					break;
				}
				case MASTERVOLUME: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.MasterVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.MasterVolume += 4;
					}
					music_volume(cfgfile.MusicVolume);
					sample_volume(-1, cfgfile.WaveVolume);
					break;
				}
				default:
					break;
				}
			}
		}

		if (buttonNeedRedraw == 1) {
			*localData = currentButton;

			draw_button(localData, 0); // current button
			do {
				read_keys();
				draw_button(localData, 1);
			} while (pressedKey == 0 && skipedKey == 0 && skipIntro == 0);
			buttonNeedRedraw = 0;
		} else {
			if (musicChanged) {
				// TODO: update volume settings

			}

			buttonNeedRedraw = 0;
			draw_button(localData, 1);
			read_keys();
			// WARNING: this is here to prevent a fade bug while quit the menu
			copy_screen(workVideoBuffer, frontVideoBuffer);
		}
	} while (!(skipedKey & 2) && !(skipedKey & 1));

	currentButton = *(localData + 5 + currentButton * 2); // get current browsed button

	read_keys();

	return currentButton;
}

/** Used to run the advanced options menu */
int advoptions_menu() {
	int32 ret = 0;

	copy_screen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (process_menu(AdvOptionsMenuSettings)) {
		case OPTIONSMENU_RETURNMENU: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

	return 0;
}

/** Used to run the save game management menu */
int savemanage_menu() {
	int32 ret = 0;

	copy_screen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (process_menu(SaveManageMenuSettings)) {
		case OPTIONSMENU_RETURNMENU: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

	return 0;
}

/** Used to run the volume menu */
int volume_menu() {
	int32 ret = 0;

	copy_screen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (process_menu(VolumeMenuSettings)) {
		case OPTIONSMENU_RETURNMENU: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

	return 0;
}

/** Used to run the options menu */
int options_menu() {
	int32 ret = 0;

	copy_screen(workVideoBuffer, frontVideoBuffer);

	stop_samples();
	//playCDtrack(9);

	do {
		switch (process_menu(OptionsMenuSettings)) {
		case OPTIONSMENU_RETURNGAME:
		case OPTIONSMENU_RETURNMENU: {
			ret = 1; // quit option menu
			break;
		}
		case OPTIONSMENU_VOLUME: {
			copy_screen(workVideoBuffer, frontVideoBuffer);
			flip(workVideoBuffer);
			volume_menu();
			break;
		}
		case OPTIONSMENU_SAVEMANAGE: {
			copy_screen(workVideoBuffer, frontVideoBuffer);
			flip(workVideoBuffer);
			savemanage_menu();
			break;
		}
		case OPTIONSMENU_ADVOPTIONS: {
			copy_screen(workVideoBuffer, frontVideoBuffer);
			flip(workVideoBuffer);
			advoptions_menu();
			break;
		}
		default:
			break;
		}
	} while (ret != 1);

	copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

	return 0;
}


/** Used to run the main menu */
void main_menu() {
	stop_samples();

	copy_screen(frontVideoBuffer, workVideoBuffer);

	// load menu effect file only once
	plasmaEffectPtr = malloc(PLASMA_EFFECT_FILESIZE);
	hqr_get_entry(plasmaEffectPtr, HQR_RESS_FILE, RESSHQR_PLASMAEFFECT);

	while (!cfgfile.Quit) {
		// TODO: RECHECK THIS LATER
		init_dialogue_bank(0);

		play_track_music(9); // LBA's Theme
		stop_samples();

		switch (process_menu(MainMenuSettings)) {
		case MAINMENU_NEWGAME: {
			new_game_menu();
			break;
		}
		case MAINMENU_CONTINUEGAME: {
			break;
		}
		case MAINMENU_OPTIONS: {
			copy_screen(workVideoBuffer, frontVideoBuffer);
			flip(workVideoBuffer);
			OptionsMenuSettings[5] = OPTIONSMENU_RETURNMENU;
			options_menu();
			break;
		}
		case MAINMENU_QUIT: {
			cfgfile.Quit = 1;
			break;
		}
		case MAINMENU_BKGIMAGE: {
			load_menu_image(1);
		}
		}
		fps_cycles(cfgfile.Fps);
	}
}

/** Used to process give up menu while playing game */
int giveup_menu() {
	//int32 saveLangue=0;
	int32 menuId;
	int16 * localMenu;

	copy_screen(frontVideoBuffer, workVideoBuffer);
	stop_samples();

	if (cfgfile.UseAutoSaving == 1)
		localMenu = GiveUpMenuSettings;
	else
		localMenu = GiveUpMenuSettingsWithSave;

	do {
		//saveLangue = languageCD1;
		//languageCD1 = 0;
		init_dialogue_bank(0);

		menuId = process_menu(localMenu);

		//languageCD1 = saveLangue;

		init_dialogue_bank(currentTextBank + 3);

		fps_cycles(cfgfile.Fps);
	} while (menuId != GIVEUPMENU_QUIT && menuId != GIVEUPMENU_CONTINUE);

	if (menuId == GIVEUPMENU_QUIT)
		return 1;

	return 0;
}

void draw_info_menu(int16 left, int16 top)
{
	int32 boxLeft, boxTop, boxRight, boxBottom; // var_4, var_8, var_10, var_C
	int32 newBoxLeft, newBoxLeft2, i;

	reset_clip();
	draw_box(left, top, left + 450, top + 80); //80; 115
	draw_splitted_box(left + 1, top + 1, left + 449, top + 79, 0); //79

	newBoxLeft2 = left + 9;

	draw_sprite(0, newBoxLeft2, top + 13, spriteTable[SPRITEHQR_LIFEPOINTS]);

	boxRight = left + 325;
	newBoxLeft = left + 25;
	boxLeft = cross_dot(newBoxLeft, boxRight, 50, sceneHero->life);

	boxTop = top + 10;
	boxBottom = top + 25;
	draw_splitted_box(newBoxLeft, boxTop, boxLeft, boxBottom, 91);
	draw_splitted_box(boxLeft, boxTop, boxRight, boxBottom, 0);

	draw_box(left + 25, top + 10, left + 325, top + 10 + 15);

	if (!gameFlags[GAMEFLAG_INVENTORY_DISABLED] && gameFlags[GAMEFLAG_TUNIC]) {
		draw_sprite(0, newBoxLeft2, top + 36, spriteTable[SPRITEHQR_MAGICPOINTS]);
		if(magicLevelIdx > 0) {
			draw_splitted_box(newBoxLeft, top + 35, cross_dot(newBoxLeft, boxRight, 80, inventoryMagicPoints),top + 50, 75);
		}
		draw_box(left + 25, top + 35, left + magicLevelIdx * 80, top + 35 + 15);
	}

	boxLeft = left + 340;

	/** draw coin sprite */
	draw_sprite(0, boxLeft, top + 15, spriteTable[SPRITEHQR_KASHES]);
	set_font_color(155);
	display_dialogue_text(left + 370, top + 5, ITOA(inventoryNumKashes));

	/** draw key sprite */
	draw_sprite(0, boxLeft, top + 55, spriteTable[SPRITEHQR_KEY]);
	set_font_color(155);
	display_dialogue_text(left + 370, top + 40, ITOA(inventoryNumKeys));

	// Clover leaf boxes
	for (i = 0; i < inventoryNumLeafsBox; i++)
	{
		draw_sprite(0, cross_dot(left + 25, left + 325, 10, i), top + 58, spriteTable[SPRITEHQR_CLOVERLEAFBOX]);
	}

	// Clover leafs
	for (i = 0; i < inventoryNumLeafs; i++)
	{
		draw_sprite(0, cross_dot(left + 25, left + 325, 10, i) + 2, top + 60, spriteTable[SPRITEHQR_CLOVERLEAF]);
	}

	copy_block_phys(left, top, left + 450, top + 135);
}

void draw_behaviour(int16 behaviour, int32 angle, int16 drawBox) {
	uint8 *currentAnim;
	int32 boxLeft, boxTop, boxRight, boxBottom, currentAnimState;
	int8 dialText[256];

	boxLeft   = behaviour * 110 + 110;
	boxRight  = boxLeft + 99;
	boxTop    = 110;
	boxBottom = 229;

	currentAnim = animTable[heroAnimIdx[behaviour]];
	currentAnimState = behaviourAnimState[behaviour];

	if (set_model_animation(currentAnimState, currentAnim, behaviourEntity, &behaviourAnimData[behaviour])) {
		currentAnimState++; // keyframe
		if (currentAnimState >= get_num_keyframes(currentAnim)) {
			currentAnimState = get_start_keyframe(currentAnim);
		}
		behaviourAnimState[behaviour] = currentAnimState;
	}

	if (drawBox == 0) {
		draw_box(boxLeft - 1, boxTop - 1, boxRight + 1, boxBottom + 1);
	}

	save_clip();
	reset_clip();

	if (behaviour != heroBehaviour) { // unselected
		draw_splitted_box(boxLeft, boxTop, boxRight, boxBottom, 0);
	} else { // selected
		draw_splitted_box(boxLeft, boxTop, boxRight, boxBottom, 69);

		// behaviour menu title
		draw_splitted_box(110, 239, 540, 279, 0);
		draw_box(110, 239, 540, 279);

		set_font_color(15);

		if (heroBehaviour == 2 && autoAgressive == 1) {
			get_menu_text(4, dialText);
		} else {
			get_menu_text(heroBehaviour, dialText);
		}

		display_dialogue_text((650 - dialogue_text_size(dialText)) / 2, 240, dialText);
	}

	render_behaviour_model(boxLeft, boxTop, boxRight, boxBottom, -600, angle, behaviourEntity);

	copy_block_phys(boxLeft, boxTop, boxRight, boxBottom);
	copy_block_phys(110, 239, 540, 279);

	load_clip();
}

void draw_behaviour_menu(int32 angle) {
	draw_box(100, 100, 550, 290);
	draw_transparent_box(101, 101, 549, 289, 2);

	set_anim_at_keyframe(behaviourAnimState[NORMAL], animTable[heroAnimIdx[NORMAL]], behaviourEntity, &behaviourAnimData[NORMAL]);
	draw_behaviour(NORMAL, angle, 0);

	set_anim_at_keyframe(behaviourAnimState[ATHLETIC], animTable[heroAnimIdx[ATHLETIC]], behaviourEntity, &behaviourAnimData[ATHLETIC]);
	draw_behaviour(ATHLETIC, angle, 0);

	set_anim_at_keyframe(behaviourAnimState[AGGRESSIVE], animTable[heroAnimIdx[AGGRESSIVE]], behaviourEntity, &behaviourAnimData[AGGRESSIVE]);
	draw_behaviour(AGGRESSIVE, angle, 0);

	set_anim_at_keyframe(behaviourAnimState[DISCRETE], animTable[heroAnimIdx[DISCRETE]], behaviourEntity, &behaviourAnimData[DISCRETE]);
	draw_behaviour(DISCRETE, angle, 0);

	draw_info_menu(100, 300);

	copy_block_phys(100, 100, 550, 290);
}

/** Process hero behaviour menu */
void process_behaviour_menu() {
	int32 tmpLanguageCD;
	int32 tmpTextBank;
	int32 tmpHeroBehaviour;
	int32 tmpTime;

	if (heroBehaviour == PROTOPACK) {
		stop_samples();
		set_behaviour(NORMAL);
	}

	behaviourEntity = bodyTable[sceneHero->entity];

	heroAnimIdx[NORMAL]     = heroAnimIdxNORMAL;
	heroAnimIdx[ATHLETIC]   = heroAnimIdxATHLETIC;
	heroAnimIdx[AGGRESSIVE] = heroAnimIdxAGGRESSIVE;
	heroAnimIdx[DISCRETE]   = heroAnimIdxDISCRETE;

	set_actor_angle_safe(sceneHero->angle, sceneHero->angle - 256, 50, &moveMenu);

	copy_screen(frontVideoBuffer, workVideoBuffer);

	tmpLanguageCD = cfgfile.LanguageCDIdx;
	cfgfile.LanguageCDIdx = 0;

	tmpTextBank = currentTextBank;
	currentTextBank = -1;

	init_dialogue_bank(0);

	draw_behaviour_menu(sceneHero->angle);

	tmpHeroBehaviour = heroBehaviour;

	set_anim_at_keyframe(behaviourAnimState[heroBehaviour], animTable[heroAnimIdx[heroBehaviour]], behaviourEntity, &behaviourAnimData[heroBehaviour]);

	read_keys();
	
	tmpTime = lbaTime;

	while (skipedKey & 4 || (skipIntro > 59 && skipIntro < 62)) {
		read_keys();
		key = pressedKey;

		if (key & 8) {
			heroBehaviour++;
		}

		if (key & 4) {
			heroBehaviour--;
		}

		if (heroBehaviour < 0) {
			heroBehaviour = 3;
		}

		if (heroBehaviour >= 4) {
			heroBehaviour = 0;
		}

		if (tmpHeroBehaviour != heroBehaviour) {
			draw_behaviour(tmpHeroBehaviour, sceneHero->angle, 1);
			tmpHeroBehaviour = heroBehaviour;
			set_actor_angle_safe(sceneHero->angle, sceneHero->angle - 256, 50, &moveMenu);
			set_anim_at_keyframe(behaviourAnimState[heroBehaviour], animTable[heroAnimIdx[heroBehaviour]], behaviourEntity, &behaviourAnimData[heroBehaviour]);

			while (pressedKey) {
				read_keys();
				draw_behaviour(heroBehaviour, -1, 1);
			}
		}
		
		draw_behaviour(heroBehaviour, -1, 1);

		fps_cycles(50);
		lbaTime++;
	}

	lbaTime = tmpTime;

	set_behaviour(heroBehaviour);
	init_engine_projections();

	currentTextBank = tmpTextBank;
	init_dialogue_bank(currentTextBank + 3);

	cfgfile.LanguageCDIdx = tmpLanguageCD;
}
