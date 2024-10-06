/** @file main.h
    @brief
    This file contains the main engine functions. It also contains
    configurations file definitions.

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

#ifndef MAIN_H
#define MAIN_H

#include "sys.h"

/** Definition for European version */
#define EUROPE_VERSION 0
/** Definition for American version */
#define USA_VERSION 1
/** Definition for Modification version */
#define MODIFICATION_VERSION 2

/** Original screen width */
#define DEFAULT_SCREEN_WIDTH			640
/** Original screen height */
#define DEFAULT_SCREEN_HEIGHT			480
/** Scale screen to double size */
#define SCALE					1
/** Original screen width */
#define SCREEN_WIDTH			DEFAULT_SCREEN_WIDTH * SCALE
/** Original screen height */
#define SCREEN_HEIGHT			DEFAULT_SCREEN_HEIGHT * SCALE
/** Original FLA screen width */
#define FLASCREEN_WIDTH			320
/** Original FLA screen height */
#define FLASCREEN_HEIGHT		200
/** Default frames per second */
#define DEFAULT_FRAMES_PER_SECOND	20

/** Number of colors used in the game */
#define NUMOFCOLORS		256


/** Configuration file structure

    Used in the engine to load/use certain parts of code according with
    this settings. Check \a lba.cfg file for valid values for each settings.\n
    All the settings with (*) means they are new and only exist in this engine. */
typedef struct config_file_s {
    /** language name */
    int8 language[10];
    /** language cd name */
    int8 language_cd[10];
    /** language identification according with language setting. */
    int32 language_id;
    /** language identification according with language setting. */
    int32 language_cd_id;
    /** enable/disable game dialogues */
    int8 flag_display_text_str[3];
    /** enable/disable game dialogues */
    int32 flag_display_text;
    /** save voice files on hard disk */
    int8 flag_keep_voice_str[3];
    /** save voice files on hard disk */
    int32 flag_keep_voice;
    /** type of music file to be used */
    int8 midi_type;
    /** wave volume */
    int32 wave_volume;
    /** chacters voices volume */
    int32 voice_volume;
    /** music volume */
    int32 music_volume;
    /** cd volume */
    int32 cd_volume;
    /** line-in volume */
    int32 line_volume;
    /** main volume controller */
    int32 master_volume;
    /** *game version */
    int32 version;
    /** to allow fullscreen or window mode. */
    int32 full_screen;
    /** if you want to use the lba cd or not */
    int32 use_cd;
    /** allow various sound types */
    int32 sound;
    /** allow various movie types */
    int32 movie;
    /** use cross fade effect while changing images, or be as the original */
    int32 cross_fade;
    /** flag used to keep the game frames per second */
    int32 fps;
    /** flag to display game debug */
    int32 debug;
    /** use original autosaving system or save when you want */
    int32 use_auto_saving;
    /** shadow mode type */
    int32 shadow_mode;
    /** auto_agressive mode type */
    int32 auto_aggressive;
    /** sce_zoom mode type */
    int32 sce_zoom;
    /** fill_details mode type */
    int32 fill_details;
    /** flag to quit the game */
    int32 quit;
    /** flag to change interface style */
    int32 interface_style;
    /** flag to toggle wall collision */
    int32 wall_collision;
} config_file_t;

/** Configuration file structure

    Contains all the data used in the engine to configurated the game in particulary ways.\n
    A detailled information of all types are in \a main.h header file. */
extern config_file_t config_file;


void init_mcga();
void init_svga();

int8* ITOA(int32 number);

#endif
