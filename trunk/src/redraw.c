/** @file redraw.c
	@brief
	This file contains engine redraw actions routines

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

#include "redraw.h"
#include "sdlengine.h"
#include "lbaengine.h"
#include "renderer.h"
#include "interface.h"
#include "images.h"
#include "grid.h"
#include "scene.h"
#include "actor.h"
#include "hqrdepack.h"
#include "resources.h"
#include "menu.h"
#include "animations.h"
#include "keyboard.h"

#ifdef GAMEMOD
#include "debug.scene.h"
#endif


typedef struct RedrawStruct {
	uint16 left;
	uint16 top;
	uint16 right;
	uint16 bottom;
} RedrawStruct;

RedrawStruct currentRedrawList[300];
RedrawStruct nextRedrawList[300];

typedef struct DrawListStruct {
	int16  posValue;
	uint16 index; // field_2
	uint16 X;
	uint16 Y;
	uint16 Z;
	uint16 field_A;
	uint16 field_C;
	uint16 field_E;
	uint16 field_10;
} DrawListStruct;

/** Draw list array to grab the necessary */
DrawListStruct drawList[150];

/** Current number of redraw regions in the screen */
int32 currNumOfRedrawBox; // fullRedrawVar8
/** Number of redraw regions in the screen */
int32 numOfRedrawBox;

/** Add a certain region to the current redraw list array
	@param left start width to redraw the region
	@param top start height to redraw the region
	@param right end width to redraw the region
	@param bottom end height to redraw the region */
void add_redraw_current_area(int32 left, int32 top, int32 right, int32 bottom) {
	int32 area;
	int32 i = 0;
	int32 leftValue;
	int32 rightValue;
	int32 topValue;
	int32 bottomValue;

	area = (right - left) * (bottom - top);

	while (i < numOfRedrawBox) {
		if (currentRedrawList[i].left >= left)
			leftValue = left;
		else
			leftValue = currentRedrawList[i].left;

		if (currentRedrawList[i].right <= right)
			rightValue = right;
		else
			rightValue = currentRedrawList[i].right;

		if (currentRedrawList[i].top >= top)
			topValue = top;
		else
			topValue = currentRedrawList[i].top;

		if (currentRedrawList[i].bottom <= bottom)
			bottomValue = bottom;
		else
			bottomValue = currentRedrawList[i].bottom;

		if ((rightValue - leftValue) *(bottomValue - topValue) < ((currentRedrawList[i].bottom - currentRedrawList[i].top) *(currentRedrawList[i].right - currentRedrawList[i].left) + area)) {
			currentRedrawList[i].left = leftValue;
			currentRedrawList[i].top = topValue;
			currentRedrawList[i].right = rightValue;
			currentRedrawList[i].bottom = bottomValue;

			if (currentRedrawList[i].bottom >= SCREEN_WIDTH)
				currentRedrawList[i].bottom = SCREEN_TEXTLIMIT_BOTTOM;
			return;
		}

		i++;
	};

	currentRedrawList[i].left = left;
	currentRedrawList[i].top = top;
	currentRedrawList[i].right = right;
	currentRedrawList[i].bottom = bottom;

	if (currentRedrawList[i].bottom >= SCREEN_WIDTH)
		currentRedrawList[i].bottom = SCREEN_TEXTLIMIT_BOTTOM;

	numOfRedrawBox++;
}

/** Add a certain region to redraw list array
	@param left start width to redraw the region
	@param top start height to redraw the region
	@param right end width to redraw the region
	@param bottom end height to redraw the region */
void add_redraw_area(int32 left, int32 top, int32 right, int32 bottom) {
	if (left < 0)
		left = 0;
	if (top < 0)
		top = 0;
	if (right >= SCREEN_WIDTH)
		right = SCREEN_TEXTLIMIT_RIGHT;
	if (bottom >= SCREEN_HEIGHT)
		bottom = SCREEN_TEXTLIMIT_BOTTOM;

	if (left > right || top > bottom)
		return;

	nextRedrawList[currNumOfRedrawBox].left = left;
	nextRedrawList[currNumOfRedrawBox].top = top;
	nextRedrawList[currNumOfRedrawBox].right = right;
	nextRedrawList[currNumOfRedrawBox].bottom = bottom;

	currNumOfRedrawBox++;

	add_redraw_current_area(left, top, right, bottom);
}

