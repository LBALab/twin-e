/** @file renderer.h
    @brief
    This file contains 3d models render routines

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

#ifndef RENDERER_H
#define RENDERER_H

#include "sys.h"

extern int32 isUsingOrhoProjection;
 
extern int16 projPosXScreen; // fullRedrawVar1
extern int16 projPosYScreen; // fullRedrawVar2
extern int16 projPosZScreen; // fullRedrawVar3
extern int16 projPosX;
extern int16 projPosY;
extern int16 projPosZ;
 
extern int32 orthoProjX; // setSomethingVar1
extern int32 orthoProjY; // setSomethingVar2
extern int32 orthoProjZ; // setSomethingVar2

extern int32 X0;
extern int32 Y0;
extern int32 Z0;

extern int16 *shadeAngleTab3; // tab3


extern int16 polyRenderType; //FillVertic_AType;
extern int32 numOfVertex;
extern int16 vertexCoordinates[193];
extern int16 *pRenderV1;

void setLightVector(int32 angleX, int32 angleY, int32 angleZ);

int32 computePolygons();
void renderPolygons(int32 ecx, int32 edi);

void prepareIsoModel(uint8 *bodyPtr); // loadGfxSub

int32 projectPositionOnScreen(int32 cX, int32 cY, int32 cZ);
void setCameraPosition(int32 X, int32 Y, int32 cX, int32 cY, int32 cZ);
void camera_set_follow(int32 transPosX, int32 transPosY, int32 transPosZ, int32 rotPosX, int32 rotPosY, int32 rotPosZ, int32 param6);
void setBaseTranslation(int32 X, int32 Y, int32 Z);
void camera_set_angle(int32 X, int32 Y, int32 Z);
void setOrthoProjection(int32 X, int32 Y, int32 Z);

void trigo_rotate(int32 X, int32 Z, int32 angle);
void trigo_world_rotate_point(int32 X, int32 Y, int32 Z);

int32 renderIsoModel(int32 X, int32 Y, int32 Z, int32 angleX, int32 angleY, int32 angleZ, uint8 *bodyPtr);

void copyActorInternAnim(uint8 *bodyPtrSrc, uint8 *bodyPtrDest);

void renderBehaviourModel(int32 boxLeft, int32 boxTop, int32 boxRight, int32 boxBottom, int32 Y, int32 angle, uint8 *entityPtr);

void renderInventoryItem(int32 X, int32 Y, uint8* itemBodyPtr, int32 angle, int32 param);

#endif
