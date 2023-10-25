#include "glcd_primitives.h"

/**
 * Draw a Vertical Line
 *
 * @param x a value from 0 to GLCD.Width-1
 * @param y a value from 0 to GLCD.Height-1
 * @param height a value from 1 to GLCD.Height-y-1
 * @param color color of line
 *
 * color of glcdBLACK or glcdWHITE is an optional parameter indicating pixel color, default is glcdBLACK
 *
 * The line drawn will be height+1 pixels.
 *
 * @note This function was previously named DrawVertLine() in the ks0108 library
 *
 * @see DrawLine()
 * @see DrawHLine()
 */


void glcd_DrawVLine(u8 x, u8 y, u8 height, u8 color) {
  	// this->FillRect(x, y, 0, length, color);
   	glcd_SetPixels(x,y,x,y+height,color);
}

/**
 * Draw a Horizontal Line
 *
 * @param x a value from 0 to GLCD.Width-1
 * @param y a value from 0 to GLCD.Height-1
 * @param width a value from 1 to GLCD.Width-x-1
 * @param color glcdBLACK or glcdWHITE
 *
 * The line drawn will be width+1 pixels.
 *
 * color is an optional parameter indicating pixel color and defaults to glcdBLACK
 *
 * @note This function was previously named DrawHoriLine() in the ks0108 library
 *
 * @see DrawLine()
 * @see DrawVLine()
 */
void glcd_DrawHLine(u8 x, u8 y, u8 width, u8 color) {
   // this->FillRect(x, y, length, 0, color);
    glcd_SetPixels(x,y, x+width, y, color);
}

/**
 * Draw a rectangle with rounder corners
 *
 * @param x the x coordinate of the upper left corner of the rectangle
 * @param y the y coordinate of the upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param radius ????
 * @param color glcdBLACK or glcdWHITE
 *
 * Draws a rectangle the same as DrawRect() but with rounded corners.
 * Radius is a value from 1 to half the height or width of the rectangle.
 * (what does that really mean?????)
 * Which is it, or is it a the smaller of the two?
 * FIXME FIXME need more description here.
 *
 * @see DrawRect()
 */
void glcd_DrawRoundRect(u8 x, u8 y, u8 width, u8 height, u8 radius, u8 color) {
  	s16 tSwitch;
	u8 x1 = 0, y1 = radius;
  	tSwitch = 3 - 2 * radius;

	while (x1 <= y1) {
	    glcd_SetDot(x+radius - x1, y+radius - y1, color);
	    glcd_SetDot(x+radius - y1, y+radius - x1, color);

	    glcd_SetDot(x+width-radius + x1, y+radius - y1, color);
	    glcd_SetDot(x+width-radius + y1, y+radius - x1, color);

	    glcd_SetDot(x+width-radius + x1, y+height-radius + y1, color);
	    glcd_SetDot(x+width-radius + y1, y+height-radius + x1, color);

	    glcd_SetDot(x+radius - x1, y+height-radius + y1, color);
	    glcd_SetDot(x+radius - y1, y+height-radius + x1, color);

	    if (tSwitch < 0) {
	    	tSwitch += (4 * x1 + 6);
	    } else {
	    	tSwitch += (4 * (x1 - y1) + 10);
	    	y1--;
	    }
	    x1++;
	}

	glcd_DrawHLine(x+radius, y, width-(2*radius), color);			// top
	glcd_DrawHLine(x+radius, y+height, width-(2*radius), color);	// bottom
	glcd_DrawVLine(x, y+radius, height-(2*radius), color);			// left
	glcd_DrawVLine(x+width, y+radius, height-(2*radius), color);	// right
}

/**
 * Draw a Circle
 *
 * @param xCenter X coordinate of the center of the circle
 * @param yCenter Y coordinate of the center of the circle
 * @param radius radius of circle
 * @param color glcdBLACK or glcdWHITE
 *
 * Draws a circle of the given radius extending out from
 * the center pixel.
 * The circle will fit inside a rectanglular area bounded by
 * x-radius,y-radius and x+radius,y+radius
 *
 * Because the circle is drawn from the center pixel out,
 * the diameter will be 2 * radius +1 pixels.
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @see FillCircle()
 */
