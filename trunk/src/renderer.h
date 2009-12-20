/** @file renderer.h
	@brief
	This file contains 3d models render routines

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

#ifndef RENDERER_H
#define RENDERER_H

#include "sys.h"

int32 isUsingOrhoProjection;

int16 projPosXScreen; // fullRedrawVar1
int16 projPosYScreen; // fullRedrawVar2
int16 projPosZScreen; // fullRedrawVar3
int16 projPosX;
int16 projPosY;
int16 projPosZ;

int32 orthoProjX; // setSomethingVar1
int32 orthoProjY; // setSomethingVar2
int32 orthoProjZ; // setSomethingVar2

int32 destX;
int32 destY;
int32 destZ;

int16 *shadeAngleTab1; // tab1
int16 *shadeAngleTab2; // tab2
int16 *shadeAngleTab3; // tab3

void set_light_vector(int32 angleX, int32 angleY, int32 angleZ);

void prepare_iso_model(uint8 *bodyPtr); // loadGfxSub

int32 project_position_on_screen(int32 cX, int32 cY, int32 cZ);
void set_camera_position(int32 X, int32 Y, int32 cX, int32 cY, int32 cZ);
void set_base_translation(int32 X, int32 Y, int32 Z);
void set_base_rotation(int32 X, int32 Y, int32 Z);
void set_ortho_projection(int32 X, int32 Y, int32 Z);

int32 render_iso_model(int32 X, int32 Y, int32 Z, int32 angleX, int32 angleY, int32 angleZ, uint8 *bodyPtr);

void copy_actor_intern_anim(uint8 *bodyPtrSrc, uint8 *bodyPtrDest);

#endif
