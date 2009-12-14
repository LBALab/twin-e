/** @file dialogues.c
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

#include "dialogues.h"
#include "main.h"
#include "hqrdepack.h"
#include "resources.h"
#include "sdlengine.h"
#include "menu.h"
#include "interface.h"

#include <math.h>

// TODO: CHECK THIS LATER
unsigned char textVar2[256];
unsigned char textVar3;

/* Internal representation for drawn strings */
typedef struct PrintedTextBuf
{
	unsigned char* ptBuffer;
	unsigned char* ptSeek;

	unsigned int Size;
	unsigned int pixelSize;
} TextBuffer;

void init_text_buf( TextBuffer* buf, unsigned char* cbuf );
void alloc_text_buf( TextBuffer* buf, int size );
int calc_justified_text_line( TextBuffer *compText, TextBuffer *lineText, TextBuffer *wordText, unsigned int maxWidth );

/** */
void draw_character(int x, int y, unsigned char character, short int instant_draw);
void draw_character_shadow(int x, int y, unsigned char character, short int instant_draw);

/** Dialogue text pointer */
unsigned char *dialTextPtr;  // bufText
/** Dialogue entry order pointer */
unsigned char *dialOrderPtr; // bufOrder
/** Number of dialogues text entries */
short int numDialTextEntries;

/** Initialize dialogue
	@param bankIdx Text bank index*/
void init_dialogue_bank(int bankIdx)
{
	int langIdx;
	int hqrSize;

	// don't load if we already have the dialogue text bank loaded
	if (bankIdx == currentTextBank)
		return;
	
	currentTextBank = bankIdx;
	// TODO: V V V CHECK THIS LATER 
	textVar2[0] = textVar3;

	// get index according with language
	langIdx = (cfgfile.LanguageId * 14) * 2  + bankIdx * 2;
	
	hqrSize = hqr_getalloc_entry(&dialOrderPtr, HQR_TEXT_FILE, langIdx);

	numDialTextEntries = hqrSize / 2;

	hqrSize = hqr_getalloc_entry(&dialTextPtr, HQR_TEXT_FILE, ++langIdx);

	/*if (cfgfile.UseCD && !cfgfile.NoSound)
		loadVox(index);*/
}

void init_text_buf( TextBuffer* buf, unsigned char* cbuf )
{
	buf->ptBuffer = buf->ptSeek = cbuf;
	buf->Size = 0;
	buf->pixelSize = 0;
}

void alloc_text_buf( TextBuffer* buf, int size )
{
	init_text_buf( buf, (unsigned char*)malloc(size*sizeof(unsigned char)) );
	*(buf->ptBuffer)='\0';
}

/* Returns 1 if this is the last line, 2 if this is the last line of the current page, 0 otherwise */
int calc_justified_text_line( TextBuffer *compText, TextBuffer *lineText, TextBuffer *wordText, unsigned int maxWidth )
{
	unsigned char *font_data;

	// New line
	int lastLine = 0;
	int spaceLength = dialCharSpace;

	// Re-init line buffer
	init_text_buf( lineText, lineText->ptBuffer );
	*lineText->ptBuffer = '\0';

	for(;;) // Each iteration is for a word
	{
		unsigned char* oldSeek = compText->ptSeek;

		// Re-init word buffer
		init_text_buf( wordText, wordText->ptBuffer );
		*wordText->ptBuffer = '\0';

		// Skip white spaces
		while( *(compText->ptSeek) == ' ' )
			++(compText->ptSeek);

		// If it's a '@', let's cut the line here
		if( *(compText->ptSeek) == '@' )
		{
			compText->ptSeek++;
			break;
		}

		// Get the current word
		wordText->pixelSize = 0;
		while( *(compText->ptSeek) != ' ' ) // Seek through the whole word
		{
			font_data = fontPtr + *((short int *) (fontPtr + (*(compText->ptSeek)) * 4));
			wordText->pixelSize += *(font_data)+2; // TODO: Use real font structure

			*(wordText->ptSeek++) = *(compText->ptSeek++);

			if( *(compText->ptSeek) == '\0' )
			{
				// This is last line
				lastLine = 1;
				break;
			}
		}
		*(wordText->ptSeek) = '\0';

		// Can we add this word ?
		if( lineText->pixelSize + wordText->pixelSize + spaceLength <= maxWidth )
		{
			// Yes, add it
			lineText->pixelSize += wordText->pixelSize;
			++lineText->Size;
			
			if( lineText->Size > 1 )
			{
				lineText->pixelSize += spaceLength;
				strcat( (char*) lineText->ptBuffer, " ");
			}

			strcat( (char*) lineText->ptBuffer, (char*) wordText->ptBuffer );
		}else{
			// We can't, we've already reached the end of line
			// revert to the previous seek pointer
			compText->ptSeek = oldSeek;

			// Calcultate space length for text to be aligned
			spaceLength += (unsigned int)floor((float)(maxWidth-lineText->pixelSize)/(float)(lineText->Size-1));

			// Even if next word is the last one, we couldn't add it.
			lastLine = 0;
			break;
		}

		// If we know this is last line, let's end (text won't be aligned, it's on purpose)
		if( lastLine )
			break;
	}

	//lineText->ptSeek = lineText->ptBuffer;
	lineText->pixelSize = spaceLength;

	return lastLine;
}

