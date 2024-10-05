/** @file sdlengine.h
    @brief
    This file contains SDL engine routines

    TwinEngine: a Little Big Adventure engine
    
    Copyright (C) 2002 The TwinEngine team

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef PLATFORM_SDL_H
#define PLATFORM_SDL_H

#include <SDL/SDL.h>

#include "sys.h"
#include "debug.h"

extern SDL_Surface *screen;


int sdl_initialize();
void sdl_close();

uint32 tick();
void fps_cycles(int32 fps);
void sdl_delay(uint32 time);
void sdl_delay_skip(uint32 time);

void set_palette(uint8 * palette);
void fade_black_to_white();

void flip();
void copy_block_phys(int32 left, int32 top, int32 right, int32 bottom);
void init_screen_buffer(uint8 *buffer, int32 width, int32 height);
void cross_fade(uint8 *buffer, uint8 *palette);
void toggle_fullscreen();

void handle_input();

void get_mouse_positions(MouseStatusStruct *mouseData);
void ttf_draw_text(int32 X, int32 Y, int8 *string, int32 center);

#endif
