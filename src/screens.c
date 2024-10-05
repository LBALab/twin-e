/** @file images.c
    @brief
    This file contains image processing.

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

#include "screens.h"
#include "resources.h"
#include "main.h"
#include "platform.h"
#include "music.h"
#include "hqr.h"
#include "lbaengine.h"

/** In-game palette (should not be used, except in special case. otherwise use other images functions instead) */
uint8 palette[NUMOFCOLORS * 3];

/** SDL converted in-game palette */
uint8 paletteRGBA[NUMOFCOLORS * 4];

/** SDL converted custom palette */
uint8 paletteRGBACustom[NUMOFCOLORS * 4];

/** flag to check if a custom palette is in use */
int16 palCustom;

/** flag to check in the game palette was changed */
int16 palReseted;

/** flag to check if the main flag is locked */
int16 lockPalette;

/** flag to check if we are using a different palette than the main one */
int16 useAlternatePalette;

/** main game palette */
uint8* mainPalette;

/** SDL converted in-game palette */
uint8 mainPaletteRGBA[NUMOFCOLORS * 4];


/** Load and display Adeline Logo */
void adelineLogo() {
    playMidiMusic(31, 0);

    loadImage(RESSHQR_ADELINEIMG, 1);
    platform_delay_skip(7000);
    fadeOut(paletteRGBACustom);
    palCustom = 1;
}

/** Load and display Main Menu image */
void loadMenuImage(int16 fade_in) {
    hqr_get_entry(workVideoBuffer, HQR_RESS_FILE, RESSHQR_MENUIMG);
    copyScreen(workVideoBuffer, frontVideoBuffer);
    if (fade_in) {
        fadeToPal(paletteRGBA);
    } else {
        platform_set_palette(paletteRGBA);
    }

    palCustom = 0;
}

/** Load a custom palette */
void loadCustomPalette(int32 index) {
    hqr_get_entry(palette, HQR_RESS_FILE, index);
    convertPalToRGBA(palette, paletteRGBACustom);
}

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect
    @param index \a RESS.HQR entry index (starting from 0) */
void loadImage(int32 index, int16 fade_in) {
    hqr_get_entry(workVideoBuffer, HQR_RESS_FILE, index);
    copyScreen(workVideoBuffer, frontVideoBuffer);
    loadCustomPalette(index + 1);
    if (fade_in) {
        fadeToPal(paletteRGBACustom);
    } else {
        platform_set_palette(paletteRGBACustom);
    }

    palCustom = 1;
}

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect and platform_delay
    @param index \a RESS.HQR entry index (starting from 0)
    @param time number of seconds to platform_delay */
void loadImageDelay(int32 index, int32 time) {
    loadImage(index, 1);
    platform_delay_skip(1000*time);
    fadeOut(paletteRGBACustom);
}

/** Converts in-game palette to SDL palette
    @param palSource palette source with RGB
    @param palDest palette destination with RGBA */
void convertPalToRGBA(uint8 * palSource, uint8 * palDest) {
    int i;

    for (i = 0; i < NUMOFCOLORS; i++) {
        palDest[0] = palSource[0];
        palDest[1] = palSource[1];
        palDest[2] = palSource[2];
        palDest += 4;
        palSource += 3;
    }
}

/** Fade image in
    @param palette current palette to fade in */
void fadeIn(uint8 * palette) {
    if (config_file.cross_fade)
        platform_cross_fade(frontVideoBuffer, palette);
    else
        fadeToPal(palette);

    platform_set_palette(palette);
}

/** Fade image out
    @param palette current palette to fade out */
void fadeOut(uint8 * palette) {
    /*if(config_file.cross_fade)
        platform_cross_fade(frontVideoBuffer, palette);
    else
        fadeToBlack(palette);*/
    if (!config_file.cross_fade)
        fadeToBlack(palette);
}

/** Calculate a new color component according with an intensity
    @param modifier color compenent
    @param color color value
    @param param unknown
    @param intensity intensity value to adjust
    @return new color component*/
int32 crossDot(int32 modifier, int32 color, int32 param, int32 intensity) {
    if (!param)
        return (color);
    return (((color - modifier) * intensity) / param) + modifier;
}

/** Adjust palette intensity
    @param R red component of color
    @param G green component of color
    @param B blue component of color
    @param palette palette to adjust
    @param intensity intensity value to adjust */
