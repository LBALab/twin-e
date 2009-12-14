/** @file menu.c
	@brief
	This file contains main menu create and processing routines.
	
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

#include "main.h"
#include "menu.h"
#include "menuoptions.h"
#include "resources.h"
#include "music.h"
#include "sound.h"
#include "images.h"
#include "sdlengine.h"
#include "hqrdepack.h"
#include "lbaengine.h"
#include "dialogues.h"
#include "interface.h"
#include "redraw.h"

/** Plasma effect file size: RESS.hqr:51 */
#define PLASMA_EFFECT_FILESIZE	262176

/** Main menu new game option key */
#define MAINMENU_NEWGAME		20
/** Main menu continue game option key */
#define MAINMENU_CONTINUEGAME	21
/** Main menu option key */
#define MAINMENU_OPTIONS		23
/** Main menu quit key */
#define MAINMENU_QUIT			22

/** Give up menu continue game key */
#define GIVEUPMENU_CONTINUE		28
/** give up menu quit game key */
#define GIVEUPMENU_QUIT			27

/** options menu return game key */
#define OPTIONSMENU_RETURNGAME		15
/** options menu return menu key */
#define OPTIONSMENU_RETURNMENU		26
/** options menu volume key */
#define OPTIONSMENU_VOLUME			30
/** options menu save game management key */
#define OPTIONSMENU_SAVEMANAGE		46
/** options menu advanced options key */
#define OPTIONSMENU_ADVOPTIONS		47

/** Main menu background image number
	
	Used when returning from credit sequence to redraw the main menu background image */
#define MAINMENU_BKGIMAGE		9999 
/** Menu buttons width */
#define MAINMENU_BUTTONWIDTH	320
/** Used to calculate the spanning between button and screen */
#define MAINMENU_BUTTONSPAN		550 

/** Music volume button key */
#define MUSICVOLUME				1
/** Sound effects volume button key */
#define SOUNDVOLUME				2
/** CD volume button key */
#define CDVOLUME				3
/** Line-in volume button key */
#define LINEVOLUME				4
/** Master volume button key */
#define MASTERVOLUME			5

/** Main Menu Settings

	Used to create the game main menu. */
short int MainMenuSettings[]={
	0,          // Current loaded button (button number)
	4,          // Num of buttons
	200,        // Buttons box height ( is used to calc the height where the first button will appear )
	0,          // unused
	0,          
	20,         // new game
	0,
	21,         // continue game
	0,
	23,         // options
	0,
	22,         // quit
};

/** Give Up Menu Settings

	Used to create the in-game menu. */
short int GiveUpMenuSettings[]={
	0,			// Current loaded button (button number)
	2,			// Num of buttons
	240,		// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	28,			// continue game
	0,
	27,			// quit game
};

/** Give Up Menu Settings

	Used to create the in-game menu. This menu have one extra item to save the game */
short int GiveUpMenuSettingsWithSave[]={
	0,			// Current loaded button (button number)
	3,			// Num of buttons
	240,		// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	28,			// continue game
	0,
	16,			// save game
	0,
	27,			// quit game
};

/** Options Menu Settings

	Used to create the options menu. */
short int OptionsMenuSettings[]={
	0,			// Current loaded button (button number)
	4,			// Num of buttons
	0,		// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	24,			// return to previous menu
	0,
	30,			// volume settings
	0,
	46,			// save game management
	0,
	47,			// advanced options
};

/** Advanced Options Menu Settings

	Used to create the advanced options menu. */
short int AdvOptionsMenuSettings[]={
	0,			// Current loaded button (button number)
	5,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	0,
	4,			// aggressive mode (manual|auto)
	6,
	31,			// Polygon detail (full|medium|low)
	7,
	32,			// Shadows (all|character|no)
	8,			
	33,			// scenary zoon (on|off)
};

/** Save Game Management Menu Settings

	Used to create the save game management menu. */
