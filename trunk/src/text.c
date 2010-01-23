/** @file text.c
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

#include <math.h>

#include "text.h"
#include "main.h"
#include "hqrdepack.h"
#include "resources.h"
#include "sdlengine.h"
#include "menu.h"
#include "interface.h"
#include "lbaengine.h"
#include "keyboard.h"
#include "images.h"
#include "renderer.h"

// TODO: CHECK THIS LATER
int32 currentBankIdx = -1; // textVar1
uint8 textVar2[256];
uint8 textVar3;

/** Dialogue text pointer */
uint8 *dialTextPtr;  // bufText
/** Dialogue entry order pointer */
uint8 *dialOrderPtr; // bufOrder
/** Number of dialogues text entries */
int16 numDialTextEntries;


// TODO: refactor this
int32 wordSizeChar;
int32 wordSizePixel;

int8 spaceChar = ' ';


/** Initialize dialogue
	@param bankIdx Text bank index*/
void init_text_bank(int32 bankIdx) { // InitDial
	int32 langIdx;
	int32 hqrSize;

	// don't load if we already have the dialogue text bank loaded
	if (bankIdx == currentBankIdx)
		return;

	currentBankIdx = bankIdx;
	// TODO: V V V CHECK THIS LATER
	textVar2[0] = textVar3;

	// get index according with language
	langIdx = (cfgfile.LanguageId * 14) * 2  + bankIdx * 2;

	hqrSize = hqr_getalloc_entry(&dialOrderPtr, HQR_TEXT_FILE, langIdx);

	numDialTextEntries = hqrSize / 2;

	hqrSize = hqr_getalloc_entry(&dialTextPtr, HQR_TEXT_FILE, ++langIdx);

	/*if (cfgfile.LanguageCDId != 0) {
		loadVox(bankIdx);
	}*/
}

/** Draw a certain character in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param character ascii character to display */
void draw_character(int32 x, int32 y, uint8 character) { // drawCharacter
	uint8 sizeX;
	uint8 sizeY;
	uint8 keptsizeX;
	uint8 keptsizeY;
	uint8 param1;
	uint8 param2;
	uint8 *data;
	uint8 *screen;

	// int temp=0;
	int32 toNextLine;
	uint8 index;

	// char color;
	uint8 usedColor;
	uint8 number;
	uint8 jump;

	int32 i;

	int32 tempX;
	int32 tempY;

	data = fontPtr + *((int16 *)(fontPtr + character * 4));

	keptsizeX = (dialTextSize = sizeX = *(data++)) + 2;
	keptsizeY = (sizeY = *(data++)) + 4;
	param1 = *(data++);
	param2 = *(data++);

	x += param1;
	y += param2;

	usedColor = dialTextColor;

	screen = frontVideoBuffer + screenLockupTable[y] + x;

	tempX = x;
	tempY = y;

	toNextLine = SCREEN_WIDTH - sizeX;

	do {
		index = *(data++);
		do {
			jump = *(data++);
			screen += jump;
			tempX += jump;
			if (--index == 0) {
				screen += toNextLine;
				tempY++;
				tempX = x;
				sizeY--;
				if (sizeY <= 0) {
					return;
				}
				break;
			} else {
				number = *(data++);
				for (i = 0; i < number; i++) {
					if (tempX >= SCREEN_TEXTLIMIT_LEFT && tempX < SCREEN_TEXTLIMIT_RIGHT && tempY >= SCREEN_TEXTLIMIT_TOP && tempY < SCREEN_TEXTLIMIT_BOTTOM)
						frontVideoBuffer[SCREEN_WIDTH*tempY + tempX] = usedColor;

					screen++;
					tempX++;
				}

				if (--index == 0) {
					screen += toNextLine;
					tempY++;
					tempX = x;

					sizeY--;
					if (sizeY <= 0) {
						return;
					}
					break;
				}
			}
		} while (1);
	} while (1);

}

/** Draw character with shadow
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param character ascii character to display
	@param color character color */
void draw_character_shadow(int32 x, int32 y, uint8 character, int32 color) { // drawDoubleLetter
	int32 left, top, right, bottom;

	if (character != 0x20)
	{
		// shadow color
		set_font_color(0);
		draw_character(x + 2, y + 4, character);

		// text color
		set_font_color(color);
		draw_character(x, y, character);

		left = x;
		top = y;
		// FIXME: get right font size
		right = x + 32;
		bottom = y + 38;

		copy_block_phys(left, top, right, bottom);
	}
}