/** Move next regions to the current redraw list */
void move_next_areas() {
	int32 i;

	numOfRedrawBox = 0;

	for (i = 0; i < currNumOfRedrawBox; i++) {
		add_redraw_current_area(nextRedrawList[i].left, nextRedrawList[i].top, nextRedrawList[i].right, nextRedrawList[i].bottom);
	}
}

/** Flip currentRedrawList regions in the screen

	This only updates small areas in the screen so few CPU processor is used */
void flip_redraw_areas() {
	int32 i;

	for (i = 0; i < numOfRedrawBox; i++) { // redraw areas on screen
		copy_block_phys(currentRedrawList[i].left, currentRedrawList[i].top, currentRedrawList[i].right, currentRedrawList[i].bottom);
	}

	numOfRedrawBox = 0;

	for (i = 0; i < currNumOfRedrawBox; i++) { //setup the redraw areas for next display
		add_redraw_current_area(nextRedrawList[i].left, nextRedrawList[i].top, nextRedrawList[i].right, nextRedrawList[i].bottom);
	}
}

/** Blit/Update all screen regions in the currentRedrawList */
void blit_background_areas() {
	int32 i;
	RedrawStruct* currentArea;
	currentArea = currentRedrawList;

	for (i = 0; i < numOfRedrawBox; i++) {
		blit_box(currentArea->left, currentArea->top, currentArea->right, currentArea->bottom, (int8 *) workVideoBuffer, currentArea->left, currentArea->top, (int8 *) frontVideoBuffer);
		currentArea++;
	}
}

/** Sort drawing list struct ordered as the first objects appear in the top left corner of the screen
	@param list drawing list variable which contains information of the drawing objects
	@param listSize number of drawing objects in the list */
void sort_drawing_list(DrawListStruct *list, int32 listSize) {
	int32 i;
	int32 j;

	DrawListStruct tempStruct;

	for (i = 0; i < listSize - 1; i++) {
		for (j = 0; j < listSize - 1 - i; j++) {
			if (list[j+1].posValue < list[j].posValue) {
				memcpy(&tempStruct, &list[j+1], sizeof(DrawListStruct));
				memcpy(&list[j+1], &list[j], sizeof(DrawListStruct));
				memcpy(&list[j], &tempStruct, sizeof(DrawListStruct));
			}
		}
	}
}

/** Process what objects must the drawn in the screen
	@param bgRedraw true if we want to redraw background grid, false if we want to update certain screen areas */
