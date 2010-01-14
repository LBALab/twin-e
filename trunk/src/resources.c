/** @file resources.c
	@brief
	This file contains the definitions of most used game resources.

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

#include "resources.h"
#include "text.h"
#include "scene.h"
#include "animations.h"
#include "images.h"
#include "sdlengine.h"
#include "sound.h"

int8 * HQR_RESS_FILE			= "ress.hqr";
int8 * HQR_TEXT_FILE			= "text.hqr";
int8 * HQR_FLASAMP_FILE			= "flasamp.hqr";
int8 * HQR_MIDI_MI_WIN_FILE		= "midi_mi_win.hqr";
int8 * HQR_MIDI_MI_WIN_MP3_FILE = "midi_mi_win_mp3.hqr";
int8 * HQR_MIDI_MI_WIN_OGG_FILE = "midi_mi_win_ogg.hqr";
int8 * HQR_SAMPLES_FILE			= "samples.hqr";
int8 * HQR_LBA_GRI_FILE			= "lba_gri.hqr";
int8 * HQR_LBA_BLL_FILE			= "lba_bll.hqr";
int8 * HQR_LBA_BRK_FILE			= "lba_brk.hqr";
int8 * HQR_SCENE_FILE			= "scene.hqr";
int8 * HQR_SPRITES_FILE			= "sprites.hqr";
int8 * HQR_FILE3D_FILE			= "file3d.hqr";
int8 * HQR_BODY_FILE			= "body.hqr";
int8 * HQR_ANIM_FILE			= "anim.hqr";

/** Init palettes */
void init_palettes() {
	// Init standard palette
	hqr_get_entry(palette, HQR_RESS_FILE, RESSHQR_MAINPAL);
	convert_pal_2_RGBA(palette, paletteRGBA);
	set_palette(paletteRGBA);

	// We use it now
	palCustom = 0;
}

/** Preload all sprites */
void preload_sprites() {
	int32 i;
	int32 numEntries = hqr_num_entries(HQR_SPRITES_FILE) - 1;

	for (i = 0; i < numEntries; i++) {
		spriteSizeTable[i] = hqr_getalloc_entry(&spriteTable[i], HQR_SPRITES_FILE, i);
	}
}

/** Preload all animations */
void preload_animations() {
	int32 i;
	int32 numEntries = hqr_num_entries(HQR_ANIM_FILE) - 1;

	for (i = 0; i < numEntries; i++) {
		animSizeTable[i] = hqr_getalloc_entry(&animTable[i], HQR_ANIM_FILE, i);
	}
}

/** Preload all animations */
void preload_samples() {
	int32 i;
	int32 numEntries = hqr_num_entries(HQR_SAMPLES_FILE) - 1;

	for (i = 0; i < numEntries; i++) {
		samplesSizeTable[i] = hqr_getalloc_entry(&samplesTable[i], HQR_SAMPLES_FILE, i);
	}
}

/** Initialize resource pointers */
void init_resources() {
	int32 size = 0;

	// Menu and in-game palette
	init_palettes();

	// load LBA font
	size = hqr_getalloc_entry(&fontPtr, HQR_RESS_FILE, RESSHQR_LBAFONT);

	set_font_parameters(2, 8);
	set_font_color(14);
	set_text_cross_color(136, 143, 2);

	hqr_getalloc_entry(&spriteShadowPtr, HQR_RESS_FILE, RESSHQR_SPRITESHADOW);

	// load sprite actors bounding box data
	size = hqr_getalloc_entry(&spriteBoundingBoxPtr, HQR_RESS_FILE, RESSHQR_SPRITEBOXDATA);

	preload_sprites();
	preload_animations();
	//preload_samples();
}
