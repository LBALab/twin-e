/** @file scene.h
	@brief
	This file contains main scenario routines

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


#ifndef SCENE_H
#define SCENE_H

#include "sys.h"
#include "actor.h"

#define NUM_SCENES_ENTRIES	120
#define NUM_SCENES	NUM_SCENES_ENTRIES-1

#define NUM_MAX_ACTORS		100
#define NUM_MAX_ZONES		100
#define NUM_MAX_TRACKS		100

int32 needChangeScene;
int32 currentSceneIdx;

uint8 *spriteBoundingBoxPtr;

int32 currentGameOverScene;
int32 alphaLight;
int32 betaLight;

typedef struct SceneSamplesStruct {
	int16 info0;
	int16 info1;
	int16 info2;
	int16 info3;
} SceneSamplesStruct;

SceneSamplesStruct sampleAmbience;
SceneSamplesStruct sampleRepeat;
SceneSamplesStruct sampleRound;

int16 sampleMinDelay;
int16 sampleMinDelayRnd;
int16 sceneMusic;

int16 sceneHeroX;
int16 sceneHeroY;
int16 sceneHeroZ;

// ACTORS
int32 sceneNumActors;
ActorStruct sceneActors[NUM_MAX_ACTORS];
ActorStruct *sceneHero;

// ZONES

typedef struct ScenePoint {
	int16 X;
	int16 Y;
	int16 Z;
} ScenePoint;

typedef struct ZoneBox {
	ScenePoint bottomLeft;
	ScenePoint topRight;
	int16 type;
	int16 info0;
	int16 info1;
	int16 info2;
	int16 info3;
	int16 snap;
	union {
		struct {
			int16 newRoomNumber;
			int16 positionX;
			int16 positionY;
			int16 positionZ;
		} ChangeRoom;
		struct {
			int16 dummy;
			int16 newCameraX;
			int16 newCameraY;
			int16 newCameraZ;
		} ForceCamera;
		struct {
			int16 zoneNumber;
		} SetActorZone;
		struct {
			int16 newGrid;
		} PatchGrid;
		struct {
			int16 textIndex;
			int16 textColor;
		} DisplayText;
	} infoData;
} ZoneBox;

int32 sceneNumZones;
ZoneBox sceneZones[NUM_MAX_ZONES];


// TRACKS

int32 sceneNumTracks;
ScenePoint sceneTracks[NUM_MAX_TRACKS];


// TODO: check what is this
int16 changeRoomVar10;

void change_scene();

#endif
