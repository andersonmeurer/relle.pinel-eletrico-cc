#ifndef	__GLCD_TEXT_H
#define __GLCD_TEXT_H

#include "_config_cpu_.h"
#include "_config_cpu_display.h"
#include "_config_lib_graphics.h"

typedef u8 tTextMode;  // type holding mode for scrolling and future attributes like padding etc.
					 // the only tTextMode supported in the current release is scrolling

#define glcdSCROLL_UP 			0
#define glcdSCROLL_DOWN 		1 // this was changed from -1 so it can used in a bitmask
#define glcdDEFAULT_SCROLLDIR 	glcdSCROLL_UP

// Font Indices
#define glcdFONT_LENGTH			0
#define glcdFONT_FIXED_WIDTH	2
#define glcdFONT_HEIGHT			3
#define glcdFONT_FIRST_CHAR		4
#define glcdFONT_CHAR_COUNT		5
#define glcdFONT_WIDTH_TABLE	6

typedef const u8* tFont;

// enums for ansi style erase function
  // These values match the ANSI EraseInLine terminal primitive: CSI n K
//
 // @ingroup glcd_enum
 // @hideinitializer
 // @brief Erase line options
 // @details
 // These enumerations are used with the
 // \ref gText::EraseTextLine( tEraseLine type) "EraseTextLine()" function call.\n
 // They provide similar functionality to the
 // ANSI EraseInLine terminal primitive: CSI n K

typedef enum  {
	eraseTO_EOL, 	// < Erase From cursor to end of Line
	eraseFROM_BOL,	// < Erase From Begining of Line to Cursor
	eraseFULL_LINE	// < Erase Entire line
} tEraseLine;

struct tArea {
	u8 x1;
	u8 y1;
	u8 x2;
	u8 y2;
	s8 mode;
};

typedef struct {
	tFont font;
	u8 fontColor;
	u8 x, y;
	struct tArea tarea;
	#if (glcdDEFERSCROLL == pdON)
	u8 need_scroll; // set when text scroll has been defered
	#endif
} tglcdText;

void glcd_TextInit(void);
void glcd_TextClearArea(void);
void glcd_TextSelectFont(tFont font, u8 color);
void glcd_SetFontColor(u8 color);
void glcd_SetTextMode(tTextMode mode);
void glcd_TextCursorToXY(u8 x, u8 y);
void glcd_TextCursorTo(u8 column, u8 row);
void glcd_TextCursorToCol(s8 column);
void glcd_TextEraseTextLine(tEraseLine type);
void glcd_TextEraseTextLinerRow(u8 row);
void glcd_TextScrollUp(u8 x1, u8 y1, u8 x2, u8 y2, u8 pixels, u8 color);
#if (glcdSCROLLDOWN == pdON)
void glcd_TextScrollDown(u8 x1, u8 y1, u8 x2, u8 y2, u8 pixels, u8 color);
#endif
void glcd_DrawString(char *str, u8 x, u8 y);
u8 glcd_TextCharWidth(u8 c);
u8 glcd_TextCharHeight(void);
u16 glcd_TextStringWidth(const char* str);
#if defined (CPU_AVR)
u16 glcd_TextStringWidth_P(PGM_P str);
#endif
int glcd_TextPutChar(n16 c);

#endif


