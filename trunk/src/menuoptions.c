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

void intro_text_clip_full() { // newGame2
	dialTextBoxLeft = 8;
	dialTextBoxTop = 8;
	dialTextBoxRight = 631;

	dialTextBoxBottom = 471;
	dialTextBoxParam1 = 11;
	dialTextBoxParam2 = 607;
}

void intro_text_clip_small() { // newGame4
	dialTextBoxLeft = 16;
	dialTextBoxTop = 334;
	dialTextBoxRight = 623;
	dialTextBoxBottom = 463;
	dialTextBoxParam1 = 3;
	dialTextBoxParam2 = 591;
}

void new_game() {
	int32 tmpFlagDisplayText;

	stop_music();

	tmpFlagDisplayText = cfgfile.FlagDisplayText;
	cfgfile.FlagDisplayText = 1;

	// intro screen 1 - twinsun
	load_image( RESSHQR_INTROSCREEN1IMG, 1 );

	newGameVar4 = 0;
	newGameVar5 = 1;

	init_text_bank(2);
	intro_text_clip_full();
	set_font_cross_color(15);
	
	draw_text_fullscreen(150);
	read_keys();

	if (skipIntro != 1) {
		// intro screen 1 - twinsun
		load_image( RESSHQR_INTROSCREEN2IMG, 1 );
		draw_text_fullscreen(151);
		read_keys();

		if (skipIntro != 1) {
			load_image( RESSHQR_INTROSCREEN3IMG, 1 );
			draw_text_fullscreen(152);
		}
	}

	newGameVar5 = 0;
	intro_text_clip_small();
	newGameVar4 = 1;

	fade_2_black(paletteRGBACustom);
	clear_screen();
	flip();
	
	play_midi_music(1, 0);
	play_movie(FLA_INTROD);

	clear_screen();
	flip();

	convert_pal_2_RGBA(palette, paletteRGBA);
	set_palette(paletteRGBA);

	cfgfile.FlagDisplayText = tmpFlagDisplayText;
}

/** Main menu new game options */
void new_game_menu() {
	//TODO: process players name
	//if(process_player_name(42))
	{
		// TODO: create new saved game
		init_engine_vars(1);
		new_game();
		if (game_engine_loop()) {
			//TODO: Show credit sequence
		}
	}
}
