/** @file holomap.c
    @brief
    This file contains holomap routines

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

#include "holomap.h"
#include "gamestate.h"
#include "lbaengine.h"
#include "screens.h"
#include "scene.h"
#include "sample.h"
#include "interface.h"
#include "platform.h"
#include "renderer.h"
#include "text.h"
#include "resources.h"

#define STEP_ANGLE 32
#define SLIG (((1024 / STEP_ANGLE) + 1) * 4)
#define SIZE_TEXT_PLANET (((1024 / STEP_ANGLE) + 1) * ((512 / STEP_ANGLE) + 1) * 4 * 2)
#define SIZE_COOR_PLANET (((1024 / STEP_ANGLE) + 1) * ((512 / STEP_ANGLE) + 1) * 3 * 2)
#define SIZE_MAP_SURFACE ((1024 / STEP_ANGLE) * ((512 / STEP_ANGLE) + 1))

#define X_CENTRE_BIG_HOLO 320
#define Y_CENTRE_BIG_HOLO 190
#define WIDTH_BIG_HOLO 300
#define HEIGHT_BIG_HOLO 280
#define ZOOM_BIG_HOLO 9500
#define X_CENTRE_TRAJ_HOLO 320 + 80
#define Y_CENTRE_TRAJ_HOLO 240

typedef struct holomap_pos_s {
    int16 alpha;
    int16 beta;
    int16 size;
    int16 mess;
} holomap_pos_t;

int32 needToLoadHolomapGFX = 0;
uint8 paletteHolomap[NUMOFCOLORS * 3];

uint16 *holomap_planet_ptr;
uint16 *holomap_planet_coord_ptr;
uint8 *holomap_surface_ptr;
uint8 *holomap_map_ptr;
uint8 *holomap_planet_body_ptr;
uint8 *holomap_arrow_ptr;
uint8 *holomap_arrow_body_ptr;
uint8 *holomap_cone_ptr;
uint8 *videoPtr9;
holomap_pos_t *holomap_position_ptr;
uint16 *holomap_tri_ptr;
uint8 *holomap_trajectory_ptr;
uint8 *holomap_last_ptr;

void setHolomapPosition(int32 locationIdx)
{
    holomapFlags[locationIdx] = 0x81;
}

void clearHolomapPosition(int32 locationIdx)
{
    holomapFlags[locationIdx] &= 0x7E;
    holomapFlags[locationIdx] |= 0x40;
}

void holomap_patch_object(uint8 *body_ptr)
{
    int16 flags;
    flags = *(int16 *)(body_ptr);

    if (flags & 2)
    { // only patch animated models
        uint8 *ptr = (uint8 *)body_ptr;
        int16 data_offset = *(int16 *)(ptr + 14);
        int16 num_groups;
        int16 num_points;

        ptr += data_offset + 16;

        num_points = *(int16 *)(ptr);
        ptr += 2 + (num_points * 6);

        num_groups = *(int16 *)(ptr);
        ptr += 2;

        for (; num_groups > 0; --num_groups)
        {
            ptr += 38;
            *(int16 *)(ptr + 6) = (*(int16 *)(ptr + 6) * 36) / 38;
        }
    }
}

int32 rule_of_three(int32 val1, int32 val2, int32 nbstep, int32 step)
{
    if (nbstep < 0)
    {
        return val2;
    }
    return (((val2 - val1) * step) / nbstep) + val1;
}

void holomap_compute_coor_mapping()
{
    int16 alpha, beta;
    uint16 *ptr;

    ptr = holomap_planet_ptr;

    for (alpha = -256; alpha <= 256; alpha += STEP_ANGLE)
    {
        for (beta = 0; beta < 1024; beta += STEP_ANGLE)
        {
            ptr += 2;
            *ptr++ = (uint16)rule_of_three(0, 255 * 256 + 255, 1023, beta);
            if (alpha == 256)
            {
                *ptr++ = 256 * 255 + 255;
            }
            else
            {
                *ptr++ = (uint16)(((alpha + 256) * 256) / 2);
            }
        }

        ptr += 2;
        *ptr++ = 255 * 256 + 255;
        if (alpha == 256)
        {
            *ptr++ = 256 * 255 + 255;
        }
        else
        {
            *ptr++ = (uint16)(((alpha + 256) * 256) / 2);
        }
    }
}

int32 z_sort(const void *a, const void *b)
{
    return (*(int16 *)a - *(int16 *)b);
}

void holomap_compute_coor_planet()
{ // TODO
}

void loadHolomapGFX()
{
    int32 i;
    int32 j;

    holomap_planet_ptr = (uint16 *)workVideoBuffer;
    holomap_planet_coord_ptr = (uint16 *)workVideoBuffer + SIZE_TEXT_PLANET;
    holomap_surface_ptr = workVideoBuffer + SIZE_TEXT_PLANET + SIZE_COOR_PLANET;
    holomap_map_ptr = workVideoBuffer + SIZE_TEXT_PLANET + SIZE_COOR_PLANET + SIZE_MAP_SURFACE;

    holomap_planet_body_ptr = workVideoBuffer + SIZE_TEXT_PLANET + SIZE_COOR_PLANET + SIZE_MAP_SURFACE + 65536;

    hqr_get_entry(holomap_surface_ptr, HQR_RESS_FILE, RESSHQR_HOLOSURFACE);
    hqr_get_entry(holomap_map_ptr, HQR_RESS_FILE, RESSHQR_HOLOIMG);

    holomap_arrow_ptr = holomap_planet_body_ptr + hqr_get_entry(holomap_planet_body_ptr, HQR_RESS_FILE, RESSHQR_HOLOTWINMDL);
    holomap_arrow_body_ptr = holomap_arrow_ptr + hqr_get_entry(holomap_arrow_ptr, HQR_RESS_FILE, RESSHQR_HOLOARROWMDL);
    holomap_cone_ptr = holomap_arrow_body_ptr + hqr_get_entry(holomap_arrow_body_ptr, HQR_RESS_FILE, RESSHQR_HOLOTWINARROWMDL);
    holomap_tri_ptr = (uint16 *)holomap_cone_ptr + hqr_get_entry(holomap_cone_ptr, HQR_RESS_FILE, RESSHQR_HOLOPOINTMDL);

    holomap_patch_object(holomap_planet_body_ptr);
    holomap_patch_object(holomap_arrow_ptr);
    holomap_patch_object(holomap_arrow_body_ptr);
    holomap_patch_object(holomap_cone_ptr);

    holomap_position_ptr = (holomap_pos_t *)holomap_tri_ptr + 4488;
    holomap_trajectory_ptr = (uint8 *)holomap_position_ptr + hqr_get_entry((uint8 *)holomap_position_ptr, HQR_RESS_FILE, RESSHQR_HOLOARROWINFO);
    holomap_last_ptr = holomap_trajectory_ptr + hqr_get_entry(holomap_trajectory_ptr, HQR_RESS_FILE, RESSHQR_HOLOPOINTANIM);

    loadCustomPalette(RESSHQR_HOLOPAL);

    j = 576;
    for (i = 0; i < 96; i += 3, j += 3) {
        paletteHolomap[i] = palette[j];
        paletteHolomap[i + 1] = palette[j + 1];
        paletteHolomap[i + 2] = palette[j + 2];
    }

    j = 576;
    for (i = 96; i < 189; i += 3, j += 3) {
        paletteHolomap[i] = palette[j];
        paletteHolomap[i + 1] = palette[j + 1];
        paletteHolomap[i + 2] = palette[j + 2];
    }

    holomap_compute_coor_mapping();
    holomap_compute_coor_planet();

    needToLoadHolomapGFX = 0;
}

void holomap_draw_title(int32 x, int32 y)
{
    int8 string[256];

    strcpy((char *)string, "HoloMap");

    x -= (int16)(getTextSize(string) / 2);
    y -= 18;

    setFontColor(12 * 16 + 10);
    drawText(x - 1, y - 1, string);
    drawText(x, y - 1, string);
    drawText(x + 1, y - 1, string);

    drawText(x - 1, y + 1, string);
    drawText(x, y + 1, string);
    drawText(x + 1, y + 1, string);

    drawText(x - 1, y, string);
    drawText(x + 1, y, string);

    setFontColor(15);
    drawText(x, y, string);
}

void holomap_draw_trajectory(int32 trajectoryIndex)
{ // TODO
}

void holomap_run()
{ // TODO
    int32 alphaLightTmp;
    int32 betaLightTmp;

    freezeTime();

    // TODO memcopy palette

    alphaLightTmp = alphaLight;
    betaLightTmp = betaLight;

    fadeToBlack(paletteRGBA);
    sample_stop_all();
    resetClip();
    clearScreen();
    platform_flip();
    copyScreen(frontVideoBuffer, workVideoBuffer);

    loadHolomapGFX();
    holomap_draw_title(320, 25);
    setCameraPosition(320, 190, 128, 1024, 1024);

    config_file.language_cd_id = 0;
    initTextBank(2);
    setFontCrossColor(9);

    // TODO

    newGameVar4 = 1;
    fadeToBlack(paletteRGBA);
    alphaLight = alphaLightTmp;
    betaLight = betaLightTmp;
    initEngineVars(0);

    initTextBank(currentTextBank + 3);

    // TODO memcopy reset palette

    unfreezeTime();
}