short int SaveManageMenuSettings[]={
	0,			// Current loaded button (button number)
	3,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	0,
	41,			// copy saved game
	0,
	45,			// delete saved game
};

/** Volume Menu Settings

	Used to create the volume menu. */
short int VolumeMenuSettings[]={
	0,			// Current loaded button (button number)
	7,			// Num of buttons
	0,			// Buttons box height ( is used to calc the height where the first button will appear )
	0,			// unused
	0,
	26,			// return to main menu
	1,
	10,			// music volume
	2,
	11,			// sfx volume
	3,
	12,			// cd volume
	4,
	13,			// line-in volume
	5,
	14,			// master volume
	0,
	16,			// save parameters
};


/** Plasma Effect pointer to file content: RESS.HQR:51 */
unsigned char *plasmaEffectPtr;
/** Plasma Effect variables 1 */
unsigned char *plasmaEffectVar1;
/** Plasma Effect variables 2 */
unsigned char *plasmaEffectVar2;


/** Plasma Effect Initialization */
void plasma_effect_init()
{
	unsigned char *temp1 = 0;
	unsigned char *temp2 = 0;
	char *temp2bis = 0;
	short int temp4 = 0;
	int i = 0;
	unsigned int *copy1 = 0;
	unsigned int *copy2 = 0;

	plasmaEffectVar1 = plasmaEffectPtr;
	plasmaEffectVar2 = plasmaEffectPtr + 16000;

	temp1 = plasmaEffectVar1 + 321; // where start first line
	temp2 = plasmaEffectVar2 + 321; // where start first line

	for (i = 0; i < 15358; i++) // for all the lines except the last one
	{
		temp4 = 0;

		temp4 += temp1[-1];
		temp4 += temp1[-320];
		temp4 += temp1[-319];
		temp4 += temp1[-321];
		temp4 += temp1[+1];
		temp4 += temp1[+320];
		temp4 += temp1[+319];
		temp4 += temp1[+321];

		temp4 = (temp4 >> 3) | ((temp4 & 0x0003) << 13);

		if (!(temp4 & 0x6500))
		{
			if (temp2 >= (plasmaEffectVar2 + 14720))
			{
				temp4--;
			}
			else if (temp4 > 0)
			{
				temp4--;
			}
		}

		temp1++;
		*temp2 = (unsigned char) temp4;
		temp2++;
	}

	copy1 = (unsigned int *) plasmaEffectVar1;
	copy2 = (unsigned int *) (plasmaEffectVar2 + 320);  // pass to the next line

	for (i = 0; i < 3840; i++)
		*(copy1++) = *(copy2++);  // copy current frame buffer with 1 line

	temp2bis = (char *) plasmaEffectVar2 + 12480; // pointer with 11 last lines

	for (i = 1600; i >= 0; i--) // prepare next frame
	{
		temp4 = *temp2bis;
		if (temp4 <= 15)
		{
			*temp2bis = -(temp4 - 11);
		}
		temp2++;
	}
}

/** Process the plasma effect
	@param top top height where the effect will be draw in the front buffer
	@param color plasma effect start color */
void process_plasma_effect(int top, int color)
{
	unsigned char *temp;
	unsigned char *out;
	int i, j;
	unsigned char temp3;
	unsigned char bh, bl;

	plasma_effect_init();
	
	temp = plasmaEffectVar1 + 1600;
	out = frontVideoBuffer + screenLockupTable[top];

	bl = color;
	bh = bl + 15;

	for (i = 25; i >= 0; i--)
	{
		for (j = MAINMENU_BUTTONWIDTH; j >= 0; j--)
		{
			temp3 = *temp;
			temp3 = temp3 >> 1;
			temp3 += bl;
			if (temp3 > bh)
				temp3 = bh;

			out[0] = temp3;
			out[1] = temp3;
			out[640] = temp3;
			out[641] = temp3;

			temp++;
			out += 2;
		}
		out += 640; //SCREEN_WIDTH;
	}
}

