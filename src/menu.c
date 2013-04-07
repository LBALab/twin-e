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
#include "screens.h"
#include "sdlengine.h"
#include "hqrdepack.h"
#include "lbaengine.h"
#include "text.h"
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

/** Main menu background image number
	Used when returning from credit sequence to redraw the main menu background image */
uint32 kPlasmaEffectFilesize = 262176;

/** Menu buttons width */
uint16 kMainMenuButtonWidth = 320;
/** Used to calculate the spanning between button and screen */
uint16 kMainMenuButtonSpan = 550;


/** Main menu types */
enum MainMenuType {
	kNewGame		= 20,
	kContinueGame	= 21,
	kOptions		= 23,
	kQuit			= 22,
	kBackground		= 9999
};

/** Give up menu types */
enum GiveUpMenuType {
	kContinue		= 28,
	kGiveUp			= 27 // quit
};

/** Options menu types */
enum OptionsMenuType {
	kReturnGame		= 15,
	kReturnMenu		= 26,
	kVolume			= 30,
	kSaveManage		= 46,
	kAdvanced		= 47
};

/** Volume menu types */
enum VolumeMenuType {
	kMusicVolume	= 1,
	kSoundVolume	= 2,
	kCDVolume		= 3,
	kLineVolume		= 4,
	kMasterVolume	= 5
};

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

int32 inventorySelectedColor;
int32 inventorySelectedItem; // currentSelectedObjectInInventory

/** Plasma Effect Initialization */
void plasmaEffectInit() {
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
void processPlasmaEffect(int32 top, int32 color) {
	uint8 *temp;
	uint8 *out;
	int32 i, j;
	uint8 temp3;
	uint8 bh, bl;

	plasmaEffectInit();

	temp = plasmaEffectVar1 + 1600;
	out = frontVideoBuffer + screenLockupTable[top];

	bl = color;
	bh = bl + 15;

	for (i = 25; i >= 0; i--) {
		for (j = kMainMenuButtonWidth; j >= 0; j--) {
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
void drawBox(int32 left, int32 top, int32 right, int32 bottom) {
	drawLine(left, top, right, top, 79);			// top line
	drawLine(left, top, left, bottom, 79);			// left line
	drawLine(right, ++top, right, bottom, 73);		// right line
	drawLine(++left, bottom, right, bottom, 73);	// bottom line
}

/** Draws main menu button
	@param width menu button width
	@param topheight is the height between the top of the screen and the first button
	@param id current button identification from menu settings
	@param value current button key pressed value
	@param mode flag to know if should draw as a hover button or not */
void drawButtonGfx(int32 width, int32 topheight, int32 id, int32 value, int32 mode) {
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

	left = width - kMainMenuButtonSpan / 2;
	right = width + kMainMenuButtonSpan / 2;

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
				newWidth = crossDot(left, right, 255, cfgfile.MusicVolume);
				break;
			}
			case 2: {
				if (cfgfile.WaveVolume > 255)
					cfgfile.WaveVolume = 255;
				if (cfgfile.WaveVolume < 0)
					cfgfile.WaveVolume = 0;
				newWidth = crossDot(left, right, 255, cfgfile.WaveVolume);
				break;
			}
			case 3: {
				if (cfgfile.CDVolume > 255)
					cfgfile.CDVolume = 255;
				if (cfgfile.CDVolume < 0)
					cfgfile.CDVolume = 0;
				newWidth = crossDot(left, right, 255, cfgfile.CDVolume);
				break;
			}
			case 4: {
				if (cfgfile.LineVolume > 255)
					cfgfile.LineVolume = 255;
				if (cfgfile.LineVolume < 0)
					cfgfile.LineVolume = 0;
				newWidth = crossDot(left, right, 255, cfgfile.LineVolume);
				break;
			}
			case 5: {
				if (cfgfile.MasterVolume > 255)
					cfgfile.MasterVolume = 255;
				if (cfgfile.MasterVolume < 0)
					cfgfile.MasterVolume = 0;
				newWidth = crossDot(left, right, 255, cfgfile.MasterVolume);
				break;
			}
			};

			processPlasmaEffect(top, 80);
			if (!(rand() % 5)) {
				plasmaEffectPtr[rand() % 140 * 10 + 1900] = 255;
			}
			drawSplittedBox(newWidth, top, right, bottom, 68);
		} else {
			processPlasmaEffect(top, 64);
			if (!(rand() % 5)) {
				plasmaEffectPtr[rand() % 320 * 10 + 6400] = 255;
			}
		}

		if (id <= 5 && id >= 1) {
			// implement this
		}
	} else {
		blitBox(left, top, right, bottom, (int8 *) workVideoBuffer, left, top, (int8 *) frontVideoBuffer);
		drawTransparentBox(left, top, right, bottom2, 4);
	}

	drawBox(left, top, right, bottom);

	setFontColor(15);
	setFontParameters(2, 8);
	getMenuText(value, dialText);
	textSize = getTextSize(dialText);
	drawText(width - (textSize / 2), topheight - 18, dialText);

	// TODO: make volume buttons

	copyBlockPhys(left, top, right, bottom);
}

