/** @file filereader.c
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

#include "filereader.h"

/** Feed buffer from file
	@param fr filereader pointer */
void frfeed(filereader* fr)
{
	fread(fr->buffer, BUFFER_SIZE, 1, fr->fd);
	fr->bufferPos = 0;
}

/** Read file
	@param fr filereader pointer 
	@param destPtr content destination pointer
	@param size size of read characters */
void frread(filereader* fr, void* destPtr, unsigned long int size)
{
	if(BUFFER_SIZE-fr->bufferPos >= size)
	{
		memcpy(destPtr,&fr->buffer[fr->bufferPos],size);
		fr->bufferPos += size;
	}
	else
	{
		// feed what we can
		char* tempPtr = (char*)destPtr;
		memcpy( tempPtr, &fr->buffer[fr->bufferPos], BUFFER_SIZE-fr->bufferPos );
		tempPtr+=BUFFER_SIZE-fr->bufferPos;
		size-=BUFFER_SIZE-fr->bufferPos;

		// feed the rest
		do
		{
			fr->currSector++;
			frfeed(fr);
			if(size>=BUFFER_SIZE)
			{
				memcpy(tempPtr, fr->buffer, BUFFER_SIZE);
				tempPtr+=BUFFER_SIZE;
				size-=BUFFER_SIZE;
			}
			else
			{
				memcpy(tempPtr, fr->buffer, size);
				fr->bufferPos += size;
				size=0;
			}
		}while(size>0);
	}
}

/** Seek file
	@param fr filereader pointer 
	@param seekPosition position to seek */
void frseek(filereader* fr, unsigned long int seekPosition)
{
	unsigned long int sectorToSeek;

	sectorToSeek = seekPosition / 2048;

	fseek(fr->fd, sectorToSeek * 2048, SEEK_SET);

	fr->currSector = sectorToSeek;
	frfeed(fr);
	fr->bufferPos = (seekPosition - (sectorToSeek*2048));
}

/** Open file
	@param fr filereader pointer 
	@param filename file path
	@return true if file open and false if error occurred */
int fropen(filereader* fr, const char* filename)
{
	fr->fd = fopen((const char*)filename,"rb");

	if(fr->fd)
	{
		fr->currSector = 0;
		frfeed(fr);
		return 1;
	}

	return 0;
}

/** Close file
	@param fr filereader pointer */
void frclose(filereader* fr)
{
	fclose(fr->fd);
}
