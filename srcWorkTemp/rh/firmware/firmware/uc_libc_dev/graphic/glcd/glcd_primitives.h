#ifndef __GLCD_PRIMITIVES_H
#define __GLCD_PRIMITIVES_H

#include "glcd.h"

void glcd_DrawVLine(u8 x, u8 y, u8 height, u8 color);
void glcd_DrawHLine(u8 x, u8 y, u8 width, u8 color);
void glcd_DrawRoundRect(u8 x, u8 y, u8 width, u8 height, u8 radius, u8 color);
void glcd_DrawCircle(u8 xCenter, u8 yCenter, u8 radius, u8 color);
void glcd_FillCircle(u8 xCenter, u8 yCenter, u8 radius, u8 color);
void glcd_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 color);
void glcd_DrawRect(u8 x, u8 y, u8 width, u8 height, u8 color);
void glcd_FillRect(u8 x, u8 y, u8 width, u8 height, u8 color);
void glcd_InvertRect(u8 x, u8 y, u8 width, u8 height);
void glcd_ClearScreen(u8 color);
void glcd_DrawBitmap(tImageBMP bitmap, u8 x, u8 y, u8 color);

#endif

