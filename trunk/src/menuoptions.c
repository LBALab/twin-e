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

void new_game() {
	int32 tmpFlagDisplayText;

	stop_music();

	tmpFlagDisplayText = cfgfile.FlagDisplayText;
	cfgfile.FlagDisplayText = 1;

#ifndef _DEBUG	
	// intro screen 1 - twinsun
	loadImage( RESSHQR_INTROSCREEN1IMG, 1 );
#endif
	newGameVar4 = 0;
	newGameVar5 = 1;

	init_text_bank(2);
	text_clip_full();
	set_font_cross_color(15);
#ifndef _DEBUG	
	draw_text_fullscreen(150);
	readKeys();

	if (skipIntro != 1) {
		// intro screen 1 - twinsun
		loadImage( RESSHQR_INTROSCREEN2IMG, 1 );
		draw_text_fullscreen(151);
		readKeys();

		if (skipIntro != 1) {
			loadImage( RESSHQR_INTROSCREEN3IMG, 1 );
			draw_text_fullscreen(152);
		}
	}
#endif
	newGameVar5 = 0;
	text_clip_small();
	newGameVar4 = 1;
#ifndef _DEBUG
	fadeToBlack(paletteRGBACustom);
	clearScreen();
	flip();
	
	playMidiMusic(1, 0);
	playMovie(FLA_INTROD);
#endif
	clearScreen();
	flip();


	// set main palette back
	setPalette(paletteRGBA);

	cfgfile.FlagDisplayText = tmpFlagDisplayText;
}

void show_credits() {
	int32 tmpShadowMode, tmpLanguageCDIdx;
	
	showCredits = 1;
	tmpShadowMode = cfgfile.ShadowMode;
	tmpLanguageCDIdx = cfgfile.LanguageCDId;
	cfgfile.ShadowMode = 0;
	cfgfile.LanguageCDId = 0;
	initEngineVars(1);
	currentSceneIdx = 119;
	needChangeScene = 119;
	
	game_engine_loop();

	showCredits = 0;
	cfgfile.ShadowMode = tmpShadowMode;
	cfgfile.LanguageCDId = tmpLanguageCDIdx;
	
	clearScreen();
	flip();

	playMovie(FLA_THEEND);

	clearScreen();
	flip();
	setPalette(paletteRGBA);
}

/** Main menu new game options */
void new_game_menu() {
	//TODO: process players name
	//if(process_player_name(42))
	{
		initEngineVars(1);
		new_game();

		if (game_engine_loop()) {
			show_credits();
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
void continue_game_menu() {
	//TODO: get list of saved games
	//if(chooseSave(21))
	{
		initEngineVars(-1); // will load game
		if (gameChapter == 0 && currentSceneIdx == 0) {
			new_game();
		} else {
			newGameVar5 = 0;
			text_clip_small();
			newGameVar4 = 1;
		}

		if (game_engine_loop()) {
			show_credits();
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
