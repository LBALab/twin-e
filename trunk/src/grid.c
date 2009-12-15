/** @file grid.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"
#include "resources.h"
#include "lbaengine.h"
#include "scene.h"
#include "sdlengine.h"
#include "interface.h"
#include "images.h"
#include "actor.h"
#include "renderer.h"

/** Grip X size */
#define GRID_SIZE_X 64
/** Grip Y size */
#define GRID_SIZE_Y 25
/** Grip Z size */
#define GRID_SIZE_Z GRID_SIZE_X

/** Total number of bricks allowed in the game */
#define NUM_BRICKS 9000

/** Total number of bricks allowed in the game */
#define CELLING_GRIDS_START_INDEX	120

/** Table with all loaded bricks */
uint8* brickTable[NUM_BRICKS];
/** Table with all loaded bricks masks */
uint8* brickMaskTable[NUM_BRICKS];
/** Table with all loaded bricks sizes */
uint32   brickSizeTable[NUM_BRICKS];
/** Table with all loaded bricks usage */
uint8  brickUsageTable[NUM_BRICKS];

/** Current grid pointer */
uint8 *currentGrid;
/** Current block library pointer */
uint8 *currentBll;
/** Number of block libraries */
int32 numberOfBll;

/** Block fragment entry */
struct BlockEntry 
{
	/** Block library index */
	uint8 blockIdx; 
	/** Brick index inside the block library */
	uint8 brickBlockIdx; 
};
/** Grid block entry types */
typedef struct BlockEntry blockMap[64][64][25]; 

/** Brick entry data */
typedef struct BrickEntry
{
	/** Brick X position in screen */
	int16 x; //z
	/** Brick Y position in screen */
	int16 y;
	/** Brick Z position in screen */
	int16 z; // x
	/** Brick pixel X position */
	int16 posX;
	/** Brick pixel Y position */
	int16 posY;
	/** Brick index */
	int16 index;
	/** Brick shape type */
	uint8 shape;
	/** Brick sound type */
	uint8 sound;
} BrickEntry;

/** Brick data buffer */
BrickEntry bricksDataBuffer[28][150];
/** Brick info buffer */
int16 brickInfoBuffer[28];

/** Current brick pixel X position */
int32 brickPixelPosX;
/** Current brick pixel Y position */
int32 brickPixelPosY;

/** Copy grid mask to allow actors to display over the bricks
	@param index current brick index
	@param x grid X coordinate
	@param y grid Y coordinate
	@param buffer work video buffer */
void copy_grid_mask(int32 index, int32 x, int32 y, uint8 *buffer)
{
	uint8 *ptr;
	int32 top;
	int32 bottom;
	int32 left;
	int32 right;
	uint8 *outPtr;
	uint8 *inPtr;
	int32 offset;
	int32 vc3;

	int32 temp;
	int32 j;

	int32 absX;
	int32 absY;

	int32 vSize;

	ptr = brickMaskTable[index];

	left = x + *(ptr + 2);
	top = y + *(ptr + 3);
	right = *ptr + left - 1;
	bottom = *(ptr + 1) + top - 1;

	if(left > textWindowRight || right < textWindowLeft || bottom < textWindowTop || top > textWindowBottom)
		return;

	ptr += 4;

	absX = left;
	absY = top;

	vSize = (bottom - top)+1;

	if(vSize <= 0)
		return;

	offset = -((right - left) - SCREEN_WIDTH)-1;

	right++;
	bottom++;

	// if line on top aren't in the blitting area...
	if(absY < textWindowTop)
	{
		int numOfLineToRemove;

		numOfLineToRemove = textWindowTop - absY;

		vSize-=numOfLineToRemove;
		if(vSize <= 0)
			return;

		absY += numOfLineToRemove;

		do
		{
			int lineDataSize;

			lineDataSize = *(ptr++);
			ptr+=lineDataSize;
		}while(--numOfLineToRemove);
	}

	// reduce the vSize to remove lines on bottom
	if(absY + vSize - 1> textWindowBottom)
	{
		vSize = textWindowBottom - absY + 1;
		if(vSize <= 0)
			return;
	}

	outPtr = frontVideoBuffer + screenLockupTable[absY] + left;
	inPtr = buffer + screenLockupTable[absY] + left;

	do
	{
		vc3 = *(ptr++);

		do
		{
			temp = *(ptr++); // skip size
			outPtr += temp;
			inPtr += temp;

			absX += temp;

			vc3--;
			if(!vc3)
				break;
      
			temp = *(ptr++); // copy size

			for(j=0;j<temp;j++)
			{
				if(absX>=textWindowLeft && absX<=textWindowRight)
					*outPtr = *inPtr;

				absX++;
				outPtr++;
				inPtr++;
			}
		}while(--vc3);

		absX = left;

		outPtr += offset;
		inPtr += offset;
	}while(--vSize);
}

