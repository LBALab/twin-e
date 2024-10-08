/** @file hqrdepack.c
    @brief
    This file contains High Quality Resource (HQR) decompress routines.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hqr.h"
#include "filereader.h"


file_reader_t fr;


void hqr_entry_decompress(uint8 * dst, uint8 * src, int32 decompsize, int32 mode) {
    uint8 b;
    int32 lenght, d, i;
    uint16 offset;
    uint8 *ptr;

    do {
        b = *(src++);
        for (d = 0; d < 8; d++) {
            if (!(b & (1 << d))) {
                offset = *(uint16*)(src);
                src += 2;
                lenght = (offset & 0x0F) + (mode + 1);
                ptr = dst - (offset >> 4) - 1;
                for (i = 0; i < lenght; i++)
                    *(dst++) = *(ptr++);
            } else {
                lenght = 1;
                *(dst++) = *(src++);
            }
            decompsize -= lenght;
            if (decompsize <= 0)
                return;
        }
    } while (decompsize);
}

void hqr_entry_decompress_lz(uint8 * dst, uint8 * src, int32 decompsize, int32 mode) {
    uint16 offset;
    int32 lenght;
    uint8 *ptr;

    while (decompsize > 0) {
        uint8 bits;
        uint8 type = *(src++);
        for (bits = 1; bits != 0; bits <<= 1) {
            if (!(type&bits)) {
                offset = *(uint16*)(src);
                src += 2;
                lenght = (offset & 0x0F) + (mode + 1);
                ptr = dst - (offset >> 4) - 1;
                if (offset == 0) {
                    memset(dst, *ptr, lenght);
                } else {
                    if ((ptr + lenght) >= dst) {
                        int32 n;
                        uint8 *tmp = dst;
                        for (n = 0; n < lenght; n++)
                            *tmp++ = *ptr++;
                    } else {
                        memcpy(dst, ptr, lenght);
                    }
                }
                dst += lenght;
            } else {
                lenght = 1;
                *(dst++) = *(src++);
            }
            decompsize -= lenght;
            if (decompsize <= 0)
                return;
        }
    }
}

int32 hqr_get_entry(uint8 * ptr, int8 *filename, int32 index) {
    uint32 headerSize;
    uint32 offsetToData;
    uint32 realSize;
    uint32 compSize;
    uint16 mode;

    if (!filename)
        return 0;

    if (!fropen2(&fr, (char*)filename, "rb"))
        printf("HQR: %s can't be found !\n", filename);

    frread(&fr, &headerSize, 4);

    if ((uint32)index >= headerSize / 4) {
        printf("\nHQR WARNING: Invalid entry index!!\n");
        frclose(&fr);
        return 0;
    }

    frseek(&fr, index*4);
    frread(&fr, &offsetToData, 4);

    frseek(&fr, offsetToData);
    frread(&fr, &realSize, 4);
    frread(&fr, &compSize, 4);
    frread(&fr, &mode, 2);

    if (!ptr)
        ptr = (uint8*)malloc(realSize);

    if (!ptr) {
        printf("\nHQR WARNING: Unable to allocate memory!!\n");
        frclose(&fr);
        return 0;
    }

    // uncompressed
    if (mode == 0) {
        frread(&fr, ptr, realSize);
    }
    // compressed: modes (1 & 2)
    else if (mode == 1 || mode == 2) {
        uint8* compDataPtr = 0;
        compDataPtr = (uint8*)malloc(compSize);
        frread(&fr, compDataPtr, compSize);
        hqr_entry_decompress(ptr, compDataPtr, realSize, mode);
        free(compDataPtr);
    }

    frclose(&fr);

    return realSize;
}

int hqr_get_entry_size(int8 *filename, int32 index) {
    uint32 headerSize;
    uint32 offsetToData;
    uint32 realSize;

    if (!filename)
        return 0;

    if (!fropen2(&fr, (char*)filename, "rb")) {
        printf("HQR: %s can't be found !\n", filename);
        exit(1);
    }

    frread(&fr, &headerSize, 4);

    if ((uint32)index >= headerSize / 4) {
        printf("\nHQR WARNING: Invalid entry index!!\n");
        frclose(&fr);
        return 0;
    }

    frseek(&fr, index*4);
    frread(&fr, &offsetToData, 4);

    frseek(&fr, offsetToData);
    frread(&fr, &realSize, 4);

    frclose(&fr);

    return realSize;
}

int hqr_get_num_entries(int8 *filename) {
    uint32 headerSize;

    if (!filename)
        return 0;

    if (!fropen2(&fr, (char*)filename, "rb")) {
        printf("HQR: %s can't be found !\n", filename);
        exit(1);
    }

    frread(&fr, &headerSize, 4);

    return headerSize / 4;
}

int32 hqr_get_entry_alloc(uint8 ** ptr, int8 *filename, int32 index) {
    int32 size;
    size = hqr_get_entry_size(filename, index);

    *ptr = (uint8*)malloc(size * sizeof(uint8));
    if (!*ptr) {
        printf("HQR WARNING: unable to allocate entry memory!!\n");
        return 0;
    }
    hqr_get_entry(*ptr, filename, index);

    return size;
}

int32 hqr_get_entry_vox(uint8 * ptr, int8 *filename, int32 index, int32 hiddenIndex) {
    uint32 headerSize;
    uint32 offsetToData;
    uint32 realSize;
    uint32 compSize;
    uint16 mode;

    if (!filename)
        return 0;

    if (!fropen2(&fr, (char*)filename, "rb"))
        printf("HQR: %s can't be found !\n", filename);

    frread(&fr, &headerSize, 4);

    if ((uint32)index >= headerSize / 4) {
        printf("\nHQR WARNING: Invalid entry index!!\n");
        frclose(&fr);
        return 0;
    }

    frseek(&fr, index*4);
    frread(&fr, &offsetToData, 4);

    frseek(&fr, offsetToData);
    frread(&fr, &realSize, 4);
    frread(&fr, &compSize, 4);
    frread(&fr, &mode, 2);

    // exist hidden entries
    if (hiddenIndex > 0) {
        int32 i = 0;
        for (i = 0; i < hiddenIndex; i++) {
            frseek(&fr, offsetToData + compSize + 10); // hidden entry
            offsetToData = offsetToData + compSize + 10; // current hidden offset
            
            frread(&fr, &realSize, 4);
            frread(&fr, &compSize, 4);
            frread(&fr, &mode, 2);
        }
    }

    if (!ptr)
        ptr = (uint8*)malloc(realSize);

    if (!ptr) {
        printf("\nHQR WARNING: Unable to allocate memory!!\n");
        frclose(&fr);
        return 0;
    }

    // uncompressed
    if (mode == 0) {
        frread(&fr, ptr, realSize);
    }
    // compressed: modes (1 & 2)
    else if (mode == 1 || mode == 2) {
        uint8* compDataPtr = 0;
        compDataPtr = (uint8*)malloc(compSize);
        frread(&fr, compDataPtr, compSize);
        hqr_entry_decompress(ptr, compDataPtr, realSize, mode);
        free(compDataPtr);
    }

    frclose(&fr);

    return realSize;
}

int hqr_get_entry_size_vox(int8 *filename, int32 index, int32 hiddenIndex) {
    uint32 headerSize;
    uint32 offsetToData;
    uint32 realSize;
    uint32 compSize;

    if (!filename)
        return 0;

    if (!fropen2(&fr, (char*)filename, "rb")) {
        printf("HQR: %s can't be found !\n", filename);
        exit(1);
    }

    frread(&fr, &headerSize, 4);

    if ((uint32)index >= headerSize / 4) {
        printf("\nHQR WARNING: Invalid entry index!!\n");
        frclose(&fr);
        return 0;
    }

    frseek(&fr, index*4);
    frread(&fr, &offsetToData, 4);

    frseek(&fr, offsetToData);
    frread(&fr, &realSize, 4);
    frread(&fr, &compSize, 4);

    // exist hidden entries
    if (hiddenIndex > 0) {
        int32 i = 0;
        for (i = 0; i < hiddenIndex; i++) {
            frseek(&fr, offsetToData + compSize + 10); // hidden entry
            offsetToData = offsetToData + compSize + 10; // current hidden offset
            
            frread(&fr, &realSize, 4);
            frread(&fr, &compSize, 4);
        }
    }

    frclose(&fr);

    return realSize;
}

int32 hqr_get_entry_alloc_vox(uint8 ** ptr, int8 *filename, int32 index, int32 hiddenIndex) {
    int32 size;
    size = hqr_get_entry_size_vox(filename, index, hiddenIndex);

    *ptr = (uint8*)malloc(size * sizeof(uint8));
    if (!*ptr) {
        printf("HQR WARNING: unable to allocate entry memory!!\n");
        return 0;
    }
    hqr_get_entry_vox(*ptr, filename, index, hiddenIndex);

    return size;
}