void glcd_DrawCircle(u8 xCenter, u8 yCenter, u8 radius, u8 color) {
   glcd_DrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);
}

/**
 * Draw a Filled in a Circle
 *
 * @param xCenter X coordinate of the center of the circle
 * @param yCenter Y coordinate of the center of the circle
 * @param radius radius of circle
 * @param color glcdWHITE or glcdBLACK
 *
 * Draws a filled in circle of the given radius extending out from
 * the center pixel.
 *
 * See DrawCircle() for the full details on sizing.
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @see DrawCircle()
 *
 */

void glcd_FillCircle(u8 xCenter, u8 yCenter, u8 radius, u8 color) {
	/*
 	* Circle fill Code is merely a modification of the midpoint
 	* circle algorithem which is an adaption of Bresenham's line algorithm
 	* http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 	* http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 	*
 	* While it looks different it is the same calculation that is in
 	* DrawRoundRect()
 	*
 	* Note: FillCircle()
 	*		could be modified to work like DrawCircle() by using
 	*		a DrawRoundRect() function.
 	* 		DrawRoundRect() first plots the cicrular quadrants and then draws
 	*		the horizontal and verticl lines between them to create
 	*		a rounded rectangle.
 	*
 	*		For fills of rounded rectangles, this type of circle fill algorithm
 	*		would fill the upper and lower quadrants on each side of the rounded
 	*		rectangle by connecting them vertically.
 	*		When that was done, a filled rectangle would be neded need to fill the
 	*		space between them.
 	*		There really isn't an easy way to fill a rounded rectangle.
 	*
 	*		For now, it is limited to circles.
 	*
 	* 			--- bperrybap
 	*/

	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	u8 x = 0;
	u8 y = radius;


	// Fill in the center between the two halves
	glcd_DrawLine(xCenter, yCenter-radius, xCenter, yCenter+radius, color);

	while(x < y) {
    	// ddF_x == 2 * x + 1;
    	// ddF_y == -2 * y;
    	// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;


		 // Now draw vertical lines between the points on the circle rather than
		 // draw the points of the circle. This draws lines between the
		 // perimeter points on the upper and lower quadrants of the 2 halves of the circle.

		glcd_DrawLine(xCenter+x, yCenter+y, xCenter+x, yCenter-y, color);
		glcd_DrawLine(xCenter-x, yCenter+y, xCenter-x, yCenter-y, color);
		glcd_DrawLine(xCenter+y, yCenter+x, y+xCenter, yCenter-x, color);
		glcd_DrawLine(xCenter-y, yCenter+x, xCenter-y, yCenter-x, color);
  	}
}

/*
 * Drawline code is based on Bresenham's line algorithm
 * http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *
 * This routine replaces the Teiele's DrawLine() routine which was
 * much larger and much slower.
 *	-- bperrybap
 */

/*
 * First, define a few macros to make the DrawLine code below read more like
 * the wikipedia example code.
 */


/**
 * Draw a line
 *
 * @param x1 a value from 0 to GLCD.Width-1  indicating start x coordinate
 * @param y1 a value fron 0 to GLCD.Height-1 indicating start y coordinate
 * @param x2 a value from 0 to GLCD.Width-1  indicating end x coordinate
 * @param y2 a value fron 0 to GLCD.Height-1 indicating end y coordinate
 * @param color glcdBLACK or glcdWHITE
 *
 * Draws a line starting at x1,y2 and ending at x2,y2.
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @see DrawHLine()
 * @see DrawVLine()
 *
 */