/** Process the menu button draw
	@param data menu settings array
	@param mode flag to know if should draw as a hover button or not */
void drawButton(int16 *menuSettings, int32 mode) {
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
				drawButtonGfx(kMainMenuButtonWidth, topHeight, menuItemId, menuItemValue, 1);
			}
		} else {
			if (currentButton == buttonNumber) {
				drawButtonGfx(kMainMenuButtonWidth, topHeight, menuItemId, menuItemValue, 1);
			} else {
				drawButtonGfx(kMainMenuButtonWidth, topHeight, menuItemId, menuItemValue, 0);
			}
		}

		currentButton++;
		topHeight += 56; // increase button top height

		// slow down the CPU
		sdldelay(1);
	} while (currentButton < maxButton);
}

/** Where the main menu options are processed
	@param menuSettings menu settings array with the information to build the menu options
	@return pressed menu button identification */
int32 processMenu(int16 * menuSettings) {
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
	currentButton = 0; // localData[0];
	localTime = lbaTime;
	maxButton = numEntry - 1;

	readKeys();

	do {
		// if its on main menu
		if (localData == MainMenuSettings) {
			if (lbaTime - localTime <= 11650) {
				if (skipIntro == 46)
					if (skipedKey != 32)
						return kBackground;
			} else {
				return kBackground;
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
				case kMusicVolume: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.MusicVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.MusicVolume += 4;
					}
					musicVolume(cfgfile.MusicVolume);
					break;
				}
				case kSoundVolume: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.WaveVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.WaveVolume += 4;
					}
					sampleVolume(-1, cfgfile.WaveVolume);
					break;
				}
				case kCDVolume: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.CDVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.CDVolume += 4;
					}
					break;
				}
				case kLineVolume: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.LineVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.LineVolume += 4;
					}
					break;
				}
				case kMasterVolume: {
					if (((uint8) key & 4)) { // on arrow key left
						cfgfile.MasterVolume -= 4;
					}
					if (((uint8) key & 8)) { // on arrow key right
						cfgfile.MasterVolume += 4;
					}
					musicVolume(cfgfile.MusicVolume);
					sampleVolume(-1, cfgfile.WaveVolume);
					break;
				}
				default:
					break;
				}
			}
		}

		if (buttonNeedRedraw == 1) {
			*localData = currentButton;

			drawButton(localData, 0); // current button
			do {
				readKeys();
				drawButton(localData, 1);
			} while (pressedKey == 0 && skipedKey == 0 && skipIntro == 0);
			buttonNeedRedraw = 0;
		} else {
			if (musicChanged) {
				// TODO: update volume settings

			}

			buttonNeedRedraw = 0;
			drawButton(localData, 1);
			readKeys();
			// WARNING: this is here to prevent a fade bug while quit the menu
			copyScreen(workVideoBuffer, frontVideoBuffer);
		}
	} while (!(skipedKey & 2) && !(skipedKey & 1));

	currentButton = *(localData + 5 + currentButton * 2); // get current browsed button

	readKeys();

	return currentButton;
}

