/** @file resources.h
	@brief
	This file contains the definitions of most used game resources. 
	
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

#ifndef RESOURCES_H
#define RESOURCES_H

#include "sys.h"
#include "hqrdepack.h"

/** RESS.HQR FILE */
#define RESSHQR_MAINPAL 0
#define RESSHQR_LBAFONT 1
#define RESSHQR_BLANK 2
#define RESSHQR_SPRITEBOXDATA 3
#define RESSHQR_SPRITESHADOW 4
#define RESSHQR_HOLOPAL 5
#define RESSHQR_HOLOSURFACE 6
#define RESSHQR_HOLOIMG 7
#define RESSHQR_HOLOARROWINFO 8
#define RESSHQR_HOLOTWINMDL 9
#define RESSHQR_HOLOARROWMDL 10
#define RESSHQR_HOLOTWINARROWMDL 11
#define RESSHQR_RELLENTIMG 12
#define RESSHQR_RELLENTPAL 13
#define RESSHQR_MENUIMG 14
#define RESSHQR_INTROSCREEN1IMG 15
#define RESSHQR_INTROSCREEN1PAL 16
#define RESSHQR_INTROSCREEN2IMG 17
#define RESSHQR_INTROSCREEN2PAL 18
#define RESSHQR_INTROSCREEN3IMG 19
#define RESSHQR_INTROSCREEN3PAL 20

#define RESSHQR_ADELINEIMG 27
#define RESSHQR_ADELINEPAL 28

#define RESSHQR_LBAIMG 49
#define RESSHQR_LBAPAL 50
#define RESSHQR_PLASMAEFFECT 51
#define RESSHQR_EAIMG 52
#define RESSHQR_EAPAL 53

#define FLA_DRAGON3 "dragon3"
#define FLA_INTROD  "introd"

#define FILE3DHQR_HERONORMAL 0
#define FILE3DHQR_HEROATHLETIC 1
#define FILE3DHQR_HEROAGGRESSIVE 2
#define FILE3DHQR_HERODISCRETE 3
#define FILE3DHQR_HEROPROTOPACK 4

extern int8 * HQR_RESS_FILE;
extern int8 * HQR_TEXT_FILE;
extern int8 * HQR_FLASAMP_FILE;
extern int8 * HQR_MIDI_MI_WIN_FILE;
extern int8 * HQR_MIDI_MI_WIN_MP3_FILE;
extern int8 * HQR_MIDI_MI_WIN_OGG_FILE;
extern int8 * HQR_SAMPLES_FILE;
extern int8 * HQR_LBA_GRI_FILE;
extern int8 * HQR_LBA_BLL_FILE;
extern int8 * HQR_LBA_BRK_FILE;
extern int8 * HQR_SCENE_FILE;
extern int8 * HQR_SPRITES_FILE;
extern int8 * HQR_FILE3D_FILE;
extern int8 * HQR_BODY_FILE;
extern int8 * HQR_ANIM_FILE;

//HQREntity* RESSentity;

#endif
