/** @file hqrdepack.h
	@brief 
	This file contains High Quality Resource (HQR) decompress routines.
	
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

#ifndef HQRDEPACK_H
#define HQRDEPACK_H

/** Get a HQR entry pointer
	@param ptr pointer to save the entry
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size */
int hqr_get_entry(unsigned char * ptr, char *filename, int index);

/** Get a HQR entry pointer
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size */
int hqr_entry_size(char *filename, int index);

/** Get a HQR total number of entries
	@param filename HQR file name
	@return total number of entries */
int hqr_num_entries(char *filename);

/** Get a HQR entry pointer with memory allocation
	@param ptr pointer to save the entry
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size */
int hqr_getalloc_entry(unsigned char ** ptr, char *filename, int index);

#endif
