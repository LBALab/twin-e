/** @file music.c
	@brief
	This file contains music playing routines

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "music.h"
#include "main.h"
#include "sdlengine.h"
#include "hqrdepack.h"
#include "resources.h"

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

/** SDL_Mixer track variable interface */
Mix_Music *current_track;

/** Auxiliar midi pointer to  */
uint8 * midiPtr;


/** Music volume
	@param current volume number */
void music_volume(int32 volume) {
	// div 2 because LBA use 255 range and SDL_mixer use 128 range
	Mix_VolumeMusic(volume / 2);
}

/** Fade music in
	@param loops number of*/
void music_fade_in(int32 loops, int32 ms) {
	Mix_FadeInMusic(current_track, loops, ms);
}

/** Fade music out
	@param ms number of miliseconds to fade*/
void music_fade_out(int32 ms) {
	while (!Mix_FadeOutMusic(ms) && Mix_PlayingMusic()) {
		SDL_Delay(100);
	}
	Mix_RewindMusic();
}


/** Play CD music
	@param track track number to play */
void play_track_music_cd(int32 track) {
	if (cdrom->numtracks == 10) {
		if (CD_INDRIVE(SDL_CDStatus(cdrom)))
			SDL_CDPlayTracks(cdrom, track, 0, 1, 0);

		/*if (cfgfile.Debug)
			printf("Playing track (CD): %d\n", track);*/
	}
}

/** Stop CD music */
void stop_track_music_cd() {
	if (cfgfile.UseCD) {
		if (cdrom != NULL) {
			SDL_CDStop(cdrom);

			/*if (cfgfile.Debug)
				printf("Stop CD track\n");*/
		}
	}
}

/** Play MP3 music
	@param track track number to play */
void play_track_music_mp3(int32 track) {
	int8 musfile[256];
	if (cfgfile.Sound == 2)
		sprintf(musfile, MUSIC_FOLDER "//%d.mp3", track);
	else
		sprintf(musfile, MUSIC_FOLDER "//%d.ogg", track);

	stop_track_music_mp3();

	music_fade_in(1, FADE_MS);

	current_track = Mix_LoadMUS(musfile);
	music_volume(cfgfile.MusicVolume);
	Mix_PlayMusic(current_track, -1);

	/*if (cfgfile.Debug) {
		if (cfgfile.Sound == 2)
			printf("Playing track (MP3): %s\n", musfile);
		else
			printf("Playing track (OGG): %s\n", musfile);
	}*/
}

/** Stop MP3 music */
void stop_track_music_mp3() {
	if (current_track != NULL) {
		Mix_FreeMusic(current_track);
		current_track = NULL;

		/*if (cfgfile.Debug)
			printf("Stop MP3 track\n");*/
	}
}

/** Generic play music, according with settings it plays CD or MP3 instead
	@param track track number to play */
void play_track_music(int32 track) {
	if (cfgfile.Sound) {
		if (track == currentMusic)
			return;
		currentMusic = track;

		stop_music();

		if (cfgfile.Sound > 1)
			play_track_music_mp3(track);
		else if (cfgfile.UseCD)
			play_track_music_cd(track);
	}
}

/** Generic stop music according with settings */
void stop_track_music() {
	if (cfgfile.Sound) {
		music_fade_out(FADE_MS);

		if (cfgfile.Sound > 1)
			stop_track_music_mp3();
		else if (cfgfile.UseCD)
			stop_track_music_cd();
	}
}

/** Play MIDI music
	@param midiIdx music index under mini_mi_win.hqr*/
void play_midi_music(int32 midiIdx, int32 loop) {
	if (cfgfile.Sound) {
		int32 midiSize;
		int8 filename[256];
		SDL_RWops *rw;

		if (midiIdx == currentMusic)
			return;

		stop_music();
		currentMusic = midiIdx;

		if (cfgfile.Sound > 1) {
			if (cfgfile.Sound == 2)
				sprintf(filename, "%s", HQR_MIDI_MI_WIN_MP3_FILE);
			else
				sprintf(filename, "%s", HQR_MIDI_MI_WIN_OGG_FILE);
		} else
			sprintf(filename, "%s", HQR_MIDI_MI_WIN_FILE);

		if (midiPtr) {
			music_fade_out(FADE_MS / 2);
			stop_midi_music();
			//free(midiPtr);
		}

		midiSize = hqr_getalloc_entry(&midiPtr, filename, midiIdx);

		rw = SDL_RWFromMem(midiPtr, midiSize);

		current_track = Mix_LoadMUS_RW(rw);

		music_fade_in(1, FADE_MS);

		music_volume(cfgfile.MusicVolume);

		if (Mix_PlayMusic(current_track, loop) == -1)
			printf("Error while playing music: %d \n", midiIdx);

		/*if (cfgfile.Debug) {
			if (cfgfile.Sound > 1) {
				if (cfgfile.Sound == 2)
					printf("Playing Music (MP3): %d\n", midiIdx);
				else
					printf("Playing Music (OGG): %d\n", midiIdx);
			} else
				printf("Playing Music (MIDI): %d\n", midiIdx);
		}*/
	}
}

/** Stop MIDI music */
void stop_midi_music() {
	if (cfgfile.Sound) {
		if (current_track != NULL) {
			Mix_FreeMusic(current_track);
			current_track = NULL;
			if (midiPtr != NULL)
				free(midiPtr);

			/*if (cfgfile.Debug)
				printf("Stop MIDI music\n");*/
		}
	}
}

/** Initialize CD-Rom */
int init_cdrom() {
	if (cfgfile.Sound) {
		int32 numOfCDROM;
		int32 cdNum;

		numOfCDROM = SDL_CDNumDrives();
		if (cfgfile.Debug)
			printf("Found %d CDROM devices\n", numOfCDROM);

		if (!numOfCDROM) {
			fprintf(stderr, "No CDROM devices available\n");
			return 0;
		}

		for (cdNum = 0; cdNum < numOfCDROM; cdNum++) {
			cdname = SDL_CDName(cdNum);
			if (cfgfile.Debug)
				printf("Testing drive %s\n", cdname);
			cdrom = SDL_CDOpen(cdNum);
			if (!cdrom) {
				if (cfgfile.Debug)
					fprintf(stderr, "Couldn't open CD drive: %s\n\n", SDL_GetError());
			} else {
				SDL_CDStatus(cdrom);
				if (cdrom->numtracks == NUM_CD_TRACKS) {
					printf("Assuming that it is LBA cd... %s\n\n", cdname);
					cdDir = "LBA";
					cfgfile.UseCD = 1;
					return 1;
				}
			}
			// not found the right CD
			cfgfile.UseCD = 0;
			SDL_CDClose(cdrom);
		}

		cdrom = NULL;

		printf("Can't find LBA CD!\n\n");
	}
	return 0;
}

/** Stop MIDI and Track music */
void stop_music() {
	stop_track_music();
	stop_midi_music();
}
