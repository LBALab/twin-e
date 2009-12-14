/** @file main.c
	@brief 
	This file contains the main engine functions. It also contains 
	configurations file definitions.
	
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
#include <assert.h>

#include "main.h"
#include "resources.h"
#include "sdlengine.h"
#include "images.h"
#include "music.h"
#include "menu.h"
#include "interface.h"
#include "movies.h"
#include "hqrdepack.h"
#include "scene.h"
#include "grid.h"
#include "lbaengine.h"
#include "redraw.h"
#include "dialogues.h"
#include "renderer.h"
#include "animations.h"

/** Engine current version */
const char* ENGINE_VERSION = "0.1.0";

/** Definition for European version */
#define EUROPE_VERSION 0
/** Definition for American version */
#define USA_VERSION 1
/** Definition for Modification version */
#define MODIFICATION_VERSION 2


/** Engine configuration filename */
const char* CONFIG_FILENAME = "lba.cfg";

/** Engine install setup filename

	This file contains informations about the game version.
	This is only used for original games. For mod games project you can 
	used \a lba.cfg file \b Version tag. If this tag is set for original game
	it will be used instead of \a setup.lst file. */
const char* SETUP_FILENAME = "setup.lst";

/** Configuration types at \a lba.cfg file

	Fill this with all needed configurations at \a lba.cfg file. 
	This engine version allows new type of configurations. 
	Check new config lines at \a lba.cfg file after the first game execution */
char CFGList[][18]=
{
	"Language:",
	"LanguageCD:",
	"FlagDisplayText:",
	"FlagKeepVoice:",
	"SvgaDriver:",
	"MidiDriver:",
	"MidiExec:",
	"MidiBase:",
	"MidiType:",
	"MidiIRQ:",
	"MidiDMA:",
	"WaveDriver:",
	"WaveExec:",
	"WaveBase:",
	"WaveIRQ:",
	"WaveDMA:",
	"WaveRate:",
	"MixerDriver:",
	"MixerBase:",
	"WaveVolume:",
	"VoiceVolume:",
	"MusicVolume:",
	"CDVolume:",
	"LineVolume:",
	"MasterVolume:",
	"Version:",
	"FullScreen:",
	"UseCD:",
	"NoSound:",
	"UseHQSnd:",
	"UseMP3:",
	"FLAwide:",
	"UseFLAPCX:",
	"UseAVI:",
	"CrossFade:",
	"Fps:",
	"Debug:",
	"UseAutoSaving:"
};


/** Allocate video memory, both front and back buffers */
void allocate_video_memory()
{
	int i, j, k;

	workVideoBuffer = (unsigned char *) malloc((SCREEN_WIDTH*SCREEN_HEIGHT) * sizeof(unsigned char));
	frontVideoBuffer = frontVideoBufferbis = (unsigned char *) malloc(sizeof(unsigned char) * SCREEN_WIDTH*SCREEN_HEIGHT);
	init_screen_buffer(frontVideoBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);

	j = 0;
	k = 0;
	for (i = SCREEN_HEIGHT; i > 0; i--)
	{
		screenLockupTable[j] = k;
		j++;
		k += SCREEN_WIDTH;
	}

	// initVideoVar1 = -1;
}

/** Gets configuration type index from lba.cfg config file 
	@param lineBuffer buffer with config line
	@return config type index */
int get_config_type_index(char* lineBuffer)
{
    int i;
    char buffer[256];
    char* ptr;

    strcpy(buffer,lineBuffer);

	ptr = strchr(buffer,' ');

    if(ptr)
    {
        *ptr = 0;
    }

    for(i=0;i<=(sizeof(CFGList)/18);i++)
    {
        if(strlen(CFGList[i]))
        {
            if(!strcmp(CFGList[i],buffer)){
                return i;
            }
        }
    }

    return -1;	
}

