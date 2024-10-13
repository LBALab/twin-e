/** @file music.c
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
#include <stdlib.h>
#include <string.h>

#include "music.h"
#include "platform_mixer.h"
#include "platform.h"
#include "utils/xmidi.h"
#include "hqr.h"
#include "resources.h"
#include "main.h"


#define MUSIC_FOLDER	"music"
#define MIDI_FOLDER	    "midi"
#define FADE_MS			500


int32 music_index;
uint8 * midi_ptr;


void music_volume(int32 volume) {
    platform_mixer_music_volume(volume);
}

void music_fade_in(int32 loops, int32 ms) {
    platform_mixer_music_fade_in(loops, ms);
    music_volume(config_file.music_volume);
}

void music_fade_out(int32 ms) {
    platform_mixer_music_fade_out(ms);
    music_volume(config_file.music_volume);
}

void music_play_cd_track(int32 track) {
    if (!config_file.use_cd) {
        return;
    }
    platform_mixer_play_cd_track(track);
}

void music_play_track(int32 track) {
    if (!config_file.sound) {
        return;
    }
    if (track == music_index) {
        return;
    }
    music_index = track;
    music_stop();
    if (config_file.sound > 1) {
        music_play_track_mp3(track + 1);
    } else if (config_file.use_cd) {
        music_play_cd_track(track);
    }
}

void music_stop_cd_track() {
    if (!config_file.use_cd) {
        return;
    }
    platform_mixer_stop_cd_track();
}

void music_stop_track() {
    if (!config_file.sound) {
        return;
    }
    music_fade_out(FADE_MS);
    if (config_file.sound > 1) {
        platform_mixer_stop_music_mp3();
    } else if (config_file.use_cd) {
        music_stop_cd_track();
    }
}

void music_play_midi(int32 midi_index, int32 loop) {
    uint8* dos_midi_ptr;
    int32 midi_size;
    int8 filename[256];

    if (!config_file.sound) {
        return;
    }
    if (midi_index == music_index) {
        return;
    }

    music_stop();
    music_index = midi_index;

    if (config_file.sound > 1) {
        music_fade_in(1, FADE_MS);
        music_volume(config_file.music_volume);
        music_play_midi_mp3(midi_index + 1);
        return;
    } else {
        sprintf(filename, "%s", HQR_MIDI_MI_FILE);
    }

    if (midi_ptr) {
        music_fade_out(FADE_MS / 2);
        music_stop_midi();
    }

    midi_size = hqr_get_entry_alloc(&midi_ptr, filename, midi_index);

    if (config_file.sound == 1) {
        midi_size = convert_to_midi(midi_ptr, midi_size, &dos_midi_ptr);
        free(midi_ptr);
        midi_ptr = dos_midi_ptr;
    }

    music_fade_in(1, FADE_MS);
    music_volume(config_file.music_volume);

    if (platform_mixer_play_music(midi_ptr, midi_size, loop) == -1) {
        printf("Error while playing music: %d \n", midi_index);
    }
}

void music_stop_midi() {
    if (!config_file.sound) {
        return;
    }
    if(platform_mixer_free_music()) {
        if (midi_ptr != NULL) {
            free(midi_ptr);
        }
    }
}

int32 music_init_cdrom() {
    if (!config_file.sound) {
        return 0;
    }
    config_file.use_cd = platform_mixer_init_cdrom(config_file.debug);
    return config_file.use_cd;
}

void music_stop() {
    music_stop_track();
    music_stop_midi();
}

void music_play_track_mp3(int32 track) {
    int8 musfile[256];
    if (config_file.sound == 2) {
        sprintf(musfile, MUSIC_FOLDER "//Track_%0d.mp3", track);
    } else {
        sprintf(musfile, MUSIC_FOLDER "//Track_%0d.ogg", track);
    }
    platform_mixer_stop_music_mp3();
    platform_mixer_music_fade_in(1, FADE_MS);
    music_volume(config_file.music_volume);
    if (platform_mixer_play_music_mp3(musfile) == -1) {
        printf("Error while playing music: %d \n", track);
    } else if (config_file.debug) {
        if (config_file.sound == 2) {
            printf("Playing track (MP3): %s\n", musfile);
        } else {
            printf("Playing track (OGG): %s\n", musfile);
        }
    }
}

void music_play_midi_mp3(int32 midi_index) {
    int8 musfile[256];
    if (config_file.sound == 2) {
        sprintf(musfile, MIDI_FOLDER "//LBA1-%0d.mp3", midi_index);
    } else {
        sprintf(musfile, MIDI_FOLDER "//LBA1-%0d.ogg", midi_index);
    }
    platform_mixer_stop_music_mp3();
    platform_mixer_music_fade_in(1, FADE_MS);
    music_volume(config_file.music_volume);
    if (platform_mixer_play_music_mp3(musfile) == -1) {
        printf("Error while playing midi: %d \n", midi_index);
    } else if (config_file.debug) {
        if (config_file.sound == 2) {
            printf("Playing midi (MP3): %s\n", musfile);
        } else {
            printf("Playing midi (OGG): %s\n", musfile);
        }
    }
}
