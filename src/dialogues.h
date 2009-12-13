/** @file dialogues.h
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
*/

#ifndef DIALOGUES_H
#define DIALOGUES_H

//TODO: add all 14 colors here for future use
/** White color value */
#define WHITE_COLOR_0		0

/** Current text bank */
int currentTextBank;
/** Current dialogue text size */
int currDialTextSize;
/** Current dialogue text pointer */
unsigned char *currDialTextPtr;

/** Font buffer pointer */
unsigned char * fontPtr;

/** Dialogue text size */
int dialTextSize;
/** Pixel size between dialogue text */
int dialSpaceBetween;
/** Pixel size of the space character */
int dialCharSpace;
/** Dialogue text color */
int dialTextColor;

/** Dialogue text start color for cross coloring dialogues */
int dialTextStartColor;
/** Dialogue text stop color for cross coloring dialogues */
int dialTextStopColor;
/** Dialogue text step size for cross coloring dialogues */
int dialTextStepSize;
/** Dialogue text buffer size for cross coloring dialogues */
int dialTextBufferSize;


/** Initialize dialogue
	@param bankIdx Text bank index*/
void init_dialogue_bank(int bankIdx);

/** Display a certain dialogue text in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param dialogue ascii text to display */
void display_dialogue_text(int x, int y, char *dialogue);

/** Gets dialogue text width size
	@param dialogue ascii text to display */
int dialogue_text_size(char *dialogue);

/** Set font type parameters
	@param spaceBetween number in pixels of space between characters
	@param charSpace number in pixels of the character space */
void set_font_parameters(int spaceBetween, int charSpace);

/** Set the font color
	@param color color number to choose */
void set_font_color(int color);

/** Set font color parameters to precess cross color display
	@param stopColor color number to stop 
	@param startColor color number to start 
	@param stepSize step size to change between those colors */
void set_dialogue_cross_color(int stopColor, int startColor, int stepSize);

/** Get dialogue text into text buffer
	@param index dialogue index */
int get_text(int index);

/** Gets menu dialogue text
	@param index text index to display
	@param dialogue dialogue text buffer to display */
void get_menu_text(int index, char *dialogue);


void display_dialogue_fullscreen(int dialog, unsigned char color);


#endif
