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

#include "sys.h"
#include "debug.h"

/** Work video buffer */
uint8 *workVideoBuffer;
/** Main game video buffer */
uint8 *frontVideoBuffer;
/** Auxiliar game video buffer */
uint8 *frontVideoBufferbis;

/** temporary screen table */
int32 screenLockupTable[2000];

/** Pressed key map - scanCodeTab1 */
uint8 pressedKeyMap[29];
/** Pressed key char map - scanCodeTab2 */
uint16 pressedKeyCharMap[31];

/** Skipped key - key1 */
extern int16 skipedKey;
/** Pressed key - printTextVar12 */
extern int16 pressedKey;
/** Skip intro variable */
extern int16 skipIntro;
/** Current key value */
extern int16 currentKey;
/** Auxiliar key value */
extern int16 key;

int16 leftMouse;
int16 rightMouse;

/** Main SDL screen surface buffer */
extern SDL_Surface *screen;

/** Close everything in the game */
void sdl_close();

/** SDL initializer
	@return SDL init state */
int sdl_initialize();

/** Frames per second sdl delay */
void fps_cycles(int32 fps);

/** Deplay certain seconds till proceed 
	@param time time in seconds to delay */
void delay(uint32 time);

/** Set a new palette in the SDL screen buffer
	@param palette palette to set */
void set_palette(uint8 * palette);

/** Fade screen from black to white */
void fade_black_2_white();

/** Blit surface in the screen */
void flip();

/** Blit surface in the screen in a determinate area
	@param left left position to start copy
	@param top top position to start copy
	@param right right position to start copy
	@param bottom bottom position to start copy */
void copy_block_phys(int32 left, int32 top, int32 right, int32 bottom);

/** Create SDL screen surface
	@param buffer screen buffer to blit surface
	@param width screen width size
	@param height screen height size */
void init_screen_buffer(uint8 *buffer, int32 width, int32 height);

/** Cross fade feature
	@param buffer screen buffer
	@param palette new palette to cross fade*/
void cross_fade(uint8 *buffer, uint8 *palette);

/** Switch between window and fullscreen modes */
void toggle_fullscreen();

/** Handle keyboard pressed keys */
void read_keys();

/** Display SDL text in screen
	@param X X coordinate in screen
	@param Y Y coordinate in screen
	@param string text to display
	@param center if the text should be centered accoding with the giving positions */
void ttf_draw_text(int32 X, int32 Y, int8 *string, int32 center);

/** Gets SDL mouse positions
	@param mouseData structure that contains mouse position info */
void get_mouse_positions(MouseStatusStruct *mouseData);

#endif
