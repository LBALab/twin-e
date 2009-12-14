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

/** New grid camera X coordinates */
int newCameraX;
/** New grid camera Y coordinates */
int newCameraY;
/** New grid camera Z coordinates */
int newCameraZ;

/** Current grid camera X coordinates */
int cameraX;
/** Current grid camera Y coordinates */
int cameraY;
/** Current grid camera Z coordinates */
int cameraZ;

/** Celling grid brick block buffer */
unsigned char *blockBuffer;


/** Flag to know if the engine is using celling grids */
short int useCellingGrid;
/** Current celling grid index */
short int cellingGridIdx;


/** Draw 3D actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_model_actor(int X, int Y, int Z);

/** Draw sprite actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_sprite_actor(int X, int Y, int Z);

/** Get sprite width and height sizes
	@param offset sprite pointer offset 
	@param width sprite width size
	@param height sprite height size
	@param spritePtr sprite buffer pointer */
void get_sprite_size(int offset, int *width, int *height, unsigned char *spritePtr);

/** Draw brick sprite in the screen
	@param index brick index to draw 
	@param posX brick X position to draw 
	@param posY brick Y position to draw */
void draw_brick(int index, int posX, int posY);

/** Draw sprite in the screen
	@param index sprite index to draw 
	@param posX sprite X position to draw 
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw */
void draw_sprite(int index, int posX, int posY, unsigned char *spritePtr);

/** Draw sprite or bricks in the screen according with the type
	@param index sprite index to draw 
	@param posX sprite X position to draw 
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw
	@param isSprite allows to identify if the sprite to display is brick or a single sprite */
void draw_brick_sprite(int index, int posX, int posY, unsigned char *spritePtr, int isSprite);

/** Get block library
	@param index block library index
	@return pointer to the current block index */
unsigned char* get_block_library(int index);

/** Create grid map from current grid to block library buffer */
void create_grid_map();

/** Initialize grid (background scenearios)
	@param index grid index number */
int init_grid(int index);

/** Initialize celling grid (background scenearios)
	@param index grid index number */
int init_celling_grid(int index);

/** Redraw grid background */
void redraw_grid();

#endif
