/** @file holomap.c
	@brief
	This file contains holomap routines

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team
	Copyright (C) 2008-2013 Prequengine team
	Copyright (C) 2002-2007 The TwinEngine team

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

/** Set Holomap location position
	@location Scene where position must be set */
void setHolomapPosition(int32 locationIdx) {
	holomapFlags[locationIdx] = 0x81;
}

/** Clear Holomap location position
	@location Scene where position must be cleared */
void clearHolomapPosition(int32 locationIdx) {
	holomapFlags[locationIdx] &= 0x7E; 
	holomapFlags[locationIdx] |= 0x40;
}

/** Load Holomap content */
void loadHolomapGFX() { // TODO

}

/** Draw Holomap Title */
void drawHolomapTitle(int32 width, int32 height) {  // TODO
	
}

/** Main holomap process loop */
void processHolomap() { // TODO
	int32 alphaLightTmp;
    int32 betaLightTmp;
	int32 tmpLanguageCDId;

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

	tmpLanguageCDId = cfgfile.LanguageCDId;
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