/** Draw the entire button box 
	@param left start width to draw the button
	@param top start height to draw the button
	@param right end width to draw the button
	@param bottom end height to draw the button */
void draw_button_box(int left, int top, int right, int bottom)
{
    draw_line(left, top, right, top, 79);			// top line
    draw_line(left, top, left, bottom, 79);			// left line
    draw_line(right, ++top, right, bottom, 73);		// right line
    draw_line(++left, bottom, right, bottom, 73);	// bottom line
}

/** Draws main menu button
	@param width menu button width
	@param topheight is the height between the top of the screen and the first button
	@param id current button identification from menu settings
	@param value current button key pressed value
	@param mode flag to know if should draw as a hover button or not */
void draw_button_gfx(int width, int topheight, int id, int value, int mode)
{
	int right;
    int top;
    int left;
    int bottom2;
    int bottom;
   /*
    * int CDvolumeRemaped; int musicVolumeRemaped; int masterVolumeRemaped; int lineVolumeRemaped;
    * int waveVolumeRemaped;
    */
    
	char dialText[256];
    int textSize;

    left = width - MAINMENU_BUTTONSPAN / 2;
    right = width + MAINMENU_BUTTONSPAN / 2;

	// topheigh is the center Y pos of the button
    top = topheight - 25;		// this makes the button be 50 height
    bottom = bottom2 = topheight + 25;	// ||

    if (mode != 0)
	{
		if (id <= 5 && id >= 1)
		{
			int newWidth=0;

			switch (id)
			{
				case 1:
				{
					if(cfgfile.MusicVolume>255)
						cfgfile.MusicVolume=255;
					if(cfgfile.MusicVolume<0)
						cfgfile.MusicVolume=0;
					newWidth = cross_dot(left,right,255,cfgfile.MusicVolume);
					break;
				}
				case 2:
				{
					if(cfgfile.WaveVolume>255)
						cfgfile.WaveVolume=255;
					if(cfgfile.WaveVolume<0)
						cfgfile.WaveVolume=0;
					newWidth = cross_dot(left,right,255,cfgfile.WaveVolume);
					break;
				}
				case 3:
				{
					if(cfgfile.CDVolume>255)
						cfgfile.CDVolume=255;
					if(cfgfile.CDVolume<0)
						cfgfile.CDVolume=0;
					newWidth = cross_dot(left,right,255,cfgfile.CDVolume);
					break;
				}
				case 4:
				{
					if(cfgfile.LineVolume>255)
						cfgfile.LineVolume=255;
					if(cfgfile.LineVolume<0)
						cfgfile.LineVolume=0;
					newWidth = cross_dot(left,right,255,cfgfile.LineVolume);
					break;
				}
				case 5:
				{
					if(cfgfile.MasterVolume>255)
						cfgfile.MasterVolume=255;
					if(cfgfile.MasterVolume<0)
						cfgfile.MasterVolume=0;
					newWidth = cross_dot(left,right,255,cfgfile.MasterVolume);
					break;
				}
			};

			process_plasma_effect(top, 80);
			if (!(rand() % 5))
			{
				plasmaEffectPtr[rand() % 140 * 10 + 1900] = 255;
			}
			draw_splitted_box(newWidth, top, right, bottom, 68);
		}
		else
		{
			process_plasma_effect(top, 64);
			if (!(rand() % 5))
			{
				plasmaEffectPtr[rand() % 320 * 10 + 6400] = 255;
			}
		}

		if (id <= 5 && id >= 1)
		{
			// implement this
		}
	}
    else
	{
		blit_box(left, top, right, bottom, (char *) workVideoBuffer, left, top,(char *) frontVideoBuffer);
		draw_transparent_box(left, top, right, bottom2, 4);
	}

    draw_button_box(left, top, right, bottom);
    
	set_font_color(15);
	get_menu_text(value, dialText);
	textSize = dialogue_text_size(dialText);
	display_dialogue_text(width - (textSize / 2), topheight - 18, dialText);

	// TODO: make volume buttons

    copy_block_phys(left, top, right, bottom);
}

