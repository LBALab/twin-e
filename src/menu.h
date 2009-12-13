/** @file menu.h
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
*/

#ifndef MENU_H
#define MENU_H

#include "sdlengine.h"

int currMenuTextIndex;
int currMenuTextBank;
char currMenuTextBuffer[256];

extern short int OptionsMenuSettings[];

/** Plasma Effect pointer to file content: RESS.HQR:51 */
extern unsigned char *plasmaEffectPtr;

/** Process the plasma effect
	@param top top height where the effect will be draw in the front buffer
	@param color plasma effect start color */
void process_plasma_effect(int top, int color);
/** Draw the entire button box 
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button */
void draw_button_box(int left, int top, int right, int bottom);
/** Draws inside buttons transparent area
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button
	@param colorAdj index to adjust the transparent box color */
void draw_transparent_box(int left, int top, int right, int bottom, int colorAdj);
/** Used to run the main menu */
void main_menu();
/** Used to run the in-game give-up menu */
int giveup_menu();
/** Used to run the options menu */
int options_menu();

int process_giveup_menu();
void process_options_menu(short int pKey);

#endif
