/** @file movies.c
	@brief
	This file contains movies routines

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

//#include <SDL/SDL_ffmpeg.h>

#include "movies.h"
#include "images.h"
#include "sdlengine.h"
#include "main.h"
#include "sound.h"
#include "filereader.h"
#include "lbaengine.h"
#include "keyboard.h"

/** Config movie types */
#define CONF_MOVIE_NONE    0
#define CONF_MOVIE_FLA     1
#define CONF_MOVIE_FLAWIDE 2
#define CONF_MOVIE_FLAPCX  3
#define CONF_MOVIE_AVI     4

/** FLA movie extension */
#define FLA_EXT ".fla"
/** Common movie directory */
#define MOVIES_DIR "movies//"
/** Common movie extension */
#define MOVIES_EXT ".avi"

/** Load palette frame opcode */
#define LOAD_PALETTE	0
/** Fade frame opcode */
#define FADE			1
/** Play sample frame opcode */
#define PLAY_SAMPLE		2
/** Stop sample frame opcode */
#define STOP_SAMPLE		4
/** Draw delta frame opcode */
#define DELTA_FRAME		5
/** Draw key frame opcode */
#define KEY_FRAME		7

/** Auxiliar FLA fade out variable */
int32 fadeOut;
/** Auxiliar FLA fade out variable to count frames between the fade */
int32 fadeOutFrames;

/** FLA movie sample auxiliar table */
int32 flaSampleTable[100];
/** Number of samples in FLA movie */
int32 samplesInFla;
/** Auxiliar work video buffer */
uint8* workVideoBufferCopy;
/** Auxiliar scale buffer */
//unsigned char workVideoBufferNonScale[(SCREEN_WIDTH*SCREEN_HEIGHT)/(SCALE*SCALE)];
/** FLA movie pointer */
//unsigned char * flaPtr;
/** FLA movie header data */
FLAHeaderStruct flaHeaderData;
/** FLA movie header data */
FLAFrameDataStruct frameData;

FileReader frFla;

/** FLA movie draw key frame
	@param ptr FLA frame buffer pointer
	@param width FLA movie width
	@param height FLA movie height */
void draw_key_frame(uint8 * ptr, int32 width, int32 height) {
	int32 a, b;
	uint8 * destPtr = (uint8 *)flaBuffer;
	uint8 * startOfLine = destPtr;
	int8 flag1;
	int8 flag2;

	do {
		flag1 = *(ptr++);

		for (a = 0; a < flag1; a++) {
			flag2 = *(ptr++);

			if (flag2 < 0) {
				flag2 = - flag2;
				for (b = 0; b < flag2; b++) {
					*(destPtr++) = *(ptr++);
				}
			} else {
				char colorFill;

				colorFill = *(ptr++);

				for (b = 0; b < flag2; b++) {
					*(destPtr++) = colorFill;
				}
			}
		}

		startOfLine = destPtr = startOfLine + width;
	} while (--height);
}

/** FLA movie draw delta frame
	@param ptr FLA frame buffer pointer
	@param width FLA movie width */
void draw_delta_frame(uint8 * ptr, int32 width) {
	int32 a, b;
	uint16 skip;
	uint8 * destPtr;
	uint8 * startOfLine;
	int32 height;

	int8 flag1;
	int8 flag2;

	skip = *((uint16*)ptr);
	ptr += 2;
	skip *= width;
	startOfLine = destPtr = (uint8 *)flaBuffer + skip;
	height = *((int16*)ptr);
	ptr += 2;

	do {
		flag1 = *(ptr++);

		for (a = 0; a < flag1; a++) {
			destPtr += (unsigned char) * (ptr++);
			flag2 = *(ptr++);

			if (flag2 > 0) {
				for (b = 0; b < flag2; b++) {
					*(destPtr++) = *(ptr++);
				}
			} else {
				char colorFill;
				flag2 = - flag2;

				colorFill = *(ptr++);

				for (b = 0; b < flag2; b++) {
					*(destPtr++) = colorFill;
				}
			}
		}

		startOfLine = destPtr = startOfLine + width;
	} while (--height);
}