void adjustPalette(uint8 R, uint8 G, uint8 B, uint8 * palette, int32 intensity) {
    uint8 localPalette[NUMOFCOLORS*4];
    uint8 *newR;
    uint8 *newG;
    uint8 *newB;
    uint8 *newA;

    int32 local;
    int32 counter = 0;
    int32 i;

    local = intensity;

    newR = &localPalette[0];
    newG = &localPalette[1];
    newB = &localPalette[2];
    newA = &localPalette[3];

    for (i = 0; i < NUMOFCOLORS; i++) {
        *newR = crossDot(R, palette[counter], 100, local);
        *newG = crossDot(G, palette[counter + 1], 100, local);
        *newB = crossDot(B, palette[counter + 2], 100, local);
        *newA = 0;

        newR += 4;
        newG += 4;
        newB += 4;
        newA += 4;

        counter += 4;
    }

    platform_set_palette(localPalette);
}

/** Adjust between two palettes
    @param pal1 palette from adjust
    @param pal2 palette to adjust */
void adjustCrossPalette(uint8 * pal1, uint8 * pal2) {
    uint8 localPalette[NUMOFCOLORS*4];

    uint8 *newR;
    uint8 *newG;
    uint8 *newB;
    uint8 *newA;

    int32 i;
    int32 counter = 0;
    int32 intensity = 0;

    do
    {
        counter = 0;

        newR = &localPalette[counter];
        newG = &localPalette[counter + 1];
        newB = &localPalette[counter + 2];
        newA = &localPalette[counter + 3];	

        for (i = 0; i < NUMOFCOLORS; i++) {
            *newR = crossDot(pal1[counter], pal2[counter], 100, intensity);
            *newG = crossDot(pal1[counter + 1], pal2[counter + 1], 100, intensity);
            *newB = crossDot(pal1[counter + 2], pal2[counter + 2], 100, intensity);
            *newA = 0;

            newR += 4;
            newG += 4;
            newB += 4;
            newA += 4;

            counter += 4;
        }

        platform_set_palette(localPalette);
        platform_fps_cycles(50);

        intensity++;
    } while(intensity <= 100);
}

/** Fade image to black
    @param palette current palette to fade */
void fadeToBlack(uint8 *palette) {
    int32 i = 0;

    if (palReseted == 0) {
        for (i = 100; i >= 0; i -= 3) {
            adjustPalette(0, 0, 0, (uint8 *) palette, i);
            platform_fps_cycles(50);
        }
    }

    palReseted = 1;
}

/** Fade image with another palette source
    @param palette current palette to fade */
void fadeToPal(uint8 *palette) {
    int32 i = 100;

    for (i = 0; i <= 100; i += 3) {
        adjustPalette(0, 0, 0, (uint8 *) palette, i);
        platform_fps_cycles(50);
    }

    platform_set_palette((uint8*)palette);

    palReseted = 0;
}

/** Fade black palette to with palette */
void blackToWhite() {
    uint8 palette[NUMOFCOLORS*4];
    int32 i;

    i = 256;
    for (i = 0; i < NUMOFCOLORS; i += 3) {
        memset(palette, i, 1024);

        platform_set_palette(palette);
    }
}

/** Resets both in-game and sdl palettes */
void setBackPal() {
    memset(palette, 0, NUMOFCOLORS*3);
    memset(paletteRGBA, 0, NUMOFCOLORS*4);

    platform_set_palette(paletteRGBA);

    palReseted = 1;
}

/** Fade palette to red palette
    @param palette current palette to fade */
void fadePalRed(uint8 *palette) {
    int32 i = 100;

    for (i = 100; i >= 0; i -= 2) {
        adjustPalette(0xFF, 0, 0, (uint8 *) palette, i);
        platform_fps_cycles(50);
    }
}


/** Fade red to palette
    @param palette current palette to fade */
void fadeRedPal(uint8 *palette) {
    int32 i = 0;

    for (i = 0; i <= 100; i += 2) {
        adjustPalette(0xFF, 0, 0, (uint8 *) palette, i);
        platform_fps_cycles(50);
    }
}

/** Copy a determinate screen buffer to another
    @param source screen buffer
    @param destination screen buffer */
void copyScreen(uint8 * source, uint8 * destination) {
    int32 w, h;

    if (SCALE == 1)
        memcpy(destination, source, SCREEN_WIDTH*SCREEN_HEIGHT);
    else if (SCALE == 2)
        for (h = 0; h < SCREEN_HEIGHT / SCALE; h++) {
            for (w = 0; w < SCREEN_WIDTH / SCALE; w++) {
                *destination++ = *source;
                *destination++ = *source++;
            }
            memcpy(destination, destination - SCREEN_WIDTH, SCREEN_WIDTH);
            destination += SCREEN_WIDTH;
        }

}

/** Clear front buffer screen */
void clearScreen() {
    memset(frontVideoBuffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
}