/** Process the menu button draw
	@param data menu settings array
	@param mode flag to know if should draw as a hover button or not */
void draw_button(short int *menuSettings, int mode)
{
	int buttonNumber;
    int maxButton;
    short int *localData = menuSettings;
    int topHeight;
    unsigned char menuItemId;
    unsigned short menuItemValue; // applicable for sound menus, to save the volume/sound bar
    char currentButton;
	
    buttonNumber = *localData;
    localData += 1;
    maxButton = *localData;
    localData += 1;
    topHeight = *localData;
    localData += 2;

    if (topHeight == 0)
	{
      topHeight = 35;
	}
	else
	{
		topHeight = topHeight - (((maxButton - 1) * 6) + ((maxButton) * 50)) / 2;
	}

    if (maxButton <= 0)
	{
      return;
	}

    currentButton = 0;

    do
	{
		// get menu item settings
		menuItemId = (unsigned char) *localData;
		localData += 1;
		menuItemValue = *localData;
		localData += 1;
		if (mode != 0)
		{
			if (currentButton == buttonNumber)
			{
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 1);
			}
		}
		else
		{
			if (currentButton == buttonNumber)
			{
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 1);
			}
			else
			{
				draw_button_gfx(MAINMENU_BUTTONWIDTH, topHeight, menuItemId, menuItemValue, 0);
			}
		}

		currentButton++;
		topHeight += 56; // increase button top height
		
		// slow down the CPU
		fps_cycles(1000);
	}
    while (currentButton < maxButton);
}

/** Where the main menu options are processed
	@param menuSettings menu settings array with the information to build the menu options
	@return pressed menu button identification */
