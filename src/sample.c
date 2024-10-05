/** @file sample.c
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

#include <stdio.h>
#include <string.h>

#include "platform_mixer.h"
#include "sample.h"
#include "flamovies.h"
#include "main.h"
#include "resources.h"
#include "hqrdepack.h"
#include "movements.h"
#include "grid.h"
#include "collision.h"
#include "text.h"


uint8* samplesTable[NUM_SAMPLES];
uint32 samplesSizeTable[NUM_SAMPLES];
int32 samplesPlaying[NUM_CHANNELS];
int32 samplesPlayingActors[NUM_CHANNELS];


inline void sample_init() {
    memset(samplesPlaying, -1, sizeof(int32) * NUM_CHANNELS);
}

void sample_volume(int32 channel, int32 volume) {
    platform_mixer_volume(channel, volume);
}

void sample_set_position(int32 channel_index, int32 x, int32 y, int32 z) {
    int32 distance;
    distance = Abs(getDistance3D(newCameraX << 9, newCameraY << 8, newCameraZ << 9, x, y, z));
    distance = getAverageValue(0, distance, 10000, 255);
    if (distance > 255) { // don't play it if its to far away
        distance = 255;
    }

    platform_mixer_set_distance(channel_index, distance);
}

void sample_play(int32 index, int32 frequency, int32 repeat, int32 x, int32 y, int32 z, int32 actorIdx) {
    if (config_file.sound) {
        int32 sampSize = 0;
        int32 channel_index = -1;
        uint8* sampPtr;

        sampSize = hqrGetallocEntry(&sampPtr, HQR_SAMPLES_FILE, index);

        // Fix incorrect sample files first byte
        if (*sampPtr != 'C')
            *sampPtr = 'C';

        channel_index = sample_free_channel_index();

        // only play if we have a free channel, otherwise we won't be able to control the sample
        if (channel_index != -1) {
            samplesPlaying[channel_index] = index;
            sample_volume(channel_index, config_file.wave_volume);

            if (actorIdx != -1) {
                sample_set_position(channel_index, x, y, z);
                // save the actor index for the channel so we can check the position
                samplesPlayingActors[channel_index] = actorIdx;
            }

            if (platform_mixer_play(sampPtr, sampSize, channel_index, repeat) == -1)
                printf("Error while playing VOC: Sample %d \n", index);
        }

        free(sampPtr);
    }
}

void sample_resume() {
    if (config_file.sound) {
        platform_mixer_resume();
    }
}

void sample_pause() {
    if (config_file.sound) {
        platform_mixer_pause();
    }
}

void sample_stop_all() {
    if (config_file.sound) {
        memset(samplesPlaying, -1, sizeof(int32) * NUM_CHANNELS);
        platform_mixer_pause();
        platform_mixer_free();
    }
}

int32 sample_get_channel_actor(int32 index) {
    int32 c = 0;
    for (c = 0; c < NUM_CHANNELS; c++) {
        if (samplesPlayingActors[c] == index) {
            return c;
        }
    }
    return -1;
}

int32 sample_get_channel(int32 index) {
    int32 c = 0;
    for (c = 0; c < NUM_CHANNELS; c++) {
        if (samplesPlaying[c] == index) {
            return c;
        }
    }
    return -1;
}

void remove_sample_channel(int32 c) {
    samplesPlaying[c] = -1;
    samplesPlayingActors[c] = -1;
}

void sample_stop(int32 index) {
    if (config_file.sound) {
        int32 stopChannel = sample_get_channel(index);
        if (stopChannel != -1) {
            remove_sample_channel(stopChannel);
            platform_mixer_pause_channel(stopChannel);
            platform_mixer_free();
        }
    }
}

int32 sample_is_playing_channel(int32 channel) {
    if (channel != -1) {
        if(platform_mixer_is_playing(channel)) {
            return 1;
        } else {
            remove_sample_channel(channel);
        }
    }
    return 0;
}

int32 sample_is_playing(int32 index) {
    int32 channel = sample_get_channel(index);
    return sample_is_playing_channel(channel);
}

int32 sample_free_channel_index() {
    int i = 0;
    for (i = 0; i < NUM_CHANNELS; i++) {
        if (samplesPlaying[i] == -1) {
            return i;
        }
    }
    //FIXME if didn't find any, lets free what is not in use
    for (i = 0; i < NUM_CHANNELS; i++) {
        if (samplesPlaying[i] != -1) {
            sample_is_playing_channel(i);
        }
    }
    return -1;
}

void sample_play_fla(int32 index, int32 frequency, int32 repeat, int32 x, int32 y) {
    if (config_file.sound) {
        int32 sampSize = 0;
        int32 channel_index = -1;
        int8 sampfile[256];
        uint8* sampPtr;

        sprintf(sampfile, FLA_DIR "%s",HQR_FLASAMP_FILE);

        sampSize = hqrGetallocEntry(&sampPtr, sampfile, index);

        // Fix incorrect sample files first byte
        if (*sampPtr != 'C')
            *sampPtr = 'C';

        channel_index = sample_free_channel_index();
        if (channel_index != -1) {
            samplesPlaying[channel_index] = index;
        }

        sample_volume(channel_index, config_file.wave_volume);

        if (platform_mixer_play(sampPtr, sampSize, channel_index, repeat) == -1)
            printf("Error while playing VOC: Sample %d \n", index);

        free(sampPtr);
    }
}

void sample_play_vox(int32 index) {
    if (config_file.sound) {
        int32 sampSize = 0;
        int32 channel_index = -1;
        uint8* sampPtr = 0;

        sampSize = hqrGetallocVoxEntry(&sampPtr, currentVoxBankFile, index, voxHiddenIndex);
        
        // Fix incorrect sample files first byte
        if (*sampPtr != 'C') {
            hasHiddenVox = *sampPtr;
            voxHiddenIndex++;
            *sampPtr = 'C';
        }

        channel_index = sample_free_channel_index();
        
        // only play if we have a free channel, otherwise we won't be able to control the sample
        if (channel_index != -1) {
            samplesPlaying[channel_index] = index;
        
            sample_volume(channel_index, config_file.voice_volume - 1);

            if (platform_mixer_play(sampPtr, sampSize, channel_index, 1) == -1)
                printf("Error while playing VOC: Sample %d \n", index);
        }

        free(sampPtr);
    }
}