/** Scale FLA movie 2 times

	According with the settins we can put the original aspect radio stretch
	to fullscreen or preserve it and use top and button black bars */
void scale_fla_2x() {
	int32 i, j;
	uint8* source = (uint8*)flaBuffer;
	uint8* dest = (uint8*)workVideoBuffer;

	if (cfgfile.Movie == CONF_MOVIE_FLAWIDE) {
		for (i = 0; i < SCREEN_WIDTH / SCALE*40; i++) {
			*(dest++) = 0x00;
		}
	}

	for (i = 0; i < FLASCREEN_HEIGHT; i++) {
		for (j = 0; j < FLASCREEN_WIDTH; j++) {
			*(dest++) = *(source);
			*(dest++) = *(source++);
		}
		if (cfgfile.Movie == CONF_MOVIE_FLAWIDE) { // include wide bars
			memcpy(dest, dest - SCREEN_WIDTH / SCALE, FLASCREEN_WIDTH*2);
			dest += FLASCREEN_WIDTH * 2;
		} else { // stretch the movie like original game.
			if (i % (2)) {
				memcpy(dest, dest - SCREEN_WIDTH / SCALE, FLASCREEN_WIDTH*2);
				dest += FLASCREEN_WIDTH * 2;
			}
			if (i % 10) {
				memcpy(dest, dest - SCREEN_WIDTH / SCALE, FLASCREEN_WIDTH*2);
				dest += FLASCREEN_WIDTH * 2;
			}
		}
	}

	if (cfgfile.Movie == CONF_MOVIE_FLAWIDE) {
		for (i = 0; i < SCREEN_WIDTH / SCALE*40; i++) {
			*(dest++) = 0x00;
		}
	}
}

/** FLA movie process frame */
void process_frame() {
	FLASampleStruct sample;
	uint32 opcodeBlockSize;
	uint8 opcode;
	int32 aux = 0;
	uint8 * ptr;

	frread(&frFla, &frameData.videoSize, 1);
	frread(&frFla, &frameData.dummy, 1);
	frread(&frFla, &frameData.frameVar0, 4);

	frread(&frFla, workVideoBufferCopy, frameData.frameVar0);

	if ((int32)frameData.videoSize <= 0)
		return;

	ptr = workVideoBufferCopy;

	do {
		opcode = *((uint8*)ptr);
		ptr += 2;
		opcodeBlockSize = *((uint16*)ptr);
		ptr += 2;

		switch (opcode - 1) {
		case LOAD_PALETTE: {
			int16 numOfColor = *((int16*)ptr);
			int16 startColor = *((int16*)(ptr + 2));
			memcpy((palette + (startColor*3)), (ptr + 4), numOfColor*3);
			break;
		}
		case FADE: {
			// FLA movies don't use cross fade
			// fade out tricky
			if (fadeOut != 1) {
				convert_pal_2_RGBA(palette, paletteRGBACustom);
				fade_2_black(paletteRGBACustom);
				fadeOut = 1;
			}
			break;
		}
		case PLAY_SAMPLE: {
			memcpy(&sample, ptr, sizeof(FLASampleStruct));
			play_fla_sample(sample.sampleNum, sample.freq, sample.repeat, sample.x, sample.y);
			break;
		}
		case STOP_SAMPLE: {
			stop_samples();
			break;
		}
		case DELTA_FRAME: {
			draw_delta_frame(ptr, FLASCREEN_WIDTH);
			if (fadeOut == 1)
				fadeOutFrames++;
			break;
		}
		case KEY_FRAME: {
			draw_key_frame(ptr, FLASCREEN_WIDTH, FLASCREEN_HEIGHT);
			break;
		}
		default: {
			return;
		}
		}

		aux++;
		ptr += opcodeBlockSize;

	} while (aux < (int32)frameData.videoSize);
	//free(workVideoBufferCopy);
}

/** Play FLA movies
	@param filname FLA movie file name */
