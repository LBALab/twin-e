/** @file sound.h
	@brief
	This file contains music playing routines

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

#ifndef SOUND_H
#define SOUND_H

#include "sys.h"

/** Sample volume
	@param channel sample channel
	@param volume sample volume number */
void sample_volume(int32 channel, int32 volume);

/** Play FLA movie samples
	@param index sample index under flasamp.hqr file
	@param frequency frequency used to play the sample
	@param repeat number of times to repeat the sample
	@param x unknown x variable
	@param y unknown y variable */
void play_fla_sample(int32 index, int32 frequency, int32 repeat, int32 x, int32 y);

/** Play samples
	@param index sample index under flasamp.hqr file
	@param frequency frequency used to play the sample
	@param repeat number of times to repeat the sample
	@param x unknown x variable
	@param y unknown y variable */
void play_sample(int32 index, int32 frequency, int32 repeat, int32 x, int32 y);

/** Stop samples */
void stop_sample();

#endif
