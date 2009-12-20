/** @file sys.h
	@brief
	This file contains system types definitions

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

#ifndef SYS_H
#define SYS_H

#define Rnd(x) rand()%x

// TYPES

typedef unsigned char byte;

typedef unsigned char uint8;
typedef signed   char int8;

typedef unsigned short uint16;
typedef signed   short int16;

typedef unsigned int uint32;
typedef signed   int int32;

typedef float int64;

// ENDIAN

#define FORCEINLINE __forceinline

FORCEINLINE uint8 READ_LE_BYTE(const void *ptr) {
	return *((const uint8 *)ptr);
}

FORCEINLINE uint16 READ_LE_UINT16(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
}

FORCEINLINE int16 READ_LE_INT16(const void *ptr) {
	return (int16)READ_LE_UINT16(ptr);
}

FORCEINLINE uint32 READ_LE_UINT32(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0]);
}

FORCEINLINE int32 READ_LE_INT32(const void *ptr) {
	return (int32)READ_LE_UINT32(ptr);
}

FORCEINLINE void WRITE_LE_UINT16(void *ptr, uint16 value) {
	uint8 *b = (uint8 *)ptr;
	b[0] = (uint8)(value >> 0);
	b[1] = (uint8)(value >> 8);
}

FORCEINLINE void WRITE_LE_INT16(void *ptr, int16 value) {
	WRITE_LE_UINT16(ptr, (uint16)value);
}

FORCEINLINE void WRITE_LE_UINT32(void *ptr, uint32 value) {
	uint8 *b = (uint8 *)ptr;
	b[0] = (uint8)(value >>  0);
	b[1] = (uint8)(value >>  8);
	b[2] = (uint8)(value >> 16);
	b[3] = (uint8)(value >> 24);
}

FORCEINLINE void WRITE_LE_INT32(void *ptr, int32 value) {
	WRITE_LE_UINT32(ptr, (uint32)value);
}

#endif