/** Init configuration file \a lba.cfg */
void init_configurations()
{
	FILE *fd;
	char buffer[256];
	int cfgtype=-1;

	fd = fopen(CONFIG_FILENAME, "rb");
	if(!fd)
		printf("Error: Can't find config file %s\n",CONFIG_FILENAME);

	// make sure it quit when it reaches the end of file
	while(fgets(buffer,256,fd) != NULL)
	{
		*strchr(buffer,0x0D0A) = 0;
		cfgtype = get_config_type_index(buffer);
		if(cfgtype!=-1)
		{
			switch(cfgtype)
			{
				case 0: sscanf(buffer,"Language: %s",cfgfile.Language); break;
				case 1: sscanf(buffer,"LanguageCD: %s",cfgfile.LanguageCD); break;
				case 2: sscanf(buffer,"FlagDisplayText: %s",cfgfile.FlagDisplayText); break;
				case 3: sscanf(buffer,"FlagKeepVoice: %s",cfgfile.FlagKeepVoice); break;
				// case 4,18: unused
				case 19: sscanf(buffer,"WaveVolume: %d",&cfgfile.WaveVolume); break;
				case 20: sscanf(buffer,"VoiceVolume: %d",&cfgfile.VoiceVolume); break;
				case 21: sscanf(buffer,"MusicVolume: %d",&cfgfile.MusicVolume); break;
				case 22: sscanf(buffer,"CDVolume: %d",&cfgfile.CDVolume); break;
				case 23: sscanf(buffer,"LineVolume: %d",&cfgfile.LineVolume); break;
				case 24: sscanf(buffer,"MasterVolume: %d",&cfgfile.MasterVolume); break;
				case 25: sscanf(buffer,"Version: %d",&cfgfile.Version); break;
				case 26: sscanf(buffer,"FullScreen: %d",&cfgfile.FullScreen); break;
				case 27: sscanf(buffer,"UseCD: %d",&cfgfile.UseCD); break;
				case 28: sscanf(buffer,"NoSound: %d",&cfgfile.NoSound); break;
				case 29: sscanf(buffer,"UseHQSnd: %d",&cfgfile.UseHQSnd); break;
				case 30: sscanf(buffer,"UseMP3: %d",&cfgfile.UseMP3); break;
				case 31: sscanf(buffer,"FLAwide: %d",&cfgfile.FLAwide); break;
				case 32: sscanf(buffer,"UseFLAPCX: %d",&cfgfile.UseFLAPCX); break;
				case 33: sscanf(buffer,"UseAVI: %d",&cfgfile.UseAVI); break;
				case 34: sscanf(buffer,"CrossFade: %d",&cfgfile.CrossFade); break;
				case 35: sscanf(buffer,"Fps: %d",&cfgfile.Fps); break;
				case 36: sscanf(buffer,"Debug: %d",&cfgfile.Debug); break;
				case 37: sscanf(buffer,"UseAutoSaving: %d",&cfgfile.UseAutoSaving); break;
			}
		}
	}

	if(!cfgfile.Fps)
		cfgfile.Fps = DEFAULT_FRAMES_PER_SECOND;

	fclose(fd);
}

/** Initialize LBA engine */
void init_engine()
{
	// getting configuration file
	init_configurations();

	// Show engine information
	printf("Prequengine v%s\n",ENGINE_VERSION);
	printf("(c)2008 The Prequel Engine dev-team. Refer to AUTHORS file for further details.\n");
	printf("Released under the terms of the GNU GPL license version 2 (or, at your opinion, any later). See COPYING file.\n\n");
	printf("The original Little Big Adventure game is:\n\t(c)1994 by Adeline Software International, All Rights Reserved.\n\n");

	if(cfgfile.Debug)
		printf("Compiled the %s at %s\n", __DATE__, __TIME__);

	sdl_initialize();
	
	allocate_video_memory();
	clear_screen();

	// Toggle fullscreen if Fullscreen flag is set
	toggle_fullscreen();

	// Check if LBA CD-Rom is on drive
	init_cdrom();

#ifndef _DEBUG

	// Display company logo
	adeline_logo();

	// verify game version screens
	if(cfgfile.Version==EUROPE_VERSION)
	{
		// Little Big Adventure screen
		load_image_delay(RESSHQR_LBAIMG,3);
		// Electronic Arts Logo
		load_image_delay(RESSHQR_EAIMG,2);
	}
	else if (cfgfile.Version==USA_VERSION)
	{
		// Relentless screen
		load_image_delay(RESSHQR_RELLENTIMG,3);
		// Electronic Arts Logo
		load_image_delay(RESSHQR_EAIMG,2);
	}
	else if (cfgfile.Version==MODIFICATION_VERSION)
	{
		// Modification screen
		load_image_delay(RESSHQR_RELLENTIMG,2);
		// TODO: add necessary screens here
	}

	play_movie(FLA_DRAGON3);

#endif

	load_menu_image( 1 );

	main_menu();
}