int process_actors_drawlist(int32 bgRedraw) {
	int32 tmpVal;
	int32 modelActorPos;  // arg_1A
	int32 spriteActorPos; // top6
	int32 shadowActorPos; // top2
	int32 drawListPos;    // a12
	ActorStruct *actor;

	modelActorPos = 0;
	drawListPos = 0;
	spriteActorPos = modelActorPos + 0x1000;
	shadowActorPos = modelActorPos + 0x0C00;

	// Process actors drawing list
	for (modelActorPos = 0; modelActorPos < sceneNumActors; modelActorPos++, spriteActorPos++, shadowActorPos++) {
		actor = &sceneActors[modelActorPos];
		actor->dynamicFlags.bIsVisible = 0; // reset visible state

		if ((useCellingGrid == -1) || actor->Y <= (*(int16 *)(cellingGridIdx*24 + (int8 *)sceneZones + 8))) {
			// no redraw required
			if (actor->staticFlags.bIsBackgrounded && bgRedraw == 0) {
				// get actor position on screen
				project_position_on_screen(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ);

				// check if actor is visible on screen, otherwise don't display it
				if (projPosX > -50 && projPosX < 680 && projPosY > -30 && projPosY < 580) {
					actor->dynamicFlags.bIsVisible = 1;
				}
			} else {
				// if the actor isn't set as hidden
				if (actor->entity != -1 && !(actor->staticFlags.bIsHidden)) {
					// get actor position on screen
					project_position_on_screen(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ);

					if (((actor->staticFlags.bUsesClipping) && projPosX > -112 && projPosX < 752 && projPosY > -50 && projPosY < 651) ||
					        ((!(actor->staticFlags.bUsesClipping)) && projPosX > -50 && projPosX < 680 && projPosY > -30 && projPosY < 580)) {
						tmpVal = actor->Z + actor->X - cameraX - cameraZ;

						// if actor is above another actor
						if (actor->standOn != -1) {
							tmpVal = sceneActors[actor->standOn].X - cameraX + sceneActors[actor->standOn].Z - cameraZ + 2;
						}

						if (actor->staticFlags.bIsSpriteActor) {
							drawList[drawListPos].index = spriteActorPos; // > 0x1000
							if (actor->staticFlags.bUsesClipping) {
								tmpVal = actor->lastX - cameraX + actor->lastZ - cameraZ;
							}
						} else {
							drawList[drawListPos].index = modelActorPos;
						}

						drawList[drawListPos].posValue = tmpVal;

						drawListPos++;

						// if use shadows
						if (cfgfile.ShadowMode != 0 && !(actor->staticFlags.bDoesntCastShadow)) {
							if (actor->standOn != -1) {
								shadowX = actor->X;
								shadowY = actor->Y - 1;
								shadowZ = actor->Z;
							} else {
								//get_shadow_position(actor->X, actor->Y, actor->Z);
							}

							tmpVal--;
							drawList[drawListPos].posValue = tmpVal; // save the shadow entry in the drawList
							drawList[drawListPos].index = 0xC00;     // shadowActorPos
							drawList[drawListPos].X = shadowX;
							drawList[drawListPos].Y = shadowY;
							drawList[drawListPos].Z = shadowZ;
							drawList[drawListPos].field_A = 2;
							drawListPos++;
						}
					}
				}
			}
		}
	}
	return drawListPos;
}

/** Process object drawing on screen

	Objects like 3D actors, sprite actors, extra bonus and shadows
	@param numDrawingList number of drawing actors in the current screen*/
