/** @file holomap.c
	@brief
	This file contains holomap routines

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team
	Copyright (C) 2008-2013 Prequengine team
	Copyright (C) 2002-2007 The TwinEngine team

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

#include "holomap.h"
#include "gamestate.h"

/** Set Holomap location position
	@location Scene where position must be set */
void setHolomapPosition(int32 location) {
	holomapFlags[location] = 0x81;
}

/** Clear Holomap location position
	@location Scene where position must be cleared */
void clearHolomapPosition(int32 location) {
	holomapFlags[location] &= 0x7E; 
	holomapFlags[location] |= 0x40;
}
