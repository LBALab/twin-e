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
#ifndef MACOSX
#include <SDL/SDL_mixer.h>
#else
#include <SDL_mixer/SDL_mixer.h>
#endif

#include "platform_mixer.h"


#define ORIGINAL_GAME_FREQUENCY		11025
#define HIGH_QUALITY_FREQUENCY		44100
#define NUM_CD_TRACKS	10


// samples
Mix_Chunk *chunk;
// music
SDL_CD *cdrom;
Mix_Music *current_track;
const int8 *cd_name;


void platform_mixer_init(int32 sound_config) {
    int32 freq = ORIGINAL_GAME_FREQUENCY;
    if (sound_config > 1) {
        freq = HIGH_QUALITY_FREQUENCY;
    }

    if (Mix_OpenAudio(freq, AUDIO_S16, 2, 256) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        exit(1);
    }
    
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) {
        printf("Mix_Init: %s\n", Mix_GetError());
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
    chunk = Mix_LoadWAV_RW(rw, 1);
}

inline void platform_mixer_free() {
    Mix_FreeChunk(chunk);
    chunk = NULL;
}

int32 platform_mixer_play(uint8 *sample_ptr, int32 samples_size, int32 channel_index, int32 loop) {
    platform_mixer_load(sample_ptr, samples_size);
    return Mix_PlayChannel(channel_index, chunk, 0);
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

void platform_mixer_music_fade_in(int32 loop, int32 ms) {
    Mix_FadeInMusic(current_track, loop, ms);
}

void platform_mixer_music_volume(int32 volume) {
    // div 2 because LBA use 255 range and SDL_mixer use 128 range
    Mix_VolumeMusic(volume / 2);
}

void platform_mixer_music_fade_out(int32 ms) {
    while (!Mix_FadeOutMusic(ms) && Mix_PlayingMusic()) {
        SDL_Delay(100);
    }
    Mix_HaltMusic();
    Mix_RewindMusic();
}

inline void platform_mixer_load_music(uint8 *music_ptr, int32 music_size) {
    SDL_RWops *rw = SDL_RWFromMem(music_ptr, music_size);
    current_track = Mix_LoadMUS_RW(rw);
}

int32 platform_mixer_free_music() {
    if (current_track != NULL) {
        Mix_FreeMusic(current_track);
        current_track = NULL;
        return 1;
    }
    return 0;
}

int32 platform_mixer_play_music(uint8 *music_ptr, int32 music_size, int32 loop) {
    platform_mixer_load_music(music_ptr, music_size);
    return Mix_PlayMusic(current_track, loop);
}


int32 platform_mixer_play_music_mp3(int8 *music_file) {
    int error_code = 0;
    current_track = Mix_LoadMUS(music_file);
    if (current_track == NULL)
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
    error_code = Mix_PlayMusic(current_track, -1);
    if (error_code == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
    }
    return error_code;
}

void platform_mixer_stop_music_mp3() {
    if (current_track != NULL) {
        Mix_FreeMusic(current_track);
        current_track = NULL;
    }
}

void platform_mixer_play_cd_track(int32 track) {
    if (cdrom->numtracks == 10) {
        if (CD_INDRIVE(SDL_CDStatus(cdrom)))
            SDL_CDPlayTracks(cdrom, track, 0, 1, 0);
    }
}

void platform_mixer_stop_cd_track() {
    if (cdrom != NULL) {
        SDL_CDStop(cdrom);
    }
}

int32 platform_mixer_init_cdrom(int32 config_debug) {
    int32 num_drives;
    int32 num_cd;

    num_drives = SDL_CDNumDrives();
    
    if (config_debug) {
        printf("Found %d CDROM devices\n", num_drives);
    }

    if (!num_drives) {
        fprintf(stderr, "No CDROM devices available\n");
        return 0;
    }

    for (num_cd = 0; num_cd < num_drives; num_cd++) {
        cd_name = SDL_CDName(num_cd);
        if (config_debug) {
            printf("Testing drive %s\n", cd_name);
        }
        cdrom = SDL_CDOpen(num_cd);
        if (!cdrom) {
            if (config_debug) {
                fprintf(stderr, "Couldn't open CD drive: %s\n\n", SDL_GetError());
            }
        } else {
            SDL_CDStatus(cdrom);
            if (cdrom->numtracks == NUM_CD_TRACKS) {
                printf("Assuming that it is LBA cd... %s\n\n", cd_name);
                return 1;
            }
        }
        // adeline cd not found
        SDL_CDClose(cdrom);
    }

    cdrom = NULL;
    printf("Can't find LBA CD!\n\n");

    return 0;
}