void glcd_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 color) {
	u8 deltax, deltay, x,y, steep;
	s8 error, ystep;

	#ifdef XXX
	/*
	 * Rather than fudge up coordinates that are outside of range,
	 * It is better to allow the algorithm to run with them "as is"
	 * so that any pixels on the line that land inside the valid display
	 * coordinates will be drawn properly.
	 *
	 * If the coordinates are patched, then the line drawn will not
	 * be anything close to the original slope as the endpoint coordinates
	 * will have been altered.
	 */

	if(x1>=glcdDISPLAY_WIDTH) x1=0;
	if(x2>=glcdDISPLAY_WIDTH) x2=0;
	if(y1>=glcdDISPLAY_HEIGHT) y1=0;
	if(y2>=glcdDISPLAY_HEIGHT) y2=0;
	#endif


	steep = _GLCD_absDiff(y1,y2) > _GLCD_absDiff(x1,x2);

	if ( steep ) {
		_GLCD_swap(x1, y1);
		_GLCD_swap(x2, y2);
	}

	if (x1 > x2) {
		_GLCD_swap(x1, x2);
		_GLCD_swap(y1, y2);
	}

	deltax = x2 - x1;
	deltay =_GLCD_absDiff(y2,y1);
	error = deltax / 2;
	y = y1;
	if(y1 < y2) ystep = 1;  else ystep = -1;

	for(x = x1; x <= x2; x++) {
		if (steep) glcd_SetDot(y,x, color); else glcd_SetDot(x,y, color);
   		error = error - deltay;
		if (error < 0) {
			y = y + ystep;
			error = error + deltax;
    	}
	}
}


/**
 * Draw a rectangle of given width and height
 *
 * @param x the x coordinate of the upper left corner of the rectangle
 * @param y the y coordinate of the upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color glcdBLACK or glcdWHITE
 *
 * Draws a rectangle with the specified width and height.
 * The upper left corner at x,y and the lower right
 * corner at x+width,y+width.
 *
 * The length of the horizontal sides will be width+1 pixels
 * The length of the vertical sides will be height+1 pixels
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @note The width and height parameters work differently than FillRect()
 *
 * @see FillRect()
 * @see InvertRect()
 * @see DrawRoundRect()
 */

void glcd_DrawRect(u8 x, u8 y, u8 width, u8 height, u8 color) {
	glcd_DrawHLine(x, y, width, color);				// top
	glcd_DrawHLine(x, y+height, width, color);		// bottom
	glcd_DrawVLine(x, y, height, color);			    // left
	glcd_DrawVLine(x+width, y, height, color);		// right
}

/**
 * Fill a Rectangle
 *
 * @param x the x coordinate of the upper left corner of the rectangle
 * @param y the y coordinate of the upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color glcdBLACK or glcdWHITE
 *
 * Fills a rectanglular area of the specified width and height.
 *
 * The resulting rectangle covers an area @em width pixels wide by @em height pixels
 * tall starting from the pixel at x,y.
 *
 * The upper left corner at x,y and the lower right corner at x+width-1,y+width-1.
 *
 *
 * The length of the horizontal sides will be width pixels
 * The length of the vertical sides will be height pixels
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @note The width and height parameters work differently than DrawRect()
 *
 *
 * @warning FillRect() behavior has changed from the previous versions of the ks0108 library.
 *	The filled rectangle will be one pixel smaller in width and height than the old version.
 *	This change was to make the functionality consistent with the way
 *	Java and C# create filled rectangles
 *
 * @see DrawRect()
 * @see InvertRect()
 */

void glcd_FillRect(u8 x, u8 y, u8 width, u8 height, u8 color) {
    glcd_SetPixels(x,y,x+width,y+height,color);
}


/**
 * Invert a Rectangular area
 *
 * @param x the x coordinate of the upper left corner of the rectangle
 * @param y the y coordinate of the upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 *
 * Inverts the pixels in the rectanglular area of the specified width and height.
 * glcdBLACK pixels becom glcdWHITE and glcdWHITE pixels become glcdBLACK.
 *
 * See FillRect() for full the full details of the rectangular area.
 *
 * @note The width and height parameters work differently than DrawRect()
 *
 * @warning InvertRect() behavior has changed from the previous versions of the ks0108 library.
 *	The inverted rectangle will be one pixel smaller in width and height than the old version.
 *
 * @see DrawRect()
 * @see FillRect()
 */


