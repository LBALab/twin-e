/** @file sdlengine.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#ifndef MACOSX
#include <SDL/SDL_mixer.h>
#else
#include <SDL_mixer/SDL_mixer.h>
#endif

#include "platform_mixer.h"


#define ORIGINAL_GAME_FREQUENCY		11025
#define HIGH_QUALITY_FREQUENCY		44100


struct sample_s {
    int32 channel;
    int32 channelIdx;
    SDL_RWops *rw;
    Mix_Chunk *chunk;
} sample;

// struct samples_s sample;

void platform_mixer_init(int32 sound_config) {
    int32 freq = ORIGINAL_GAME_FREQUENCY;
    if (sound_config > 1) {
        freq = HIGH_QUALITY_FREQUENCY;
    }

    if (Mix_OpenAudio(freq, AUDIO_S16, 2, 256) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        exit(1);
    }

    Mix_AllocateChannels(32);
}

inline void platform_mixer_close() {
    Mix_CloseAudio();
}

inline void platform_mixer_volume(int32 channel, int32 volume) {
    Mix_Volume(channel, volume / 2);
}

inline int32 platform_mixer_is_playing(int32 channel) {
    return Mix_Playing(channel);
}

inline void platform_mixer_set_distance(int32 channel_index, int32 distance) {
    Mix_SetDistance(channel_index, distance);
}

inline void platform_mixer_load(uint8 *sample_ptr, int32 samples_size) {
    SDL_RWops *rw = SDL_RWFromMem(sample_ptr, samples_size);
    sample.chunk = Mix_LoadWAV_RW(rw, 1);
}

inline void platform_mixer_free() {
    Mix_FreeChunk(sample.chunk);
    sample.chunk = NULL;
}

int32 platform_mixer_play(uint8 *sample_ptr, int32 samples_size, int32 channel_index, int32 repeat) {
    platform_mixer_load(sample_ptr, samples_size);
    return Mix_PlayChannel(channel_index, sample.chunk, 0);
}

inline void platform_mixer_resume() {
    Mix_Resume(-1);
}

inline void platform_mixer_pause() {
    Mix_HaltChannel(-1);
}

inline void platform_mixer_pause_channel(int32 channel) {
    Mix_HaltChannel(channel);
}