/** Display a certain dialogue text in the screen
	@param x X coordinate in screen
	@param y Y coordinate in screen
	@param dialogue ascii text to display */
void draw_text(int32 x, int32 y, int8 *dialogue) { // Font
	uint8 currChar;

	if (fontPtr == 0)   // if the font is not defined
		return;

	do {
		currChar = (uint8) * (dialogue++); // read the next char from the string

		if (currChar == 0) // if the char is 0x0, -> end of string
			break;

		if (currChar == 0x20)  // if it's a space char
			x += dialCharSpace;
		else {
			dialTextSize = *(fontPtr + (*((int16 *)(fontPtr + currChar * 4))));  // get the length of the character
			draw_character(x, y, currChar); // draw the character on screen
			// add the length of the space between 2 characters
			x += dialSpaceBetween;
			// add the length of the current character
			x += dialTextSize;
		}
	} while (1);
}

/** Gets dialogue text width size
	@param dialogue ascii text to display */
int32 get_text_size(int8 *dialogue) {  // SizeFont
	uint8 currChar;
	dialTextSize = 0;

	do {
		currChar = (uint8) * (dialogue++);

		if (currChar == 0)
			break;

		if (currChar == 0x20) {
			dialTextSize += dialCharSpace;
		} else {
			dialTextSize += dialSpaceBetween;
			dialTextSize += *(fontPtr + *((int16 *)(fontPtr + currChar * 4)));
		}
	} while (1);

	return (dialTextSize);
}

void init_dialogue_box() { // InitDialWindow
	blit_box(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom, workVideoBuffer, dialTextBoxLeft, dialTextBoxTop, frontVideoBuffer);

	if (newGameVar4 != 0) {
		draw_box(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom);
		draw_transparent_box(dialTextBoxLeft + 1, dialTextBoxTop + 1, dialTextBoxRight - 1, dialTextBoxBottom - 1, 3);
	}

	copy_block_phys(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom);
	printText8Var3 = 0;
	blit_box(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom, frontVideoBuffer, dialTextBoxLeft, dialTextBoxTop, workVideoBuffer);
}

void init_inventory_dialogue_box() { // SecondInitDialWindow
	blit_box(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom, workVideoBuffer, dialTextBoxLeft, dialTextBoxTop, frontVideoBuffer);
	copy_block_phys(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom);
	printText8Var3 = 0;
}

// TODO: refactor this code
void init_text(int32 index) { // initText
	printTextVar13 = 0;

	if (!get_text(index)) {
		return;
	}

	printText8Ptr1 = buf1;
	printText8Ptr2 = buf2;

	printTextVar13 = 1;

	printText8Var1 = 0;
	buf1[0] = 0;
	buf2[0] = 0;
	printText8Var2 = index;
	printText8Var3 = 0;
	TEXT_CurrentLetterX = dialTextBoxLeft + 8;
	printText8Var5 = 0;
	printText8Var6 = 0;
	TEXT_CurrentLetterY = dialTextBoxTop + 8;
	printText8Var8 = currDialTextPtr;

	// lba font is get while engine start
	set_font_parameters(2, 7);
}

void initProgressiveTextBuffer() {
	int32 i = 0;

	buf2[0] = 0;

	while (i < dialTextBufferSize) {
		strcat(buf2, " ");
		i++;
	};

	printText8Ptr2 = buf2;
	addLineBreakX = 16;
	printText8Var1 = 0;
}

void printText8Sub4(int16 a, int16 b, int16 c) {
	int32 temp;
	int32 counter2 = 0;
	int32 var1;
	int32 var2;

	if (printText8Var3 < 32) {
		temp = printText8Var3 * 3;
		pt8s4[temp] = c;
		pt8s4[temp+1] = a;
		pt8s4[temp+2] = b;

		printText8Var3++;
	} else {
		while (counter2 < 31) {
			var1 = (counter2 + 1) * 3;
			var2 = counter2 * 3;
			pt8s4[var2] = pt8s4[var1];
			pt8s4[var2+1] = pt8s4[var1+1];
			pt8s4[var2+2] = pt8s4[var1+2];
			counter2++;
		};
		pt8s4[93] = c;
		pt8s4[94] = a;
		pt8s4[95] = b;
	}
}

