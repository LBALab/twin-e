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

#include <SDL/SDL.h>
#ifndef MACOSX
#include <SDL/SDL_mixer.h>
#else
#include <SDL_mixer/SDL_mixer.h>
#endif

#include "music.h"
#include "main.h"
#include "platform.h"
#include "hqr.h"
#include "resources.h"
#include "xmidi.h"

/** MP3 music folder */
#define MUSIC_FOLDER	"music"
/** LBA1 default number of tracks */
#define NUM_CD_TRACKS	10
/** Number of miliseconds to fade music */
#define FADE_MS			500

/** SDL CD variable interface */
SDL_CD *cdrom;
/** CD drive letter */
const int8 *cdname;

/** Track number of the current playing music */
int32 currentMusic;

/** SDL_Mixer track variable interface */
Mix_Music *current_track;

/** Auxiliar midi pointer to  */
uint8 * midiPtr;


/** Music volume
    @param current volume number */
void musicVolume(int32 volume) {
    // div 2 because LBA use 255 range and SDL_mixer use 128 range
    Mix_VolumeMusic(volume / 2);
}

/** Fade music in
    @param loops number of*/
void musicFadeIn(int32 loops, int32 ms) {
    Mix_FadeInMusic(current_track, loops, ms);
    musicVolume(config_file.music_volume);
}

/** Fade music out
    @param ms number of miliseconds to fade*/
void musicFadeOut(int32 ms) {
    while (!Mix_FadeOutMusic(ms) && Mix_PlayingMusic()) {
        platform_delay(100);
    }
    Mix_HaltMusic();
    Mix_RewindMusic();
    musicVolume(config_file.music_volume);
}


/** Play CD music
    @param track track number to play */
void playTrackMusicCd(int32 track) {
    if (!config_file.use_cd) {
        return;
    }

    if (cdrom->numtracks == 10) {
        if (CD_INDRIVE(SDL_CDStatus(cdrom)))
            SDL_CDPlayTracks(cdrom, track, 0, 1, 0);
    }
}

/** Stop CD music */
void stopTrackMusicCd() {
    if (!config_file.use_cd) {
        return;
    }

    if (cdrom != NULL) {
        SDL_CDStop(cdrom);
    }
}

/** Generic play music, according with settings it plays CD or MP3 instead
    @param track track number to play */
void playTrackMusic(int32 track) {
    if (!config_file.sound) {
        return;
    }
    
    if (track == currentMusic)
        return;
    currentMusic = track;

    stopMusic();
    playTrackMusicCd(track);
}

/** Generic stop music according with settings */
void stopTrackMusic() {
    if (!config_file.sound) {
        return;
    }
    
    musicFadeOut(FADE_MS);
    stopTrackMusicCd();
}

/** Play MIDI music
    @param midiIdx music index under mini_mi_win.hqr*/
void playMidiMusic(int32 midiIdx, int32 loop) {
    uint8* dos_midi_ptr;
    int32 midiSize;
    int8 filename[256];
    SDL_RWops *rw;

    if (!config_file.sound) {
        return;
    }

    if (midiIdx == currentMusic) {
        return;
    }

    stopMusic();
    currentMusic = midiIdx;

    if (config_file.midi_type == 0)
        sprintf(filename, "%s", HQR_MIDI_MI_DOS_FILE);
    else
        sprintf(filename, "%s", HQR_MIDI_MI_WIN_FILE);

    if (midiPtr) {
        musicFadeOut(FADE_MS / 2);
        stopMidiMusic();
    }

    midiSize = hqr_get_entry_alloc(&midiPtr, filename, midiIdx);

    if (config_file.sound == 1 && config_file.midi_type == 0) {
        midiSize = convert_to_midi(midiPtr, midiSize, &dos_midi_ptr);
        free(midiPtr);
        midiPtr = dos_midi_ptr;
    }

    rw = SDL_RWFromMem(midiPtr, midiSize);

    current_track = Mix_LoadMUS_RW(rw);

    musicFadeIn(1, FADE_MS);

    musicVolume(config_file.music_volume);

    if (Mix_PlayMusic(current_track, loop) == -1)
        printf("Error while playing music: %d \n", midiIdx);
}

/** Stop MIDI music */
void stopMidiMusic() {
    if (!config_file.sound) {
        return;
    }
    
    if (current_track != NULL) {
        Mix_FreeMusic(current_track);
        current_track = NULL;
        if (midiPtr != NULL)
            free(midiPtr);
    }
}

/** Initialize CD-Rom */
int initCdrom() {
    int32 numOfCDROM;
    int32 cdNum;

    if (!config_file.sound) {
        return 0;
    }

    numOfCDROM = SDL_CDNumDrives();
    
    if (config_file.debug)
        printf("Found %d CDROM devices\n", numOfCDROM);

    if (!numOfCDROM) {
        fprintf(stderr, "No CDROM devices available\n");
        return 0;
    }

    for (cdNum = 0; cdNum < numOfCDROM; cdNum++) {
        cdname = SDL_CDName(cdNum);
        if (config_file.debug)
            printf("Testing drive %s\n", cdname);
        cdrom = SDL_CDOpen(cdNum);
        if (!cdrom) {
            if (config_file.debug)
                fprintf(stderr, "Couldn't open CD drive: %s\n\n", SDL_GetError());
        } else {
            SDL_CDStatus(cdrom);
            if (cdrom->numtracks == NUM_CD_TRACKS) {
                printf("Assuming that it is LBA cd... %s\n\n", cdname);
                cd_directory = "LBA";
                config_file.use_cd = 1;
                return 1;
            }
        }
        // not found the right CD
        config_file.use_cd = 0;
        SDL_CDClose(cdrom);
    }

    cdrom = NULL;

    printf("Can't find LBA CD!\n\n");

    return 0;
}

/** Stop MIDI and Track music */
void stopMusic() {
    stopTrackMusic();
    stopMidiMusic();
}
