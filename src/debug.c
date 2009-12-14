/** @file debug.c
	@brief
	This file contains the main game debug window routines
	
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

#include "debug.h"

#ifdef GAMEMOD
#include "debug.scene.h"
#include "debug.grid.h"
#include "scene.h"
#include "sdlengine.h"
#include "menu.h"
#include "interface.h"
#include "dialogues.h"
#include "lbaengine.h"
#include "images.h"
#include "redraw.h"

enum ButtonType
{
	NO_ACTION,
	FREE_CAMERA,
	CHANGE_SCENE,
	SHOW_CELLING_GRID,
	SHOW_ZONES,
	SHOW_ZONE_CUBE,
	SHOW_ZONE_CAMERA,
	SHOW_ZONE_SCENARIC,
	SHOW_ZONE_CELLINGGRID,
	SHOW_ZONE_OBJECT,
	SHOW_ZONE_TEXT,
	SHOW_ZONE_LADDER
};

enum WindowType
{
	NO_MENU,
	FREE_CAMERA_INFO_MENU,
	CHANGE_SCENE_INFO_MENU,
	ZONES_MENU
};

typedef struct DebugButtonStruct
{
	int left;
	int top;
	int right;
	int bottom;
	char *text;
	int textLeft;
	int textTop;
	int isActive;
	int color;
	int activeColor;
	int submenu;
	int type;
} DebugButtonStruct;

typedef struct DebugWindowStruct
{
	int left;
	int top;
	int right;
	int bottom;
	int alpha;
	int isActive;
	int numLines;
	char *text[20];
	int numButtons;
	DebugButtonStruct debugButtons[50];
} DebugWindowStruct;

DebugWindowStruct debugWindows[10];
int numDebugWindows=0;


void debug_fill_button(int X, int Y, int width, int height, char color)
{
	int i, j;
	unsigned char *ptr;
	int offset;

	ptr = frontVideoBuffer + screenLockupTable[Y] + X;
	offset = 640 - (width);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*(ptr++) = color;
		}
		ptr += offset;
	}
}

void debug_draw_button(int left, int top, int right, int bottom, char *text, int textLeft, int textRight, int isActive, char color)
{
	debug_fill_button(left+1,top+1,right-left-1,bottom-top-1,color);
	draw_button_box(left,top,right,bottom);
	ttf_draw_text(textLeft,textRight,text,0);
	copy_block_phys(left,top,right,bottom);
}

void debug_draw_window_box(int left, int top, int right, int bottom, int alpha)
{
	draw_transparent_box(left,top,right,bottom,alpha);
	draw_button_box(left,top,right,bottom);
	//copy_block_phys(left,top,right,bottom);
}

void debug_draw_window_buttons(int w)
{
	int b;
	for(b=0; b < debugWindows[w].numButtons; b++)
	{
		int left = debugWindows[w].debugButtons[b].left;
		int top = debugWindows[w].debugButtons[b].top;
		int right = debugWindows[w].debugButtons[b].right;
		int bottom = debugWindows[w].debugButtons[b].bottom;
		char *text = debugWindows[w].debugButtons[b].text;
		int textLeft = debugWindows[w].debugButtons[b].textLeft;
		int textTop = debugWindows[w].debugButtons[b].textTop;
		int isActive = debugWindows[w].debugButtons[b].isActive;
		char color = debugWindows[w].debugButtons[b].color;
		if(isActive>0)
			color = debugWindows[w].debugButtons[b].activeColor;
		
		debug_draw_button(left,top,right,bottom,text,textLeft,textTop,isActive,color);
	}
}

void debug_draw_window(int w)
{
	int left = debugWindows[w].left;
	int top = debugWindows[w].top;
	int right = debugWindows[w].right;
	int bottom = debugWindows[w].bottom;
	int alpha = debugWindows[w].alpha;

	debug_draw_window_box(left,top,right,bottom,alpha);

	if(debugWindows[w].numLines>0)
	{
		int l;
		for(l=0; l < debugWindows[w].numLines; l++)
		{
			ttf_draw_text(left+10,top+l*20+5,debugWindows[w].text[l],0);
		}
	}

	copy_block_phys(left,top,right,bottom);

	debug_draw_window_buttons(w);
}

int debug_type_use_menu(int type)
{
	int w,b;
	for(w=0; w < numDebugWindows; w++)
	{
		if(debugWindows[w].isActive>0)
		{
			for(b=0; b < debugWindows[w].numButtons; b++)
			{
				if(debugWindows[w].debugButtons[b].type==type)
				{
					int submenu = debugWindows[w].debugButtons[b].submenu;
					if(submenu>0)
						debugWindows[submenu].isActive = !debugWindows[submenu].isActive;
					return submenu;
				}
			}
		}
	}
	return 0;
}

void debug_reset_buttons_state()
{
	int w,b;
	for(w=0; w < numDebugWindows; w++)
	{
		if(debugWindows[w].isActive>0)
		{
			for(b=0; b < debugWindows[w].numButtons; b++)
			{
				if(debugWindows[w].debugButtons[b].type<=-1)
					debugWindows[w].debugButtons[b].isActive = 0;
			}
		}	
	}
}

void debug_refresh_buttons(int type)
{
	int w,b;
	for(w=0; w < numDebugWindows; w++)
	{
		if(debugWindows[w].isActive>0)
		{
			for(b=0; b < debugWindows[w].numButtons; b++)
			{
				if(debugWindows[w].debugButtons[b].type==type)
				{
					int left = debugWindows[w].debugButtons[b].left;
					int top = debugWindows[w].debugButtons[b].top;
					int right = debugWindows[w].debugButtons[b].right;
					int bottom = debugWindows[w].debugButtons[b].bottom;
					char *text = debugWindows[w].debugButtons[b].text;
					int textLeft = debugWindows[w].debugButtons[b].textLeft;
					int textTop = debugWindows[w].debugButtons[b].textTop;
					char color = debugWindows[w].debugButtons[b].color;
					int isActive = debugWindows[w].debugButtons[b].isActive = !debugWindows[w].debugButtons[b].isActive;
					
					if(isActive>0)
						color = debugWindows[w].debugButtons[b].activeColor;
					
					debug_draw_button(left,top,right,bottom,text,textLeft,textTop,isActive,color);

					if(debugWindows[w].debugButtons[b].submenu && isActive>0)
						debug_draw_window(debugWindows[w].debugButtons[b].submenu);
				}
			}
		}	
	}
}

void debug_draw_windows()
{
	int w;
	for(w=0; w < numDebugWindows; w++)
	{
		if(debugWindows[w].isActive>0)
		{
			debug_draw_window(w);
		}
	}
}

void debug_reset_button(int type)
{
	int w,b;
	for(w=0; w < numDebugWindows; w++)
	{
		if(debugWindows[w].isActive>0)
		{
			for(b=0; b < debugWindows[w].numButtons; b++)
			{
				if(debugWindows[w].debugButtons[b].type==type)
				{
					int submenu = debugWindows[w].debugButtons[b].submenu;
					debugWindows[w].debugButtons[b].isActive = 0;
					if(submenu>0)
					{
						debugWindows[submenu].debugButtons[b].isActive = !debugWindows[submenu].debugButtons[b].isActive;
					}

					return;	
				}
			}
		}
	}
}

void debug_redraw_screen() 
{
	redraw_engine_actions(1);
	copy_screen(frontVideoBuffer,workVideoBuffer);
	debug_draw_windows();
}

int debug_get_actions_state(int type)
{
	int state=0;
	switch(type)
	{
		case FREE_CAMERA:
			state = useFreeCamera;
			break;
		case CHANGE_SCENE:
			state = canChangeScenes;
			break;
		case SHOW_ZONES:
			state = showingZones;
			break;
		case SHOW_ZONE_CUBE:
		case SHOW_ZONE_CAMERA:
		case SHOW_ZONE_SCENARIC:
		case SHOW_ZONE_CELLINGGRID:
		case SHOW_ZONE_OBJECT:
		case SHOW_ZONE_TEXT:
		case SHOW_ZONE_LADDER:
			state = typeZones;
			break;
		default:
			break;
	}
	return state;
}

void debug_set_actions(int type)
{
	switch(type)
	{
		case FREE_CAMERA:
			useFreeCamera = !useFreeCamera;
			break;
	
		case CHANGE_SCENE:
			canChangeScenes = !canChangeScenes;
			break;

		case SHOW_ZONES:
			showingZones = !showingZones;
			debug_reset_button(-1);
			debug_reset_button(-2);
			debug_redraw_screen();
			break;
		case SHOW_ZONE_CUBE:
			if(showingZones)
			{
				if(typeZones & 0x01)
					typeZones &= ~0x01;
				else
					typeZones |= 0x01;
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_CAMERA:
			if(showingZones)
			{
				if(typeZones & 0x02)
					typeZones &= ~0x02;
				else
					typeZones |= 0x02;
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_SCENARIC:
			if(showingZones)
			{
				if(typeZones & 0x04)
					typeZones &= ~0x04;
				else
					typeZones |= 0x04;
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_CELLINGGRID:
			if(showingZones)
			{
				if(typeZones & 0x08)
					typeZones &= ~0x08;
				else
					typeZones |= 0x08;
				debug_redraw_screen();
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_OBJECT:
			if(showingZones)
			{
				if(typeZones & 0x10)
					typeZones &= ~0x10;
				else
					typeZones |= 0x10;
				debug_redraw_screen();
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_TEXT:
			if(showingZones)
			{
				if(typeZones & 0x20)
					typeZones &= ~0x20;
				else
					typeZones |= 0x20;
				debug_redraw_screen();
			}
			break;
		case SHOW_ZONE_LADDER:
			if(showingZones)
			{
				if(typeZones & 0x40)
					typeZones &= ~0x40;
				else
					typeZones |= 0x40;
				debug_redraw_screen();
			}
			break;


		case -1:
			debug_reset_button(-2);
			debug_redraw_screen();
			break;
		case -2:
			debug_reset_button(-1);
			debug_redraw_screen();
			break;
		default:
			break;
	}
}

void debug_add_button(int window, int left, int top, int right, int bottom, char * text, int textLeft, int textTop, int isActive, int color, int activeColor, int submenu, int type)
{
	int button = debugWindows[window].numButtons;
	debugWindows[window].debugButtons[button].left = left;
	debugWindows[window].debugButtons[button].top = top;
	debugWindows[window].debugButtons[button].right = right;
	debugWindows[window].debugButtons[button].bottom = bottom;
	debugWindows[window].debugButtons[button].text = text;
	debugWindows[window].debugButtons[button].textLeft = textLeft;
	debugWindows[window].debugButtons[button].textTop = textTop;
	debugWindows[window].debugButtons[button].isActive = debug_get_actions_state(type);
	debugWindows[window].debugButtons[button].color = color;
	debugWindows[window].debugButtons[button].activeColor = activeColor;
	debugWindows[window].debugButtons[button].submenu = submenu;
	debugWindows[window].debugButtons[button].type = type;
	debugWindows[window].numButtons++;
}

void debug_add_window_text(int window, char *text)
{
	int line = debugWindows[window].numLines;
	debugWindows[window].text[line] = text;
	debugWindows[window].numLines++;
}

void debug_add_window(int left, int top, int right, int bottom, int alpha, int isActive)
{
	debugWindows[numDebugWindows].left = left;
	debugWindows[numDebugWindows].top = top;
	debugWindows[numDebugWindows].right = right;
	debugWindows[numDebugWindows].bottom = bottom;
	debugWindows[numDebugWindows].alpha = alpha;
	debugWindows[numDebugWindows].numButtons = 0;
	debugWindows[numDebugWindows].isActive = isActive;
	numDebugWindows++;
}

void debug_left_menu()
{
	// left menu window
	debug_add_window(5,60,200,474,4,1);
	debug_add_button(0,5,55,160,75,"Use free camera",30,60,0,87,119,NO_MENU,FREE_CAMERA);
	debug_add_button(0,161,55,200,75,"info",171,60,0,87,119,FREE_CAMERA_INFO_MENU,-1);
	debug_add_button(0,5,76,160,96,"Change scenes",30,81,0,87,119,NO_MENU,CHANGE_SCENE);
	debug_add_button(0,161,76,200,96,"info",171,81,0,87,119,CHANGE_SCENE_INFO_MENU,-2);
	debug_add_button(0,5,97,200,117,"Show celling grids",30,102,0,87,119,NO_MENU,3);
	debug_add_button(0,5,118,200,138,"Show zones",30,123,0,87,119,ZONES_MENU,SHOW_ZONES);

	// add submenu windows
	//   - free camera window
	debug_add_window(205,55,634,160,4,0);
	debug_add_window_text(FREE_CAMERA_INFO_MENU,"When enable, use the following keys to browse through the scenes:");
	debug_add_window_text(FREE_CAMERA_INFO_MENU,"           - S to go North");
	debug_add_window_text(FREE_CAMERA_INFO_MENU,"           - X to go South");
	debug_add_window_text(FREE_CAMERA_INFO_MENU,"           - Z to go West");
	debug_add_window_text(FREE_CAMERA_INFO_MENU,"           - C to go East");

	//   - change scene window
	debug_add_window(205,55,634,137,4,0);
	debug_add_window_text(CHANGE_SCENE_INFO_MENU,"When enable, use the following keys to change to another scene:");
	debug_add_window_text(CHANGE_SCENE_INFO_MENU,"           - R to go Next Scene");
	debug_add_window_text(CHANGE_SCENE_INFO_MENU,"           - F to go Previous Scene");

	//   - zones window
	debug_add_window(205,55,634,97,4,0);
	debug_add_window_text(ZONES_MENU,"You can enable or disable each zone type:");
	debug_add_button(ZONES_MENU,205,118,350,138,"Cube Zones",215,123,1,87,119,0,SHOW_ZONE_CUBE);
	debug_add_button(ZONES_MENU,205,139,350,159,"Camera Zones",215,144,2,87,119,0,SHOW_ZONE_CAMERA);
	debug_add_button(ZONES_MENU,205,160,350,180,"Scenaric Zones",215,165,3,87,119,0,SHOW_ZONE_SCENARIC);
	debug_add_button(ZONES_MENU,205,181,350,201,"Celling Grid Zones",215,186,4,87,119,0,SHOW_ZONE_CELLINGGRID);
	debug_add_button(ZONES_MENU,205,202,350,222,"Object Zones",215,207,5,87,119,0,SHOW_ZONE_OBJECT);
	debug_add_button(ZONES_MENU,205,223,350,243,"Text Zones",215,228,6,87,119,0,SHOW_ZONE_TEXT);
	debug_add_button(ZONES_MENU,205,244,350,264,"Ladder Zones",215,249,7,87,119,0,SHOW_ZONE_LADDER);
}

int debug_process_button(int X, int Y)
{
	int i;
	int j;

	for (i = 0; i < numDebugWindows; i++)
	{
		for (j = 0; j < debugWindows[i].numButtons; j++)
		{
			if(X > (debugWindows[i].debugButtons[j].left)
			&& X < (debugWindows[i].debugButtons[j].right)
			&& Y > (debugWindows[i].debugButtons[j].top)
			&& Y < (debugWindows[i].debugButtons[j].bottom))
			{
				return (debugWindows[i].debugButtons[j].type);
			}
		}
	}

	return 0;
}

void debug_plasma_window(char *text, int color)
{
	int textSize;
	process_plasma_effect(5, color);
	if (!(rand() % 5))
	{
		plasmaEffectPtr[rand() % 320 * 10 + 6400] = 255;
	}
	textSize = dialogue_text_size(text);
	display_dialogue_text((SCREEN_WIDTH/2)-(textSize/2),10,text);
	draw_button_box(5,5,634,50);
	copy_block_phys(5,5,634,50);
}

void debug_process_window()
{
	if(rightMouse)
	{
		int quit=0;
		char * text = "Game Debug Window";
		int color = 64;
		int colorIdx = 4;
		int count = 0;
		MouseStatusStruct mouseData;
		rightMouse = 0;
		leftMouse = 0;

		copy_screen(frontVideoBuffer,workVideoBuffer);

		debug_reset_buttons_state();
		if(numDebugWindows==0)
			debug_left_menu();
		debug_draw_windows();	
	
		do
		{
			read_keys();
			get_mouse_positions(&mouseData);
			
			if(mouseData.left)
			{
				int type=0;			
				if((type=debug_process_button(mouseData.X,mouseData.Y))!=NO_ACTION) // process menu item
				{
					if(debug_type_use_menu(type))
					{
						copy_screen(workVideoBuffer,frontVideoBuffer);
						copy_block_phys(205,55,634,474);
					}

					debug_refresh_buttons(type);
					debug_set_actions(type);
				}
				mouseData.left = 0;
			}

			// draw window plasma effect
			if(count==256)
			{
				colorIdx++;
				count=0;
			}
			color=colorIdx*16;
			if(color>=240){
				color=64;
				colorIdx=4;
			}
			debug_plasma_window(text, color);

			// quit 
			if(mouseData.right)
				quit=1;

			fps_cycles(25); // rest

			count++;
		}
		while(!quit);
		reqBgRedraw = 1;
	}
}

void process_debug(short int pKey)
{
	debug_process_window();

	change_grid(pKey);
	change_grid_camera(pKey);
	if(needChangeScene==0);
		apply_celling_grid(pKey);
}

#endif