void getWordSize(uint8 *arg1, uint8 *arg2) {
	int32 temp = 0;
	uint8 *arg2Save = arg2;

	while (*arg1 != 0 && *arg1 != 1 && *arg1 != 0x20) {
		temp++;
		*arg2++ = *arg1++;
	}

	wordSizeChar = temp;
	*arg2 = 0;
	wordSizePixel = get_text_size(arg2Save);
}

void processTextLine() {
	int16 var4;
	uint8 *buffer;
	uint8 *temp;

	buffer = printText8Var8;
	dialCharSpace = 7;
	var4 = 1;

	addLineBreakX = 0;
	printText8PrepareBufferVar2 = 0;
	buf2[0] = 0;

	for (;;)
	{
		if (*buffer == 0x20) {
			buffer++;
			continue;
		}

		if (*buffer != 0) {
			printText8Var8 = buffer;
			getWordSize(buffer, buf1);
			if (addLineBreakX + dialCharSpace + wordSizePixel < dialTextBoxParam2) {
				temp = buffer + 1;
				if (*buffer == 1) {
					var4 = 0;
					buffer = temp;
				} else {
					if (*buf1 == 0x40) {
						var4 = 0;
						buffer = temp;
						if (addLineBreakX == 0) {
							addLineBreakX = 7;
							*buf2 = spaceChar;
						}
						if (buf1[1] == 0x50) {
							printText8Var1 = dialTextBoxParam1;
							buffer++;
						}
					} else {
						buffer += wordSizeChar;
						printText8Var8 = buffer;
						strcat(buf2, buf1);
						strcat(buf2, " ");  // not 100% accurate
						printText8PrepareBufferVar2++;

						addLineBreakX += wordSizePixel + dialCharSpace;
						if (*printText8Var8 != 0) {
							printText8Var8++;
							continue;
						}
					}
				}
			}
		}
		break;
	}

	if (printText8PrepareBufferVar2 != 0)
		printText8PrepareBufferVar2--;

	if (*printText8Var8 != 0 && var4 == 1) {
		dialCharSpace += (dialTextBoxParam2 - addLineBreakX) / printText8PrepareBufferVar2;
		printText10Var1 = dialTextBoxParam2 - addLineBreakX - dialTextBoxParam2 - addLineBreakX;  // stupid... recheck
	}

	printText8Var8 = buffer;

	printText8Ptr2 = buf2;

}

// draw next page arrow polygon
void printText10Sub() { // printText10Sub()
	vertexCoordinates[0] = dialTextStopColor;
	vertexCoordinates[1] = dialTextBoxRight - 3;
	vertexCoordinates[2] = dialTextBoxBottom - 24;
	vertexCoordinates[3] = dialTextStopColor;
	vertexCoordinates[4] = dialTextBoxRight - 24;
	vertexCoordinates[5] = dialTextBoxBottom - 3;
	vertexCoordinates[6] = dialTextStartColor;
	vertexCoordinates[7] = vertexCoordinates[1];
	vertexCoordinates[8] = vertexCoordinates[5];

	polyRenderType = 0; // POLYGONTYPE_FLAT
	numOfVertex = 3;

	if (compute_polygons())
	{
		render_polygons(polyRenderType, dialTextStopColor);
	}

	copy_block_phys(dialTextBoxRight - 24, dialTextBoxBottom - 24, dialTextBoxRight - 3, dialTextBoxBottom - 3);
}

void printText10Sub2() { // printText10Sub2()
	int32 currentLetter;
	int32 currentIndex;
	int32 counter;
	int32 counter2;
	int16 *ptr;

	currentLetter = printText8Var3;
	currentLetter--;

	currentIndex = currentLetter * 3;

	ptr = pt8s4 + currentIndex;

	delay(15);

	counter = printText8Var3;
	counter2 = dialTextStartColor;

	while (--counter >= 0) {
		set_font_color(counter2);
		draw_character_shadow(*(ptr + 1), *(ptr + 2), (uint8)*ptr, counter2);
		counter2 -= dialTextStepSize;
		if (counter2 > dialTextStopColor)
			counter2 = dialTextStopColor;
		ptr -= 3;
	};

}

void TEXT_GetLetterSize(int8 character, int32 *pLetterWidth, int32 *pLetterHeight, int8 * pFont) { // TEXT_GetLetterSize
	byte *temp;

	temp = pFont + *((int16 *)(pFont + character * 4));
	*pLetterWidth = *(temp);
	*pLetterHeight = *(temp + 1);
}

