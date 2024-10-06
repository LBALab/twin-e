/** @file music.h
    @brief
    This file contains music playing routines

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

#ifndef MUSIC_H
#define MUSIC_H

#include "sys.h"


extern int32 music_index;


void music_volume(int32 volume);
void music_play_cd_track(int32 track);
void music_stop_cd_track();

void playTrackMusicMp3(int32 track);
void stopTrackMusicMp3();

void music_play_midi(int32 midiIdx, int32 loop);
void music_stop_midi();
void music_stop();

void music_play_track(int32 track);
void music_stop_track();
int32 music_init_cdrom();

#endif