void play_fla_movie(int8 *filename) {
	int32 i;
	int32 quit = 0;
	int32 currentFrame;

	fadeOut = -1;
	fadeOutFrames = 0;

	if (!fropen(&frFla, filename, "rb"))
		return;

	if (cfgfile.Debug)
		printf("Playing FLA: %s ", filename);

	workVideoBufferCopy = workVideoBuffer;

	frread(&frFla, &flaHeaderData.version, 6);
	frread(&frFla, &flaHeaderData.numOfFrames, 4);
	frread(&frFla, &flaHeaderData.speed, 1);
	frread(&frFla, &flaHeaderData.var1, 1);
	frread(&frFla, &flaHeaderData.xsize, 2);
	frread(&frFla, &flaHeaderData.ysize, 2);

	frread(&frFla, &samplesInFla, 4);

	// TODO: to remove
	samplesInFla &= 0xFFFF;

	if (cfgfile.Debug)
		printf("- Frames: %d - Speed: %d - Samples: %d\n", flaHeaderData.numOfFrames, flaHeaderData.speed, samplesInFla);

	for (i = 0; i < samplesInFla; i++) {
		//flaSampleTable[i] = *((int16 *)flaPtr); flaPtr+=4;
		int16 var0;
		int16 var1;
		frread(&frFla, &var0, 2);
		frread(&frFla, &var1, 2);
		flaSampleTable[i] = var0;
	}

	if (!strcmp(flaHeaderData.version, "V1.3")) {
		currentFrame = 0;

		if (!quit) {
			do {
				if (currentFrame == flaHeaderData.numOfFrames)
					quit = 1;
				else {
					process_frame();
					scale_fla_2x();
					copy_screen(workVideoBuffer, frontVideoBuffer);

					// Only blit to screen if isn't a fade
					if (fadeOut == -1) {
						convert_pal_2_RGBA(palette, paletteRGBACustom);
						if (!currentFrame) // fade in the first frame
							fade_in(paletteRGBACustom);
						else
							set_palette(paletteRGBACustom);
					}

					// TRICKY: fade in tricky
					if (fadeOutFrames >= 2) {
						flip(frontVideoBuffer);
						convert_pal_2_RGBA(palette, paletteRGBACustom);
						fade_2_pal(paletteRGBACustom);
						fadeOut = -1;
						fadeOutFrames = 0;
					}

					currentFrame++;

					fps_cycles(flaHeaderData.speed + 1);

					read_keys();
					if (skipIntro)
						break;
				}
			} while (!quit);
		}
	}
	if (cfgfile.CrossFade)
		cross_fade(frontVideoBuffer, paletteRGBACustom);
	else
		fade_2_black(paletteRGBACustom);
	stop_samples();
}

/** Generic play movies, according with the settings
	@param movie - movie file path */
void play_movie(int8 *movie) {
	int8 fileBuf[256];

	switch(cfgfile.Movie) {
	case CONF_MOVIE_NONE:
		break;
	case CONF_MOVIE_FLA:
	case CONF_MOVIE_FLAWIDE:
		sprintf(fileBuf, FLA_DIR);
		strcat(fileBuf, movie);
		strcat(fileBuf, FLA_EXT);
		play_fla_movie(fileBuf);
		break;
	case CONF_MOVIE_FLAPCX:
	case CONF_MOVIE_AVI:
		sprintf(fileBuf, MOVIES_DIR);
		strcat(fileBuf, movie);
		strcat(fileBuf, MOVIES_EXT);
	default:
		printf("Movie type not supported yet!! Type: %d", cfgfile.Movie);
	}
}


