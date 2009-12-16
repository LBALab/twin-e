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
#include "dialogues.h"

void new_game()
{
	/* Intro screen */
	/*load_image( RESSHQR_INTROSCREEN1IMG, 1 );
	init_dialogue_bank(2);
	display_dialogue_fullscreen(150, 240);*/
	
	//delay(1000);
	/*for(;;)
	{
		read_keys();
	}	*/
	/* Engine init */
	init_vars();
}

/** Main menu new game options */
void new_game_menu()
{
	//TODO: process players name
	//if(process_player_name(42))
	{
		// TODO: create new saved game
		init_engine_vars(1);
		new_game();
		if(game_engine_loop())
		{
			//TODO: Show credit sequence
		}
	}
}
