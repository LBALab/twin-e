/** @file resources.c
    @brief
    This file contains the definitions of most used game resources.

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

#include "resources.h"
#include "text.h"
#include "scene.h"
#include "animations.h"
#include "screens.h"
#include "platform.h"
#include "sample.h"


int8 * HQR_RESS_FILE			= "ress.hqr";
int8 * HQR_TEXT_FILE			= "text.hqr";
int8 * HQR_FLASAMP_FILE			= "flasamp.hqr";
int8 * HQR_MIDI_MI_FILE		    = "midi_mi.hqr";
int8 * HQR_SAMPLES_FILE			= "samples.hqr";
int8 * HQR_LBA_GRI_FILE			= "lba_gri.hqr";
int8 * HQR_LBA_BLL_FILE			= "lba_bll.hqr";
int8 * HQR_LBA_BRK_FILE			= "lba_brk.hqr";
int8 * HQR_SCENE_FILE			= "scene.hqr";
int8 * HQR_SPRITES_FILE			= "sprites.hqr";
int8 * HQR_FILE3D_FILE			= "file3d.hqr";
int8 * HQR_BODY_FILE			= "body.hqr";
int8 * HQR_ANIM_FILE			= "anim.hqr";
int8 * HQR_INVOBJ_FILE			= "invobj.hqr";

uint8* inventory_item_list[NUM_INVENTORY_ITEMS];
uint32 inventory_item_size_list[NUM_INVENTORY_ITEMS];


void resources_init_palettes() {
    hqr_get_entry_alloc(&mainPalette, HQR_RESS_FILE, RESSHQR_MAINPAL);
    convertPalToRGBA(mainPalette, mainPaletteRGBA);
    
    memcpy(palette, mainPalette, NUMOFCOLORS * 3);

    convertPalToRGBA(palette, paletteRGBA);
    // platform_set_palette(paletteRGBA);

    // We use it now
    palCustom = 0;
}

void resources_preload_sprites() {
    int32 i;
    int32 numEntries = hqr_get_num_entries(HQR_SPRITES_FILE) - 1;

    for (i = 0; i < numEntries; i++) {
        spriteSizeTable[i] = hqr_get_entry_alloc(&spriteTable[i], HQR_SPRITES_FILE, i);
    }
}

void resources_preload_animations() {
    int32 i;
    int32 numEntries = hqr_get_num_entries(HQR_ANIM_FILE) - 1;

    for (i = 0; i < numEntries; i++) {
        animSizeTable[i] = hqr_get_entry_alloc(&animTable[i], HQR_ANIM_FILE, i);
    }
}

// void reesource_preload_samples() {
//     int32 i;
//     int32 numEntries = hqr_get_num_entries(HQR_SAMPLES_FILE) - 1;

//     for (i = 0; i < numEntries; i++) {
//         samplesSizeTable[i] = hqrGetallocEntry(&samplesTable[i], HQR_SAMPLES_FILE, i);
//     }
// }

void resources_preload_inventory_items() {
    int32 i;
    int32 numEntries = hqr_get_num_entries(HQR_INVOBJ_FILE) - 1;

    for (i = 0; i < numEntries; i++) {
        inventory_item_size_list[i] = hqr_get_entry_alloc(&inventory_item_list[i], HQR_INVOBJ_FILE, i);
    }
}

void resources_init() {
    resources_init_palettes();

    hqr_get_entry_alloc(&fontPtr, HQR_RESS_FILE, RESSHQR_LBAFONT);
    setFontParameters(2, 8);
    setFontColor(14);
    setTextCrossColor(136, 143, 2);

    hqr_get_entry_alloc(&spriteShadowPtr, HQR_RESS_FILE, RESSHQR_SPRITESHADOW);
    hqr_get_entry_alloc(&spriteBoundingBoxPtr, HQR_RESS_FILE, RESSHQR_SPRITEBOXDATA);

    resources_preload_sprites();
    resources_preload_animations();
    // reesource_preload_samples();
    resources_preload_inventory_items();
}