// TODO: refactor this code
int printText10() { // printText10()
	int32 charWidth, charHeight; // a, b

	if (printTextVar13 == 0) {
		return 0;
	}

	if (*(printText8Ptr2) == 0) {
		if (printText8Var5 != 0) {
			if (newGameVar5 != 0) {
				printText10Sub();
			}
			printTextVar13 = 0;
			return 0;
		}
		if (printText8Var6 != 0) {
			blit_box(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom, workVideoBuffer, dialTextBoxLeft, dialTextBoxTop, frontVideoBuffer);
			copy_block_phys(dialTextBoxLeft, dialTextBoxTop, dialTextBoxRight, dialTextBoxBottom);
			printText8Var3 = 0;
			printText8Var6 = 0;
			TEXT_CurrentLetterX = dialTextBoxLeft + 8;
			TEXT_CurrentLetterY = dialTextBoxTop + 8;
		}
		if (*(printText8Var8) == 0) {
			initProgressiveTextBuffer();
			printText8Var5 = 1;
			return 1;
		}
		processTextLine();
	}

	// TODO: recheck this
	if (*(printText8Ptr2) == 0) {
		return 1;
	}

	printText8Sub4(TEXT_CurrentLetterX, TEXT_CurrentLetterY, *printText8Ptr2);
	printText10Sub2();
	TEXT_GetLetterSize(*printText8Ptr2, &charWidth, &charHeight, fontPtr);

	if (*(printText8Ptr2) != 0x20) {
		TEXT_CurrentLetterX += charWidth + 2;
	} else {
		if (printText10Var1 != 0) {
			TEXT_CurrentLetterX++;
			printText10Var1--;
		}
		TEXT_CurrentLetterX += dialCharSpace;
	}

	// next character
	printText8Ptr2++;

	if (*(printText8Ptr2) != 0)
		return 1;

	TEXT_CurrentLetterY += 38;
	TEXT_CurrentLetterX = dialTextBoxLeft + 8;

	if (printText8Var6 == 1 && printText8Var5 == 0) {
	  printText10Sub();
	  return 2;
	}

	printText8Var1++;
	if (printText8Var1 < dialTextBoxParam1) {
		return 1;
	}

	initProgressiveTextBuffer();
	printText8Var6 = 1;

	if (*(printText8Var8) == 0) {
		printText8Var5 = 1;
	}

	return 1;
}

// TODO: refactor this code
void draw_text_fullscreen(int32 index) { // printTextFullScreen
	int32 printedText;
	int32 skipText = 0;

	save_clip();
	reset_clip();
	copy_screen(frontVideoBuffer, workVideoBuffer);

	// TODO: get right VOX entry index
	// TODO: if we don't display text, than still plays vox file

	init_text(index);
	init_dialogue_box();

	do {
		read_keys();
		printedText = printText10();
		
		if (printedText == 2) {
			do {
				read_keys();
				if (skipIntro == 0 && skipedKey == 0 && pressedKey == 0) {
					break;
				}
				// TODO: missing vox processing
				delay(1);
			} while(1);

			do {
				read_keys();
				if (skipIntro != 0 || skipedKey != 0 || pressedKey != 0) {
					break;
				}
				// TODO: missing vox processing
				delay(1);
			} while(1);
		}

		if (skipIntro == 1) {
			skipText = 1;
		}

		// TODO: missing vox processing
		delay(1);
	} while(!skipText);

	printTextVar5 = 0;

	// TODO: missing vox processing

	printTextVar13 = 0;

	if (printedText != 0) {
		load_clip();
		return;
	}

	if (skipText != 0) {
		load_clip();
		return;
	}

	// TODO: recheck this
	// wait displaying text
	do {
		read_keys();
		delay(1);
	} while(skipIntro || skipedKey || pressedKey);

	// TODO: recheck
	// wait key to display next text
	do {
		read_keys();
		if (skipIntro != 0) {
			load_clip();
			return;
		}
		if (skipedKey != 0) {
			load_clip();
			return;
		}
		delay(1);
	} while(!pressedKey);

	load_clip();
}

void set_font(uint8 *font, int32 spaceBetween, int32 charSpace) {
	fontPtr = font;
	dialCharSpace = charSpace;
	dialSpaceBetween = spaceBetween;
}

/** Set font type parameters
	@param spaceBetween number in pixels of space between characters
	@param charSpace number in pixels of the character space */
