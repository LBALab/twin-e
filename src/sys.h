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

#include <stdlib.h>
#include <math.h>

#define Rnd(x) rand()%x
#define Abs(x) abs(x)

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

#ifdef UNIX
#define FORCEINLINE static __inline__
#elif MINGW32
#define FORCEINLINE inline
#else
#define FORCEINLINE __forceinline
#endif

FORCEINLINE uint8 READ_BYTE(const void *ptr);

FORCEINLINE uint16 READ_UINT16(const void *ptr);

FORCEINLINE int16 READ_INT16(const void *ptr);

FORCEINLINE uint32 READ_UINT32(const void *ptr);

FORCEINLINE int32 READ_INT32(const void *ptr);

FORCEINLINE void WRITE_UINT16(void *ptr, uint16 value);

FORCEINLINE void WRITE_INT16(void *ptr, int16 value);

FORCEINLINE void WRITE_UINT32(void *ptr, uint32 value);

FORCEINLINE void WRITE_INT32(void *ptr, int32 value);

#endif