//
//int audioCallback(void *udata, Uint8 *stream, int len)
//{
//
//    /* unpack our void pointer */
//    SDL_ffmpegFile *file = (SDL_ffmpegFile*)udata;
//
//    int unsigned charsUsed;
//    int offset = 0;
//    SDL_ffmpegAudioFrame *frame = SDL_ffmpegGetAudioFrame(file);
//    if(!frame) return -1;
//
//    while(len > 0) {
//
//        /* check if we need a new frame */
//        if(!frame->size) {
//            frame = SDL_ffmpegGetAudioFrame(file);
//            if(!frame) return -1;
//        }
//
//        if(frame->size <= len) {
//            /* this frame is smaller or equal to the amount of data needed. */
//            unsigned charsUsed = frame->size;
//        } else {
//            /* this frame has more data than needed */
//            unsigned charsUsed = len;
//        }
//
//        /* copy the correct amount of data */
//        memcpy(stream+offset, frame->buffer, unsigned charsUsed);
//        /* adjust the needed length accordingly */
//        len -= unsigned charsUsed;
//        offset += unsigned charsUsed;
//
//        /* we release our audio data, so the decode thread can fill it again */
//        /* we also inform this function of the amount of unsigned chars we used, so it can */
//        /* move the buffer accordingly */
//        /* important! this call is paired with SDL_ffmpegGetAudio */
//        SDL_ffmpegReleaseAudio(file, frame, unsigned charsUsed);
//    }
//
//    return 0;
//}
//
//
//
//void play_avi_movie(char *filename)
//{
//    int s;
//	int w,h;
//	int done = 0;
//    SDL_ffmpegFile* film = SDL_ffmpegOpen(filename);
//	SDL_ffmpegStream *str;
//	SDL_AudioSpec *specs;
//
//	/* open file from arg[1] */
//    if(!film) {
//        printf("error opening file\n");
//        return;
//    }
//
//    /* print some info on detected stream to output */
//    for(s = 0; s<film->AStreams; s++) {
//        str = SDL_ffmpegGetAudioStream(film, s);
//
//        printf("Info on audiostream #%i:\n", s);
//        printf("\tChannels: %i\n",      str->channels);
//        if(strlen(str->language)) printf("\tLanguage: %s\n",      str->language);
//        printf("\tSampleRate: %i\n",    str->sampleRate);
//    }
//
//    for(s = 0; s<film->VStreams; s++) {
//        str = SDL_ffmpegGetVideoStream(film, s);
//
//        printf("Info on videostream #%i:\n", s);
//        if(strlen(str->language)) printf("\tLanguage: %s\n",      str->language);
//        printf("\tFrame: %ix%i\n",  str->width, str->height);
//        printf("\tFrameRate: %.2ffps\n",  1.0 / (str->frameRate[0] / str->frameRate[1]));
//    }
//
//    /* select the streams you want to decode (example just uses 0 as a default) */
//    SDL_ffmpegSelectVideoStream(film, 0);
//    SDL_ffmpegSelectAudioStream(film, 0);
//
//    /* get the audiospec which fits the selected videostream, if no videostream */
//    /* is selected, default values are used (2 channel, 48Khz) */
//    specs = SDL_ffmpegGetAudioSpec(film, 512, audioCallback);
//
//
//    /* we get the size from our active video stream, if no active video stream */
//    /* exists, width and height are set to default values (320x240) */
//    SDL_ffmpegGetVideoSize(film, &w, &h);
//
//    /* we start our decode thread, this always tries to buffer in some frames */
//    /* so we can enjoy smooth playback */
//    SDL_ffmpegStartDecoding(film);
//
//    /* we unpause the audio so our audiobuffer gets read */
//    SDL_PauseAudio(0);
//
//    while( !done ) {
//		SDL_ffmpegVideoFrame* frame;
//
//        /* just some standard SDL event stuff */
//        SDL_Event event;
//        while(SDL_PollEvent(&event)) {
//
//            if(event.type == SDL_QUIT) {
//                done = 1;
//                break;
//            }
//
//            if(event.type == SDL_MOUSEBUTTONDOWN) {
//				int x,y;
//				int64_t time;
//
//                SDL_PumpEvents();
//
//                SDL_GetMouseState(&x, &y);
//                /* by clicking you turn on the stream, seeking to the percentage */
//                /* in time, based on the x-position you clicked on */
//                time = ((double)x / w) * SDL_ffmpegGetDuration(film);
//
//                /* we seek to time (milliseconds) */
//                SDL_ffmpegSeek(film, time);
//
//                /* by passing 0(false) as our second argument, we play the file */
//                /* passing a non-zero value would mean we pause our file */
//                SDL_ffmpegPause(film, 0);
//            }
//        }
//
//        /* we retrieve the current image from the file */
//        /* we get 0 if no file could be retrieved */
//        /* important! please note this call should be paired with SDL_ffmpegReleaseVideo */
//        frame = SDL_ffmpegGetVideoFrame(film);
//
//        if(frame) {
//
//            /* we got a frame, so we better show this one */
//            SDL_BlitSurface(frame->buffer, 0, screen, 0);
//
//            /* After releasing this frame, you can no longer use it. */
//            /* you should call this function every time you get a frame! */
//            SDL_ffmpegReleaseVideo(film, frame);
//
//            /* we flip the double buffered screen so we might actually see something */
//            SDL_Flip(screen);
//        }
//
//        /* we wish not to kill our poor cpu, so we give it some timeoff */
//        SDL_Delay(5);
//    }
//
//    /* after all is said and done, we should call this */
//    SDL_ffmpegFree(film);
//
//    /* the SDL_Quit function offcourse... */
//    SDL_Quit();
//
//    return;
//}

