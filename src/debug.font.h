#ifndef DEBUG_FONT_H
#define DEBUG_FONT_H

#include "sys.h"

void debug_set_colour(uint8 colour);
void debug_draw_text(int32 x, int32 y, int8 *str, int32 centered);

#endif
