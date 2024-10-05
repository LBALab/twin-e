/** @file main.c
    @brief
    This file contains the main engine functions.
    It also contains configurations file definitions.

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
#include <assert.h>

#include "main.h"
#include "resources.h"
#include "platform.h"
#include "screens.h"
#include "music.h"
#include "menu.h"
#include "interface.h"
#include "flamovies.h"
#include "hqr.h"
#include "scene.h"
#include "grid.h"
#include "lbaengine.h"
#include "redraw.h"
#include "text.h"
#include "renderer.h"
#include "animations.h"
#include "gamestate.h"
#include "keyboard.h"
#include "actor.h"
#include "sample.h"
#include "fcaseopen.h"


const int8* ENGINE_VERSION = (int8*) "0.3.0";
const int8* CONFIG_FILENAME = (int8*) "lba.cfg";
const int8* SETUP_FILENAME = (int8*) "setup.lst";

int8 * cd_directory;

int8 CFGList[][22] = {
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
    "MidiDMA:", // 10
    "WaveDriver:", 
    "WaveExec:",
    "WaveBase:",
    "WaveIRQ:",
    "WaveDMA:",
    "WaveRate:",
    "MixerDriver:",
    "MixerBase:",  
    "WaveVolume:",
    "VoiceVolume:", // 20
    "MusicVolume:",
    "CDVolume:",
    "LineVolume:",
    "MasterVolume:",
    "Version:",
    "FullScreen:",
    "UseCD:",
    "Sound:",
    "Movie:",
    "cross_fade:", // 30
    "Fps:",
    "Debug:",
    "UseAutoSaving:",
    "CombatAuto:", 
    "Shadow:",
    "SceZoom:",
    "FillDetails:",
    "InterfaceStyle", 
    "WallCollision" // 39
};

int8 LanguageTypes[][10] = {
    "English",
    "Français",
    "Deutsch",
    "Español",
    "Italiano",
    "Português"
};
config_file_t config_file;


void alloc_video_memory() {
    int32 i, j, k;

    workVideoBuffer = (uint8 *) malloc((SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(uint8));
    frontVideoBuffer = frontVideoBufferbis = (uint8 *) malloc(sizeof(uint8) * SCREEN_WIDTH * SCREEN_HEIGHT);
    platform_init_screen_buffer(frontVideoBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);

    j = 0;
    k = 0;
    for (i = SCREEN_HEIGHT; i > 0; i--) {
        screenLookupTable[j] = k;
        j++;
        k += SCREEN_WIDTH;
    }

    // initVideoVar1 = -1;
}


int get_config_type_index(int8* lineBuffer) {
    int32 i;
    char buffer[256];
    char* ptr;

    strcpy(buffer, lineBuffer);

    ptr = strchr(buffer, ' ');

    if (ptr) {
        *ptr = 0;
    }

    for (i = 0; i < (sizeof(CFGList) / 22); i++) {
        if (strlen(CFGList[i])) {
            if (!strcmp(CFGList[i], buffer)) {
                return i;
            }
        }
    }

    return -1;
}

int get_language_type_index(int8* language) {
    int32 i;
    char buffer[256];
    char* ptr;

    strcpy(buffer, language);

    ptr = strchr(buffer, ' ');

    if (ptr) {
        *ptr = 0;
    }

    for (i = 0; i < (sizeof(LanguageTypes) / 10); i++) {
        if (strlen(LanguageTypes[i])) {
            if (!strcmp(LanguageTypes[i], buffer)) {
                return i;
            }
        }
    }

    return 0; // English
}

void init_configurations() {
    FILE *fd, *fd_test;
    int8 buffer[256], tmp[16];
    int32 cfgtype = -1;

    fd = fcaseopen(CONFIG_FILENAME, "rb");
    if (!fd)
        printf("Error: Can't find config file %s\n", CONFIG_FILENAME);

    // make sure it quit when it reaches the end of file
    while (fgets(buffer, 256, fd) != NULL) {
        *strchr(buffer, 0x0D0A) = 0;
        cfgtype = get_config_type_index(buffer);
        if (cfgtype != -1) {
            switch (cfgtype) {
            case 0:
                sscanf(buffer, "Language: %s", config_file.language);
                config_file.language_id = get_language_type_index(config_file.language);
                break;
            case 1:
                sscanf(buffer, "LanguageCD: %s", config_file.language_cd);
                config_file.language_cd_id = get_language_type_index(config_file.language) + 1;
                break;
            case 2:
                sscanf(buffer, "FlagDisplayText: %s", config_file.flag_display_text_str);
                if (!strcmp(config_file.flag_display_text_str,"ON")) {
                    config_file.flag_display_text = 1;
                } else {
                    config_file.flag_display_text = 0;
                }
                break;
            case 3:
                sscanf(buffer, "FlagKeepVoice: %s", config_file.flag_keep_voice_str);
                break;
            case 8:
                sscanf(buffer, "MidiType: %s", tmp);
                if (strcmp(tmp, "auto") == 0) {
                    fd_test = fcaseopen(HQR_MIDI_MI_WIN_FILE, "rb");
                    if (fd_test) {
                        fclose(fd_test);
                        config_file.midi_type = 1;
                    }
                    else
                        config_file.midi_type = 0;
                }
                else if (strcmp(tmp, "midi") == 0)
                    config_file.midi_type = 1;
                else 
                    config_file.midi_type = 0;
                break;
            case 19:
                sscanf(buffer, "WaveVolume: %d", &config_file.wave_volume);
                config_file.voice_volume = config_file.wave_volume;
                break;
            case 20:
                sscanf(buffer, "VoiceVolume: %d", &config_file.voice_volume);
                break;
            case 21:
                sscanf(buffer, "MusicVolume: %d", &config_file.music_volume);
                break;
            case 22:
                sscanf(buffer, "CDVolume: %d", &config_file.cd_volume);
                break;
            case 23:
                sscanf(buffer, "LineVolume: %d", &config_file.line_volume);
                break;
            case 24:
                sscanf(buffer, "MasterVolume: %d", &config_file.master_volume);
                break;
            case 25:
                sscanf(buffer, "Version: %d", &config_file.version);
                break;
            case 26:
                sscanf(buffer, "FullScreen: %d", &config_file.full_screen);
                break;
            case 27:
                sscanf(buffer, "UseCD: %d", &config_file.use_cd);
                break;
            case 28:
                sscanf(buffer, "Sound: %d", &config_file.sound);
                break;
            case 29:
                sscanf(buffer, "Movie: %d", &config_file.movie);
                break;
            case 30:
                sscanf(buffer, "cross_fade: %d", &config_file.cross_fade);
                break;
            case 31:
                sscanf(buffer, "Fps: %d", &config_file.fps);
                break;
            case 32:
                sscanf(buffer, "Debug: %d", &config_file.debug);
                break;
            case 33:
                sscanf(buffer, "UseAutoSaving: %d", &config_file.use_auto_saving);
                break;
            case 34:
                sscanf(buffer, "CombatAuto: %d", &config_file.auto_aggressive);
                break;
            case 35:
                sscanf(buffer, "Shadow: %d", &config_file.shadow_mode);
                break;
            case 36:
                sscanf(buffer, "SceZoom: %d", &config_file.sce_zoom);
                break;
            case 37:
                sscanf(buffer, "FillDetails: %d", &config_file.fill_details);
                break;
            case 38:
                sscanf(buffer, "InterfaceStyle: %d", &config_file.interface_style);
                break;
            case 39:
                sscanf(buffer, "WallCollision: %d", &config_file.wall_collision);
                break;
            }
        }
    }

    if (!config_file.fps)
        config_file.fps = DEFAULT_FRAMES_PER_SECOND;

    fclose(fd);
}

void init_engine() {
    init_configurations();

    // Show engine information
    printf("TwinEngine v%s\n\n", ENGINE_VERSION);
    printf("(c)2002 The TwinEngine team. Refer to AUTHORS file for further details.\n");
    printf("Released under the terms of the GNU GPL license version 2 (or, at your opinion, any later). See COPYING file.\n\n");
    printf("The intellectual property is currently owned by [2.21].\n");
    printf("Originaly developed by Adeline Software International in 1994.\n\n");

    if (config_file.debug)
        printf("Compiled the %s at %s\n", __DATE__, __TIME__);

    platform_initialize();

    srand(platform_tick()); // always get a different seed while starting the game

    alloc_video_memory();
    clearScreen();

    // Toggle fullscreen if Fullscreen flag is set
    platform_toggle_fullscreen();

    // Check if LBA CD-Rom is on drive
    initCdrom();
    if (config_file.movie > 0) {
        // Display company logo
        adelineLogo();

        // verify game version screens
        if (config_file.version == EUROPE_VERSION) {
            // Little Big Adventure screen
            loadImageDelay(RESSHQR_LBAIMG, 3);
            // Electronic Arts Logo
            loadImageDelay(RESSHQR_EAIMG, 2);
        } else if (config_file.version == USA_VERSION) {
            // Relentless screen
            loadImageDelay(RESSHQR_RELLENTIMG, 3);
            // Electronic Arts Logo
            loadImageDelay(RESSHQR_EAIMG, 2);
        } else if (config_file.version == MODIFICATION_VERSION) {
            // Modification screen
            loadImageDelay(RESSHQR_RELLENTIMG, 2);
        }

        playFlaMovie(FLA_DRAGON3);
    }

    loadMenuImage(1);

    mainMenu();
}

void init_mcga() {
    drawInGameTransBox = 1;
}

void init_svga() {
    drawInGameTransBox = 0;
}

void init_all() {
    blockBuffer = (uint8 *) malloc(64*64*25*2 * sizeof(uint8));
    animBuffer1 = animBuffer2 = (uint8 *) malloc(5000 * sizeof(uint8));
    sample_init();
    memset(itemAngle, 256, sizeof(itemAngle)); // reset inventory items angles

    bubbleSpriteIndex = SPRITEHQR_DIAG_BUBBLE_LEFT;
    bubbleActor = -1;
    showDialogueBubble = 1;
    
    currentTextBank = -1;
    currMenuTextIndex = -1;
    currMenuTextBank = -1;
    autoAgressive = 1;
    
    sceneHero = &sceneActors[0];

    renderLeft = 0;
    renderTop = 0;
    renderRight = SCREEN_TEXTLIMIT_RIGHT;
    renderBottom = SCREEN_TEXTLIMIT_BOTTOM;
    // Set clip to fullscreen by default, allows main menu to render properly after load
    resetClip();

    rightMouse = 0;
    leftMouse = 0;

    resources_init();

    init_svga();
}

int main(int argc, char *argv[]) {
    init_all();
    init_engine();
    platform_close();
    printf("\n\nLBA/Relentless < %s / %s >\n\nOK.\n\n", __DATE__, __TIME__);
    printf("TwinEngine v%s closed\n", ENGINE_VERSION);
    if (config_file.debug) {
        printf("\nPress <ENTER> to quit debug mode\n");
        getchar();
    }
    return 0;
}

int8* ITOA(int32 number) {
    int32 numDigits = 1;
    int8* text;

    if (number >=10 && number <= 99) {
        numDigits = 2;
    } else if (number >=100 && number <= 999) {
        numDigits = 3;
    }

    text = (int8 *)malloc(sizeof(int8) * (numDigits + 1));
    sprintf(text, "%d", number);
    return text;
}


/** \mainpage Twinsen's Engine Doxxy Documentation

    \section intro_sec Introduction

    TwinEngine is a reimplementation project upon the popular 
    Little Big Adventure games released respectively in 
    1994 (Relentless in North America) and 	1997 (Twinsen's Odyssey). 

    \section doc_sec Documentation

    This document, allows you to easily search for particulary things among
    the code. We plan to comment as best as we can and with the most necessary
    informations. The source code is also shared in this document and you can
    use it in the terms of the license.

    Feel free to contact us if you wanna help improving this documentation and
    the engine itself.

    \section copy_sec Copyright

    Copyright (c) Adeline Software International 1994, All Rights Reserved.
    Copyright (c) [2.21], All Rights Reserved.
    Copyright (c) 2002 The TwinEngine team.

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
