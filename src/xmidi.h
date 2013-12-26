#ifndef XMIDI_H
#define XMIDI_H

#include "sys.h"

/**
 * Credit where credit is due:
 * Most of code to convert XMIDI to MIDI is adapted from either the ScummVM
 * project or the Exult game engine.
 * //risca
 */

/*
 * Takes a pointer to XMIDI data of size 'size' and converts it to MIDI. The
 * result is allocated dynamically and saved to the 'dest' pointer. Returns
 * the size of the MIDI data or 0 on error.
 */
uint32 convert_to_midi(uint8* data, uint32 size, uint8** dest);

#endif // XMIDI_H