int process_menu(short int * menuSettings)
{
	int localTime;
    int numEntry;
    int buttonNeedRedraw;
    int maxButton;
    short int *localData = menuSettings;
    short int currentButton;
    short int id;
    int musicChanged;
    int buttonReleased = 1;

    musicChanged = 0;

    buttonNeedRedraw = 1;

    numEntry = localData[1];
    currentButton = localData[0];
    localTime = lbaTime;
    maxButton = numEntry - 1;

	read_keys();

    do
	{
		// if its on main menu
		if (localData == MainMenuSettings)
		{
			if (lbaTime - localTime <= 11650)
			{
				if (skipIntro == 46)
					if (skipedKey != 32)
						return MAINMENU_BKGIMAGE;
			}
			else
			{
				return MAINMENU_BKGIMAGE;
			}
		}

		if (pressedKey == 0)
		{
			buttonReleased = 1;
		}

		if (buttonReleased)
		{
			key = pressedKey;

			if (((unsigned char) key & 2)) // on arrow key down
			{
				currentButton++;
				if (currentButton == numEntry) // if current button is the last, than next button is the first
				{
					currentButton = 0;
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			if (((unsigned char) key & 1)) // on arrow key up
			{
				currentButton--;
				if (currentButton < 0) // if current button is the first, than previous button is the last
				{
					currentButton = maxButton;
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			if (*(localData + 8) <= 5) // if its a volume button
			{
				id = *(localData + currentButton * 2 + 4); // get button parameters from settings array

				switch (id)
				{
					case MUSICVOLUME:
						{
							if (((unsigned char) key & 4)) // on arrow key left
							{
								cfgfile.MusicVolume-=4;
							}
							if (((unsigned char) key & 8)) // on arrow key right
							{
								cfgfile.MusicVolume+=4;
							}
							music_volume(cfgfile.MusicVolume);
							break;
						}
					case SOUNDVOLUME:
						{
							if (((unsigned char) key & 4)) // on arrow key left
							{
								cfgfile.WaveVolume-=4;
							}
							if (((unsigned char) key & 8)) // on arrow key right
							{
								cfgfile.WaveVolume+=4;
							}
							sample_volume(-1, cfgfile.WaveVolume);
							break;
						}
					case CDVOLUME:
						{
							if (((unsigned char) key & 4)) // on arrow key left
							{
								cfgfile.CDVolume-=4;
							}
							if (((unsigned char) key & 8)) // on arrow key right
							{
								cfgfile.CDVolume+=4;
							}
							break;
						}
					case LINEVOLUME:
						{
							if (((unsigned char) key & 4)) // on arrow key left
							{
								cfgfile.LineVolume-=4;
							}
							if (((unsigned char) key & 8)) // on arrow key right
							{
								cfgfile.LineVolume+=4;
							}
							break;
						}
					case MASTERVOLUME:
						{
							if (((unsigned char) key & 4)) // on arrow key left
							{
								cfgfile.MasterVolume-=4;
							}
							if (((unsigned char) key & 8)) // on arrow key right
							{
								cfgfile.MasterVolume+=4;
							}
							music_volume(cfgfile.MusicVolume);
							sample_volume(-1, cfgfile.WaveVolume);
							break;
						}
					default:
						break;
				}
			}
		}

		if (buttonNeedRedraw == 1)
		{
			*localData = currentButton;

			draw_button(localData, 0); // current button
			do
			{
				read_keys();
				draw_button(localData, 1);
			}
			while (pressedKey == 0 && skipedKey == 0 && skipIntro == 0);
			buttonNeedRedraw = 0;
		}
		else
		{
			if (musicChanged)
			{
				// TODO: update volume settings
				
			}

			buttonNeedRedraw = 0;
			draw_button(localData, 1);
			read_keys();
			// WARNING: this is here to prevent a fade bug while quit the menu
			copy_screen(workVideoBuffer,frontVideoBuffer);
		}
	}
    while (!(skipedKey & 2) && !(skipedKey & 1));

    currentButton = *(localData + 5 + currentButton * 2); // get current browsed button

    read_keys();

    return currentButton;
}

/** Used to run the advanced options menu */
int advoptions_menu()
{
    int ret = 0;

    copy_screen(workVideoBuffer, frontVideoBuffer);

    do
	{
		switch(process_menu(AdvOptionsMenuSettings))
		{
			case OPTIONSMENU_RETURNMENU:
			{
				ret = 1; // quit option menu
				break;
			}
			//TODO: add other options
			default:
				break;
		}
	}
    while (ret != 1);

    copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

    return 0;
}

/** Used to run the save game management menu */
int savemanage_menu()
{
    int ret = 0;

    copy_screen(workVideoBuffer, frontVideoBuffer);

    do
	{
		switch(process_menu(SaveManageMenuSettings))
		{
			case OPTIONSMENU_RETURNMENU:
			{
				ret = 1; // quit option menu
				break;
			}
			//TODO: add other options
			default:
				break;
		}
	}
    while (ret != 1);

    copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

    return 0;
}

/** Used to run the volume menu */
int volume_menu()
{
    int ret = 0;

    copy_screen(workVideoBuffer, frontVideoBuffer);

    do
	{
		switch(process_menu(VolumeMenuSettings))
		{
			case OPTIONSMENU_RETURNMENU:
			{
				ret = 1; // quit option menu
				break;
			}
			//TODO: add other options
			default:
				break;
		}
	}
    while (ret != 1);

    copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

    return 0;
}

/** Used to run the options menu */
int options_menu()
{
    int ret = 0;

    copy_screen(workVideoBuffer, frontVideoBuffer);

    //HQ_StopSample();
    //playCDtrack(9);

    do
	{
		switch(process_menu(OptionsMenuSettings))
		{
			case OPTIONSMENU_RETURNGAME:
			case OPTIONSMENU_RETURNMENU:
			{
				ret = 1; // quit option menu
				break;
			}
			case OPTIONSMENU_VOLUME:
			{
				copy_screen(workVideoBuffer, frontVideoBuffer);
				flip(workVideoBuffer);
				volume_menu();
				break;
			}
			case OPTIONSMENU_SAVEMANAGE:
			{
				copy_screen(workVideoBuffer, frontVideoBuffer);
				flip(workVideoBuffer);
				savemanage_menu();
				break;
			}
			case OPTIONSMENU_ADVOPTIONS:
			{
				copy_screen(workVideoBuffer, frontVideoBuffer);
				flip(workVideoBuffer);
				advoptions_menu();
				break;
			}
			default:
				break;
		}
	}
    while (ret != 1);

    copy_screen(workVideoBuffer, frontVideoBuffer);
	flip(workVideoBuffer);

    return 0;
}


/** Used to run the main menu */
void main_menu()
{
	// TODO: stop samples

	copy_screen(frontVideoBuffer, workVideoBuffer);

    // load menu effect file only once
	plasmaEffectPtr = malloc(PLASMA_EFFECT_FILESIZE);
	hqr_get_entry(plasmaEffectPtr, HQR_RESS_FILE, RESSHQR_PLASMAEFFECT);

	while(!cfgfile.Quit)
	{
		// TODO: RECHECK THIS LATER
		init_dialogue_bank(0);

		play_track_music(9); // LBA's Theme
		// TODO: stop samples
		
		switch(process_menu(MainMenuSettings))
		{
			case MAINMENU_NEWGAME:
			{
				new_game_menu();
				break;
			}
			case MAINMENU_CONTINUEGAME:
			{
				break;
			}
			case MAINMENU_OPTIONS:
			{
				copy_screen(workVideoBuffer, frontVideoBuffer);
				flip(workVideoBuffer);
				OptionsMenuSettings[5] = OPTIONSMENU_RETURNMENU;
				options_menu();
				break;
			}
			case MAINMENU_QUIT:
			{
				cfgfile.Quit = 1;
				break;
			}
			case MAINMENU_BKGIMAGE:
			{
				load_menu_image( 1 );
			}
		}
		fps_cycles(cfgfile.Fps);
	}
}

/** Used to run the in-game give-up menu */
int giveup_menu()
{
    //int saveLangue=0;
    int menuId;
	short * localMenu;

    copy_screen(frontVideoBuffer, workVideoBuffer);
	//TODO: halt samples

	if(cfgfile.UseAutoSaving==1)
		localMenu = GiveUpMenuSettings;
	else
		localMenu = GiveUpMenuSettingsWithSave;

	do
	{
		//saveLangue = languageCD1;
		//languageCD1 = 0;
		init_dialogue_bank(0);

		menuId = process_menu(localMenu);

		//languageCD1 = saveLangue;

		init_dialogue_bank(currentTextBank + 3);

		fps_cycles(cfgfile.Fps);
	}
    while (menuId != GIVEUPMENU_QUIT && menuId != GIVEUPMENU_CONTINUE);

	if(menuId == GIVEUPMENU_QUIT)
		return 1;

	return 0;
}

int process_giveup_menu()
{
	if(skipIntro == 1)// && twinsen->life > 0 && twinsen->costumeIndex != -1 && !twinsen->staticFlagsBF.bNoDisplay)
	{
		freeze_time();
		if(giveup_menu())
		{
			unfreeze_time();
			redraw_engine_actions(1);
			freeze_time();
			//TODO: save game
			quitGame=0;
			cfgfile.Quit = 0;
			unfreeze_time();
			return 1;
		}
		else
		{
			unfreeze_time();
			redraw_engine_actions(1);
		}
	}
	return 0;
}

void process_options_menu(short int pKey)
{
	// Press F6
	if(pKey == 0x40)
	{
		int tmpLangCD = cfgfile.LanguageCDId;
		freeze_time();
		//TODO: Stop Samples
		OptionsMenuSettings[5] = 15;
		cfgfile.LanguageCDId = 0;
		init_dialogue_bank(0);
		options_menu();
		cfgfile.LanguageCDId = tmpLangCD;
		init_dialogue_bank(currentTextBank+3);
		//TODO: play music
		unfreeze_time();
		redraw_engine_actions(1);
	}
}