/** Used to run the advanced options menu */
int32 advoptionsMenu() {
	int32 ret = 0;

	copyScreen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (processMenu(AdvOptionsMenuSettings)) {
		case kReturnMenu: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copyScreen(workVideoBuffer, frontVideoBuffer);
	flip();

	return 0;
}

/** Used to run the save game management menu */
int32 savemanageMenu() {
	int32 ret = 0;

	copyScreen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (processMenu(SaveManageMenuSettings)) {
		case kReturnMenu: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copyScreen(workVideoBuffer, frontVideoBuffer);
	flip();

	return 0;
}

/** Used to run the volume menu */
int32 volumeMenu() {
	int32 ret = 0;

	copyScreen(workVideoBuffer, frontVideoBuffer);

	do {
		switch (processMenu(VolumeMenuSettings)) {
		case kReturnMenu: {
			ret = 1; // quit option menu
			break;
		}
		//TODO: add other options
		default:
			break;
		}
	} while (ret != 1);

	copyScreen(workVideoBuffer, frontVideoBuffer);
	flip();

	return 0;
}

/** Used to run the options menu */
int32 optionsMenu() {
	int32 ret = 0;

	copyScreen(workVideoBuffer, frontVideoBuffer);

	stopSamples();
	//playCDtrack(9);

	do {
		switch (processMenu(OptionsMenuSettings)) {
		case kReturnGame:
		case kReturnMenu: {
			ret = 1; // quit option menu
			break;
		}
		case kVolume: {
			copyScreen(workVideoBuffer, frontVideoBuffer);
			flip();
			volumeMenu();
			break;
		}
		case kSaveManage: {
			copyScreen(workVideoBuffer, frontVideoBuffer);
			flip();
			savemanageMenu();
			break;
		}
		case kAdvanced: {
			copyScreen(workVideoBuffer, frontVideoBuffer);
			flip();
			advoptionsMenu();
			break;
		}
		default:
			break;
		}
	} while (ret != 1);

	copyScreen(workVideoBuffer, frontVideoBuffer);
	flip();

	return 0;
}


/** Used to run the main menu */
void mainMenu() {
	stopSamples();

	copyScreen(frontVideoBuffer, workVideoBuffer);

	// load menu effect file only once
	plasmaEffectPtr = (uint8 *)malloc(kPlasmaEffectFilesize);
	hqrGetEntry(plasmaEffectPtr, HQR_RESS_FILE, RESSHQR_PLASMAEFFECT);

	while (!cfgfile.Quit) {
		initTextBank(0);

		playTrackMusic(9); // LBA's Theme
		stopSamples();

		switch (processMenu(MainMenuSettings)) {
		case kNewGame: {
			newGameMenu();
			break;
		}
		case kContinueGame: {
			continueGameMenu();
			break;
		}
		case kOptions: {
			copyScreen(workVideoBuffer, frontVideoBuffer);
			flip();
			OptionsMenuSettings[5] = kReturnMenu;
			optionsMenu();
			break;
		}
		case kQuit: {
			cfgfile.Quit = 1;
			break;
		}
		case kBackground: {
			loadMenuImage(1);
		}
		}
		fpsCycles(cfgfile.Fps);
	}
}

/** Used to process give up menu while playing game */
int32 giveupMenu() {
	//int32 saveLangue=0;
	int32 menuId;
	int16 * localMenu;

	copyScreen(frontVideoBuffer, workVideoBuffer);
	pauseSamples();

	if (cfgfile.UseAutoSaving == 1)
		localMenu = GiveUpMenuSettings;
	else
		localMenu = GiveUpMenuSettingsWithSave;

	do {
		//saveLangue = languageCD1;
		//languageCD1 = 0;
		initTextBank(0);

		menuId = processMenu(localMenu);

		//languageCD1 = saveLangue;

		initTextBank(currentTextBank + 3);

		fpsCycles(cfgfile.Fps);
	} while (menuId != kGiveUp && menuId != kContinue);

	if (menuId == kGiveUp)
	{
		stopSamples();
		return 1;
	}

	resumeSamples();
	return 0;
}

void drawInfoMenu(int16 left, int16 top)
{
	int32 boxLeft, boxTop, boxRight, boxBottom;
	int32 newBoxLeft, newBoxLeft2, i;

	resetClip();
	drawBox(left, top, left + 450, top + 80);
	drawSplittedBox(left + 1, top + 1, left + 449, top + 79, 0);

	newBoxLeft2 = left + 9;

	drawSprite(0, newBoxLeft2, top + 13, spriteTable[SPRITEHQR_LIFEPOINTS]);

	boxRight = left + 325;
	newBoxLeft = left + 25;
	boxLeft = crossDot(newBoxLeft, boxRight, 50, sceneHero->life);

	boxTop = top + 10;
	boxBottom = top + 25;
	drawSplittedBox(newBoxLeft, boxTop, boxLeft, boxBottom, 91);
	drawBox(left + 25, top + 10, left + 324, top + 10 + 14);

	if (!gameFlags[GAMEFLAG_INVENTORY_DISABLED] && gameFlags[GAMEFLAG_TUNIC]) {
		drawSprite(0, newBoxLeft2, top + 36, spriteTable[SPRITEHQR_MAGICPOINTS]);
		if(magicLevelIdx > 0) {
			drawSplittedBox(newBoxLeft, top + 35, crossDot(newBoxLeft, boxRight, 80, inventoryMagicPoints),top + 50, 75);
		}
		drawBox(left + 25, top + 35, left + magicLevelIdx * 80 + 20, top + 35 + 15);
	}

	boxLeft = left + 340;

	/** draw coin sprite */
	drawSprite(0, boxLeft, top + 15, spriteTable[SPRITEHQR_KASHES]);
	setFontColor(155);
	drawText(left + 370, top + 5, ITOA(inventoryNumKashes));

	/** draw key sprite */
	drawSprite(0, boxLeft, top + 55, spriteTable[SPRITEHQR_KEY]);
	setFontColor(155);
	drawText(left + 370, top + 40, ITOA(inventoryNumKeys));

	// prevent 
	if (inventoryNumLeafs > inventoryNumLeafsBox) {
		inventoryNumLeafs = inventoryNumLeafsBox;
	}

	// Clover leaf boxes
	for (i = 0; i < inventoryNumLeafsBox; i++)
	{
		drawSprite(0, crossDot(left + 25, left + 325, 10, i), top + 58, spriteTable[SPRITEHQR_CLOVERLEAFBOX]);
	}

	// Clover leafs
	for (i = 0; i < inventoryNumLeafs; i++)
	{
		drawSprite(0, crossDot(left + 25, left + 325, 10, i) + 2, top + 60, spriteTable[SPRITEHQR_CLOVERLEAF]);
	}

	copyBlockPhys(left, top, left + 450, top + 135);
}

void drawBehaviour(int16 behaviour, int32 angle, int16 cantDrawBox) {
	uint8 *currentAnim;
	int32 boxLeft, boxTop, boxRight, boxBottom, currentAnimState;
	int8 dialText[256];

	boxLeft   = behaviour * 110 + 110;
	boxRight  = boxLeft + 99;
	boxTop    = 110;
	boxBottom = 229;

	currentAnim = animTable[heroAnimIdx[behaviour]];
	currentAnimState = behaviourAnimState[behaviour];

	if (setModelAnimation(currentAnimState, currentAnim, behaviourEntity, &behaviourAnimData[behaviour])) {
		currentAnimState++; // keyframe
		if (currentAnimState >= getNumKeyframes(currentAnim)) {
			currentAnimState = getStartKeyframe(currentAnim);
		}
		behaviourAnimState[behaviour] = currentAnimState;
	}

	if (cantDrawBox == 0) {
		drawBox(boxLeft - 1, boxTop - 1, boxRight + 1, boxBottom + 1);
	}

	saveClip();
	resetClip();

	if (behaviour != heroBehaviour) { // unselected
		drawSplittedBox(boxLeft, boxTop, boxRight, boxBottom, 0);
	} else { // selected
		drawSplittedBox(boxLeft, boxTop, boxRight, boxBottom, 69);

		// behaviour menu title
		drawSplittedBox(110, 239, 540, 279, 0);
		drawBox(110, 239, 540, 279);

		setFontColor(15);

		if (heroBehaviour == 2 && autoAgressive == 1) {
			getMenuText(4, dialText);
		} else {
			getMenuText(heroBehaviour, dialText);
		}

		drawText((650 - getTextSize(dialText)) / 2, 240, dialText);
	}

	renderBehaviourModel(boxLeft, boxTop, boxRight, boxBottom, -600, angle, behaviourEntity);

	copyBlockPhys(boxLeft, boxTop, boxRight, boxBottom);
	copyBlockPhys(110, 239, 540, 279);

	loadClip();
}

void drawBehaviourMenu(int32 angle) {
	drawBox(100, 100, 550, 290);
	drawTransparentBox(101, 101, 549, 289, 2);

	setAnimAtKeyframe(behaviourAnimState[kNormal], animTable[heroAnimIdx[kNormal]], behaviourEntity, &behaviourAnimData[kNormal]);
	drawBehaviour(kNormal, angle, 0);

	setAnimAtKeyframe(behaviourAnimState[kAthletic], animTable[heroAnimIdx[kAthletic]], behaviourEntity, &behaviourAnimData[kAthletic]);
	drawBehaviour(kAthletic, angle, 0);

	setAnimAtKeyframe(behaviourAnimState[kAggressive], animTable[heroAnimIdx[kAggressive]], behaviourEntity, &behaviourAnimData[kAggressive]);
	drawBehaviour(kAggressive, angle, 0);

	setAnimAtKeyframe(behaviourAnimState[kDiscrete], animTable[heroAnimIdx[kDiscrete]], behaviourEntity, &behaviourAnimData[kDiscrete]);
	drawBehaviour(kDiscrete, angle, 0);

	drawInfoMenu(100, 300);

	copyBlockPhys(100, 100, 550, 290);
}

/** Process hero behaviour menu */
void processBehaviourMenu() {
	int32 tmpLanguageCD;
	int32 tmpTextBank;
	int32 tmpHeroBehaviour;
	int32 tmpTime;

	if (heroBehaviour == kProtoPack) {
		stopSamples();
		setBehaviour(kNormal);
	}

	behaviourEntity = bodyTable[sceneHero->entity];

	heroAnimIdx[kNormal]     = heroAnimIdxNORMAL;
	heroAnimIdx[kAthletic]   = heroAnimIdxATHLETIC;
	heroAnimIdx[kAggressive] = heroAnimIdxAGGRESSIVE;
	heroAnimIdx[kDiscrete]   = heroAnimIdxDISCRETE;

	setActorAngleSafe(sceneHero->angle, sceneHero->angle - 256, 50, &moveMenu);

	copyScreen(frontVideoBuffer, workVideoBuffer);

	tmpLanguageCD = cfgfile.LanguageCDId;
	cfgfile.LanguageCDId = 0;

	tmpTextBank = currentTextBank;
	currentTextBank = -1;

	initTextBank(0);

	drawBehaviourMenu(sceneHero->angle);

	tmpHeroBehaviour = heroBehaviour;

	setAnimAtKeyframe(behaviourAnimState[heroBehaviour], animTable[heroAnimIdx[heroBehaviour]], behaviourEntity, &behaviourAnimData[heroBehaviour]);

	readKeys();
	
	tmpTime = lbaTime;

	while (skipedKey & 4 || (skipIntro > 59 && skipIntro < 62)) {
		readKeys();
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
			drawBehaviour(tmpHeroBehaviour, sceneHero->angle, 1);
			tmpHeroBehaviour = heroBehaviour;
			setActorAngleSafe(sceneHero->angle, sceneHero->angle - 256, 50, &moveMenu);
			setAnimAtKeyframe(behaviourAnimState[heroBehaviour], animTable[heroAnimIdx[heroBehaviour]], behaviourEntity, &behaviourAnimData[heroBehaviour]);

			while (pressedKey) {
				readKeys();
				drawBehaviour(heroBehaviour, -1, 1);
			}
		}
		
		drawBehaviour(heroBehaviour, -1, 1);

		fpsCycles(50);
		lbaTime++;
	}

	lbaTime = tmpTime;

	setBehaviour(heroBehaviour);
	initEngineProjections();

	currentTextBank = tmpTextBank;
	initTextBank(currentTextBank + 3);

	cfgfile.LanguageCDId = tmpLanguageCD;
}

/** Draw the entire button box
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button */
void drawMagicItemsBox(int32 left, int32 top, int32 right, int32 bottom, int32 color) { // Rect
	drawLine(left, top, right, top, color);			// top line
	drawLine(left, top, left, bottom, color);			// left line
	drawLine(right, ++top, right, bottom, color);		// right line
	drawLine(++left, bottom, right, bottom, color);	// bottom line
}

void drawItem(int32 item) {
	int32 itemX = (item / 4) * 85 + 64;
	int32 itemY = (item & 3) * 75 + 52;

	int32 left   = itemX - 37;
	int32 right  = itemX + 37;
	int32 top    = itemY - 32;
	int32 bottom = itemY + 32;

	if (inventorySelectedItem == item) {
		drawSplittedBox(left, top, right, bottom, inventorySelectedColor);
	} else {
		drawSplittedBox(left, top, right, bottom, 0);
	}

	if (gameFlags[item] && !gameFlags[GAMEFLAG_INVENTORY_DISABLED] && item < NUM_INVENTORY_ITEMS) {
		prepareIsoModel(inventoryTable[item]);
		itemAngle[item] += 8;
		renderInventoryItem(itemX, itemY, inventoryTable[item], itemAngle[item], 15000);

		if (item == 15) { // has GAS
			setFontColor(15);
			drawText(left + 3, top + 32, ITOA(inventoryNumGas));
		}
	}

	drawBox(left, top, right, bottom);
	copyBlockPhys(left, top, right, bottom);
}

void drawInventoryItems() {
	int32 item;

	drawTransparentBox(17, 10, 622, 320, 4);
	drawBox(17, 10, 622, 320);
	drawMagicItemsBox(110, 18, 188, 311, 75);
	copyBlockPhys(17, 10, 622, 320);

	for (item = 0; item < NUM_INVENTORY_ITEMS; item++) {
		drawItem(item);
	}
}

/** Process in-game inventory menu */
void processInventoryMenu() {
	int32 di = 1;
	int32 prevSelectedItem, tmpLanguageCD, bx, tmpAlphaLight, tmpBetaLight;

	tmpAlphaLight = alphaLight;
	tmpBetaLight  = betaLight;

	copyScreen(frontVideoBuffer, workVideoBuffer);

	setLightVector(896, 950, 0);

	inventorySelectedColor = 68;

	if (inventoryNumLeafs > 0) {
		gameFlags[GAMEFLAG_HAS_CLOVER_LEAF] = 1;
	}

	drawInventoryItems();

	tmpLanguageCD = cfgfile.LanguageCDId;
	cfgfile.LanguageCDId = 0;

	initTextBank(2);

	bx = 3;

	setFontCrossColor(4);
	initDialogueBox();

	while (skipIntro != 1) {
		readKeys();
		prevSelectedItem = inventorySelectedItem;

		if (!di) {
			key  = pressedKey;
			loopPressedKey = skipedKey;
			loopCurrentKey = skipIntro;
			
			if (key != 0 || skipedKey != 0) {
				di = 1;
			}
		} else {
			loopCurrentKey = 0;
			key = 0;
			loopPressedKey = 0;
			if (!pressedKey && !skipedKey) {
				di = 0;
			}
		}

		if (loopCurrentKey == 1 || loopPressedKey & 0x20)
			break;

		if (key & 2) { // down
			inventorySelectedItem++;
			if (inventorySelectedItem >= NUM_INVENTORY_ITEMS) {
				inventorySelectedItem = 0;
			}
			drawItem(prevSelectedItem);
			bx = 3;
		}

		if (key & 1) { // up
			inventorySelectedItem--;
			if (inventorySelectedItem < 0) {
				inventorySelectedItem = NUM_INVENTORY_ITEMS - 1;
			}
			drawItem(prevSelectedItem);
			bx = 3;
		}

		if (key & 4) { // left
			inventorySelectedItem -= 4;
			if (inventorySelectedItem < 0) {
				inventorySelectedItem += NUM_INVENTORY_ITEMS;
			}
			drawItem(prevSelectedItem);
			bx = 3;
		}

		if (key & 8) { // right
			inventorySelectedItem += 4;
			if (inventorySelectedItem >= NUM_INVENTORY_ITEMS) {
				inventorySelectedItem -= NUM_INVENTORY_ITEMS;
			}
			drawItem(prevSelectedItem);
			bx = 3;
		}

		if (bx == 3) {
			initInventoryDialogueBox();

			if (gameFlags[inventorySelectedItem] == 1 && !gameFlags[GAMEFLAG_INVENTORY_DISABLED] && inventorySelectedItem < NUM_INVENTORY_ITEMS) {
				initText(inventorySelectedItem + 100);
			} else {
				initText(128);
			}
			bx = 0;
		}

		if (bx != 2) {
			bx = printText10();
		}

		// TRICKY: 3D model rotation delay - only apply when no text is drawing
		if (bx == 0 || bx == 2) {
			sdldelay(15);
		}

		if (loopPressedKey & 1) {
			if (bx == 2) {
				initInventoryDialogueBox();
				bx = 0;
			} else {
				if (gameFlags[inventorySelectedItem] == 1 && !gameFlags[GAMEFLAG_INVENTORY_DISABLED] && inventorySelectedItem < NUM_INVENTORY_ITEMS) {
					initInventoryDialogueBox();
					initText(inventorySelectedItem + 100);
				}
			}
		}

		drawItem(inventorySelectedItem);

		if ((loopPressedKey & 2) && gameFlags[inventorySelectedItem] == 1 && !gameFlags[GAMEFLAG_INVENTORY_DISABLED] && inventorySelectedItem < NUM_INVENTORY_ITEMS) {
			loopInventoryItem = inventorySelectedItem;
			inventorySelectedColor = 91;
			drawItem(inventorySelectedItem);
			break;
		}
	}

	printTextVar13 = 0;

	alphaLight = tmpAlphaLight;
	betaLight = tmpBetaLight;

	initEngineProjections();

	cfgfile.LanguageCDId = tmpLanguageCD;

	initTextBank(currentTextBank + 3);

	while (skipIntro != 0 && skipedKey != 0) {
		readKeys();
		sdldelay(1);
	}
}