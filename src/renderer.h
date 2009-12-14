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


extern int isUsingOrhoProjection;

extern int orthoProjX; // setSomethingVar1
extern int orthoProjY; // setSomethingVar2
extern int orthoProjZ; // setSomethingVar2

extern int destX;
extern int destY;
extern int destZ;

void set_light_vector(int angleX, int angleY, int angleZ);

void prepare_iso_model(unsigned char *bodyPtr); // loadGfxSub

int project_position_on_screen(int cX, int cY, int cZ);
void set_camera_position( int X, int Y, int cX, int cY, int cZ );
void set_base_translation(int X, int Y, int Z);
void set_base_rotation(int X, int Y, int Z);
void set_ortho_projection(int X, int Y, int Z);

int render_iso_model(int X, int Y, int Z, int angleX, int angleY, int angleZ, unsigned char *bodyPtr);

void copy_actor_intern_anim(unsigned char *bodyPtrSrc, unsigned char *bodyPtrDest);

#endif
