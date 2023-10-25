#ifndef __CONFIG_LIB_GRAPHICS_H
#define __CONFIG_LIB_GRAPHICS_H

#include "_config_cpu_.h"

#define glcdREAD_CACHE pdON
				// Turns on code that uses a frame buffer for a read cache
				// This adds only ~52 bytes of code but...
				// will use DISPLAY_HEIGHT/8 * DISPLAY_WIDTH bytes of RAM
				// A typical 128x64 ks0108 will use 1k of RAM for this.
				// performance increase is quite noticeable (double or so on FPS test)
				// This will not work on smaller AVRs like the mega168 that only have 1k of RAM total.

#define glcdTRUE_WRITE pdOFF
				// does writes to glcd memory on page crossings vs ORs
				// This option only affects writes that span LCD pages.
				// None of the graphic rouintes nor the NEW_FONTDRAW rendering option do this.
				// Only the old font rendering and bitmap rendering do unaligned PAGE writes.
				// While this fixes a few issus for the old routines,
				// it also creates new ones.
				// The issue is routines like the bitmap rendering
				// routine attempt to use a drawing method that does not work.
				// when this is on, pixels are no longer ORd in but are written in.
				// so all the good/desired pixels get set, but then so do some
				// undesired pixels.
				// current RECOMMENDED setting: OFF

#define glcdXCOL_SUPPORT pdON
				// turns on code to track the hardware X/column to minimize set column commands.
				// Believe it or not, the code on the ks0108s runs slower with this enabled.


#define glcdSCROLLDOWN pdON
#define glcdDEFERSCROLL pdON

#endif
