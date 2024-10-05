/** @file filereader.c
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

#include <ctype.h>

#include "filereader.h"
#include "utils/fcaseopen.h"


inline void frfeed(file_reader_t* fr) {
    fread(fr->buffer, BUFFER_SIZE, 1, fr->fd);
    fr->bufferPos = 0;
}

void frread(file_reader_t* fr, void* destPtr, uint32 size) {
    if (BUFFER_SIZE - fr->bufferPos >= size) {
        memcpy(destPtr, &fr->buffer[fr->bufferPos], size);
        fr->bufferPos += size;
    } else {
        // feed what we can
        int8* tempPtr = (int8*)destPtr;
        memcpy(tempPtr, &fr->buffer[fr->bufferPos], BUFFER_SIZE - fr->bufferPos);
        tempPtr += BUFFER_SIZE - fr->bufferPos;
        size -= BUFFER_SIZE - fr->bufferPos;

        // feed the rest
        do {
            fr->currSector++;
            frfeed(fr);
            if (size >= BUFFER_SIZE) {
                memcpy(tempPtr, fr->buffer, BUFFER_SIZE);
                tempPtr += BUFFER_SIZE;
                size -= BUFFER_SIZE;
            } else {
                memcpy(tempPtr, fr->buffer, size);
                fr->bufferPos += size;
                size = 0;
            }
        } while (size > 0);
    }
}

void frseek(file_reader_t* fr, uint32 seekPosition) {
    uint32 sectorToSeek;

    sectorToSeek = seekPosition / 2048;

    fseek(fr->fd, sectorToSeek * 2048, SEEK_SET);

    fr->currSector = sectorToSeek;
    frfeed(fr);
    fr->bufferPos = (seekPosition - (sectorToSeek * 2048));
}

int32 fropen2(file_reader_t* fr, char* filename, const char* mode) {
    fr->fd = fcaseopen(filename, mode);

    if (fr->fd) {
        fr->currSector = 0;
        frfeed(fr);
        return 1;
    }

    return 0;
}

inline void frwrite(file_reader_t* fr, void* destPtr, uint32 size, uint32 count) {
    fwrite(destPtr, size, count, fr->fd);
}

inline void frclose(file_reader_t* fr) {
    fclose(fr->fd);
}
