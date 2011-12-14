/** @file keyboard.c
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

#include "keyboard.h"

/** Initialize engine auxiliar keymap */
void initKeymap() {
	pressedKeyMap[0] = 0x48;
	pressedKeyMap[1] = 0x50;
	pressedKeyMap[2] = 0x4B;
	pressedKeyMap[3] = 0x4D;
	pressedKeyMap[4] = 0x47;
	pressedKeyMap[5] = 0x49;
	pressedKeyMap[6] = 0x51;
	pressedKeyMap[7] = 0x4F;

	pressedKeyMap[8] = 0x39;
	pressedKeyMap[9] = 0x1C;
	pressedKeyMap[10] = 0x1D;
	pressedKeyMap[11] = 0x38;
	pressedKeyMap[12] = 0x53;
	pressedKeyMap[13] = 0x2A;
	pressedKeyMap[14] = 0x36;

	pressedKeyMap[15] = 0x3B;
	pressedKeyMap[16] = 0x3C;
	pressedKeyMap[17] = 0x3D;
	pressedKeyMap[18] = 0x3E;
	pressedKeyMap[19] = 0x3F;
	pressedKeyMap[20] = 0x40; // LBAKEY_F6
	pressedKeyMap[21] = 0x41;
	pressedKeyMap[22] = 0x42;
	pressedKeyMap[23] = 0x43;
	pressedKeyMap[24] = 0x44;
	pressedKeyMap[25] = 0x57;
	pressedKeyMap[26] = 0x58;
	pressedKeyMap[27] = 0x2A;
	pressedKeyMap[28] = 0x0;

	pressedKeyCharMap[0] = 0x0100; // up
	pressedKeyCharMap[1] = 0x0200; // down
	pressedKeyCharMap[2] = 0x0400; // left
	pressedKeyCharMap[3] = 0x0800; // right
	pressedKeyCharMap[4] = 0x0500; // home
	pressedKeyCharMap[5] = 0x0900; // pageup
	pressedKeyCharMap[6] = 0x0A00; // pagedown
	pressedKeyCharMap[7] = 0x0600; // end

	pressedKeyCharMap[8] = 0x0101; // space bar
	pressedKeyCharMap[9] = 0x0201; // enter
	pressedKeyCharMap[10] = 0x0401;  // ctrl
	pressedKeyCharMap[11] = 0x0801;  // alt
	pressedKeyCharMap[12] = 0x1001;  // del
	pressedKeyCharMap[13] = 0x2001;  // left shift
	pressedKeyCharMap[14] = 0x2001;  // right shift

	pressedKeyCharMap[15] = 0x0102;  // F1
	pressedKeyCharMap[16] = 0x0202;  // F2
	pressedKeyCharMap[17] = 0x0402;  // F3
	pressedKeyCharMap[18] = 0x0802;  // F4
	pressedKeyCharMap[19] = 0x1002;  // F5
	pressedKeyCharMap[20] = 0x2002;  // F6
	pressedKeyCharMap[21] = 0x4002;  // F7
	pressedKeyCharMap[22] = 0x8002;  // F8

	pressedKeyCharMap[23] = 0x0103;  // F9
	pressedKeyCharMap[24] = 0x0203;  // F10
	pressedKeyCharMap[25] = 0x0403;  // ?
	pressedKeyCharMap[26] = 0x0803;  // ?
	pressedKeyCharMap[27] = 0x00FF;  // left shift
	pressedKeyCharMap[28] = 0x00FF;
	pressedKeyCharMap[29] = 0x0;
	pressedKeyCharMap[30] = 0x0;
}
