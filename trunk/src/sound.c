/** @file sound.c
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

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "sound.h"
#include "movies.h"
#include "main.h"
#include "resources.h"
#include "hqrdepack.h"


/** SDL_Mixer channels */
int32 channel;
/** Samples chunk variable */
Mix_Chunk *sample;

/** Sample volume
	@param channel sample channel
	@param volume sample volume number */
void sample_volume(int32 channel, int32 volume) {
	Mix_Volume(channel, volume / 2);
}

/** Play FLA movie samples
	@param index sample index under flasamp.hqr file
	@param frequency frequency used to play the sample
	@param repeat number of times to repeat the sample
	@param x unknown x variable
	@param y unknown y variable */
void play_fla_sample(int32 index, int32 frequency, int32 repeat, int32 x, int32 y) {
	if (cfgfile.Sound) {
		int32 sampSize = 0;
		int8 sampfile[256];
		SDL_RWops *rw;
		uint8* sampPtr;

		sprintf(sampfile, FLA_DIR "//%s", HQR_FLASAMP_FILE);

		sampSize = hqr_getalloc_entry(&sampPtr, sampfile, index);

		// Fix incorrect sample files first byte
		if (*sampPtr != 'C')
			*sampPtr = 'C';

		rw = SDL_RWFromMem(sampPtr, sampSize);
		sample = Mix_LoadWAV_RW(rw, 1);

		sample_volume(-1, cfgfile.WaveVolume);

		if (Mix_PlayChannel(-1, sample, repeat - 1) == -1)
			printf("Error while playing VOC: Sample %d \n", index);

		if (cfgfile.Debug)
			printf("Playing VOC: Sample %d\n", index);

		free(sampPtr);
	}
}

/** Play samples
	@param index sample index under flasamp.hqr file
	@param frequency frequency used to play the sample
	@param repeat number of times to repeat the sample
	@param x unknown x variable
	@param y unknown y variable
	@param z unknown z variable */
void play_sample(int32 index, int32 frequency, int32 repeat, int32 x, int32 y, int32 z) {
	if (cfgfile.Sound) {
		int32 sampSize = 0;
		SDL_RWops *rw;
		uint8* sampPtr;

		sampSize = hqr_getalloc_entry(&sampPtr, HQR_SAMPLES_FILE, index);

		// Fix incorrect sample files first byte
		if (*sampPtr != 'C')
			*sampPtr = 'C';

		rw = SDL_RWFromMem(sampPtr, sampSize);
		sample = Mix_LoadWAV_RW(rw, 1);

		sample_volume(-1, cfgfile.WaveVolume);

		if (Mix_PlayChannel(-1, sample, repeat - 1) == -1)
			printf("Error while playing VOC: Sample %d \n", index);

		if (cfgfile.Debug)
			printf("Playing VOC: Sample %d\n", index);

		free(sampPtr);
	}
}

/** Resume samples */
void resume_samples() {
	if (cfgfile.Sound) {
		Mix_Resume(-1);
		if (cfgfile.Debug)
			printf("Resume VOC samples\n");
	}
}

/** Pause samples */
void pause_samples() {
	if (cfgfile.Sound) {
		Mix_HaltChannel(-1);
		if (cfgfile.Debug)
			printf("Pause VOC samples\n");
	}
}

/** Stop samples */
void stop_samples() {
	if (cfgfile.Sound) {
		Mix_HaltChannel(-1);
		//clean up
		Mix_FreeChunk(sample);
		sample = NULL; //make sure we free it
		if (cfgfile.Debug)
			printf("Stop VOC samples\n");
	}
}
