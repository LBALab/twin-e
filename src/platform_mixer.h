/** @file platform_mixer.h
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

#ifndef PLATFORM_MIXER_H
#define PLATFORM_MIXER_H

#include "sys.h"


void platform_mixer_init(int32 sound_config);
void platform_mixer_close();

void platform_mixer_volume(int32 channel, int32 volume);
int32 platform_mixer_is_playing(int32 channel);

void platform_mixer_set_distance(int32 channel_index, int32 distance);

void platform_mixer_load(uint8 *sample_ptr, int32 samples_size);
void platform_mixer_free();
int32 platform_mixer_play(uint8 *sample_ptr, int32 samples_size, int32 channel_index, int32 loop);

void platform_mixer_resume();
void platform_mixer_pause();
void platform_mixer_pause_channel(int32 channel_index);

void platform_mixer_music_fade_in(int32 loop, int32 ms);
void platform_mixer_music_volume(int32 volume);
void platform_mixer_music_fade_out(int32 ms);

void platform_mixer_load_music(uint8 *music_ptr, int32 music_size);
int32 platform_mixer_free_music();
int32 platform_mixer_play_music(uint8 *music_ptr, int32 music_size, int32 loop);

void platform_mixer_play_cd_track(int32 track);
void platform_mixer_stop_cd_track();
int32 platform_mixer_init_cdrom(int32 config_debug);


#endif