void glcd_InvertRect(u8 x, u8 y, u8 width, u8 height) {
	u8 mask, pageOffset, h, i, data, tmpData;
	height++;

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


	 // First do the fractional pages at the top of the region
	glcd_GotoXY(x, y);
	for(i=0; i<=width; i++) {
		data = glcd_ReadData();
		tmpData = ~data;
		data = (tmpData & mask) | (data & ~mask);
		glcd_WriteData(data);
	}

	// Now do the full pages
	while(h+8 <= height) {
		h += 8;
		y += 8;
		glcd_GotoXY(x, y);

		for(i=0; i<=width; i++) {
			data = glcd_ReadData();
			glcd_WriteData(~data);
		}
	}

	// Now do the fractional pages aat the bottom of the region
	if(h < height) {
		mask = ~(0xFF << (height-h));
		glcd_GotoXY(x, y+8);

		for(i=0; i<=width; i++) {
			data = glcd_ReadData();
			tmpData = ~data;
			data = (tmpData & mask) | (data & ~mask);
			glcd_WriteData(data);
		}
	}
}



 // Clear the lcd display
 //
 // @param color glcdBLACK or glcdWHITE
 //
 // Sets all the pixels on the display from 0,0 to GLCD.Width-1,GLCD.Height-1
 // to the specified color.
 //
 // Color is optional and defaults to glcdWHITE.
 //
 // @note
 // If the display is in INVERTED mode, then the color glcdWHITE will paint the screen
 // glcdBLACK and the color glcdBLACK will paint the screen glcdWHITE.
 //
 //


void glcd_ClearScreen(u8 color) {
	glcd_SetPixels(0, 0, glcdDISPLAY_WIDTH-1, glcdDISPLAY_HEIGHT-1, color);
}



 // Draw a glcd bitmap image
 // @param bitmap a ponter to the bitmap data
 // @param x the x coordinate of the upper left corner of the bitmap
 // @param y the y coordinate of the upper left corner of the bitmap
 // @param color glcdBLACK or glcdWHITE
 //
 // Draws a bitmap image with the upper left corner at location x,y
 // The bitmap data is assumed to be in program memory.
 //
 // Color is optional and defaults to glcdBLACK.
void glcd_DrawBitmap(tImageBMP bitmap, u8 x, u8 y, u8 color){
	u8 width, height;
	u8 i, j;

  	#if defined (CPU_AVR)
  	width = ReadPgmData(bitmap++);
  	height = ReadPgmData(bitmap++);
  	#else
  	width = *bitmap++;
  	height = *bitmap++;
  	#endif

	#ifdef BITMAP_FIX 	// temporary ifdef just to show what changes if a new
						// bit rendering routine is written.
	// In the absence of a new/better/proper bitmap rendering routine,
   	// this routine will provide a temporary fix for drawing bitmaps that are
   	// are non multiples of 8 pixels in height and start on non LCD page Y addresses.
   	//
   	// Note: nomally a routine like this should not have knowledge of how
   	//	   how the lower level write routine works. But in this case it does.
   	//
   	//	Currently, the low level WriteData() routine ORs in the pixels when data spans
   	//	a LCD memory page. So.....
   	//
   	// In order to ensure that the bitmap data is written to the pixels *exactly* as it
   	// it defined in the bitmap data, i.e every black pixels is black and every white
   	// pixel is white,...
   	//
   	// If height or y coordinate is not on a page boundary, clear the background first
   	// Techincally, this could be done all the time and it wouldn't hurt, it
   	//	would just suck up a few more cycles.
  	if( (y & 7) || (height & 7))
  		glcd_FillRect(x, y, width, height, glcdWHITE);

	#endif

  	for(j = 0; j < height / 8; j++) {
    	 glcd_GotoXY(x, y + (j*8) );
	 	for(i = 0; i < width; i++) {
			 #if defined (CPU_AVR)
			 u8 displayData = ReadPgmData(bitmap++);
			 #else
			 u8 displayData = *bitmap++;
			 #endif
	   	 	if(color == glcdBLACK)
				glcd_WriteData(displayData);
		 	else
		    	glcd_WriteData(~displayData);
	 	}
  	}
}


