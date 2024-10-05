/** @file hqr.h
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

#ifndef HQR_H
#define HQR_H

#include "sys.h"

int32 hqr_get_num_entries(int8 *filename);

int32 hqr_get_entry(uint8 * ptr, int8 *filename, int32 index);
int32 hqr_get_entry_size(int8 *filename, int32 index);
int32 hqr_get_entry_alloc(uint8 ** ptr, int8 *filename, int32 index);

int32 hqr_get_entry_vox(uint8 * ptr, int8 *filename, int32 index, int32 hiddenIndex);
int32 hqr_get_entry_alloc_vox(uint8 ** ptr, int8 *filename, int32 index, int32 hiddenIndex);

#endif
