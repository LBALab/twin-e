/** @file platform.h
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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL/SDL.h>

#include "sys.h"
#include "debug.h"


int platform_initialize();
void platform_close();

uint32 platform_tick();
void platform_fps_cycles(int32 fps);
void platform_delay(uint32 time);
void platform_delay_skip(uint32 time);

void platform_set_palette(uint8 * palette);
void platform_fade_black_to_white();

void platform_flip();
void platform_copy_block_phys(int32 left, int32 top, int32 right, int32 bottom);
void platform_init_screen_buffer(uint8 *buffer, int32 width, int32 height);
void platform_cross_fade(uint8 *buffer, uint8 *palette);
void platform_toggle_fullscreen();

void platform_handle_input();

void platform_get_mouse_positions(MouseStatusStruct *mouseData);
void platform_draw_text(int32 X, int32 Y, int8 *string, int32 center);

#endif