/*
void fla_pcxList(char *flaName)
{
	// check if FLAPCX file exist
//	if(!checkIfFileExist("FLA_PCX.HQR") || !checkIfFileExist("FLA_GIF.HQR")){
//		printf("FLA_PCX file doesn't exist!");
		//return;
	//}

	// TODO: done this with the HQR 23th entry (movies informations)
	if(!strcmp(flaName,"INTROD"))
	{
		prepareFlaPCX(1);
		WaitTime(5000);
		prepareFlaPCX(2);
		WaitTime(5000);
		prepareFlaPCX(3);
		WaitTime(5000);
		prepareFlaPCX(4);
		WaitTime(5000);
		prepareFlaPCX(5);
		WaitTime(5000);

	}
	else if(!strcmp(flaName,"BAFFE") || !strcmp(flaName,"BAFFE2") || !strcmp(flaName,"BAFFE3") || !strcmp(flaName,"BAFFE4"))
	{
		prepareFlaPCX(6);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"bateau") || !strcmp(flaName,"bateau2"))
	{
		prepareFlaPCX(7);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"flute2"))
	{
		prepareFlaPCX(8);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"navette"))
	{
		prepareFlaPCX(15);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"templebu"))
	{
		prepareFlaPCX(12);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"glass2"))
	{
		prepareFlaPCX(8);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"surf"))
	{
		prepareFlaPCX(9);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"verser") || !strcmp(flaName,"verser2"))
	{
		prepareFlaPCX(10);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"capture"))
	{
		prepareFlaPCX(14);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"neige2"))
	{
		prepareFlaPCX(11);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"sendel"))
	{
		prepareFlaPCX(14);
		WaitTime(5000);
	}
	else if(!strcmp(flaName,"sendel2"))
	{
		prepareFlaPCX(17);
		WaitTime(5000);
	}
}

void prepareFlaPCX(int index)
{
	int i;
	SDL_Surface *image;

	// TODO: Done this without SDL_Image Library
	if(checkIfFileExist("FLA_PCX.HQR"))
		image = IMG_LoadPCX_RW(SDL_RWFromMem(HQR_Get(HQR_FlaPCX,index), Size_HQR("FLA_PCX.HQR", index))); // rwop
	else if(checkIfFileExist("FLA_GIF.HQR"))
		image = IMG_LoadGIF_RW(SDL_RWFromMem(HQR_Get(HQR_FlaGIF,index), Size_HQR("fla_gif.hqr", index))); // rwop

	if(!image) {
		printf("Can't load FLA PCX: %s\n", IMG_GetError());
	}

	osystem_FlaPCXCrossFade(image);
}*/
