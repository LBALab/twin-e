/** @file xmidi.h
	@brief
	This file contains MIDI-related routines.
	These routines were adapted from ScrummVM/Exult engine source code.

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team
	Copyright (C) 2013 The ScrummVM/ExultEngine team

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

#ifndef XMIDI_H
#define XMIDI_H

#include "sys.h"

/**
 * Credit where credit is due:
 * Most of code to convert XMIDI to MIDI is adapted from either the ScummVM
 * project or the Exult game engine.
 * //risca
 */

/*
 * Takes a pointer to XMIDI data of size 'size' and converts it to MIDI. The
 * result is allocated dynamically and saved to the 'dest' pointer. Returns
 * the size of the MIDI data or 0 on error.
 */
uint32 convert_to_midi(uint8* data, uint32 size, uint8** dest);

#endif // XMIDI_H
