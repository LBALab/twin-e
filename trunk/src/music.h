/** @file music.h
	@brief 
	This file contains music playing routines
	
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
*/

#ifndef MUSIC_H
#define MUSIC_H

/** Track number of the current playing music */
int currentMusic;

/** Music volume
	@param current volume number */
void music_volume(int volume);
/** Play CD music
	@param track track number to play */
void play_track_music_cd(int track);
/** Stop CD music */
void stop_track_music_cd();
/** Play MP3 music
	@param track track number to play */
void play_track_music_mp3(int track);
/** Stop MP3 music */
void stop_track_music_mp3();
/** Generic play music, according with settings it plays CD or high quality sounds instead
	@param track track number to play*/
void play_track_music(int track);
/** Generic stop music according with settings */
void stop_track_music();
/** Play MIDI music
	@param midiIdx music index under mini_mi_win.hqr*/
void play_midi_music(int midiIdx, int loop);
/** Stop MIDI music */
void stop_midi_music();

/** Initialize CD-Rom */
int init_cdrom();

#endif
