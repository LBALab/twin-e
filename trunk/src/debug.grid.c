/** @file debug.grid.c
	@brief
	This file contains grid debug routines

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

#include "debug.grid.h"
#include "grid.h"
#include "lbaengine.h"
#include "scene.h"
#include "main.h"

int32 useFreeCamera = 0;
int32 canChangeScenes = 0;

/** Change scenario camera positions */
void change_grid_camera(int16 pKey) {
	if (useFreeCamera) {
		// Press up - more X positions
		if (pKey == 'c') {
			newCameraZ--;
			reqBgRedraw = 1;
		}

		// Press down - less X positions
		if (pKey == 'z') {
			newCameraZ++;
			reqBgRedraw = 1;
		}

		// Press left - less Z positions
		if (pKey == 's') {
			newCameraX--;
			reqBgRedraw = 1;
		}

		// Press right - more Z positions
		if (pKey == 'x') {
			newCameraX++;
			reqBgRedraw = 1;
		}
	}
}

/** Change grid index */
void change_grid(int16 pKey) {
	if (canChangeScenes) {
		// Press up - more X positions
		if (pKey == 'r') {
			currentSceneIdx++;
			if (currentSceneIdx > NUM_SCENES)
				currentSceneIdx = 0;
			needChangeScene = currentSceneIdx;
			reqBgRedraw = 1;
		}

		// Press down - less X positions
		if (pKey == 'f') {
			currentSceneIdx--;
			if (currentSceneIdx < 0)
				currentSceneIdx = NUM_SCENES;
			needChangeScene = currentSceneIdx;
			reqBgRedraw = 1;
		}

		if (cfgfile.Debug && (pKey == 'f' || pKey == 'r'))
			printf("\nGrid index changed: %d\n", needChangeScene);
	}
}

/** Apply and change disappear celling grid */
void apply_celling_grid(int16 pKey) {
	// Increase celling grid index
	if (pKey == 'g') {
		cellingGridIdx++;
		if (cellingGridIdx > 133)
			cellingGridIdx = 133;
	}
	// Decrease celling grid index
	if (pKey == 'b') {
		cellingGridIdx--;
		if (cellingGridIdx < 0)
			cellingGridIdx = 0;
	}

	// Enable/disable celling grid
	if (pKey == 't' && useCellingGrid == -1) {
		useCellingGrid = 1;
		//create_grid_map();
		init_celling_grid(cellingGridIdx);
		if (cfgfile.Debug && pKey == 't')
			printf("\nEnable Celling Grid index: %d\n", cellingGridIdx);
		needChangeScene = -2; // tricky to make the fade
	} else if (pKey == 't' && useCellingGrid == 1) {
		useCellingGrid = -1;
		create_grid_map();
		reqBgRedraw = 1;
		if (cfgfile.Debug && pKey == 't')
			printf("\nDisable Celling Grid index: %d\n", cellingGridIdx);
		needChangeScene = -2; // tricky to make the fade
	}
}

