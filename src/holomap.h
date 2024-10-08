/** @file holomap.h
    @brief
    This file contains holomap routines

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

#ifndef HOLOMAP_H
#define HOLOMAP_H

#include "sys.h"

/** Set Holomap location position
    @location Scene where position must be set */
void setHolomapPosition(int32 locationIdx);

/** Clear Holomap location position
    @location Scene where position must be cleared */
void clearHolomapPosition(int32 locationIdx);

/** Draw Holomap Title */
void holomap_draw_title(int32 width, int32 height);

/** Draw Holomap Trajectory */
void holomap_draw_trajectory(int32 trajectoryIndex);

void loadGfxSub(uint8* modelPtr);
void loadGfxSub1();
void loadGfxSub2();

/** Load Holomap content */
void holomap_init();

/** Main holomap process loop */
void holomap_run();

#endif