void display_dialogue_fullscreen(int dialog, unsigned char color) //char *dialogue
{	
	int				x=16, y=16;
	unsigned char	start_color=color;
	unsigned char	end_color=color+12;
	unsigned char	currChar;
	unsigned int	wordCount=0;

	TextBuffer compText;	// Page Buffer
	TextBuffer lineText;	// Line Buffer
	TextBuffer wordText;	// Word buffer

	if (fontPtr == 0)   // if the font is not defined
		return;

	// Init of our temporary text buffers
	get_text(dialog);
	init_text_buf( &compText, currDialTextPtr );
	alloc_text_buf( &lineText, 100 );
	alloc_text_buf( &wordText, 30 );

	dialTextColor = start_color;

	for(;;) // Each iteration is drawing a line
	{
		int lastline = calc_justified_text_line( &compText, &lineText, &wordText, 604 );

		// Line drawing code
		for( ;; )
		{
			currChar = (unsigned char) *(lineText.ptSeek);  // read the next char from the string

			if( currChar == ' ' )
			{
				// White space
				x += lineText.pixelSize;
				++wordCount;
			}
			else if( currChar == '\0' )
			{
				// End of line
				break;
			}
			else
			{
				/*for( localSeek = lineText->ptSeek; localSeek != lineText->ptSeek-5 && localSeek != lineText->ptBuffer-1; --localSeek )
				{
					// Draw character
					printf( "%c", *localSeek );
				}*/
				draw_character_shadow(x, y, currChar, 1); // draw the character on screen			
				// add the length of the space between 2 characters
				x += dialSpaceBetween;  
				// add the length of the current character
				x += dialTextSize;    

				dialTextColor++;
				if( dialTextColor > end_color )
					dialTextColor = end_color;

			}

			++lineText.ptSeek;
		}

		// Init next line position
		x =  16;
		y += 38;

		if( y > 400 )
		{
			// End of current page
			break;
		}else if (lastline)
		{
			// End of dialog
			break;
		}
	}

	free(lineText.ptBuffer);
	free(wordText.ptBuffer);

	/*do
	{
		currChar = (unsigned char) *(currDialTextPtr++);  // read the next char from the string

		if (currChar == 0) // if the char is 0x0, -> end of string
			break;

		if (currChar == 0x20)  // if it's a space char
			x += dialCharSpace;
		else
		{			
			draw_character_shadow(x, y, currChar, 1); // draw the character on screen			
			// add the length of the space between 2 characters
			x += dialSpaceBetween;  
			// add the length of the current character
			x += dialTextSize;    

			dialTextColor++;
			if( dialTextColor > end_color )
				dialTextColor = end_color;
		}
	}while (1);*/
}

void draw_character_shadow(int x, int y, unsigned char character, short int instant_draw)
{
	unsigned char color;

	color = dialTextColor;
	dialTextColor = 0;
	draw_character( x+2, y+4, character, 0 ); // No instant draw needed
	dialTextColor = color;
	draw_character( x, y, character, instant_draw );
}

/** Draw a certain character in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param character ascii character to display */
void draw_character(int x, int y, unsigned char character, short int instant_draw)
{
	unsigned char sizeX;
	unsigned char sizeY;
	unsigned char keptsizeX;
	unsigned char keptsizeY;
	unsigned char param1;
	unsigned char param2;
	unsigned char *data;
	unsigned char *screen;

	// int temp=0;
	int toNextLine;
	unsigned char index;

	// char color;
	unsigned char usedColor;
	unsigned char number;
	unsigned char jump;

	int i;

	int tempX;
	int tempY;

	data = fontPtr + *((short int *) (fontPtr + character * 4));

	keptsizeX = (dialTextSize = sizeX = *(data++))+2;
	keptsizeY = (sizeY = *(data++))+4;
	param1 = *(data++);
	param2 = *(data++);

	x += param1;
	y += param2;

	usedColor = dialTextColor;

	screen = frontVideoBuffer + screenLockupTable[y] + x;

	tempX = x;
	tempY = y;

	toNextLine = SCREEN_WIDTH - sizeX;

	do
	{
		index = *(data++);
		do
		{
			jump = *(data++);
			screen += jump;
			tempX += jump;
			if (--index == 0)
			{
				screen += toNextLine;
				tempY++;
				tempX = x;
				sizeY--;
				if (sizeY <= 0)
				{
					// If instant_draw, update the screen rectangle
					if( instant_draw )
						copy_block_phys(x, y, x + keptsizeX, y + keptsizeY );
					return;
				}
				break;
			}
			else
			{
				number = *(data++);
				for (i = 0; i < number; i++)
				{
					if(tempX >= SCREEN_TEXTLIMIT_LEFT && tempX < SCREEN_TEXTLIMIT_RIGHT && tempY >= SCREEN_TEXTLIMIT_TOP && tempY < SCREEN_TEXTLIMIT_BOTTOM)
						frontVideoBuffer[SCREEN_WIDTH*tempY + tempX] = usedColor;

					screen++;
					tempX++;
				}

				if (--index == 0)
				{
					screen += toNextLine;
					tempY++;
					tempX = x;

					sizeY--;
					if (sizeY <= 0)
					{
						// If instant_draw, update the screen rectangle
						if( instant_draw )
							copy_block_phys(x, y, x + keptsizeX, y + keptsizeY );
						return;
					}
					break;
				}
			}
		}while (1);
	}while (1);
	
}

