/** @file sample.h
    @brief
    This file contains music playing routines

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

#ifndef SAMPLE_H
#define SAMPLE_H

#include "sys.h"


#define NUM_SAMPLES 243
#define NUM_CHANNELS 32


void sample_init();

int32 sample_is_playing(int32 index);
void sample_play(int32 index, int32 frequency, int32 repeat, int32 x, int32 y, int32 z, int32 actorIdx);
void sample_pause();
void sample_resume();
void sample_stop_all();
void sample_stop(int32 index);
void sample_volume(int32 channel, int32 volume);
void sample_set_position(int32 channelIdx, int32 x, int32 y, int32 z);

int32 sample_get_channel_actor(int32 index);
int32 sample_get_channel(int32 index);
int32 sample_free_channel_index();
void remove_sample_channel(int32 index);

void sample_play_fla(int32 index, int32 frequency, int32 repeat, int32 x, int32 y);
void sample_play_vox(int32 index);

#endif