/** Draw 3D actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_model_actor(int32 X, int32 Y, int32 Z)
{
	int32 CopyBlockPhysLeft;
	int32 CopyBlockPhysRight;
	int32 i;
	int32 j;
	BrickEntry *currBrickEntry;

	CopyBlockPhysLeft = ((textWindowLeft + 24) / 24 ) - 1;
	CopyBlockPhysRight = ((textWindowRight + 24) / 24);

	for( j = CopyBlockPhysLeft; j <= CopyBlockPhysRight; j++ )
	{
		for (i = 0; i < brickInfoBuffer[j]; i++)
		{
			currBrickEntry = &bricksDataBuffer[j][i];

			if (currBrickEntry->posY + 38 > textWindowTop && currBrickEntry->posY <= textWindowBottom && currBrickEntry->y >= Y)
			{
				if (currBrickEntry->x + currBrickEntry->z > Z + X)
				{
					copy_grid_mask(currBrickEntry->index,(j * 24) - 24, currBrickEntry->posY, workVideoBuffer);
				}
			}
		}
	}
}

/** Draw sprite actor over bricks
	@param X actor X coordinate
	@param Y actor Y coordinate
	@param Z actor Z coordinate */
void draw_over_sprite_actor(int32 X, int32 Y, int32 Z)
{
	int32 CopyBlockPhysLeft;
	int32 CopyBlockPhysRight;
	int32 i;
	int32 j;
	BrickEntry *currBrickEntry;

	CopyBlockPhysLeft = ((textWindowLeft + 24) / 24) - 1;
	CopyBlockPhysRight = (textWindowRight + 24) / 24;

	for(j = CopyBlockPhysLeft; j <= CopyBlockPhysRight; j++)
	{
		for (i = 0; i < brickInfoBuffer[j]; i++)
		{
			currBrickEntry = &bricksDataBuffer[j][i];

			if (currBrickEntry->posY + 38 > textWindowTop && currBrickEntry->posY <= textWindowBottom && currBrickEntry->y >= Y)
			{
				if ((currBrickEntry->x == X) && (currBrickEntry->z == Z))
				{
					copy_grid_mask(currBrickEntry->index, (j * 24) - 24, currBrickEntry->posY, workVideoBuffer);
				}

				if ((currBrickEntry->x > X) || (currBrickEntry->z > Z))
				{
					copy_grid_mask(currBrickEntry->index, (j * 24) - 24, currBrickEntry->posY, workVideoBuffer);
				}
			}
		}
	}
}

/** Process brick masks to allow actors to display over the bricks 
	@param buffer brick pointer buffer
	@param ptr brick mask pointer buffer */
