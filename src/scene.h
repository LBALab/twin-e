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

#include "actor.h"

#define NUM_SCENES_ENTRIES	120
#define NUM_SCENES	NUM_SCENES_ENTRIES-1

#define NUM_MAX_ACTORS		100
#define NUM_MAX_ZONES		100
#define NUM_MAX_TRACKS		100

int needChangeScene;
int currentSceneIdx;

unsigned char *spriteBoundingBoxPtr;

int currentGameOverScene;
int alphaLight;
int betaLight;

typedef struct SceneSamplesStruct
{
	short int info0;
	short int info1;
	short int info2;
	short int info3;
} SceneSamplesStruct;

SceneSamplesStruct sampleAmbience;
SceneSamplesStruct sampleRepeat;
SceneSamplesStruct sampleRound;

short int sampleMinDelay;
short int sampleMinDelayRnd;
short int sceneMusic;

short int sceneHeroX;
short int sceneHeroY;
short int sceneHeroZ;

// ACTORS
int sceneNumActors;
ActorStruct sceneActors[NUM_MAX_ACTORS];
ActorStruct *sceneHero;

// ZONES

typedef struct ScenePoint
{
	short int X;
	short int Y;
	short int Z;
} ScenePoint;

typedef struct ZoneBox
{
	ScenePoint bottomLeft;
	ScenePoint topRight;
	short int type;
	short int info0;
	short int info1;
	short int info2;
	short int info3;
	short int snap;
	union
	{
		struct
		{
			short int newRoomNumber;
			short int positionX;
			short int positionY;
			short int positionZ;
		} ChangeRoom;
		struct
		{
			short int dummy;
			short int newCameraX;
			short int newCameraY;
			short int newCameraZ;
		} ForceCamera;
		struct
		{
			short int zoneNumber;
		} SetActorZone;
		struct
		{
			short int newGrid;
		} PatchGrid;
		struct
		{
			short int textIndex;
			short int textColor;
		} DisplayText;
	} infoData;
} ZoneBox;

int sceneNumZones;
ZoneBox sceneZones[NUM_MAX_ZONES];


// TRACKS

int sceneNumTracks;
ScenePoint sceneTracks[NUM_MAX_TRACKS];


// TODO: check what is this
short int changeRoomVar10;

void change_scene();

#endif