void set_font_parameters(int32 spaceBetween, int32 charSpace) {
	dialSpaceBetween = spaceBetween;
	dialCharSpace = charSpace;
}

/** Set the font cross color
	@param color color number to choose */
void set_font_cross_color(int32 color) { // TestCoulDial
	dialTextStepSize = -1;
	dialTextBufferSize = 14;
	dialTextStartColor = color << 4;
	dialTextStopColor = (color << 4) + 12;
}

/** Set the font color
	@param color color number to choose */
void set_font_color(int32 color) {
	dialTextColor = color;
}

/** Set font color parameters to process cross color display
	@param stopColor color number to stop
	@param startColor color number to start
	@param stepSize step size to change between those colors */
void set_text_cross_color(int32 stopColor, int32 startColor, int32 stepSize) {
	dialTextStartColor = startColor;
	dialTextStopColor = stopColor;
	dialTextStepSize = stepSize;
	dialTextBufferSize = ((startColor - stopColor) + 1) / stepSize;
}

/** Get dialogue text into text buffer
	@param index dialogue index */
int32 get_text(int32 index) { // findString
	int32 currIdx = 0;
	int32 orderIdx = 0;
	int32 numEntries;
	int32 ptrCurrentEntry;
	int32 ptrNextEntry;

	int16 *localTextBuf = (int16 *) dialTextPtr;
	int16 *localOrderBuf = (int16 *) dialOrderPtr;

	numEntries = numDialTextEntries;

	// choose right text from order index
	do {
		orderIdx = *(localOrderBuf++);
		if (orderIdx == index)
			break;
		currIdx++;
	} while (currIdx < numDialTextEntries);

	if (currIdx >= numEntries)
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
void copy_text(int8 *src, int8 *dst, int32 size) { // copyStringToString
	int32 i;
	for (i = 0; i < size; i++)
		*(dst++) = *(src++);
}

/** Gets menu dialogue text
	@param index text index to display
	@param dialogue dialogue text buffer to display */
void get_menu_text(int32 index, int8 *text) { // GetMultiText
	if (index == currMenuTextIndex) {
		if (currMenuTextBank == currentTextBank) {
			strcpy(text, currMenuTextBuffer);
			return;
		}
	}
	if (!get_text(index)) {
		// if doesn't have text
		text[0] = 0;
		return;
	}

	if ((currDialTextSize - 1) > 0xFF)
		currDialTextSize = 0xFF;

	copy_text((int8 *) currDialTextPtr, text, currDialTextSize);
	currDialTextSize++;
	copy_text(text, currMenuTextBuffer, currDialTextSize);

	currMenuTextIndex = index;
	currMenuTextBank = currentTextBank;
}

void text_clip_full() { // newGame2
	dialTextBoxLeft = 8;
	dialTextBoxTop = 8;
	dialTextBoxRight = 631;

	dialTextBoxBottom = 471;
	dialTextBoxParam1 = 11;
	dialTextBoxParam2 = 607;
}

void text_clip_small() { // newGame4
	dialTextBoxLeft = 16;
	dialTextBoxTop = 334;
	dialTextBoxRight = 623;
	dialTextBoxBottom = 463;
	dialTextBoxParam1 = 3;
	dialTextBoxParam2 = 591;
}

/*

// Internal representation for drawn strings
typedef struct PrintedTextBuf {
	uint8* ptBuffer;
	uint8* ptSeek;

	uint32 Size;
	uint32 pixelSize;
} TextBuffer;

void init_text_buf(TextBuffer* buf, uint8* cbuf);
void alloc_text_buf(TextBuffer* buf, int32 size);
int32 calc_justified_text_line(TextBuffer *compText, TextBuffer *lineText, TextBuffer *wordText, uint32 maxWidth);



void init_text_buf(TextBuffer* buf, uint8* cbuf) {
	buf->ptBuffer = buf->ptSeek = cbuf;
	buf->Size = 0;
	buf->pixelSize = 0;
}

void alloc_text_buf(TextBuffer* buf, int size) {
	init_text_buf(buf, (uint8*)malloc(size*sizeof(uint8)));
	*(buf->ptBuffer) = '\0';
}

// Returns 1 if this is the last line, 2 if this is the last line of the current page, 0 otherwise 
int32 calc_justified_text_line(TextBuffer *compText, TextBuffer *lineText, TextBuffer *wordText, uint32 maxWidth) {
	uint8 *font_data;

	// New line
	int32 lastLine = 0;
	int32 spaceLength = dialCharSpace;

	// Re-init line buffer
	init_text_buf(lineText, lineText->ptBuffer);
	*lineText->ptBuffer = '\0';

	for (;;) { // Each iteration is for a word
		uint8* oldSeek = compText->ptSeek;

		// Re-init word buffer
		init_text_buf(wordText, wordText->ptBuffer);
		*wordText->ptBuffer = '\0';

		// Skip white spaces
		while (*(compText->ptSeek) == ' ')
			++(compText->ptSeek);

		// If it's a '@', let's cut the line here
		if (*(compText->ptSeek) == '@') {
			compText->ptSeek++;
			break;
		}

		// Get the current word
		wordText->pixelSize = 0;
		while (*(compText->ptSeek) != ' ') { // Seek through the whole word
			font_data = fontPtr + *((int16 *)(fontPtr + (*(compText->ptSeek)) * 4));
			wordText->pixelSize += *(font_data) + 2; // TODO: Use real font structure

			*(wordText->ptSeek++) = *(compText->ptSeek++);

			if (*(compText->ptSeek) == '\0') {
				// This is last line
				lastLine = 1;
				break;
			}
		}
		*(wordText->ptSeek) = '\0';

		// Can we add this word ?
		if (lineText->pixelSize + wordText->pixelSize + spaceLength <= maxWidth) {
			// Yes, add it
			lineText->pixelSize += wordText->pixelSize;
			++lineText->Size;

			if (lineText->Size > 1) {
				lineText->pixelSize += spaceLength;
				strcat((int8*) lineText->ptBuffer, " ");
			}

			strcat((int8*) lineText->ptBuffer, (int8*) wordText->ptBuffer);
		} else {
			// We can't, we've already reached the end of line
			// revert to the previous seek pointer
			compText->ptSeek = oldSeek;

			// Calcultate space length for text to be aligned
			spaceLength += (uint32)floor((int64)(maxWidth - lineText->pixelSize) / (int64)(lineText->Size - 1));

			// Even if next word is the last one, we couldn't add it.
			lastLine = 0;
			break;
		}

		// If we know this is last line, let's end (text won't be aligned, it's on purpose)
		if (lastLine)
			break;
	}

	//lineText->ptSeek = lineText->ptBuffer;
	lineText->pixelSize = spaceLength;

	return lastLine;
}

void display_dialogue_fullscreen(int32 dialog, uint8 color) { //char *dialogue
	int32  x = 16, y = 16;
	uint8  start_color = color;
	uint8  end_color = color + 12;
	uint8  currChar;
	uint32 wordCount = 0;

	TextBuffer compText;	// Page Buffer
	TextBuffer lineText;	// Line Buffer
	TextBuffer wordText;	// Word buffer

	if (fontPtr == 0)   // if the font is not defined
		return;

	// Init of our temporary text buffers
	get_text(dialog);
	init_text_buf(&compText, currDialTextPtr);
	alloc_text_buf(&lineText, 100);
	alloc_text_buf(&wordText, 30);

	dialTextColor = start_color;

	for (;;) { // Each iteration is drawing a line
		int lastline = calc_justified_text_line(&compText, &lineText, &wordText, 604);

		// Line drawing code
		for (;;) {
			currChar = (uint8) * (lineText.ptSeek); // read the next char from the string

			if (currChar == ' ') {
				// White space
				x += lineText.pixelSize;
				++wordCount;
			} else if (currChar == '\0') {
				// End of line
				break;
			} else {
				for( localSeek = lineText->ptSeek; localSeek != lineText->ptSeek-5 && localSeek != lineText->ptBuffer-1; --localSeek )
				{
					// Draw character
					printf( "%c", *localSeek );
				}
				draw_character_shadow(x, y, currChar, 1); // draw the character on screen
				// add the length of the space between 2 characters
				x += dialSpaceBetween;
				// add the length of the current character
				x += dialTextSize;

				dialTextColor++;
				if (dialTextColor > end_color)
					dialTextColor = end_color;

			}

			++lineText.ptSeek;
		}

		// Init next line position
		x =  16;
		y += 38;

		if (y > 400) {
			// End of current page
			break;
		} else if (lastline) {
			// End of dialog
			break;
		}
	}

	free(lineText.ptBuffer);
	free(wordText.ptBuffer);

	do
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
	}while (1);
}
*/
