/** @file interface.c
	@brief
	This file contains in-game interface routines
	
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

#include "interface.h"
#include "sdlengine.h"
#include "main.h"

/** Draw button line
	@param startWidth width value where the line starts
	@param startHeight height value where the line starts
	@param endWidth width value where the line ends
	@param endHeight height value where the line ends
	@param lineColor line color in the current palette */
void draw_line(int startWidth, int startHeight, int endWidth, int endHeight, int lineColor)
{
	int temp;
	short int flag;
	int flag2;
	unsigned char *out;
	short int color;
	short int var2;
	short int xchg;
	int currentLineColor = lineColor;

	// draw line from left to right
	if (startWidth > endWidth) 
	{
		temp = endWidth;
		endWidth = startWidth;
		startWidth = temp;

		temp = endHeight;
		endHeight = startHeight;
		startHeight = temp;
	}

	flag = 0;

	if (startWidth < SCREEN_TEXTLIMIT_LEFT)
	{
		flag |= 1;
	}
	else
	{
		if (startWidth > SCREEN_TEXTLIMIT_RIGHT)
			return;
	}

	if (startHeight < SCREEN_TEXTLIMIT_TOP)
	{
		flag |= 8;
	}
	else
	{
		if (startHeight > SCREEN_TEXTLIMIT_BOTTOM)
			flag |= 4;
	}

	flag <<= 8;

	if (endWidth < SCREEN_TEXTLIMIT_LEFT)
		return;
	if (endWidth <= SCREEN_TEXTLIMIT_LEFT)
		flag |= 2;

	if (endHeight < SCREEN_TEXTLIMIT_TOP)
	{
		flag |= 8;
	}
	else
	{
		if (endHeight > SCREEN_TEXTLIMIT_BOTTOM)
			flag |= 4;
	}

	flag2 = flag;

	if (flag)
		return;

	flag2 = 640;//SCREEN_WIDTH;
	endWidth -= startWidth;
	endHeight -= startHeight;
	if (endHeight < 0)
	{
		flag2 = -flag2;
		endHeight = -endHeight;
	}

	out = frontVideoBuffer + screenLockupTable[startHeight] + startWidth;

	color = currentLineColor;
	if (endWidth < endHeight)      // significant slope
	{
		xchg = endWidth;
		endWidth = endHeight;
		endHeight = xchg;
		var2 = endWidth;
		var2 <<= 1;
		startHeight = endWidth;
		endHeight <<= 1;
		endWidth++;
		do
		{
			*out = (unsigned char) color;
			startHeight -= endHeight;
			if (startHeight > 0)
			{
				out += flag2;
			}
			else
			{
				startHeight += var2;
				out += flag2 + 1;
			}
		}while (--endWidth);
	}
	else      // reduced slope
	{
		var2 = endWidth;
		var2 <<= 1;
		startHeight = endWidth;
		endHeight <<= 1;
		endWidth++;
		do
		{
			*out = (unsigned char) color;
			out++;
			startHeight -= endHeight;
			if (startHeight < 0)
			{
				startHeight += var2;
				out += flag2;
			}
		}while (--endWidth);
	}
}

/** Blit button box from working buffer to front buffer
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button
	@source source screen buffer, in this case working buffer
	@param leftDest start width to draw the button in destination buffer
	@param topDest start height to draw the button in destination buffer
	@dest destination screen buffer, in this case front buffer */
void blit_box(int left, int top, int right, int bottom, char *source, int leftDest, int topDest, char *dest)
{
	int width;
	int height;
	char *s;
	char *d;
	int insideLine;
	int temp3;
	int i;
	int j;

	s = screenLockupTable[top] + source + left;
	d = screenLockupTable[topDest] + dest + leftDest;

	width = right - left + 1;
	height = bottom - top + 1;

	insideLine = SCREEN_WIDTH - width;
	temp3 = left;

	left >>= 2;
	temp3 &= 3;

	for(j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			*(d++) = *(s++);
		}

		d += insideLine;
		s += insideLine;
	}
}

/** Draws inside buttons transparent area
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button
	@param colorAdj index to adjust the transparent box color */
void draw_transparent_box(int left, int top, int right, int bottom, int colorAdj)
{
    unsigned char *pos;
    int width;
    int height;
    int height2;
    int temp;
    int localMode;
    int var1;
    char color;
    char color2;

	if (left > SCREEN_TEXTLIMIT_RIGHT)
		return;
	if (right < SCREEN_TEXTLIMIT_LEFT)
		return;
	if (top > SCREEN_TEXTLIMIT_BOTTOM)
		return;
    if (bottom < SCREEN_TEXTLIMIT_TOP)
		return;

    if (left < SCREEN_TEXTLIMIT_LEFT)
		left = SCREEN_TEXTLIMIT_LEFT;
    if (right > SCREEN_TEXTLIMIT_RIGHT)
		right = SCREEN_TEXTLIMIT_RIGHT;
	if (top < SCREEN_TEXTLIMIT_TOP)
		top = SCREEN_TEXTLIMIT_TOP;
	if (bottom > SCREEN_TEXTLIMIT_BOTTOM)
		bottom = SCREEN_TEXTLIMIT_BOTTOM;

	pos = screenLockupTable[top] + frontVideoBuffer + left;
	height2 = height = bottom - top;
	height2++;

    width = right - left + 1;

    temp = 640 - width; // SCREEN_WIDTH
    localMode = colorAdj;

    do
	{
		var1 = width;
		do
		{
			color2 = color = *pos;
			color2 &= 0xF0;
			color &= 0x0F;
            color -= localMode;
			if (color < 0)
				color = color2;
			else
				color += color2;
			*pos++ = color;
			var1--;
		}
		while (var1 > 0);
		pos += temp;
		height2--;
	}
    while (height2 > 0);
}

void draw_splitted_box(int left, int top, int right, int bottom, unsigned char e)
{
	unsigned char *ptr;

    int offset;

    int x;
    int y;

    if (left > SCREEN_TEXTLIMIT_RIGHT)
		return;
	if (right < SCREEN_TEXTLIMIT_LEFT)
		return;
	if (top > SCREEN_TEXTLIMIT_BOTTOM)
		return;
    if (bottom < SCREEN_TEXTLIMIT_TOP)
		return;

	// cropping
    offset = -((right - left) - SCREEN_WIDTH);

    ptr = frontVideoBuffer + screenLockupTable[top] + left;

    for (x = top; x < bottom; x++)
	{
		for (y = left; y < right; y++)
		{
			*(ptr++) = e;
		}
		ptr += offset;
	}
}

void set_clip(int left, int top, int right, int bottom)
{
	if (left < 0)
		left = 0;
    textWindowLeft = left;

	if (top < 0)
		top = 0;
	textWindowTop = top;

	if (right >= SCREEN_WIDTH)
		right = SCREEN_TEXTLIMIT_RIGHT;
	textWindowRight = right;

	if (bottom >= SCREEN_HEIGHT)
		bottom = SCREEN_TEXTLIMIT_BOTTOM;
	textWindowBottom = bottom;
}

void save_clip() // saveTextWindow
{
	textWindowLeftSave = textWindowLeft;
	textWindowTopSave = textWindowTop;
	textWindowRightSave = textWindowRight;
	textWindowBottomSave = textWindowBottom;
}

void reset_clip()
{
	textWindowTop = textWindowLeft = SCREEN_TEXTLIMIT_TOP;
	textWindowRight = SCREEN_TEXTLIMIT_RIGHT;
	textWindowBottom = SCREEN_TEXTLIMIT_BOTTOM;
}