/** Initialize engine auxiliar keymap */
void init_keymap()
{
	pressedKeyMap[0] = 0x48;
	pressedKeyMap[1] = 0x50;
	pressedKeyMap[2] = 0x4B;
	pressedKeyMap[3] = 0x4D;
	pressedKeyMap[4] = 0x47;
	pressedKeyMap[5] = 0x49;
	pressedKeyMap[6] = 0x51;
	pressedKeyMap[7] = 0x4F;

	pressedKeyMap[8] = 0x39;
	pressedKeyMap[9] = 0x1C;
	pressedKeyMap[10] = 0x1D;
	pressedKeyMap[11] = 0x38;
	pressedKeyMap[12] = 0x53;
	pressedKeyMap[13] = 0x2A;
	pressedKeyMap[14] = 0x36;

	pressedKeyMap[15] = 0x3B;
	pressedKeyMap[16] = 0x3C;
	pressedKeyMap[17] = 0x3D;
	pressedKeyMap[18] = 0x3E;
	pressedKeyMap[19] = 0x3F;
	pressedKeyMap[20] = 0x40; // LBAKEY_F6
	pressedKeyMap[21] = 0x41;
	pressedKeyMap[22] = 0x42;
	pressedKeyMap[23] = 0x43;
	pressedKeyMap[24] = 0x44;
	pressedKeyMap[25] = 0x57;
	pressedKeyMap[26] = 0x58;
	pressedKeyMap[27] = 0x2A;
	pressedKeyMap[28] = 0x0;

	pressedKeyCharMap[0] = 0x0100; // up
	pressedKeyCharMap[1] = 0x0200; // down
	pressedKeyCharMap[2] = 0x0400; // left
	pressedKeyCharMap[3] = 0x0800; // right
	pressedKeyCharMap[4] = 0x0500; // home
	pressedKeyCharMap[5] = 0x0900; // pageup
	pressedKeyCharMap[6] = 0x0A00; // pagedown
	pressedKeyCharMap[7] = 0x0600; // end

	pressedKeyCharMap[8] = 0x0101; // space bar
	pressedKeyCharMap[9] = 0x0201; // enter
	pressedKeyCharMap[10] = 0x0401;  // ctrl
	pressedKeyCharMap[11] = 0x0801;  // alt
	pressedKeyCharMap[12] = 0x1001;  // del
	pressedKeyCharMap[13] = 0x2001;  // left shift
	pressedKeyCharMap[14] = 0x2001;  // right shift

	pressedKeyCharMap[15] = 0x0102;  // F1
	pressedKeyCharMap[16] = 0x0202;  // F2
	pressedKeyCharMap[17] = 0x0402;  // F3
	pressedKeyCharMap[18] = 0x0802;  // F4
	pressedKeyCharMap[19] = 0x1002;  // F5
	pressedKeyCharMap[20] = 0x2002;  // F6
	pressedKeyCharMap[21] = 0x4002;  // F7
	pressedKeyCharMap[22] = 0x8002;  // F8

	pressedKeyCharMap[23] = 0x0103;  // F9
	pressedKeyCharMap[24] = 0x0203;  // F10
	pressedKeyCharMap[25] = 0x0403;  // ?
	pressedKeyCharMap[26] = 0x0803;  // ?
	pressedKeyCharMap[27] = 0x00FF;  // left shift
	pressedKeyCharMap[28] = 0x00FF;
	pressedKeyCharMap[29] = 0x0;
	pressedKeyCharMap[30] = 0x0;
}