int process_grid_mask(uint8 *buffer, uint8 *ptr)
{
	uint32 *ptrSave = (uint32 *)ptr;
	uint8 *ptr2;
	uint8 *esi;
	uint8 *edi;
	uint8 iteration, ch, numOfBlock, ah, bl, al, bh;
	int32 ebx;

	ebx = *((uint32 *)buffer); // brick flag
	buffer+=4;
	*((uint32 *)ptr) = ebx;
	ptr+=4;

	bh = (ebx & 0x0000FF00) >> 8;

	esi = (uint8 *) buffer;
	edi = (uint8 *) ptr;

	iteration = 0;
	ch = 0;

	do
	{
		numOfBlock = 0;
		ah = 0;
		ptr2 = edi;

		edi++;

		bl = *(esi++);

		if (*(esi) & 0xC0) // the first time isn't skip. the skip size is 0 in that case
		{
			*edi++ = 0;
			numOfBlock++;
		}

		do
		{
			al = *esi++;
			iteration = al;
			iteration &= 0x3F;
			iteration++;

			if (al & 0x80)
			{
				ah += iteration;
				esi++;
			}
			else if (al & 0x40)
			{
				ah += iteration;
				esi += iteration;
			}
			else // skip
			{
				if (ah) 
				{
					*edi++ = ah; // write down the number of pixel passed so far
					numOfBlock++;
					ah = 0;
				}
				*(edi++) = iteration; //write skip
				numOfBlock++;
			}
		}while (--bl > 0);

		if (ah)
		{
			*edi++ = ah;
			numOfBlock++;

			ah = 0;
		}

		*ptr2 = numOfBlock;
	}while (--bh > 0);

	return ((int) ((uint8 *) edi - (uint8 *) ptrSave));
}

/** Create grid masks to allow display actors over the bricks */
void create_grid_mask()
{
	int32 b;

	for(b=0; b<NUM_BRICKS; b++)
	{
		if(brickUsageTable[b])
		{
			if(brickMaskTable[b])
				free(brickMaskTable[b]);
			brickMaskTable[b] = (uint8*)malloc(brickSizeTable[b]);
			process_grid_mask(brickTable[b], brickMaskTable[b]);
		}
	}
}

/** Get sprite width and height sizes
	@param offset sprite pointer offset 
	@param width sprite width size
	@param height sprite height size
	@param spritePtr sprite buffer pointer */
void get_sprite_size(int32 offset, int32 *width, int32 *height, uint8 *spritePtr)
{
	spritePtr += *((int32 *)(spritePtr + offset * 4));

	*width = *spritePtr;
	*height = *(spritePtr + 1);
}

/** Load grid bricks according with block librarie usage
	@param gridSize size of the current grid
	@return true if everything went ok*/
int32 load_grid_bricks(int32 gridSize)
{
	uint32 firstBrick = 60000;
	uint32 lastBrick = 0;
	uint8* ptrToBllBits;
	uint32 i;
	uint32 j;
	uint32 currentBllEntryIdx = 0;
  
	memset(brickTable, 0, sizeof(brickTable));
	memset(brickSizeTable, 0, sizeof(brickSizeTable));
	memset(brickUsageTable, 0, sizeof(brickUsageTable));

	// get block librarie usage bits
	ptrToBllBits = currentGrid + (gridSize - 32);
  
	// for all bits under the 32bytes (256bits)
	for(i=1; i<256; i++)
	{
		uint8 currentBitByte = *(ptrToBllBits + (i/8));
		uint8 currentBitMask = 1 << (7-(i&7));
	
		if(currentBitByte & currentBitMask)
		{
			uint32 currentBllOffset = *((uint32 *)(currentBll + currentBllEntryIdx));
			uint8* currentBllPtr = currentBll + currentBllOffset;

			uint32 bllSizeX = currentBllPtr[0];
			uint32 bllSizeY = currentBllPtr[1];
			uint32 bllSizeZ = currentBllPtr[2];
	  
			uint32 bllSize = bllSizeX * bllSizeY * bllSizeZ;

			uint8* bllDataPtr = currentBllPtr + 5;
	         
			for(j=0; j<bllSize; j++)
			{
				uint32 brickIdx = *((int16*)(bllDataPtr));
		
				if(brickIdx)
				{
					brickIdx--;

					if (brickIdx <= firstBrick)
						firstBrick = brickIdx;

					if (brickIdx > lastBrick)
						lastBrick = brickIdx;

					brickUsageTable[brickIdx] = 1;
				}
				bllDataPtr += 4;
			}
		}
		currentBllEntryIdx += 4;
	}
     
	for(i=firstBrick; i<=lastBrick; i++)
	{
		if(brickUsageTable[i])
		{
			brickSizeTable[i] = hqr_getalloc_entry(&brickTable[i],HQR_LBA_BRK_FILE,i);
		}
	}

	return 1;
}

