/*
 *
  gText.cpp - Support for Text output on a graphical device 
  Copyright (c) 2009,2010  Bill Perry and Michael Margolis

  vi:ts=4
 
  This file is part of the Arduino GLCD library.

  GLCD is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  GLCD is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with GLCD.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "glcd_text.h"
#include "glcd.h"

typedef u8 (*tFontCallback)(tFont);
static tFontCallback glcd_FontRead;               // font callback shared across all instances
static void glcd_TextSpecialChar(u8 c);
static u8 glcd_TextDefineArea(u8 x1, u8 y1, u8 x2, u8 y2, tTextMode mode);
static void glcd_TextPuts(char *str);

// the following returns true if the given font is fixed width
// zero length is flag indicating fixed width font (array does not contain width data entries)
#define isFixedWidthFont(font)  (glcd_FontRead(font+glcdFONT_LENGTH) == 0 && glcd_FontRead(font+glcdFONT_LENGTH+1) == 0))

tglcdText glcdText;

// * Select a Font and font color
// *
// * @param font a font definition
// * @param color  can be WHITE or BLACK and defaults to black
// * @param callback optional font read routine
// *
// *
// * Selects the font definition as the current font for the text area.
// *
// * All subsequent printing functions will use this font.
// *
// * Font definitions from included font definition files are stored in program memory
// * You can have as many fonts defines as will fit in program memory up to 64k and can
// * switch between them with this function.
// *
// * If the optional callback argument is ommitted, a default routine
// * is selected that assumes that the font is in program memory (flash).
// *
// * @note
// * When the display is initilized in normal mode, BLACK renders dark
// * pixels on a white background and WHITE renders white pixels on
// * black background; however, if the display is set to INVERTED mode
// * all colors are inverted.
// *
// * @see SetFontColor()
// * @see SetTextMode()

static u8 glcd_TextReadFont(tFont font);
static u8 glcd_TextReadFont(tFont font) {
	return *font;
}

// This constructor creates a text area using the entire display
// The device pointer is initialized using the global GLCD instance
// New constuctors can be added to take an exlicit glcd instance pointer
// if multiple glcd instances need to be supported
void glcd_TextInit(void) {
   	glcd_FontRead = glcd_TextReadFont;  // this sets the callback that will be used by all instances of gText
    glcd_TextDefineArea(0, 0, glcdDISPLAY_WIDTH-1, glcdDISPLAY_HEIGHT-1, glcdDEFAULT_SCROLLDIR); // this should never fail
}

// Clear text area with the current font background color
 // and home the cursor to upper left corner of the text area.
  // @see DefineArea()
void glcd_TextClearArea(void) {
	// fill the area with font background color
	glcd_SetPixels(glcdText.tarea.x1, glcdText.tarea.y1, glcdText.tarea.x2, glcdText.tarea.y2,
			glcdText.fontColor==glcdBLACK?glcdWHITE:glcdBLACK);
	 // put cursor at home position of text area to ensure we are always inside area.
	 glcd_TextCursorToXY(0, 0);
}

void glcd_TextSelectFont(tFont font, u8 color) {
	glcdText.font = font;
	glcdText.fontColor = color;
}

 // Select a font color
 //
 // @param color  can be WHITE or BLACK
 //
 //
 // @see SelectFont()
 // @see SetTextMode()
void glcd_SetFontColor(u8 color) {
   	glcdText.fontColor = color;
}

//
 // Set TextArea mode
 //
 // @param mode  text area mode
 //
 // Currently mode is a scroll direction
 // @arg glcdSCROLL_UP
 // @arg glcdSCROLL_DOWN
 //
 // @see SelectFont()
 // @see SetFontColor()
 // @see DefineArea()
void glcd_SetTextMode(tTextMode mode) {
 	// when other modes are added the tarea.mode variable will hold a bitmask or enum for the modde and should be renamed
   	glcdText.tarea.mode = mode;
}

 // Positions cursor to a X,Y position
 //
 // @param x specifies the horizontal location
 // @param y specifies the vertical location
 //
 //	X & Y are zero based pixel coordinates and are relative to
 //	the upper left corner of the text area.
 //
 // @see CursorTo()
void glcd_TextCursorToXY(u8 x, u8 y) {
	// Text position is relative to current text area

	glcdText.x = glcdText.tarea.x1 + x;
	glcdText.y = glcdText.tarea.y1 + y;

	#if (glcdDEFERSCROLL == pdON)
	// Make sure to clear a deferred scroll operation when repositioning
	glcdText.need_scroll = 0;
	#endif
}

// Positions cursor to a character based column and row.
 //
 // @param column specifies the horizontal position
 // @param row  specifies the vertical position
 //
 //	Column and Row are zero based character positions
 //	and are relative the the upper left corner of the
 //	text area base on the size of the currently selected font.
 //
 // While intended for fixed width fonts, positioning will work for variable
 // width fonts.
 //
 // When variable width fonts are used, the column is based on assuming a width
 // of the widest character.
 //
 // @see CursorToXY()
void glcd_TextCursorTo( u8 column, u8 row) {
	if(glcdText.font == 0) return; // no font selected
	// Text position is relative to current text area
	glcdText.x = column * (glcd_FontRead(glcdText.font+glcdFONT_FIXED_WIDTH)+1) + glcdText.tarea.x1;
	glcdText.y = row * (glcd_FontRead(glcdText.font+glcdFONT_HEIGHT)+1) + glcdText.tarea.y1;

	#if (glcdDEFERSCROLL == pdON)
 	//Make sure to clear a deferred scroll operation when repositioning
	glcdText.need_scroll = 0;
	#endif
}

// Positions cursor to a character based column on the current row.
 //
 // @param column specifies the horizontal position
 //
 //	Column is a 0 based character position
 //	based on the size of the currently selected font.
 //
 // If column is negative then the column position is relative to the current cursor
 // position.
 //
 // @warning
 // While intended only for fixed width fonts, cursor repositioning will be done for variable
 // width fonts.
 // When variable width fonts are used, the column is based on assuming a width
 // of the widest character in the font.
 // Because the widest character is used for the amount of cursor movement, the amount
 // of cursor movement when using relative positioning will often not be consistent with
 // the number characters previously rendered. For example, if a letter "l" was written
 // and the cursor was reposisitioned with a -1, the amount backed up will be much larger
 // than the width of the "l".
 //
 //
 // @see CursorToXY()
void glcd_TextCursorToCol(s8 column) {
	if(glcdText.font == 0)
		return; // no font selected

	 // Text position is relative to current text area
	 // negative value moves the cursor backwards

    if(column >= 0)
	  glcdText.x = column * (glcd_FontRead(glcdText.font+glcdFONT_FIXED_WIDTH)+1) + glcdText.tarea.x1;
	else
   	  glcdText.x -= column * (glcd_FontRead(glcdText.font+glcdFONT_FIXED_WIDTH)+1);

	#if (glcdDEFERSCROLL == pdON)
	// Make sure to clear a deferred scroll operation when repositioning

	glcdText.need_scroll = 0;
	#endif
}

 // Erase in Line
 //
 // @param type type of line erase
 //
 // @arg \ref eraseTO_EOL Erase from cursor to end of line
 // @arg \ref eraseFROM_BOL Erase from beginning of line to cursor
 // @arg \ref eraseFULL_LINE Erase entire line
 //
 // Erases all or part of a line of text depending on the type
 // of erase specified.
 //
 // If type is not specified it is assumed to be \ref eraseTO_EOL
 //
 // The cursor position does not change.
 //
 // @see ClearArea()
 // @see tEraseLine
void glcd_TextEraseTextLine(tEraseLine type) {
	u8 x = glcdText.x;
	u8 y = glcdText.y;
	u8 height = glcd_FontRead(glcdText.font+glcdFONT_HEIGHT);
	u8 color = (glcdText.fontColor == glcdBLACK)?glcdWHITE:glcdBLACK;

	switch(type) {
	case eraseTO_EOL:
		glcd_SetPixels(x, y, glcdText.tarea.x2, y+height, color);
		break;
	case eraseFROM_BOL:
		glcd_SetPixels(glcdText.tarea.x1, y, x, y+height, color);
		break;
	case eraseFULL_LINE:
		glcd_SetPixels(glcdText.tarea.x1, y, glcdText.tarea.x2, y+height, color);
		break;
	}

	// restore cursor position
	glcd_TextCursorToXY(x,y);
}

 // Erase Text Line
 //
 // @param row row # of text to earase
 //
 // Erases a line of text and moves the cursor
 // to the begining of the line. Rows are zero based so
 // the top line/row of a text area is 0.
 //
 // @see ClearArea()
void glcd_TextEraseTextLinerRow(u8 row) {
   glcd_TextCursorTo(0, row);
   glcd_TextEraseTextLine(eraseTO_EOL);
}

 // Scroll a pixel region up.
 //  	Area scrolled is defined by x1,y1 through x2,y2 inclusive.
 //   x1,y1 is upper left corder, x2,y2 is lower right corner.
 //
 // 	color is the color to be used for the created space along the
 // 	bottom.
 //
 // 	pixels is the *exact* pixels to scroll. 1 is 1 and 9 is 9 it is
 //   not 1 less or 1 more than what you want. It is *exact*.
void glcd_TextScrollUp(u8 x1, u8 y1, u8 x2, u8 y2, u8 pixels, u8 color) {
	u8 dy;
	u8 dbyte;
	u8 sy;
	u8 sbyte;
	u8 col;

	 //  Scrolling up more than area height?
	if(y1 + pixels > y2) {
		//  fill the region with "whitespace" because
		//  it is being totally scrolled out.
		glcd_SetPixels(x1, y1, x2, y2, color);
		return;
	}

	for(col = x1; col <= x2; col++) {
		dy = y1;
		glcd_GotoXY(col, dy & ~7);
		dbyte = glcd_ReadData();

		//  preserve bits outside/above scroll region
		dbyte &= (_BV((dy & 7)) - 1);

		sy = dy + pixels;
		glcd_GotoXY(col, sy & ~7);
		sbyte = glcd_ReadData();

		while(sy <= y2) {
			if(sbyte & _BV(sy & 7))	dbyte |= _BV(dy & 7);
			sy++;
			if((sy & 7) == 0) {
				 //  If we just crossed over, then we should be done.
				if(sy < glcdDISPLAY_HEIGHT) 	{
					glcd_GotoXY(col, sy & ~7);
					sbyte = glcd_ReadData();
				}
			}

			if((dy & 7) == 7) {
				glcd_GotoXY(col, dy & ~7);	// Should be able to remove this
				glcd_WriteData(dbyte);
				dbyte = 0;
			}
			dy++;
		}


		//  Handle the new area at the bottom of the region
		u8 p;
		for(p = pixels; p; p--) {
			if(color == glcdBLACK)
					dbyte |= _BV(dy & 7);
			else	dbyte &= ~_BV(dy & 7);

			if((dy & 7) == 7) {
				glcd_GotoXY(col, dy & ~7); // should be able to remove this.
				glcd_WriteData(dbyte);
				dbyte = 0;
			}
			dy++;
		}

		 //  Flush out the final destination byte
		if(dy & 7) {
			dy--;

			glcd_GotoXY(col, dy & ~7);
			sbyte = glcd_ReadData();

			 //  Preserver bits outside/below region
			dy++;
			sbyte &= ~(_BV((dy & 7)) - 1);
			dbyte |= sbyte;

			glcd_WriteData(dbyte);
		}
	}
}

#if (glcdSCROLLDOWN == pdON)
void glcd_TextScrollDown(u8 x1, u8 y1, u8 x2, u8 y2, u8 pixels, u8 color) {
	u8 dy;
	u8 dbyte;
	u8 sy;
	u8 sbyte;
	u8 col;

	//  Scrolling up more than area height?
	if(y1 + pixels > y2) {
		//  fill the region with "whitespace" because
		//  it is being totally scrolled out.
		glcd_SetPixels(x1, y1, x2, y2, color);
		return;
	}

	 //  Process region from left to right
	for(col = x1; col <= x2; col++) {
		dy = y2;
		glcd_GotoXY(col, dy & ~7);
		dbyte = glcd_ReadData();

		//  preserve bits outside/below scroll region
		dbyte &= ~(_BV(((dy & 7)+1)) - 1);
		sy = dy - pixels;
		glcd_GotoXY(col, sy & ~7);
		sbyte = glcd_ReadData();

		while(sy >= y1)	{
			if(sbyte & _BV(sy & 7))	dbyte |= _BV(dy & 7);

			if((dy & 7) == 0) {
				glcd_GotoXY(col, dy & ~7);	// Should be able to remove this
				glcd_WriteData(dbyte);
				dbyte = 0;
			}
			dy--;

			if(!sy)	break; // if we bottomed out, we are done
			sy--;
			if((sy & 7) == 7) {
				glcd_GotoXY(col, sy & ~7);
				sbyte = glcd_ReadData();
			}
		}

		//  Handle the new area at the top of the column
		u8 p;
		for(p = pixels; p; p--) {
			if(color == glcdBLACK)
					dbyte |= _BV(dy & 7);
			else	dbyte &= ~_BV(dy & 7);

			if((dy & 7) == 0) {
				glcd_GotoXY(col, dy & ~7); // should be able to remove this.
				glcd_WriteData(dbyte);
				dbyte = 0;
			}
			dy--;
		}

		dy++; // point dy back to last destination row

		//  Flush out the final destination byte
		if(dy & 7) {
			glcd_GotoXY(col, dy & ~7);
			sbyte = glcd_ReadData();

			//  Preserve bits outside/above region
			sbyte &= (_BV((dy & 7)) - 1);
			dbyte |= sbyte;
			glcd_WriteData(dbyte);
		}

	}

}
#endif //#if (glcdSCROLLDOWN == pdON)



 // output a character string at x,y coordinate
 //
 // @param str String class string
 // @param x specifies the horizontal location
 // @param y specifies the vertical location
 //
 //
 // Outputs all the characters in the string to the text area.
 // X & Y are zero based pixel coordinates and are relative to
 // the upper left corner of the text area.
 //
 // See PutChar() for a full description of how characters are
 // written to the text area.
 //
 //
 // @see PutChar()
 // @see Puts()
 // @see Puts_P()
 // @see DrawString_P()
 // @see write()


void glcd_DrawString(char *str, u8 x, u8 y) {
	glcd_TextCursorToXY(x,y);
	glcd_TextPuts(str);
}

// Bill, I think the following would be a useful addition to the API
// Should we add a sanity check to these?





 // Returns the pixel width of a character
 //
 // @param c character to be sized
 //
 // @return The width in pixels of the given character
 // including any inter-character gap pixels following the character when the character is
 // rendered on the display.
 //
 // @note The font for the character is the most recently selected font.
 //
 // @see StringWidth()
 // @see StringWidth_P()
//#include "stdio_uc.h"
u8 glcd_TextCharWidth(u8 c) {
	u8 width = 0;

    if (isFixedWidthFont(glcdText.font) {
		width = glcd_FontRead(glcdText.font+glcdFONT_FIXED_WIDTH)+1;  // there is 1 pixel pad here
	} else {
	    // variable width font
		u8 firstChar = glcd_FontRead(glcdText.font+glcdFONT_FIRST_CHAR);
		u8 charCount = glcd_FontRead(glcdText.font+glcdFONT_CHAR_COUNT);

		// read width data
		if ( (c >= firstChar) && (c < firstChar+charCount) ) {
			c -= firstChar;
			width = glcd_FontRead(glcdText.font+glcdFONT_WIDTH_TABLE+c)+1;
		}

		//lprintf("TEXT: c=%d firstChar=%d charCount=%d width=%d"CMD_TERMINATOR, c, firstChar, charCount, width);
	}
	return width;
}

u8 glcd_TextCharHeight(void) {
 	return glcd_FontRead(glcdText.font+glcdFONT_HEIGHT)+1;  // there is 1 pixel pad here
}


 // Returns the pixel width of a string
 //
 // @param str pointer to string stored in RAM
 //
 // @return the width in pixels of the sum of all the characters in the
 // the string pointed to by str.
 //
 // @see CharWidth()
 // @see StringWidth_P()
u16 glcd_TextStringWidth(const char* str) {
	u16 width = 0;

	while(*str != 0) {
		width += glcd_TextCharWidth(*str++);
	}

	return width;
}


#if defined (CPU_AVR)
//
 // Returns the pixel width of a character
 //
 // @param str pointer to string stored in program memory
 //
 // @return the width in pixels of the sum of all the characters in the
 // the string pointed to by str.
 //
 // @see CharWidth()
 // @see StringWidth()
u16 glcd_TextStringWidth_P(PGM_P str) {
	u16 width = 0;

	while(pgm_read_byte(str) != 0) {
		width += glcd_TextCharWidth(pgm_read_byte(str++));
	}

	return width;
}

#endif


// output a character
//  If the character will not fit on the current text line
//  inside the text area,
//  the text position is wrapped to the next line. This might be
//  the next lower or the next higher line depending on the
//  scroll direction.
//
//  If there is not enough room to fit a full line of new text after
//  wrapping, the entire text area will be scrolled to make room for a new
//  line of text. The scroll direction will be up or down
//  depending on the scroll direction for the text area.
int glcd_TextPutChar(n16 c) {
	if(glcdText.font == 0)  return 0; // no font selected

	// check for special character processing
	if(c < 0x20) {
		glcd_TextSpecialChar(c);
		return 1;
	}

	u8 width = 0;
	u8 height = glcd_FontRead(glcdText.font+glcdFONT_HEIGHT);
	u8 bytes = (height+7)/8;  // calculates height in rounded up bytes

	u8 firstChar = glcd_FontRead(glcdText.font+glcdFONT_FIRST_CHAR);
	u8 charCount = glcd_FontRead(glcdText.font+glcdFONT_CHAR_COUNT);

	u16 index = 0;
	u8 thielefont;

	if(c < firstChar || c >= (firstChar+charCount)) {
		return 0; // invalid char
	}
	c-= firstChar;

	if( isFixedWidthFont(glcdText.font) {
		thielefont = 0;
		width = glcd_FontRead(glcdText.font+glcdFONT_FIXED_WIDTH);
		index = c*bytes*width+glcdFONT_WIDTH_TABLE;
	} else {
		// variable width font, read width data, to get the index
		thielefont = 1;

		// Because there is no table for the offset of where the data
		// for each character glyph starts, run the table and add up all the
		// widths of all the characters prior to the character we
		// need to locate.
		u8 i;
	   	for(i=0; i<c; i++)
			index += glcd_FontRead(glcdText.font+glcdFONT_WIDTH_TABLE+i);

		// Calculate the offset of where the font data
	 	// for our character starts.
		// The index value from above has to be adjusted because
		// there is potentialy more than 1 byte per column in the glyph,
		// when the characgter is taller than 8 bits.
		// To account for this, index has to be multiplied
		// by the height in bytes because there is one byte of font
		// data for each vertical 8 pixels.
		// The index is then adjusted to skip over the font width data
		// and the font header information.
	   	index = index*bytes+charCount+glcdFONT_WIDTH_TABLE;

		// Finally, fetch the width of our character
	   	width = glcd_FontRead(glcdText.font+glcdFONT_WIDTH_TABLE+c);
    }


	#if (glcdDEFERSCROLL == pdON)
	// check for a defered scroll
	// If there is a deferred scroll,
	// Fake a newline to complete it.
	if(glcdText.need_scroll) {
		glcd_TextPutChar('\n'); // fake a newline to cause wrap/scroll
		glcdText.need_scroll = 0;
	}
	#endif


	// If the character won't fit in the text area,
	// fake a newline to get the text area to wrap and
	// scroll if necessary.
	// NOTE/WARNING: the below calculation assumes a 1 pixel pad.
	// This will need to be changed if/when configurable pixel padding is supported.

	if(glcdText.x + width > glcdText.tarea.x2) {
		glcd_TextPutChar('\n'); // fake a newline to cause wrap/scroll
		#if (glcdDEFERSCROLL == pdON)
	 	// We can't defer a scroll at this point since we need to ouput
		// a character right now.

		if(glcdText.need_scroll) {
			glcd_TextPutChar('\n'); // fake a newline to cause wrap/scroll
			glcdText.need_scroll = 0;
		}
		#endif
	}

	// Paint font data bits and write them to LCD memory 1 LCD page at a time.
	//	  This is very different from simply reading 1 byte of font data
	//	  and writing all 8 bits to LCD memory and expecting the write data routine
	//	  to fragement the 8 bits across LCD 2 memory pages when necessary.
	//	  That method (really doesn't work) and reads and writes the same LCD page
	//	 more than once as well as not do sequential writes to memory.
	//
	//	  This method of rendering while much more complicated, somewhat scrambles the font
	//	  data reads to ensure that all writes to LCD pages are always sequential and a given LCD
	//	  memory page is never read or written more than once.
	//	  And reads of LCD pages are only done at the top or bottom of the font data rendering
	//	  when necessary.
	//	  i.e it ensures the absolute minimum number of LCD page accesses
	//	  as well as does the sequential writes as much as possible.

	u8 pixels = height +1;  // 1 for gap below character
	u8 p;
	u8 dy;
	u8 tfp;
	u8 dp;
	u8 dbyte;
	u8 fdata;

	for(p = 0; p < pixels;) {
		dy = glcdText.y + p;
		// Align to proper Column and page in LCD memory
		glcd_GotoXY(glcdText.x, (dy & ~7));

		u16 page = p/8 * width; // page must be 16 bit to prevent overflow
		u8 j;
		for(j=0; j<width; j++) { // each column of font data
			// Fetch proper byte of font data.
			//	Note:
			//	This code "cheats" to add the horizontal space/pixel row below the font.
			//	It essentially creates a font pixel of 0 when the pixels are
			//	out of the defined pixel map.
			//
			//	fake a fondata read read when we are on the very last
			//	bottom "pixel". This lets the loop logic continue to run
			//	with the extra fake pixel. If the loop is not the
			//	the last pixel the pixel will come from the actual
			//	font data, but that is ok as it is 0 padded.
			if(p >= height) {
				// fake a font data read for padding below character.
				fdata = 0;
			} else {
				fdata = glcd_FontRead(glcdText.font+index+page+j);
				//  Have to shift font data because Thiele shifted residual
				//  font bits the wrong direction for LCD memory.
				//  The real solution to this is to fix the variable width font format to
				//  not shift the residual bits the wrong direction!!!!

				if(thielefont && (height - (p&~7)) < 8) {
					fdata >>= 8 - (height & 7);
				}
			}

			if(glcdText.fontColor == glcdWHITE)
				fdata ^= 0xff;	 // inverted data for "white" font color

			//Check to see if a quick full byte write of font data can be done.
			if(!(dy & 7) && !(p & 7) && ((pixels -p) >= 8)) {
				//  destination pixel is on a page boundary
				//  Font data is on byte boundary
				//  And there are 8 or more pixels left
				//  to paint so a full byte write can be done.
				glcd_WriteData(fdata);
				continue;
			} else {
				// No, so must fetch byte from LCD memory.
				dbyte = glcd_ReadData();
			}

			// At this point there is either not a full page of data
			// left to be painted  or the font data spans multiple font
			// data bytes. (or both) So, the font data bits will be painted
			// into a byte and then written to the LCD memory.page.
			tfp = p;		 // font pixel bit position
			dp = dy & 7;	 // data byte pixel bit position

		  	// paint bits until we hit bottom of page/byte or run out of pixels to paint.
			while((dp <= 7) && (tfp) < pixels) {
				if(fdata & _BV(tfp & 7))
						dbyte |= _BV(dp);
				else	dbyte &= ~_BV(dp);

				// Check for crossing font data bytes
				if((tfp & 7)== 7){
					fdata = glcd_FontRead(glcdText.font+index+page+j+width);
					// Have to shift font data because Thiele shifted residual
					// font bits the wrong direction for LCD memory.
					if((thielefont) && ((height - tfp) < 8))
						fdata >>= (8 - (height & 7));
					if(glcdText.fontColor == glcdWHITE)
						fdata ^= 0xff;	// inverted data for "white" color
				}
				tfp++;
				dp++;
			}
			// Now flush out the painted byte.
			glcd_WriteData(dbyte);
		}


		//		  now create a horizontal gap (vertical line of pixels) between characters.
		//		  Since this gap is "white space", the pixels painted are oposite of the
		//		  font color.

		//		  Since full LCD pages are being written, there are 4 combinations of filling
		//		  in the this gap page.
		//		 	- pixels start at bit 0 and go down less than 8 bits
		//		 	- pixels don't start at 0 but go down through bit 7
		//		 	- pixels don't start at 0 and don't go down through bit 7 (fonts shorter than 6 hi)
		//		 	- pixels start at bit 0 and go down through bit 7 (full byte)

		//		  The code below creates a mask of the bits that should not be painted.
		//
		//		  Then it is easy to paint the desired bits since if the color is WHITE,
		//		  the paint bits are set, and if the coloer is not WHITE the paint bits are stripped.
		//		  and the paint bits are the inverse of the desired bits mask.
		if((dy & 7) || (pixels - p < 8)) {
			u8 mask = 0;
			dbyte = glcd_ReadData();

			if (dy & 7) mask |= _BV(dy & 7) -1;
			if ((pixels-p) < 8)	mask |= ~(_BV(pixels - p) -1);
			if (glcdText.fontColor == glcdWHITE)
					dbyte |= ~mask;
			else 	dbyte &= mask;
		} else {
			if(glcdText.fontColor == glcdWHITE)
					dbyte = 0xff;
			else	dbyte = 0;
		}

		glcd_WriteData(dbyte);

		// advance the font pixel for the pixels
		// just painted.
		p += 8 - (dy & 7);
	}

	// Since this rendering code always starts off with a GotoXY() it really isn't necessary
	// to do a real GotoXY() to set the h/w location after rendering a character.
	// We can get away with only setting the s/w version of X & Y.
	//
	// Since y didn't change while rendering, it is still correct.
	// But update x for the pixels rendered.


	glcdText.x = glcdText.x+width+1;

	return 1; // valid char
}

// Handle all special processing characters
static void glcd_TextSpecialChar(u8 c) {
	if (c == '\n') {
		u8 height = glcd_FontRead(glcdText.font+glcdFONT_HEIGHT);
		//	Erase all pixels remaining to edge of text area.on all wraps
		//	It looks better when using inverted (WHITE) text, on proportional fonts, and
		//		  doing WHITE scroll fills.
		if (glcdText.x < glcdText.tarea.x2)
			glcd_SetPixels(glcdText.x, glcdText.y, glcdText.tarea.x2, glcdText.y+height, glcdText.fontColor == glcdBLACK ? glcdWHITE : glcdBLACK);


		 // Check for scroll up vs scroll down (scrollup is normal)

		#if (glcdSCROLLDOWN == pdON)
		if(glcdText.tarea.mode == glcdSCROLL_UP)
		#endif
		{
			 // Normal/up scroll
			 // Note this comparison and the pixel calcuation below takes into
			 // consideration that fonts
			 // are atually 1 pixel taller when rendered.
			 // This extra pixel is along the bottom for a "gap" between the character below.

			if(glcdText.y + 2*height >= glcdText.tarea.y2) {
				#if (glcdDEFERSCROLL == pdON)
				if(!glcdText.need_scroll) {
					glcdText.need_scroll = 1;
					return;
				}
				#endif


				// forumula for pixels to scroll is:
				//	(assumes "height" is one less than rendered height)
				//		pixels = height - ((glcdText.tarea.y2 - glcdText.y)  - height) +1;
				//
				//		The forumala below is unchanged
				//		But has been re-written/simplified in hopes of better code

				u8 pixels = 2*height + glcdText.y - glcdText.tarea.y2 +1;


			 	// Scroll everything to make room
				// * NOTE: (FIXME, slight "bug")
				// When less than the full character height of pixels is scrolled,
				// There can be an issue with the newly created empty line.
				// This is because only the # of pixels scrolled will be colored.
				// What it means is that if the area starts off as white and the text
				// color is also white, the newly created empty text line after a scroll
				// operation will not be colored BLACK for the full height of the character.
				// The only way to fix this would be alter the code use a "move pixels"
				// rather than a scroll pixels, and then do a clear to end line immediately
				// after the move and wrap.
				//
				// Currently this only shows up when
				// there are are less than 2xheight pixels below the current Y coordinate to
				// the bottom of the text area
				// and the current background of the pixels below the current text line
				// matches the text color
				// and  a wrap was just completed.
				//
				// After a full row of text is printed, the issue will resolve itself.

				glcd_TextScrollUp(glcdText.tarea.x1, glcdText.tarea.y1,
					glcdText.tarea.x2, glcdText.tarea.y2, pixels, glcdText.fontColor == glcdBLACK?glcdWHITE:glcdBLACK);

				glcdText.x = glcdText.tarea.x1;
				glcdText.y = glcdText.tarea.y2 - height;
			} else {
				// Room for simple wrap
				glcdText.x = glcdText.tarea.x1;
				glcdText.y = glcdText.y+height+1;
			}
		}
		#if (glcdSCROLLDOWN == pdON)
		else {
			// Reverse/Down scroll
			// Check for Wrap vs scroll.
			//
			// Note this comparison and the pixel calcuation below takes into
			// consideration that fonts
			// are atually 1 pixel taller when rendered.

			if(glcdText.y > glcdText.tarea.y1 + height) {
		 		// There is room so just do a simple wrap
				glcdText.x = glcdText.tarea.x1;
				glcdText.y = glcdText.y - (height+1);
			} else {
				#if (glcdDEFERSCROLL == pdON)
				if(!glcdText.need_scroll) {
					glcdText.need_scroll = 1;
					return;
				}
				#endif
				// Scroll down everything to make room for new line
				//	(assumes "height" is one less than rendered height)
				u8 pixels = height+1 - (glcdText.tarea.y1 - glcdText.y);

				glcd_TextScrollDown(glcdText.tarea.x1, glcdText.tarea.y1,
					glcdText.tarea.x2, glcdText.tarea.y2, pixels, glcdText.fontColor == glcdBLACK?glcdWHITE:glcdBLACK);

				glcdText.x = glcdText.tarea.x1;
				glcdText.y = glcdText.tarea.y1;
			}
		}
	#endif
	}
}

// Define a text area by absolute coordinates
 //
 // @param x1 X coordinate of upper left corner
 // @param y1 Y coordinate of upper left corner
 // @param x2 X coordinate of lower right corner
 // @param y2 Y coordinate of lower right corner
 // @param mode constants glcdSCROLL_DOWN and glcdSCROLL_UP control scroll direction
 //
 // Defines a text area based on absolute coordinates.
 // The pixel coordinates for the text area are inclusive so x2,y2 is the lower right
 // pixel of the text area.
 //
 // x1,y1 and x2,y2 are an absolute coordinates and are relateive to the 0,0 origin of the
 // display.
 //
 // The area within the newly defined text area is intentionally not cleared.
 //
 // mode is an optional parameter and defaults to normal/up scrolling
 //
 // @returns true with the given area selected if all the coordinates are valid,
 // otherwise returns returns false with the area set to the full display
 //
 // @note
 // Upon creation of the text area, the cursor position for the text area will be set to x1, y1
 //
 // @see ClearArea()

static u8 glcd_TextDefineArea(u8 x1, u8 y1, u8 x2, u8 y2, tTextMode mode) {
	u8 ret = pdFALSE;
	if(		(x1 >= x2)
		||	(y1 >= y2)
		||	(x1 >= glcdDISPLAY_WIDTH)
		||	(y1 >= glcdDISPLAY_HEIGHT)
		||	(x2 >= glcdDISPLAY_WIDTH)
		||	(y2 >= glcdDISPLAY_WIDTH)
	) {
	    // failed sanity check so set defaults and return false
		glcdText.tarea.x1 = 0;
		glcdText.tarea.y1 = 0;
		glcdText.tarea.x2 = glcdDISPLAY_WIDTH -1;
		glcdText.tarea.y2 = glcdDISPLAY_HEIGHT -1;
		glcdText.tarea.mode = glcdDEFAULT_SCROLLDIR;
	} else {
	    glcdText.tarea.x1 = x1;
	    glcdText.tarea.y1 = y1;
		glcdText.tarea.x2 = x2;
	    glcdText.tarea.y2 = y2;
		glcdText.tarea.mode = mode; // not yet sanity checked
		ret = pdTRUE;
	}

	// set cursor position for the area
	glcdText.x = x1;
	glcdText.y = y1;

	#if (glcdDEFERSCROLL == pdON)
	// Make sure to clear a deferred scroll operation when re defining areas.
	glcdText.need_scroll = 0;
	#endif
    return ret;
}

/**
 * output a character string
 *
 * @param str pointer to a null terminated character string.
 *
 * Outputs all the characters in the string to the text area.
 * See PutChar() for a full description of how characters are
 * written to the text area.
 *
 * @see PutChar()
 * @see Puts_P()
 * @see DrawString()
 * @see DrawString_P()
 * @see write()
 */
static void glcd_TextPuts(char *str) {
    while(*str) {
        glcd_TextPutChar((n16)*str);
        str++;
    }
}