/*

#define BITMAP_FIX // enables a bitmap rendering fix/patch


  * Set LCD Display mode
 *
 * @param invert Inverted mode
 *
 * Sets the graphical state mode for the entire LCD display
 * to @b NON_INVERTED (glcdBLACK colorerd pixeld are dark)
 * or @b INVERTED (glcdWHITE colored pixels are dark)

void glcd::SetDisplayMode(u8 invert) {  // was named SetInverted

	if(this->Inverted != invert) {
		this->InvertRect(0,0,glcdDISPLAY_WIDTH-1,glcdDISPLAY_HEIGHT-1);
		this->Inverted = invert;
	}
}

#ifdef NOTYET

 * Draw a glcd bitmap image in x11 XBM bitmap data format
 *
 * @param bitmapxbm a ponter to the glcd XBM bitmap data
 * @param x the x coordinate of the upper left corner of the bitmap
 * @param y the y coordinate of the upper left corner of the bitmap
 * @param color glcdBLACK or glcdWHITE
 *
 * Draws a x11 XBM bitmap image with the upper left corner at location x,y
 * The glcd xbm bitmap data format consists of 1 byte of width followed by 1 byte of height followed
 * by the x11 xbm pixel data bytes.
 * The bitmap data is assumed to be in program memory.
 *
 * Color is optional and defaults to glcdBLACK.
 *
 * @see DrawBitmapXBM_P()
 * @see DrawBitmap()

void glcd::DrawBitmapXBM(tImageXBM bitmapxbm, u8 x, u8 y, u8 color)
{
u8 width, height;
u8 bg_color;
u8 *xbmbits;


	xbmbits = (u8 *) bitmapxbm;

	#if defined (CPU_AVR)
	width = ReadPgmData(xbmbits++);
	height = ReadPgmData(xbmbits++);
	#else
  	width = *bitmap++;
  	height = *bitmap++;
	#endif

	if(color == glcdBLACK)
		bg_color = glcdWHITE;
	else
		bg_color = glcdBLACK;
	DrawBitmapXBM_P(width, height, xbmbits, x, y, color, bg_color);
}

 * Draw a x11 XBM bitmap image
 *
 * @param width pixel width of the image
 * @param height pixel height of the image
 * @param xbmbits a ponter to the XBM bitmap pixel data
 * @param x the x coordinate of the upper left corner of the bitmap
 * @param y the y coordinate of the upper left corner of the bitmap
 * @param fg_color foreground color
 * @param bg_color background color
 *
 * Draws a x11 XBM bitmap image with the upper left corner at location x,y
 * The xbm bitmap pixel data format is the same as the X11 bitmap pixel data.
 * The bitmap data is assumed to be in program memory.
 *
 * @note All parameters are mandatory
 *
 * @see DrawBitmapXBM_P()
 * @see DrawBitmap()


void glcd::DrawBitmapXBM_P(u8 width, u8 height, u8 *xbmbits,
			u8 x, u8 y, u8 fg_color, u8 bg_color)
{
u8 xbmx, xbmy;
u8 xbmdata;

	 // Traverse through the XBM data byte by byte and plot pixel by pixel
	for(xbmy = 0; xbmy < height; xbmy++)
	{
		for(xbmx = 0; xbmx < width; xbmx++)
		{
			if(!(xbmx & 7))	// read the flash data only once per byte
				#if defined (CPU_AVR)
				xbmdata = ReadPgmData(xbmbits++);
				#else
				#endif

			if(xbmdata & _BV((xbmx & 7)))
				this->SetDot(x+xbmx, y+xbmy, fg_color); // XBM 1 bits are fg color
			else
				this->SetDot(x+xbmx, y+xbmy, bg_color); // XBM 0 bits are bg color
		}
	}

}

#endif // NOTYET

// the following inline functions were added 2 Dec 2009 to replace macros

//
// Font Functions
//


 // Below here are text wrapper functions
// override GotoXY to also call CursorToxy for backward compability
// with older v2 ks0108 library
// (older library didn't have seperate x & y for hardware/graphics vs text )
void glcd::GotoXY(u8 x, u8 y)
{
	glcd_Device::GotoXY(x, y);
  	CursorToXY(x,y);
}

// Make one instance for the user
glcd GLCD = glcd();

 */