/** Create grid Y column in block buffer
	@param gridEntry current grid index
	@param dest destination block buffer */
void create_grid_column(uint8 *gridEntry, uint8 *dest)
{
	int32 blockCount;
	int32 brickCount;
	int32 flag;
	int32 gridIdx;
	int32 i;
	uint16 *gridBuffer;
	uint16 *blockByffer;

	brickCount = *(gridEntry++);

	do
	{
		flag = *(gridEntry++);

		blockCount = (flag & 0x3F) + 1;

		gridBuffer = (uint16 *) gridEntry;
		blockByffer = (uint16 *) dest;

		if (!(flag & 0xC0))
		{
			for (i = 0; i < blockCount; i++)
				*(blockByffer++) = 0;
		}
		else if (flag & 0x40)
		{
			for (i = 0; i < blockCount; i++)
				*(blockByffer++) = *(gridBuffer++);
		}
		else
		{
			gridIdx = *(gridBuffer++);
			for (i = 0; i < blockCount; i++)
				*(blockByffer++) = gridIdx;
		}

		gridEntry = (uint8 *) gridBuffer;
		dest = (uint8 *) blockByffer;

	}while (--brickCount);
}

/** Create grid Y column in block buffer
	@param gridEntry current grid index
	@param dest destination block buffer */
void create_celling_grid_column(uint8 *gridEntry, uint8 *dest)
{
	int32 blockCount;
	int32 brickCount;
	int32 flag;
	int32 gridIdx;
	int32 i;
	uint16 *gridBuffer;
	uint16 *blockByffer;

	brickCount = *(gridEntry++);

	do
	{
		flag = *(gridEntry++);

		blockCount = (flag & 0x3F) + 1;

		gridBuffer = (uint16*) gridEntry;
		blockByffer = (uint16 *) dest;

		if (!(flag & 0xC0))
		{
			for (i = 0; i < blockCount; i++)
				blockByffer++;
		}
		else if (flag & 0x40)
		{
			for (i = 0; i < blockCount; i++)
				*(blockByffer++) = *(gridBuffer++);
		}
		else
		{
			gridIdx = *(gridBuffer++);
			for (i = 0; i < blockCount; i++)
				*(blockByffer++) = gridIdx;
		}

		gridEntry = (uint8 *) gridBuffer;
		dest = (uint8 *) blockByffer;

	}while (--brickCount);
}

/** Create grid map from current grid to block library buffer */
void create_grid_map()
{
	int32 currOffset = 0;
	int32 blockOffset;
	int32 gridIdx;
	int32 x,z;

	for(z=0; z<GRID_SIZE_Z; z++)
	{
		blockOffset = currOffset;
		gridIdx = z << 6;

		for(x=0; x<GRID_SIZE_X; x++)
		{
			int32 gridOffset = *((uint16 *)(currentGrid + 2 * (x + gridIdx)));
			create_grid_column(currentGrid+gridOffset, blockBuffer + blockOffset);
			blockOffset += 50;
		}
		currOffset += 3200;
	}
}

/** Create celling grid map from celling grid to block library buffer
	@param gridPtr celling grid buffer pointer */
void create_celling_grid_map(uint8* gridPtr)
{
	int32 currGridOffset=0;
	int32 currOffset=0;
	int32 blockOffset;
	int32 z,x;
	uint8* tempGridPtr;

	for(z=0; z<GRID_SIZE_Z; z++)
	{
		blockOffset = currOffset;
		tempGridPtr = gridPtr+currGridOffset;

		for(x=0; x<GRID_SIZE_X; x++)
		{
			int gridOffset = *((uint16 *)tempGridPtr); tempGridPtr+=2;
			create_celling_grid_column(gridPtr + gridOffset, blockBuffer + blockOffset);
			blockOffset += 50;
		}
		currGridOffset += 128;
		currOffset += 3200;
	}
}