/** Initialize resource pointers */
void init_resources()
{
	int size=0;

	// Menu and in-game palette
	init_palettes();

	// load LBA font
	size = hqr_getalloc_entry(&fontPtr, HQR_RESS_FILE, RESSHQR_LBAFONT);

	set_font_parameters(2, 8);
	set_font_color(14);
	set_dialogue_cross_color(136, 143, 2);

	// load sprite actors bounding box data
	size = hqr_getalloc_entry(&spriteBoundingBoxPtr, HQR_RESS_FILE, RESSHQR_SPRITEBOXDATA);

	preload_sprites();
	preload_animations();
}

/** Initialize main engine variables */
void init_vars()
{
	set_camera_position(40,40,128,200,200);

	needChangeScene = 0;
	reqBgRedraw = 1;
	currentSceneIdx = 0;
	currentTextBank = -1;
	currMenuTextIndex = -1;
	currMenuTextBank = -1;
	isUsingOrhoProjection = 1;

	sceneHero = &sceneActors[0];

	useCellingGrid = -1;
	cellingGridIdx = 0;

	currentlyFollowedActor = 0;

	/*setSomethingVar1 = 320;
	setSomethingVar2 = 200;
	setSomethingVar3 = 500;*/
	// TODO: configure ortho
	orthoProjX = 311;
	orthoProjY = 240;
	orthoProjZ = 512;

	renderLeft = 0;
	renderTop = 0;
	renderRight = SCREEN_TEXTLIMIT_RIGHT;
	renderBottom = SCREEN_TEXTLIMIT_BOTTOM;

	rightMouse=0;
	leftMouse=0;

	currentPositionInBodyPtrTab=0; // TODO: put this in scene.c file under clear_scene()
}


/** Initialize all needed stuffs at first time running engine */
void init_all()
{
	blockBuffer = (unsigned char *)malloc(204800);  // 204800 = 64*64*25*2

	animBuffer1 = animBuffer2 = (unsigned char *)malloc(5000);

	// Init engine keymap
	init_keymap();
	init_resources();
	init_vars();
}

/** Main engine function 
	@param argc numner of arguments 
	@param argv array with all arguments strings */
int main(int argc, char *argv[])
{
	init_all();
	init_engine();
	sdl_close();
	printf("\n\nLBA/Relentless < %s / %s >\n\nOK.\n\n",__DATE__,__TIME__);
	printf("Prequengine v%s closed\n",ENGINE_VERSION);
	if(cfgfile.Debug)
	{
		printf("\nPress <ENTER> to quit debug mode\n");
		//getchar();
	}
	return 0;
}


/** \mainpage LBA Prequel Engine Doxxy Documentation
 
	\section intro_sec Introduction
 
	LBA Prequel is project based upon the popular Little Big Adventure games
	released respectively in 1994 (Relentless in North America) and 
	1997 (Twinsen's Odyssey). Based on the actual LBA game engine, 
	our goal is to create an original game that is loosely based on the 
	story of Twinsen's ancestor, Hegesippe.

	\section proj_sec The Project

	While making the first game write and design we notice we need something
	more from using the original game engine. Something that could make the
	difference. Better movies quality, sound quality, quick all the old 
	sound formats the engine support, etc.. So, because of this type of things
	we, the Prequel team, decide to create a side project which aims to build
	an engine based on reverse of the original LBA1 engine. We want to make
	an exact engine since we don't wanna loose all that atmosphere the original
	games gives to us.

	Even beeing a side project, the engine, for another project, the game, this
	engine will be available for all people who'd like to make a modification
	game of LBA1.

	\section doc_sec Documentation

	This document, allows you to easily search for particulary things among
	the code. We plan to comment as best as we can and with the most necessary
	informations. The source code is also shared in this document and you can
	use it in the terms of the license.

	Feel free to contact us if you wanna help improving this documentation and
	the engine itself.

	\section copy_sec Copyright

	Copyright (c) Adeline Software International 1994, All Rights Reserved.\n
	Copyright (c) 2002-2007 The TwinEngine team.\n
	Copyright (c) 2008 Prequengine team.

	\section licenc_sec License

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

	For a full license, check the license file in source code.
*/
