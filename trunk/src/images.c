/** @file images.c
	@brief
	This file contains image processing.
	
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "images.h"
#include "resources.h"
#include "main.h"
#include "sdlengine.h"
#include "music.h"
#include "hqrdepack.h"


/** Load and display Adeline Logo */
void adeline_logo()
{
	play_midi_music(31,0);

	load_image(RESSHQR_ADELINEIMG, 1);
	delay(7000); //TODO: adjust frames per second using cfgfile.Fps value
	fade_out(paletteRGBACustom);
	palCustom=1;
}

/** Load and display Main Menu image */
void load_menu_image( short int fade_in )
{
	hqr_get_entry(workVideoBuffer, HQR_RESS_FILE, RESSHQR_MENUIMG);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	if( fade_in )
	{
		fade_2_pal(paletteRGBA);
	}else{
		set_palette(paletteRGBA);
	}

	palCustom=0;
}

/** Load a custom palette */
void load_custom_palette(int index)
{	
	hqr_get_entry(palette, HQR_RESS_FILE, index);
	convert_pal_2_RGBA(palette, paletteRGBACustom);
}

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect
	@param index \a RESS.HQR entry index (starting from 0) */
void load_image(int index, short int fade_in)
{
	hqr_get_entry(workVideoBuffer, HQR_RESS_FILE, index);
	copy_screen(workVideoBuffer, frontVideoBuffer);
	load_custom_palette(index+1);
	if( fade_in )
	{
		fade_2_pal(paletteRGBACustom);
	}else{
		set_palette(paletteRGBACustom);
	}

	palCustom = 1;
}

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect and delay
	@param index \a RESS.HQR entry index (starting from 0)
	@param time number of seconds to delay */
void load_image_delay(int index, int time)
{
	load_image(index, 1);
	delay(1000*time);
	fade_out(paletteRGBACustom);
}

/** Converts in-game palette to SDL palette
	@param palSource palette source with RGB
	@param palDest palette destination with RGBA */
void convert_pal_2_RGBA(unsigned char * palSource, unsigned char * palDest)
{
  int i;

  for (i = 0; i < NUMOFCOLORS; i++)
  {
    palDest[0] = palSource[0];
    palDest[1] = palSource[1];
    palDest[2] = palSource[2];
    palDest+=4;
    palSource+=3;
  }
}

/** Fade image in
	@param palette current palette to fade in */
void fade_in(unsigned char * palette)
{
	if(cfgfile.CrossFade)
		cross_fade(frontVideoBuffer, palette);
	else
		fade_2_pal(palette);

	set_palette(palette);
}

/** Fade image out
	@param palette current palette to fade out */
void fade_out(unsigned char * palette)
{
	/*if(cfgfile.CrossFade)
		cross_fade(frontVideoBuffer, palette);
	else
		fade_2_black(palette);*/
	if(!cfgfile.CrossFade)
		fade_2_black(palette);
}

/** Calculate a new color component according with an intensity
	@param modifier color compenent
	@param color color value
	@param param unknown
	@param intensity intensity value to adjust
	@return new color component*/
int cross_dot(int modifier, int color, int param, int intensity)
{
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
void adjust_palette(unsigned char R, unsigned char G, unsigned char B, unsigned char * palette, int intensity)
{
	unsigned char localPalette[NUMOFCOLORS*4];
	unsigned char *newR;
	unsigned char *newG;
	unsigned char *newB;
	unsigned char *newA;

	int local;
	int counter = 0;
	int i;

	local = intensity;

	newR = &localPalette[0];
	newG = &localPalette[1];
	newB = &localPalette[2];
	newA = &localPalette[3];

	for (i = 0; i < NUMOFCOLORS; i++)
	{
		*newR = cross_dot(R, palette[counter], 100, local);
		*newG = cross_dot(G, palette[counter + 1], 100, local);
		*newB = cross_dot(B, palette[counter + 2], 100, local);
		*newA = 0;

		newR += 4;
		newG += 4;
		newB += 4;
		newA += 4;

		counter += 4;
	}

	set_palette(localPalette);
}

/** Fade image to black
	@param palette current palette to fade */
void fade_2_black(unsigned char *palette)
{
	int i = 0;

	if (palReseted == 0)
	{
		for (i = 100; i >= 0; i -= 3)
		{
			adjust_palette(0, 0, 0, (unsigned char *) palette, i);
			fps_cycles(50); //TODO: adjust frames per second using cfgfile.Fps value
		}
	}

	palReseted = 1;
}

/** Fade image with another palette source
	@param palette current palette to fade */
void fade_2_pal(unsigned char *palette)
{
	int i = 100;

	for (i = 0; i <= 100; i += 3)
	{
		adjust_palette(0, 0, 0, (unsigned char *) palette, i);
		fps_cycles(50); //TODO: adjust frames per second using cfgfile.Fps value
	}

	set_palette( (unsigned char*)palette );

	palReseted = 0;

}

/** Fade black palette to with palette */
void black_2_white()
{
	unsigned char palette[NUMOFCOLORS*4];
	int i;

	i = 256;
	for (i = 0; i < NUMOFCOLORS; i += 3)
	{
		memset(palette, i, 1024);

		set_palette(palette);
	}
}

/** Resets both in-game and sdl palettes */
void set_back_pal()
{
	memset(palette, 0, NUMOFCOLORS*3);
	memset(paletteRGBA, 0, NUMOFCOLORS*4);

	set_palette(paletteRGBA);

	palReseted = 1;
}

/** Copy a determinate screen buffer to another
	@param source screen buffer
	@param destination screen buffer */
void copy_screen(unsigned char * source, unsigned char * destination)
{
	int w,h;
	if(SCALE==1)
		memcpy(destination, source, SCREEN_WIDTH*SCREEN_HEIGHT);
	else if (SCALE==2)
		for(h=0; h<SCREEN_HEIGHT/SCALE; h++)
		{
			for(w=0; w<SCREEN_WIDTH/SCALE; w++)
			{
				*destination++ = *source;
				*destination++ = *source++;
			}
			memcpy(destination,destination-SCREEN_WIDTH,SCREEN_WIDTH);
			destination+=SCREEN_WIDTH;
		}
	
}

/** Clear front buffer screen */
void clear_screen()
{
	memset(frontVideoBuffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
}

/** Init palettes */
void init_palettes()
{
	// Init standard palette
	hqr_get_entry(palette, HQR_RESS_FILE, RESSHQR_MAINPAL);
	convert_pal_2_RGBA(palette, paletteRGBA);
	set_palette(paletteRGBA);

	// We use it now
	palCustom=0;
}