/** Initialize grid (background scenearios)
	@param index grid index number */
int32 init_grid(int32 index)
{
	int32 gridSize;
	int32 bllSize;
	int32 brickSize;

	// load grids from file
	gridSize = hqr_getalloc_entry(&currentGrid,HQR_LBA_GRI_FILE,index);
	// load layouts from file
	bllSize = hqr_getalloc_entry(&currentBll,HQR_LBA_BLL_FILE,index);

	brickSize = load_grid_bricks(gridSize);

	create_grid_mask();
	
	numberOfBll = (*((uint32 *)currentBll) >> 2);

	create_grid_map();

	return 1;
}

/** Initialize celling grid (background scenearios)
	@param index grid index number */
int32 init_celling_grid(int32 index)
{
	int32 gridSize;
	uint8* gridPtr;

	// load grids from file
	gridSize = hqr_getalloc_entry(&gridPtr, HQR_LBA_GRI_FILE, index+CELLING_GRIDS_START_INDEX);

	create_celling_grid_map(gridPtr);

	if(gridPtr)
		free(gridPtr);

	reqBgRedraw=1;

	return 0;
}

/** Draw brick sprite in the screen
	@param index brick index to draw 
	@param posX brick X position to draw 
	@param posY brick Y position to draw */
void draw_brick(int32 index, int32 posX, int32 posY)
{
	draw_brick_sprite(index, posX, posY, brickTable[index],0);
}

/** Draw sprite in the screen
	@param index sprite index to draw 
	@param posX sprite X position to draw 
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw */
void draw_sprite(int32 index, int32 posX, int32 posY, uint8 *ptr)
{
	draw_brick_sprite(index, posX, posY, ptr, 1);
}

// WARNING: Rewrite this function to have better performance
/** Draw sprite or bricks in the screen according with the type
	@param index sprite index to draw 
	@param posX sprite X position to draw 
	@param posY sprite Y position to draw
	@param ptr sprite buffer pointer to draw
	@param isSprite allows to identify if the sprite to display is brick or a single sprite */
void draw_brick_sprite(int32 index, int32 posX, int32 posY, uint8 *ptr, int32 isSprite)
{
	//unsigned char *ptr;
	int32 top;
	int32 bottom;
	int32 left;
	int32 right;
	uint8 *outPtr;
	int32 offset;
	int32 c1;
	int32 c2;
	int32 vc3;

	int32 temp;
	int32 iteration;
	int32 i;

	int32 x;
	int32 y;

	if(isSprite==1)
		ptr = ptr + *((uint32 *)(ptr + index * 4));

	left = posX + *(ptr + 2);
	top = posY + *(ptr + 3);
	right = *ptr + left - 1;
	bottom = *(ptr + 1) + top - 1;

	ptr += 4;

	x = left;
	y = top;

	 //if (left >= textWindowLeft-2 && top >= textWindowTop-2 && right <= textWindowRight-2 && bottom <= textWindowBottom-2) // crop
	{     
		right++;
		bottom++;

		outPtr = frontVideoBuffer + screenLockupTable[top] + left;

		offset = -((right - left) - SCREEN_WIDTH);

		for (c1 = 0; c1 < bottom - top; c1++)
		{
			vc3 = *(ptr++);
			for (c2 = 0; c2 < vc3; c2++)
			{
				temp = *(ptr++);
				iteration = temp & 0x3F;
				if (temp & 0xC0)
				{
					iteration++;
					if (!(temp & 0x40))
					{
						temp = *(ptr++);
						for (i = 0; i < iteration; i++)
						{
							if(x>=textWindowLeft && x<textWindowRight && y>=textWindowTop && y<textWindowBottom)
								frontVideoBuffer[y*SCREEN_WIDTH+x] = temp;

							x++;
							outPtr++;
						}
					}
					else
					{
						for (i = 0; i < iteration; i++)
						{
							if(x>=textWindowLeft && x<textWindowRight && y>=textWindowTop && y<textWindowBottom)
								frontVideoBuffer[y*SCREEN_WIDTH+x] = *ptr;

							x++;
							ptr++;
							outPtr++;
						}
					}
				}
				else
				{
					outPtr += iteration + 1;
					x+=iteration + 1;
				}
			}
			outPtr += offset;	
			x = left;
			y++;
		}
	}
}

