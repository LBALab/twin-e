/** @file keyboard.c
	@brief
	This file contains movies routines

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team \n
	Copyright (C) 2008-2013 Prequengine team \n
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
uint8 pressedKeyMap[29] = {
	[0] = 0x48,
	[1] = 0x50,
	[2] = 0x4B,
	[3] = 0x4D,
	[4] = 0x47,
	[5] = 0x49,
	[6] = 0x51,
	[7] = 0x4F,

	[8] = 0x39,
	[9] = 0x1C,
	[10] = 0x1D,
	[11] = 0x38,
	[12] = 0x53,
	[13] = 0x2A,
	[14] = 0x36,

	[15] = 0x3B,
	[16] = 0x3C,
	[17] = 0x3D,
	[18] = 0x3E,
	[19] = 0x3F,
	[20] = 0x40, // LBAKEY_F6
	[21] = 0x41,
	[22] = 0x42,
	[23] = 0x43,
	[24] = 0x44,
	[25] = 0x57,
	[26] = 0x58,
	[27] = 0x2A,
	[28] = 0x0,
};

uint16 pressedKeyCharMap[31] = {
	[0] = 0x0100, // up
	[1] = 0x0200, // down
	[2] = 0x0400, // left
	[3] = 0x0800, // right
	[4] = 0x0500, // home
	[5] = 0x0900, // pageup
	[6] = 0x0A00, // pagedown
	[7] = 0x0600, // end

	[8] = 0x0101, // space bar
	[9] = 0x0201, // enter
	[10] = 0x0401,  // ctrl
	[11] = 0x0801,  // alt
	[12] = 0x1001,  // del
	[13] = 0x2001,  // left shift
	[14] = 0x2001,  // right shift

	[15] = 0x0102,  // F1
	[16] = 0x0202,  // F2
	[17] = 0x0402,  // F3
	[18] = 0x0802,  // F4
	[19] = 0x1002,  // F5
	[20] = 0x2002,  // F6
	[21] = 0x4002,  // F7
	[22] = 0x8002,  // F8

	[23] = 0x0103,  // F9
	[24] = 0x0203,  // F10
	[25] = 0x0403,  // ?
	[26] = 0x0803,  // ?
	[27] = 0x00FF,  // left shift
	[28] = 0x00FF,
	[29] = 0x0,
	[30] = 0x0,
};
