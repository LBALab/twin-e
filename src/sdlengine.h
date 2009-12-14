/** @file sdlengine.h
	@brief 
	This file contains SDL engine routines
	
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

#ifndef SCREEN_H
#define SCREEN_H

#include <SDL/SDL.h>

#include "debug.h"

/** Work video buffer */
unsigned char *workVideoBuffer;
/** Main game video buffer */
unsigned char *frontVideoBuffer;
/** Auxiliar game video buffer */
unsigned char *frontVideoBufferbis;

/** temporary screen table */
int screenLockupTable[2000];

/** Pressed key map - scanCodeTab1 */
unsigned char pressedKeyMap[29];
/** Pressed key char map - scanCodeTab2 */
unsigned short int pressedKeyCharMap[31];

/** Skipped key - key1 */
extern short int skipedKey;
/** Pressed key - printTextVar12 */
extern short int pressedKey;
/** Skip intro variable */
extern short int skipIntro;
/** Current key value */
extern short int currentKey;
/** Auxiliar key value */
extern short int key;

short int leftMouse;
short int rightMouse;

/** Main SDL screen surface buffer */
extern SDL_Surface *screen;

/** Close everything in the game */
void sdl_close();

/** SDL initializer
	@return SDL init state */
int sdl_initialize();

/** Frames per second sdl delay */
void fps_cycles(int fps);

/** Deplay certain seconds till proceed 
	@param time time in seconds to delay */
void delay(unsigned int time);

/** Set a new palette in the SDL screen buffer
	@param palette palette to set */
void set_palette(unsigned char * palette);

/** Fade screen from black to white */
void fade_black_2_white();

/** Blit surface in the screen */
void flip();

/** Blit surface in the screen in a determinate area
	@param left left position to start copy
	@param top top position to start copy
	@param right right position to start copy
	@param bottom bottom position to start copy */
void copy_block_phys(int left, int top, int right, int bottom);

/** Create SDL screen surface
	@param buffer screen buffer to blit surface
	@param width screen width size
	@param height screen height size */
void init_screen_buffer(unsigned char *buffer, int width, int height);

/** Cross fade feature
	@param buffer screen buffer
	@param palette new palette to cross fade*/
void cross_fade(unsigned char *buffer, unsigned char *palette);

/** Switch between window and fullscreen modes */
void toggle_fullscreen();

/** Handle keyboard pressed keys */
void read_keys();

/** Display SDL text in screen
	@param X X coordinate in screen
	@param Y Y coordinate in screen
	@param string text to display
	@param center if the text should be centered accoding with the giving positions */
void ttf_draw_text(int X, int Y, char *string, int center);

/** Gets SDL mouse positions
	@param mouseData structure that contains mouse position info */
void get_mouse_positions(MouseStatusStruct *mouseData);

#endif