void process_drawing(int32 numDrawingList) {
	int32 drawListPos = 0;

	// if has something to draw
	if (numDrawingList > 0) {
		uint32 flags;
		int32 actorIdx;
		ActorStruct *actor;

		do {
			actorIdx = drawList[drawListPos].index & 0x3FF;
			actor = &sceneActors[actorIdx];
			flags = ((uint32) drawList[drawListPos].index) & 0xFC00;

			// Drawing actors
			if (flags < 0xC00) {
				if (!flags) {
					if (!actorIdx) { // RECHECK
					}

					if (actor->previousAnimIdx != -1)
						set_model_animation(actor->animPosition, animTable[actor->previousAnimIdx], bodyTable[actor->entity], &actor->animTimerData);

					if (!render_iso_model(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ, 0, actor->angle, 0, bodyTable[actor->entity])) {
						if (renderLeft < 0)
							renderLeft = SCREEN_TEXTLIMIT_LEFT;

						if (renderTop < 0)
							renderTop = SCREEN_TEXTLIMIT_TOP;

						if (renderRight >= SCREEN_WIDTH)
							renderRight = SCREEN_TEXTLIMIT_RIGHT;

						if (renderBottom >= SCREEN_HEIGHT)
							renderBottom = SCREEN_TEXTLIMIT_BOTTOM;

						set_clip(renderLeft, renderTop, renderRight, renderBottom);

						if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
							int32 tempX;
							int32 tempY;
							int32 tempZ;

							actor->dynamicFlags.bIsVisible = 1;

							tempX = (actor->X + 0x100) >> 9;
							tempY = actor->Y >> 8;

							if (actor->field_3 & 0x7F)
								tempY++;

							tempZ = (actor->Z + 0x100) >> 9;

							draw_over_model_actor(tempX, tempY, tempZ);

							add_redraw_area(textWindowLeft, textWindowTop, renderRight, renderBottom);
						}
					}
				}
			}
			// Drawing shadows
			else if (flags == 0xC00) {

			}
			// Drawing unknown
			else if (flags < 0x1000) {

			}
			// Drawing sprite actors
			else if (flags == 0x1000) {
				int32 spriteWidth, spriteHeight;
				//int32 spriteSize = spriteSizeTable[actor->entity];
				uint8 *spritePtr = spriteTable[actor->entity];

				// get actor position on screen
				project_position_on_screen(actor->X - cameraX, actor->Y - cameraY, actor->Z - cameraZ);

				get_sprite_size(0, &spriteWidth, &spriteHeight, spritePtr);

				// calculate sprite position on screen
				renderLeft = projPosX + *(int16 *)(spriteBoundingBoxPtr + actor->entity * 16);
				renderTop = projPosY + *(int16 *)(spriteBoundingBoxPtr + actor->entity * 16 + 2);
				renderRight = renderLeft + spriteWidth;
				renderBottom = renderTop + spriteHeight;

				if (actor->staticFlags.bUsesClipping) {
					set_clip(projPosXScreen + actor->info0, projPosYScreen + actor->info1, projPosXScreen + actor->info2, projPosYScreen + actor->info3);
				} else {
					set_clip(renderLeft, renderTop, renderRight, renderBottom);
				}

				if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
					draw_sprite(0, renderLeft, renderTop, spritePtr);

					actor->dynamicFlags.bIsVisible = 1;

					if (actor->staticFlags.bUsesClipping) {
						draw_over_sprite_actor((actor->lastX + 0x100) >> 9, actor->lastY >> 8, (actor->lastZ + 0x100) >> 9);
					} else {
						int tempX, tempZ, tempY;

						tempX = (actor->X + actor->boudingBox.X.topRight + 0x100) >> 9;
						tempZ = actor->Y >> 8;
						if (actor->field_3 & 0x7F)
							tempZ++;
						tempY = (actor->Z + actor->boudingBox.Z.topRight + 0x100) >> 9;

						draw_over_sprite_actor(tempX, tempZ, tempY);
					}

					add_redraw_area(textWindowLeft, textWindowTop, textWindowRight, textWindowBottom);

					// show clipping area
					//draw_button_box(renderLeft, renderTop, renderRight, renderBottom);
				}
			}
			// Drawing extras
			else if (flags == 0x1800) {

			}

			reset_clip();
			drawListPos++;
		} while (drawListPos < numDrawingList);
	}
}

/** This is responsible for the entire game screen redraw
	@param bgRedraw true if we want to redraw background grid, false if we want to update certain screen areas */
void redraw_engine_actions(int32 bgRedraw) { // fullRedraw
	int16 tmpProjPosX;
	int16 tmpProjPosY;
	int32 numDrawingList;

	tmpProjPosX = projPosXScreen;
	tmpProjPosY = projPosYScreen;

	reset_clip();

	if (bgRedraw) {
		freeze_time();
		if (needChangeScene != -1 && needChangeScene != -2)
			fade_out(paletteRGBA);
		clear_screen();
		redraw_grid();
		// TODO: Update overlay sprites positions
		copy_screen(frontVideoBuffer, workVideoBuffer);
		if (needChangeScene != -1 && needChangeScene != -2) {
			fade_in(paletteRGBA);
			set_palette(paletteRGBA);
		}
	} else {
		blit_background_areas();
	}

	// first loop
	numDrawingList = process_actors_drawlist(bgRedraw);

	sort_drawing_list(drawList, numDrawingList);

	currNumOfRedrawBox = 0;
	process_drawing(numDrawingList);

#ifdef GAMEMOD
	display_zones(skipIntro);
#endif

	// make celling grid fade
	// need to be here to fade after drawing all actors in scene
	if (needChangeScene == -2)
		cross_fade(frontVideoBuffer, paletteRGBA);

	if (bgRedraw) {
		flip();
		move_next_areas();
	} else {
		flip_redraw_areas();
	}
}
