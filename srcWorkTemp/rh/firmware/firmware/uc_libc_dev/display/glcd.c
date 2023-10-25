#include "glcd.h"

static tGlcd glcd;


#if defined (CPU_AVR)
static u8 ReadPgmData(const u8* ptr) {  // note this is a static function
static u8 ReadPgmData(const u8* ptr) {  // note this is a static function
	return pgm_read_byte(ptr);
}
#endif


#if (glcdREAD_CACHE == pdON)
 // Declare a static buffer for the Frame buffer for the Read Cache
static u8 glcd_rdcache[glcdDISPLAY_HEIGHT/8][glcdDISPLAY_WIDTH];
#else
static u8 glcd_DoReadData(void);
#endif

//  Low level h/w initialization of display and AVR pins
//
//  @param invert specifices whether display is in normal mode or inverted mode.
//
//  This should only be called by other library code.
//
//  It does all the low level hardware initalization of the display device.
//
//  The optional invert parameter specifies if the display should be run in a normal
//  mode, dark pixels on light background or inverted, light pixels on a dark background.
//
//  To specify dark pixels use the define @b NON-INVERTED and to use light pixels use
//  the define @b INVERTED
//
//  @returns 0 when successful or non zero error code when unsucessful
//
//  Upon successful completion of the initialization, the entire display will be cleared
//  and the x,y postion will be set to 0,0
//
//  @note
//  This function can be called more than once
//  to re-initliaze the hardware.
//
void glcd_Init(u8 invert) {
	#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
	glcd_ConfigPorts();
	#endif

	glcd.Coord.x = -1;  // invalidate the s/w coordinates so the first GotoXY() works
	glcd.Coord.y = -1;  // invalidate the s/w coordinates so the first GotoXY() works
	glcd.Inverted = invert;


	//  Reset the glcd module if there is a reset pin defined
	//lcdReset();
	//lcdDelayMilliseconds(2);
	//lcdUnReset();


	 // extra blind delay for slow rising external reset signals
	 // and to give time for glcd to get up and running
	delay_us(glcdDELAY_RST);

	//#if defined(GLCD_TEENSY_PCB_RESET_WAIT) && defined(CORE_TEENSY)
	// Delay for Teensy ks0108 PCB adapter reset signal
	// Reset polling is not realiable by itself so this is easier and much less code
	// see long comment above where GLCD_POLL_RESET is defined
	//delay_us(250);
	//#endif

	 // Each chip on the module must be initliazed
	u8 chip;
	for(chip=0; chip < glcd_CHIP_COUNT; chip++) {
		//u8 status;

		//#ifndef GLCD_NOINIT_CHECKS
		 // At this point RESET better be complete and the glcd better have
		 // cleared BUSY status for the chip and be ready to go.
		 // So we check them and if the GLCD chip is not ready to go, we fail the init.

		//status = this->GetStatus(chip);
		//if(lcdIsResetStatus(status))
		//return(GLCD_ERESET);
		//if(lcdIsBusyStatus(status))
			//return(GLCD_EBUSY);
		//#endif

		 // flush out internal state to force first GotoXY() to talk to GLCD hardware

		glcd.Coord.chip[chip].page = -1;
		#if (glcdXCOL_SUPPORT == pdON)
		glcd.Coord.chip[chip].col = -1;
		#endif

		//#ifdef glcd_DeviceInit // this provides custom chip specific init
		//status = glcd_DeviceInit(chip);	// call device specific initialization if defined
		//if(status) return(status);
		//#else
		glcd_WriteCommand(glcdON, chip);			// display on
		glcd_WriteCommand(glcdDISP_START, chip);	// display start line = 0
		//#endif
	}


	//	  All hardware initialization is complete.
	//
	//	  Now, clear the screen and home the cursor to ensure that the display always starts
	//	  in an identical state after being initialized.
	//
	//	  Note: the reason that SetPixels() below always uses glcdWHITE, is that once the
	//	  the invert flag is in place, the lower level read/write code will invert data
	//	  as needed.
	//	  So clearing an areas to glcdWHITE when the mode is INVERTED will set the area to glcdBLACK
	//	  as is required.
	//

	glcd_SetPixels(0, 0, glcdDISPLAY_WIDTH-1, glcdDISPLAY_HEIGHT-1, glcdWHITE);
	glcd_GotoXY(0,0);

	//return(GLCD_ENOERR);
}


//  set an area of pixels
//
//  @param x X coordinate of upper left corner
//  @param y Y coordinate of upper left corner
//  @param x2 X coordinate of lower right corner
//  @param y2 Y coordinate of lower right corner
//  @param color
//
//  sets the pixels an area bounded by x,y to x2,y2 inclusive
//  to the specified color.
//
//  The width of the area is x2-x + 1.
//  The height of the area is y2-y+1

// set pixels from upper left edge x,y to lower right edge x1,y1 to the given color
// the width of the region is x1-x + 1, height is y1-y+1
void glcd_SetPixels(u8 x, u8 y, u8 x2, u8 y2, u8 color) {
	u8 mask, pageOffset, h, i, data;
	u8 height = y2-y+1;
	u8 width = x2-x+1;

	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;

	glcd_GotoXY(x, y);
	for(i=0; i < width; i++) {
		data = glcd_ReadData();

		if(color == glcdBLACK) {
			data |= mask;
		} else {
			data &= ~mask;
		}

		glcd_WriteData(data);
	}

	while(h+8 <= height) {
		h += 8;
		y += 8;
		glcd_GotoXY(x, y);

		for(i=0; i <width; i++) glcd_WriteData(color);
	}

	if (h < height) {
		mask = ~(0xFF << (height-h));
		glcd_GotoXY(x, y+8);

		for(i=0; i < width; i++) {
			data = glcd_ReadData();

			if(color == glcdBLACK) {
				data |= mask;
			} else {
				data &= ~mask;
			}

			glcd_WriteData(data);
		}
	}
}


//  set current x,y coordinate on display device
//
//  @param x X coordinate
//  @param y Y coordinate
//
//  Sets the current pixel location to x,y.
//  x and y are relative to the 0,0 origin of the display which
//  is the upper left most pixel on the display.
void glcd_GotoXY(u8 x, u8 y) {
 	u8 chip, cmd;

 	if( (x == glcd.Coord.x) && (y == glcd.Coord.y) ) return;
  	if( (x > glcdDISPLAY_WIDTH-1) || (y > glcdDISPLAY_HEIGHT-1) ) return;	// exit if coordinates are not legal

 	glcd.Coord.x = x;								// save new coordinates
  	glcd.Coord.y = y;
  	chip = glcd_DevXYval2Chip(x, y);

	if(y/8 != glcd.Coord.chip[chip].page) {
  		glcd.Coord.chip[chip].page = y/8;
		cmd = glcdSET_PAGE | glcd.Coord.chip[chip].page;
	   	glcd_WriteCommand(cmd, chip);
	}

	//	 * NOTE: For now, the "if" below is intentionally commented out.
	//	 * In order for this to work, the code must properly track
	//	 * the x coordinate of the chips and not allow it go beyond proper
	//	 * boundaries. It isnn't complicated to do, it jsut isn't done that
	//	 * way right now.

	x = glcd_DevXval2ChipCol(x);

	#if (glcdXCOL_SUPPORT == pdON)
	if(x != glcd.Coord.chip[chip].col)
	glcd.Coord.chip[chip].col = x;
	#endif

	#ifdef LCD_SET_ADDLO
	cmd = LCD_SET_ADDLO | glcd_DevCol2addrlo(x);
   	this->WriteCommand(cmd, chip);
	cmd = LCD_SET_ADDHI | glcd_DevCol2addrhi(x);
   	this->WriteCommand(cmd, chip);
	#else
	cmd = glcdSET_ADD | x;
   	glcd_WriteCommand(cmd, chip);
	#endif
}

//  read a data byte from display device memory
//
//  @return the data byte at the current x,y position
//
//  @note the current x,y location is not modified by the routine.
// 	This allows a read/modify/write operation.
// 	Code can call ReadData() modify the data then
//   call WriteData() and update the same location.
//
//  @see WriteData()
#if (glcdREAD_CACHE == pdON)
u8 glcd_ReadData(void) {
	u8 x, data;
	x = glcd.Coord.x;
	if(x >= glcdDISPLAY_WIDTH) return(0);

	data = glcd_rdcache[glcd.Coord.y/8][x];

	if (glcd.Inverted) 	data = ~data;

	return(data);
}
#else
u8 glcd_ReadData(void) {
	u8 x, data;
	x = glcd.Coord.x;
	if(x >= glcdDISPLAY_WIDTH) return(0);

	glcd_DoReadData();				// dummy read TODO ????

	data = glcd_DoReadData();			// "real" read

	if(glcd.Inverted) data = ~data;

	glcd.Coord.x = -1;	// force a set column on GotoXY
	glcd_GotoXY(x, glcd.Coord.y);

	return(data);
}

 // read a single data byte from chip
static u8 glcd_DoReadData(void) {
	u8 data, chip;

	chip = glcd_DevXYval2Chip(glcd.Coord.x, glcd.Coord.y);

	glcd_WaitReady(chip);
	data = glcd_ReadRAM(chip);

	#if (glcdXCOL_SUPPORT == pdON)
	glcd.Coord.chip[chip].col++;
	#endif
	return data;
}
#endif


/**
 * Write a byte to display device memory
 *
 * @param data date byte to write to memory
 *
 * The data specified is written to glcd memory at the current
 * x,y position. If the y location is not on a byte boundary, the write
 * is fragemented up into multiple writes.
 *
 * @note the full behavior of this during split byte writes
 * currently varies depending on a compile time define.
 * The code can be configured to either OR in 1 data bits or set all
 * the data bits.
 * @b glcdTRUE_WRITE controls this behavior.
 *
 * @note the x,y address will not be the same as it was prior to this call.
 * 	The y address will remain the aame but the x address will advance by one.
 *	This allows back to writes to write sequentially through memory without having
 *	to do additional x,y positioning.
 *
 * @see ReadData()
 *
 */

void glcd_WriteData(u8 data) {
	u8 displayData, yOffset, chip;

	//	#ifdef GLCD_DEBUG
	//	volatile uint16_t i;
	//	for(i=0; i<5000; i++);
	//	#endif

	if(glcd.Coord.x >= glcdDISPLAY_WIDTH) return;
    chip = glcd_DevXYval2Chip(glcd.Coord.x, glcd.Coord.y);

	yOffset = glcd.Coord.y%8;

	if (yOffset != 0) {
		// first page
		displayData = glcd_ReadData();
		#if (glcdTRUE_WRITE == pdON)
		// Strip out bits we need to update.
		displayData &= (_BV(yOffset)-1);
		#endif
		displayData |= data << yOffset;
		if (glcd.Inverted) displayData = ~displayData;

		glcd_WaitReady(chip);
		glcd_WriteRAM(displayData, chip);
		#if (glcdREAD_CACHE == pdON)
		glcd_rdcache[glcd.Coord.y/8][glcd.Coord.x] = displayData; // save to read cache
		#endif

		// second page
	 	// Make sure to goto y address of start of next page
	 	// and ensure that we don't fall off the bottom of the display.
		u8 ysave = glcd.Coord.y;

		if(((ysave+8) & ~7) >= glcdDISPLAY_HEIGHT) {
			glcd_GotoXY(glcd.Coord.x+1, ysave);
			return;
		}

		glcd_GotoXY(glcd.Coord.x, ((ysave+8) & ~7));

		displayData = glcd_ReadData();
		#if (glcdTRUE_WRITE == pdON)
		// Strip out bits we need to update.
		displayData &= ~(_BV(yOffset)-1);
		#endif
		displayData |= data >> (8-yOffset);
		if (glcd.Inverted) displayData = ~displayData;

		glcd_WaitReady(chip);
		glcd_WriteRAM(displayData, chip);
		#if (glcdREAD_CACHE == pdON)
		glcd_rdcache[glcd.Coord.y/8][glcd.Coord.x] = displayData; // save to read cache
		#endif
		glcd_GotoXY(glcd.Coord.x+1, ysave);
	} else  {
		// just this code gets executed if the write is on a single page
		if(glcd.Inverted) data = ~data;

		glcd_WaitReady(chip);
		glcd_WriteRAM(data, chip);
		#if (glcdREAD_CACHE == pdON)
		glcd_rdcache[glcd.Coord.y/8][glcd.Coord.x] = data; // save to read cache
		#endif

		// NOTE/WARNING:
		// This bump can cause the s/w X coordinate to bump beyond a legal value
		// for the display. This is allowed because after writing to the display
		// display, the column (x coordinate) is always bumped. However,
		// when writing to the the very last column, the resulting column location
		// inside the hardware is somewhat undefined.
		// Some chips roll it back to 0, some stop the maximu of the LCD, and others
		// advance further as the chip supports more pixels than the LCD shows.

		// So to ensure that the s/w is never indicating a column (x value) that is
		// incorrect, we allow it bump beyond the end.

		// Future read/writes will not attempt to talk to the chip until this
		// condition is remedied (by a GotoXY()) and by having this somewhat
		// "invalid" value, it also ensures that the next GotoXY() will always send
		// both a set column and set page address to reposition the glcd hardware.


		glcd.Coord.x++;
		#if (glcdXCOL_SUPPORT == pdON)
		glcd.Coord.chip[chip].col++;
		#endif

	 	// Check for crossing into the next chip.
		if( glcd_DevXYval2Chip(glcd.Coord.x, glcd.Coord.y) != chip) {
			if(glcd.Coord.x < glcdDISPLAY_WIDTH) {
				u8 x = glcd.Coord.x;
				glcd.Coord.x = -1;
				glcd_GotoXY(x, glcd.Coord.y);
			}
 		}
	}
}



//  set pixel at x,y to the given color
//
//  @param x X coordinate, a value from 0 to GLCD.Width-1
//  @param y Y coordinate, a value from 0 to GLCD.Heigh-1
//  @param color glcdWHITE or glcdBLACK
//
//  Sets the pixel at location x,y to the specified color.
//  x and y are relative to the 0,0 origin of the display which
//  is the upper left corner.
//  Requests to set pixels outside the range of the display will be ignored.
//
//  @note If the display has been set to INVERTED mode then the colors
//  will be automically reversed.
void glcd_SetDot(u8 x, u8 y, u8 color) {
	u8 data;

	if ( (x >= glcdDISPLAY_WIDTH) || (y >= glcdDISPLAY_HEIGHT)) return;

	glcd_GotoXY(x, y-y%8);					// read data from display memory
	data = glcd_ReadData();
	if(color == glcdBLACK) 	data |= 0x01 << (y%8);				// set dot
	else  data &= ~(0x01 << (y%8));			// clear dot

	glcd_WriteData(data);					// write data back to display
}
