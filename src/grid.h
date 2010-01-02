/** @file grid.h
	@brief
	This file contains grid manipulation routines

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


#ifndef GRID_H
#define GRID_H

#include "sys.h"

enum ShapeType {
	kNone = 0,
	kSolid = 1,
	kStairsTopLeft = 2,
	kStairsTopRight = 3,
	kStairsBottomLeft = 4,
	kStairsBottomRight = 5,
	kDoubleSideStairsTop1 = 6,
	kDoubleSideStairsBottom1 = 7,
	kDoubleSideStairsLeft1 = 8,
	kDoubleSideStairsRight1 = 9,
	kDoubleSideStairsTop2 = 10,
	kDoubleSideStairsBottom2 = 11,
	kDoubleSideStairsLeft2 = 12,
	kDoubleSideStairsRight2 = 13,
	kFlatBottom1 = 14,
	kFlatBottom2 = 15
};

/** New grid camera X coordinates */
int32 newCameraX;
/** New grid camera Y coordinates */
int32 newCameraY;
/** New grid camera Z coordinates */
int32 newCameraZ;

/** Current grid camera X coordinates */
int32 cameraX;
/** Current grid camera Y coordinates */
int32 cameraY;
/** Current grid camera Z coordinates */
int32 cameraZ;

/** Celling grid brick block buffer */
uint8 *blockBuffer;


/** Flag to know if the engine is using celling grids */
int16 useCellingGrid; // useAnotherGrm
/** Current celling grid index */
int16 cellingGridIdx; // currentGrid2


/** Draw 3D actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_model_actor(int32 X, int32 Y, int32 Z);

/** Draw sprite actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_sprite_actor(int32 X, int32 Y, int32 Z);

/** Get sprite width and height sizes
	@param offset sprite pointer offset
	@param width sprite width size
	@param height sprite height size
	@param spritePtr sprite buffer pointer */
void get_sprite_size(int32 offset, int32 *width, int32 *height, uint8 *spritePtr);

/** Draw brick sprite in the screen
	@param index brick index to draw
	@param posX brick X position to draw
	@param posY brick Y position to draw */
void draw_brick(int32 index, int32 posX, int32 posY);

/** Draw sprite in the screen
	@param index sprite index to draw
	@param posX sprite X position to draw
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw */
void draw_sprite(int32 index, int32 posX, int32 posY, uint8 *spritePtr);

/** Draw sprite or bricks in the screen according with the type
	@param index sprite index to draw
	@param posX sprite X position to draw
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw
	@param isSprite allows to identify if the sprite to display is brick or a single sprite */
void draw_brick_sprite(int32 index, int32 posX, int32 posY, uint8 *spritePtr, int32 isSprite);

/** Get block library
	@param index block library index
	@return pointer to the current block index */
uint8* get_block_library(int32 index);

/** Create grid map from current grid to block library buffer */
void create_grid_map();

/** Initialize grid (background scenearios)
	@param index grid index number */
int32 init_grid(int32 index);

/** Initialize celling grid (background scenearios)
	@param index grid index number */
int32 init_celling_grid(int32 index);

/** Redraw grid background */
void redraw_grid();

int32 get_brick_shape(int32 x, int32 y, int32 z);

int32 get_brick_shape_full(int32 x, int32 y, int32 z, int32 y2);

int32 get_brick_sound_type(int32 x, int32 y, int32 z);

#endif
