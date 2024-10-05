/** @file filereader.h
    @brief
    This file contains file read routines

    TwinEngine: a Little Big Adventure engine
    
    Copyright (C) 2002 The TwinEngine team

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

#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"


#define SECTORS_IN_BUFFER (3)
#define BUFFER_SIZE (2048*SECTORS_IN_BUFFER)


typedef struct file_reader_s {
    FILE* fd;
    uint8 buffer[BUFFER_SIZE];
    uint32 bufferPos;
    uint32 currSector;
} file_reader_t;


void frfeed(file_reader_t* fr);
void frread(file_reader_t* fr, void* destPtr, uint32 size);
void frseek(file_reader_t* fr, uint32 seekPosition);
int32 fropen2(file_reader_t* fr, char* filename, const char* mode);
void frwrite(file_reader_t* fr, void* destPtr, uint32 size, uint32 count);
void frclose(file_reader_t* fr);

#endif
