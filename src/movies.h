/** @file movies.h
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
*/

#ifndef MOVIES_H
#define MOVIES_H

#include "main.h"

/** FLA movie directory */
#define FLA_DIR "fla//"

/** FLA movie header structure */
typedef struct FLAHeaderStruct
{
	/** FLA version */
	char version[6];
	/** Number of frames */
	int numOfFrames;
	/** Frames per second */
	char speed;
	/** Unknown var1 */
	char var1;
	/** Frame width */
	short int xsize;
	/** Frame height */
	short int ysize;
} FLAHeaderStruct;

/** FLA movie frame structure */
typedef struct FLAFrameDataStruct
{
	/** Current frame size */
	char videoSize;
	/** Dummy variable */
	char dummy;
	/** Unknown frameVar0 */
	int frameVar0;
} FLAFrameDataStruct;

/** FLA movie sample structure */
typedef struct FLASampleStruct
{
	/** Number os samples */
	short int sampleNum;
	/** Sample frequency */
	short int freq;
	/** Numbers of time to repeat */
	short int repeat;
	/** Dummy variable */
	char dummy;
	/** Unknown x */
	unsigned char x;
	/** Unknown y */
	unsigned char y;
} FLASampleStruct;

/** FLA movie file buffer */
unsigned char flaBuffer[FLASCREEN_WIDTH*FLASCREEN_HEIGHT];

/** General function to play movies
	@param movie - movie file path */
void play_movie(char *movie);

#endif
