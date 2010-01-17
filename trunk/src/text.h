/** @file text.h
	@brief
	This file contains dialogues processing. It contains text and font functions.

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

#ifndef DIALOGUES_H
#define DIALOGUES_H

#include "sys.h"

//TODO: add all 14 colors here for future use
/** White color value */
#define WHITE_COLOR_0		0

/** Current text bank */
int32 currentTextBank;
/** Current dialogue text size */
int32 currDialTextSize;
/** Current dialogue text pointer */
uint8 *currDialTextPtr;

/** Font buffer pointer */
uint8 * fontPtr;

/** Dialogue text size */
int32 dialTextSize;
/** Pixel size between dialogue text */
int32 dialSpaceBetween;
/** Pixel size of the space character */
int32 dialCharSpace;
/** Dialogue text color */
int32 dialTextColor;

/** Dialogue text start color for cross coloring dialogues */
int32 dialTextStartColor;
/** Dialogue text stop color for cross coloring dialogues */
int32 dialTextStopColor;
/** Dialogue text step size for cross coloring dialogues */
int32 dialTextStepSize;
/** Dialogue text buffer size for cross coloring dialogues */
int32 dialTextBufferSize;

int32 dialTextBoxLeft;   // dialogueBoxLeft
int32 dialTextBoxTop;    // dialogueBoxTop
int32 dialTextBoxRight;  // dialogueBoxRight
int32 dialTextBoxBottom; // dialogueBoxBottom

int32 dialTextBoxParam1; // dialogueBoxParam1
int32 dialTextBoxParam2; // dialogueBoxParam2

// TODO: refactor all this variables and related functions
int32 printTextVar13;
uint8 buf1[256];
uint8 buf2[256];
uint8 *printText8Ptr1;
uint8 *printText8Ptr2;
int32 printText8Var1;
int32 printText8Var2;
int32 printText8Var3;
int32 TEXT_CurrentLetterX;
int32 printText8Var5;
int32 printText8Var6;
int32 TEXT_CurrentLetterY;
uint8 *printText8Var8;
int32 newGameVar4;
int32 newGameVar5;
int32 printTextVar5;
int32 printText10Var1;
int32 addLineBreakX;
int16 pt8s4[96];
int32 printText8PrepareBufferVar2;


/** Initialize dialogue
	@param bankIdx Text bank index*/
void init_text_bank(int32 bankIdx);

/** Display a certain dialogue text in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param dialogue ascii text to display */
void draw_text(int32 x, int32 y, int8 *dialogue);

void draw_text_fullscreen(int32 index);

/** Gets dialogue text width size
	@param dialogue ascii text to display */
int32 get_text_size(int8 *dialogue);

void set_font(uint8 *font, int32 charSpace, int32 spaceBetween);

/** Set font type parameters
	@param spaceBetween number in pixels of space between characters
	@param charSpace number in pixels of the character space */
void set_font_parameters(int32 spaceBetween, int32 charSpace);

/** Set the font cross color
	@param color color number to choose */
void set_font_cross_color(int32 color);

/** Set the font color
	@param color color number to choose */
void set_font_color(int32 color);

/** Set font color parameters to precess cross color display
	@param stopColor color number to stop
	@param startColor color number to start
	@param stepSize step size to change between those colors */
void set_text_cross_color(int32 stopColor, int32 startColor, int32 stepSize);

/** Get dialogue text into text buffer
	@param index dialogue index */
int32 get_text(int32 index);

/** Gets menu dialogue text
	@param index text index to display
	@param dialogue dialogue text buffer to display */
void get_menu_text(int32 index, int8 *text);


#endif
