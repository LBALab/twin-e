/** @file holomap.c
	@brief
	This file contains holomap routines

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

#include "holomap.h"
#include "gamestate.h"
#include "lbaengine.h"
#include "screens.h"
#include "scene.h"
#include "sound.h"
#include "interface.h"
#include "sdlengine.h"
#include "renderer.h"
#include "text.h"
#include "resources.h"

int32 needToLoadHolomapGFX = 0;
uint8 paletteHolomap[NUMOFCOLORS * 3];

uint8 *videoPtr1;
uint8 *videoPtr2;
uint8 *videoPtr3;
uint8 *videoPtr4;
uint8 *videoPtr5;
uint8 *videoPtr6;
uint8 *videoPtr7;
uint8 *videoPtr8;
uint8 *videoPtr9;
uint8 *videoPtr10;
uint8 *videoPtr11;
uint8 *videoPtr12;
uint8 *videoPtr13;


void setHolomapPosition(int32 locationIdx) {
	holomapFlags[locationIdx] = 0x81;
}

void clearHolomapPosition(int32 locationIdx) {
	holomapFlags[locationIdx] &= 0x7E; 
	holomapFlags[locationIdx] |= 0x40;
}


void patchObject(uint8* modelPtr) { // TODO

}


void computeCoorMapping() { // TODO

}


void computeCoorGlobe() { // TODO

}

void loadHolomapGFX() { // TODO
	int32 i;
	int32 j;

	videoPtr1 = workVideoBuffer;
	videoPtr2 = workVideoBuffer + 4488;
	videoPtr3 = workVideoBuffer + 7854;
	videoPtr4 = workVideoBuffer + 8398;

	videoPtr5 = workVideoBuffer + 73934;

	hqrGetEntry(videoPtr3, HQR_RESS_FILE, RESSHQR_HOLOSURFACE);
	hqrGetEntry(videoPtr4, HQR_RESS_FILE, RESSHQR_HOLOIMG);

	videoPtr6 = videoPtr5 + hqrGetEntry(videoPtr5, HQR_RESS_FILE, RESSHQR_HOLOTWINMDL);
	videoPtr7 = videoPtr6 + hqrGetEntry(videoPtr6, HQR_RESS_FILE, RESSHQR_HOLOARROWMDL);
	videoPtr8 = videoPtr7 + hqrGetEntry(videoPtr7, HQR_RESS_FILE, RESSHQR_HOLOTWINARROWMDL);
	videoPtr11 = videoPtr8 + hqrGetEntry(videoPtr8, HQR_RESS_FILE, RESSHQR_HOLOPOINTMDL);

	patchObject(videoPtr5);
	patchObject(videoPtr6);
	patchObject(videoPtr7);

	patchObject(videoPtr8);

	videoPtr10 = videoPtr11 + 4488;
	videoPtr12 = videoPtr10 + hqrGetEntry(videoPtr10, HQR_RESS_FILE, RESSHQR_HOLOARROWINFO);
	videoPtr13 = videoPtr12 + hqrGetEntry(videoPtr12, HQR_RESS_FILE, RESSHQR_HOLOPOINTANIM);

	loadCustomPalette(RESSHQR_HOLOPAL);

	j = 576;
	for (i = 0; i < 96; i += 3, j += 3) {
		paletteHolomap[i] = palette[j];
		paletteHolomap[i + 1] = palette[j + 1];
		paletteHolomap[i + 2] = palette[j + 2];
	}

	j = 576;
	for (i = 96; i < 189; i += 3, j += 3) {
		paletteHolomap[i] = palette[j];
		paletteHolomap[i + 1] = palette[j + 1];
		paletteHolomap[i + 2] = palette[j + 2];
	}

	computeCoorMapping();
	computeCoorGlobe();

	needToLoadHolomapGFX=0;
}

void drawHolomapTitle(int32 x, int32 y) {
	int8	string[256] ;

	strcpy( (char*)string, "HoloMap" ) ;

	x -= (int16)(getTextSize( string )/2) ;
	y -= 18 ;

	setFontColor( 12*16+10 ) ;
	drawText( x-1, y-1, string ) ;
	drawText( x  , y-1, string ) ;
	drawText( x+1, y-1, string ) ;

	drawText( x-1, y+1, string ) ;
	drawText( x  , y+1, string ) ;
	drawText( x+1, y+1, string ) ;

	drawText( x-1, y , string ) ;
	drawText( x+1, y , string ) ;

	setFontColor( 15 ) ;
	drawText( x  , y, string ) ;
}

void drawHolomapTrajectory(int32 trajectoryIndex) { // TODO
	
}

void processHolomap() { // TODO
	int32 alphaLightTmp;
    int32 betaLightTmp;

	freezeTime();

	// TODO memcopy palette

	alphaLightTmp = alphaLight;
    betaLightTmp = betaLight;

	fadeToBlack(paletteRGBA);
	stopSamples();
	resetClip();
	clearScreen();
	flip();
	copyScreen(frontVideoBuffer, workVideoBuffer);

	loadHolomapGFX();
	drawHolomapTitle(320, 25);
	setCameraPosition(320, 190, 128, 1024, 1024);

	cfgfile.LanguageCDId = 0;
	initTextBank(2);
	setFontCrossColor(9);

	// TODO

	newGameVar4 = 1;
	fadeToBlack(paletteRGBA);
	alphaLight = alphaLightTmp;
    betaLight = betaLightTmp;
	initEngineVars(0);

	initTextBank(currentTextBank + 3);

	// TODO memcopy reset palette

	unfreezeTime();
}