/** Display a certain dialogue text in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param dialogue ascii text to display */
void display_dialogue_text(int x, int y, char *dialogue)
{
	unsigned char currChar;

	if (fontPtr == 0)   // if the font is not defined
		return;

	do
	{
		currChar = (unsigned char) *(dialogue++);  // read the next char from the string

		if (currChar == 0) // if the char is 0x0, -> end of string
			break;

		if (currChar == 0x20)  // if it's a space char
			x += dialCharSpace;
		else
		{
			dialTextSize = *(fontPtr + (*((short int *) (fontPtr + currChar * 4)))); // get the length of the character
			draw_character(x, y, currChar, 0); // draw the character on screen
			// add the length of the space between 2 characters
			x += dialSpaceBetween;  
			// add the length of the current character
			x += dialTextSize;    
		}
	}while (1);
}

/** Gets dialogue text width size
	@param dialogue ascii text to display */
int dialogue_text_size(char *dialogue)
{
    unsigned char currChar;
    dialTextSize = 0;

    do
	{	
		currChar = (unsigned char) *(dialogue++);

		if (currChar == 0)
			break;

		if (currChar == 0x20)
		{
			dialTextSize += dialCharSpace;
		}
		else
		{
			dialTextSize += dialSpaceBetween;
			dialTextSize += *(fontPtr + *((short int *) (fontPtr + currChar * 4)));
		}
	}
    while (1);

    return (dialTextSize);
}

//TODO: RECHECK THIS LATER
/** Set font type parameters
	@param spaceBetween number in pixels of space between characters
	@param charSpace number in pixels of the character space */
void set_font_parameters(int spaceBetween, int charSpace)
{
	dialSpaceBetween = spaceBetween;
	dialCharSpace = charSpace;
}

/** Set the font color
	@param color color number to choose */
void set_font_color(int color)
{
	dialTextColor = color;
}

/** Set font color parameters to precess cross color display
	@param stopColor color number to stop 
	@param startColor color number to start 
	@param stepSize step size to change between those colors */
void set_dialogue_cross_color(int stopColor, int startColor, int stepSize)
{
	dialTextStartColor = startColor;
	dialTextStopColor = stopColor;
	dialTextStepSize = stepSize;
	dialTextBufferSize = ((startColor-stopColor)+1)/stepSize;
}

/** Get dialogue text into text buffer
	@param index dialogue index */
int get_text(int index)
{
	int currIdx = 0;
	int orderIdx = 0;
	int numEntries;
	int ptrCurrentEntry;
	int ptrNextEntry;

	short int *localTextBuf = (short int *) dialTextPtr;
	short int *localOrderBuf = (short int *) dialOrderPtr;

	numEntries = numDialTextEntries;

	// choose right text from order index
	do
	{
		orderIdx = *(localOrderBuf++);
		if(orderIdx==index)
			break;
		currIdx++;
	}while(currIdx < numDialTextEntries);

	if(currIdx >= numEntries)
		return 0;

	ptrCurrentEntry = localTextBuf[currIdx];
	ptrNextEntry = localTextBuf[currIdx + 1];

	currDialTextPtr = (dialTextPtr + ptrCurrentEntry);
	currDialTextSize = ptrNextEntry - ptrCurrentEntry; 
	numDialTextEntries = numEntries;

	return 1;
}

/** Copy dialogue text
	@param src source text buffer 
	@param dst destination text buffer
	@param size text size */
void copy_dialogues_text(char *src, char *dst, int size)
{
    int i;
    for (i = 0; i < size; i++)
		*(dst++) = *(src++);
}

/** Gets menu dialogue text
	@param index text index to display
	@param dialogue dialogue text buffer to display */
void get_menu_text(int index, char *dialogue)
{
	if (index == currMenuTextIndex)
	{
		if (currMenuTextBank == currentTextBank)
		{
			strcpy(dialogue, currMenuTextBuffer);
			return;
		}
	}
	if (!get_text(index))
	{
		// if doesn't have text
		dialogue[0] = 0;
		return;
	}

    if ((currDialTextSize - 1) > 0xFF)
		currDialTextSize = 0xFF;

    copy_dialogues_text((char *) currDialTextPtr, dialogue, currDialTextSize);
    currDialTextSize++;
    copy_dialogues_text(dialogue, currMenuTextBuffer, currDialTextSize);

    currMenuTextIndex = index;
    currMenuTextBank = currentTextBank;
}