/** Get block library
	@param index block library index
	@return pointer to the current block index */
uint8* get_block_library(int32 index) 
{
	int32 offset = *((uint32 *)(currentBll + 4 * index));
	return (uint8 *)(currentBll + offset);
}

/** Get brick position in the screen
	@param x column x position in the current camera
	@param y column y position in the current camera
	@param z column z position in the current camera */
void get_brick_pos(int32 x, int32 y, int32 z)
{
	brickPixelPosX = (x - z) * 24 + 288; // x pos
	brickPixelPosY = ((x + z)*12) - (y * 15) + 215;  // y pos
}

/** Draw a specific brick in the grid column according with the block index
	@param blockIdx block library index
	@param brickBlockIdx brick index inside the block 
	@param x column x position
	@param y column y position
	@param z column z position */
void draw_column_grid(int32 blockIdx, int32 brickBlockIdx, int32 x, int32 y, int32 z)
{
	uint8 *blockPtr;
	uint16 brickIdx;
	uint8 brickShape;
	uint8 brickSound;
	int32 brickBuffIdx;
	BrickEntry *currBrickEntry;

	blockPtr = get_block_library(blockIdx) + 3 + brickBlockIdx * 4;
	
	brickShape = *((uint8 *)(blockPtr));
	brickSound = *((uint8 *)(blockPtr+1));
	brickIdx = *((uint16 *)(blockPtr+2));

	if (!brickIdx)
		return;

	get_brick_pos(x - newCameraX, y - newCameraY, z - newCameraZ);
	//get_brick_pos(x, y, z);

	if (brickPixelPosX < -24)
		return;
	if (brickPixelPosX >= SCREEN_WIDTH)
		return;
	if (brickPixelPosY < -38)
		return;
	if (brickPixelPosY >= SCREEN_HEIGHT)
		return;

	// draw the background brick
	draw_brick(brickIdx-1, brickPixelPosX, brickPixelPosY);

	brickBuffIdx = (brickPixelPosX + 24) / 24;

	if (brickInfoBuffer[brickBuffIdx] >= 150)
	{
		printf("\nGRID WARNING: brick buffer exceeded! \n");
		return;
	}

	currBrickEntry = &bricksDataBuffer[brickBuffIdx][brickInfoBuffer[brickBuffIdx]];

	currBrickEntry->x = x;
	currBrickEntry->y = y;
	currBrickEntry->z = z;
	currBrickEntry->posX = brickPixelPosX;
	currBrickEntry->posY = brickPixelPosY;
	currBrickEntry->index = brickIdx - 1;
	currBrickEntry->shape = brickShape;
	currBrickEntry->sound = brickSound;

	brickInfoBuffer[brickBuffIdx]++;
}

/** Redraw grid background */
void redraw_grid()
{
	int32 i, x, y, z;
	uint8 blockIdx;
	blockMap* map = (blockMap*)blockBuffer;

	cameraX = newCameraX << 9;
	cameraY = newCameraY << 8;
	cameraZ = newCameraZ << 9;

	project_position_on_screen(-cameraX, -cameraY, -cameraZ);

	projPosXScreen = projPosX;
	projPosYScreen = projPosY;

	for (i = 0; i < 28; i++)
	{
		brickInfoBuffer[i] = 0;
	}

	// TODO: check this later
	//if (changeRoomVar10 == 0)
	//		return;

	for (z = 0; z < GRID_SIZE_Z; z++)
    {
		for (x = 0; x < GRID_SIZE_X; x++)
		{
			for (y = 0; y < GRID_SIZE_Y; y++)
			{
				blockIdx = (*map)[z][x][y].blockIdx;
				if(blockIdx)
				{
					draw_column_grid(blockIdx-1, (*map)[z][x][y].brickBlockIdx,x,y,z);
				}
			}
		}
    }
}
