/** @file filereader.h
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

#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Number of sector in the buffer */
#define SECTORS_IN_BUFFER (3)
/** Buffer size */
#define BUFFER_SIZE (2048*SECTORS_IN_BUFFER)

/** File reader structure */
typedef struct filereader
{
	/** File descriptor */
	FILE* fd;
	/** Content buffer */
	unsigned char buffer[BUFFER_SIZE];
	/** Current position in the buffer */
	unsigned long int bufferPos;
	/** Current sector in the buffer */
	unsigned long int currSector;
} filereader;

/** Feed buffer from file
	@param fr filereader pointer */
void frfeed(filereader* fr);

/** Read file
	@param fr filereader pointer 
	@param destPtr content destination pointer
	@param size size of read characters */
void frread(filereader* fr, void* destPtr, unsigned long int size);

/** Seek file
	@param fr filereader pointer 
	@param seekPosition position to seek */
void frseek(filereader* fr, unsigned long int seekPosition);

/** Open file
	@param fr filereader pointer 
	@param filename file path
	@return true if file open and false if error occurred */
int fropen(filereader* fr, const char* filename);

/** Close file
	@param fr filereader pointer */
void frclose(filereader* fr);

#endif
