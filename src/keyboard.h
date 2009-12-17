/** @file keyboard.h
	@brief
	This file contains movies routines

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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "sys.h"

/** Pressed key map - scanCodeTab1 */
uint8 pressedKeyMap[29];
/** Pressed key char map - scanCodeTab2 */
uint16 pressedKeyCharMap[31];

/** Skipped key - key1 */
int16 skipedKey;
/** Pressed key - printTextVar12 */
int16 pressedKey;
//int printTextVar13;
/** Skip intro variable */
int16 skipIntro;
/** Current key value */
int16 currentKey;
/** Auxiliar key value */
int16 key;

int32 heroPressedKey;
int32 heroPressedKey2;

/** Initialize engine auxiliar keymap */
void init_keymap();

#endif
