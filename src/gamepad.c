/** @file gamepad.c
    @brief
    This file contains gamepad input routines

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

#include <SDL/SDL.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "gamepad.h"
#include "keyboard.h"

#define DEFAULT_DEADZONE 8000


enum GAMEPAD_BUTTONS {
    GAMEPAD_BUTTON_A = 0,           // Action
    GAMEPAD_BUTTON_B = 1,           // Jump
    GAMEPAD_BUTTON_X = 2,           // Inventory
    GAMEPAD_BUTTON_Y = 3,           // Menu
    GAMEPAD_BUTTON_LS = 9,          // Shoulder Left
    GAMEPAD_BUTTON_RS = 10,         // Shoulder Right
    // GAMEPAD_BUTTON_LT = 6,          // Trigger Left
    // GAMEPAD_BUTTON_RT = 7,          // Trigger Right
    GAMEPAD_BUTTON_BACK = 4,        // Holomap
    GAMEPAD_BUTTON_START = 6,       // Pause
    GAMEPAD_BUTTON_LP = 7,          // Left Stick Press
    GAMEPAD_BUTTON_RP = 8,          // Right Stick Press
    GAMEPAD_BUTTON_DPAD_UP = 11,    // D-Pad Up
    GAMEPAD_BUTTON_DPAD_DOWN = 12,  // D-Pad Down
    GAMEPAD_BUTTON_DPAD_LEFT = 13,  // D-Pad Left
    GAMEPAD_BUTTON_DPAD_RIGHT = 14  // D-Pad Right
    // 16 - xbox
    // 17 - share
};

SDL_Joystick* joystick = NULL;

void gamepad_init() {
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
        printf("Failed to initialize SDL joystick subsystem: %s\n", SDL_GetError());
        return;
    }

    SDL_JoystickEventState(SDL_ENABLE);
    
    int32 num_joysticks = SDL_NumJoysticks();
    printf("SDL detected %d joystick(s)\n", num_joysticks);

    joystick = SDL_JoystickOpen(0);
    if (joystick) {
        const char* name = SDL_JoystickName(0);
        int num_axes = SDL_JoystickNumAxes(joystick);
        int num_buttons = SDL_JoystickNumButtons(joystick);
        int num_hats = SDL_JoystickNumHats(joystick);
        
        printf("Gamepad: %s\n", name ? name : "Unknown");
        printf("  - Axes: %d, Buttons: %d, Hats: %d\n", num_axes, num_buttons, num_hats);
    } else {
        printf("Failed to open joystick: %s\n", SDL_GetError());
    }
}

void gamepad_close() {
    SDL_JoystickClose(joystick);
    joystick = NULL;

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void gamepad_handle_input() {
    int32 i = 0;
    int32 j = 0;
    int32 find = 0;
    int8 found = 0;
    int32 localKey[32];
    int16 temp;
    uint8 temp2;

    if (!joystick) {
        return;
    }

    pressedKey = 0;
    memset(localKey, 0, sizeof(localKey));

    SDL_JoystickUpdate();

    // Handle left stick for movement
    if (SDL_JoystickNumAxes(joystick) >= 2) {
        Sint16 left_x = SDL_JoystickGetAxis(joystick, 0);
        Sint16 left_y = SDL_JoystickGetAxis(joystick, 1);
        
        if (left_y < -DEFAULT_DEADZONE) {
            localKey[i++] = 0x48; // Up arrow
        } else if (left_y > DEFAULT_DEADZONE) {
            localKey[i++] = 0x50; // Down arrow
        }
        if (left_x < -DEFAULT_DEADZONE) {
            localKey[i++] = 0x4B; // Left arrow
        } else if (left_x > DEFAULT_DEADZONE) {
            localKey[i++] = 0x4D; // Right arrow
        }
    }

    // Handle D-pad movement (overrides analog stick if pressed)
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_DPAD_UP)) {
        localKey[i++] = 0x48; // Up arrow
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_DPAD_DOWN)) {
        localKey[i++] = 0x50; // Down arrow
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_DPAD_LEFT)) {
        localKey[i++] = 0x4B; // Left arrow
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_DPAD_RIGHT)) {
        localKey[i++] = 0x4D; // Right arrow
    }
    
    // Handle action buttons
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_A)) { // A/Cross - Space bar
        localKey[i++] = 0x39; // Space bar
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_X)) { // X/Square - Enter
        localKey[i++] = 0x11; // Action
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_Y)) { // Y/Triangle - Ctrl
        localKey[i++] = 0x36; // Shift
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_B)) { // B/Circle - Alt
        localKey[i++] = 0x01; // Escape
    }
    
    // Handle shoulder buttons
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_LS)) { // Left shoulder
        localKey[i++] = 0x1D;
        localKey[i++] = 0x4B;
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_RS)) { // Right shoulder
        localKey[i++] = 0x1D;
        localKey[i++] = 0x4D;
    }
    
    // Handle trigger buttons as function keys
    // if (SDL_JoystickGetAxis(joystick, 4)) { // Left trigger
    //     localKey = 0x38; // Alt
    // }
    if (SDL_JoystickGetAxis(joystick, 5) > DEFAULT_DEADZONE) { // Right trigger
        localKey[i++] = 0x38; // Alt
    }

    // Handle stick presses
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_LP)) { // Left stick press - F3
        localKey[i++] = 0x1C; // Enter / Recenter screen
    }
    // if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_RP)) { // Right stick press - F4
    //     localKey[i++] = 0x3B; // F4
    // }

    // Handle special buttons with direct key codes (these bypass the mapping system)
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_START)) { // Start - Pause
        localKey[i++] = 0x19; // P key for pause
    }
    if (SDL_JoystickGetButton(joystick, GAMEPAD_BUTTON_BACK)) { // Back/Select - Holomap
        localKey[i++] = 0x23; // H key for holomap
    }

    for (j = 0; j < i; j++) {
        if (localKey[j] != 0) {
            // Find localKey in pressedKeyMap array (same logic as platform_sdl.c)
            for (int k = 0; k < 28; k++) {
                if (pressedKeyMap[k] == localKey[j]) {
                    find = k;
                    found = 1;
                    break;
                }
            }
            
            if (found != 0) {
                temp = pressedKeyCharMap[find];
                temp2 = temp & 0x00FF;
                
                if (temp2 == 0) {
                    // pressed valid keys
                    if (!(localKey[j] & 0x80)) {
                        pressedKey |= (temp & 0xFF00) >> 8;
                    } else {
                        pressedKey &= -((temp & 0xFF00) >> 8);
                    }
                }
                // pressed inactive keys
                else {
                    skippedKey |= (temp & 0xFF00) >> 8;
                }
                printf("gamepad pressedKey: 0x%X, skippedKey: 0x%X\n", pressedKey, skippedKey);
                
            }
            skipIntro = localKey[j];
            printf("gamepad skipIntro: 0x%X\n", skipIntro);
        }
    }
}
