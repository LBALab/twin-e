/** @file menuoptions.c
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

#include "menuoptions.h"
#include "lbaengine.h"
#include "main.h"
#include "images.h"
#include "resources.h"
#include "sdlengine.h"
#include "text.h"
#include "gamestate.h"
#include "music.h"
#include "keyboard.h"
#include "movies.h"
#include "scene.h"
#include "interface.h"
#include "menu.h"
#include "text.h"

/** Main menu continue game option key */
#define MAINMENU_CONTINUEGAME		21
/** Main menu enter players name */
#define MAINMENU_ENTERPLAYERNAME	42

int8 allowedCharIndex[] = " ABCDEFGHIJKLM.NOPQRSTUVWXYZ-abcdefghijklm?nopqrstuvwxyz!0123456789\040\b\r\0";


void newGame() {
	int32 tmpFlagDisplayText;

	stopMusic();

	tmpFlagDisplayText = cfgfile.FlagDisplayText;
	cfgfile.FlagDisplayText = 1;

//#ifndef _DEBUG	
	// intro screen 1 - twinsun
	loadImage( RESSHQR_INTROSCREEN1IMG, 1 );
//#endif
	newGameVar4 = 0;
	newGameVar5 = 1;

	initTextBank(2);
	textClipFull();
	setFontCrossColor(15);
//#ifndef _DEBUG	
	drawTextFullscreen(150);
	readKeys();

	if (skipIntro != 1) {
		// intro screen 1 - twinsun
		loadImage( RESSHQR_INTROSCREEN2IMG, 1 );
		drawTextFullscreen(151);
		readKeys();

		if (skipIntro != 1) {
			loadImage( RESSHQR_INTROSCREEN3IMG, 1 );
			drawTextFullscreen(152);
		}
	}
//#endif
	newGameVar5 = 0;
	textClipSmall();
	newGameVar4 = 1;
//#ifndef _DEBUG
	fadeToBlack(paletteRGBACustom);
	clearScreen();
	flip();
	
	playMidiMusic(1, 0);
	playMovie(FLA_INTROD);
//#endif
	clearScreen();
	flip();


	// set main palette back
	setPalette(paletteRGBA);

	cfgfile.FlagDisplayText = tmpFlagDisplayText;
}

void showCredits() {
	int32 tmpShadowMode, tmpLanguageCDIdx;
	
	canShowCredits = 1;
	tmpShadowMode = cfgfile.ShadowMode;
	tmpLanguageCDIdx = cfgfile.LanguageCDId;
	cfgfile.ShadowMode = 0;
	cfgfile.LanguageCDId = 0;
	initEngineVars(1);
	currentSceneIdx = 119;
	needChangeScene = 119;
	
	gameEngineLoop();

	canShowCredits = 0;
	cfgfile.ShadowMode = tmpShadowMode;
	cfgfile.LanguageCDId = tmpLanguageCDIdx;
	
	clearScreen();
	flip();

	playMovie(FLA_THEEND);

	clearScreen();
	flip();
	setPalette(paletteRGBA);
}

void drawSelectableCharacter(int32 x, int32 y, int32 arg) {
	int8 buffer[256];
	int32 centerX, left, top, centerY, bottom, right, right2;

	buffer[0] = allowedCharIndex[y + x * 14];

	centerX = y * 45 + 25;
	left = centerX - 20;
	right = centerX + 20;
	top = x * 56 + 200 - 25;
	buffer[1] = 0;
	centerY = x * 56 + 200;
	bottom = x * 56 + 200 + 25;

	if (arg != 0) {
		drawSplittedBox(left, top, right, bottom, 91);
	} else {
		blitBox(left, top, right, bottom, (int8 *) workVideoBuffer, left, top, (int8 *)frontVideoBuffer);
		right2 = right;
		drawTransparentBox(left, top, right2, bottom, 4);
	}

	drawBox(left, top, right, bottom);
	right2 = right;

	setFontColor(15);
	drawText(centerX - getTextSize(buffer) / 2, centerY - 18, buffer);

	copyBlockPhys(left, top, right2, bottom);
}

void drawSelectableCharacters(void) {
	int8 x, y;

	for (x = 0; x < 5; x++) {
		for (y = 0; y < 14; y++) {
			drawSelectableCharacter(x, y, 0);
		}
	}
}

int32 enterPlayerName(int32 textIdx) {
	int8 buffer[256];

	int32 a, b, c, e;

	e = 1;
	a = -1;
	b = 0;
	c = 0;

	while(1) {
		copyScreen(workVideoBuffer, frontVideoBuffer);
		flip(frontVideoBuffer);
		initTextBank(0);
		getMenuText(textIdx, buffer);
		setFontColor(15);
		drawText(320 - (getTextSize(buffer) / 2), 20, buffer);
		copyBlockPhys(0, 0, 639, 99);
		playerName[0] = enterPlayerNameVar1;
		//drawSmallButton(320, 100, (int)playerName, 1);
		drawSelectableCharacters();

		do {
			readKeys();
			do {
				readKeys();
			} while(skipIntro);
		} while(skipedKey);

		enterPlayerNameVar2 = 1;

		do {
			readKeys();
		} while(pressedKey);

		while (!skipIntro) {
			readKeys();
			// TODO
		}

		// FIXME: remove this lines after implementing everything
		if (skipIntro)
			break;
	}
	copyScreen(workVideoBuffer, frontVideoBuffer);
	flip(frontVideoBuffer);

	return 1;
}

/** Main menu new game options */
void newGameMenu() {
	//TODO: process players name
	if(enterPlayerName(MAINMENU_ENTERPLAYERNAME))
	{
		initEngineVars(1);
		newGame();

		if (gameEngineLoop()) {
			showCredits();
		}
		
		copyScreen(frontVideoBuffer, workVideoBuffer);
		// TODO: recheck this
		do {
			readKeys();
			do {
				readKeys();
			} while(skipedKey != 0);
		} while(skipIntro != 0);
	}
}

/** Main menu continue game options */
void continueGameMenu() {
	//TODO: get list of saved games
	//if(chooseSave(MAINMENU_CONTINUEGAME))
	{
		initEngineVars(-1); // will load game
		if (gameChapter == 0 && currentSceneIdx == 0) {
			newGame();
		} else {
			newGameVar5 = 0;
			textClipSmall();
			newGameVar4 = 1;
		}

		if (gameEngineLoop()) {
			showCredits();
		}

		copyScreen(frontVideoBuffer, workVideoBuffer);
		// TODO: recheck this
		do {
			readKeys();
			do {
				readKeys();
			} while(skipedKey != 0);
		} while(skipIntro != 0);
	}
}
