#ifndef __CONFIG_EMWIN_H
#define __CONFIG_EMWIN_H

#include "_config_lib_.h"

// Define the available number of bytes available for the GUI
#define GUI_NUMBYTES  (1024 * 1024) * 12  // x MByte

// Physical display size
#define XSIZE_PHYS 800
#define YSIZE_PHYS 600

// COLOR CONVERSION
// 	Para mais tipos de cores ver LCD.h do emWin
#define COLOR_CONVERSION GUICC_565

// Display driver
//	para mais tipos de drivers ver GUIDVR_Lin.h
#define DISPLAY_DRIVER GUIDRV_LIN_16

// Buffers / VScreens
#define NUM_BUFFERS  1 // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

// Multi layer/display support
#define GUI_NUM_LAYERS       (16)    // Maximum number of available layers

// Multi tasking support
#define GUI_OS               (0)  // Compile with multitasking support

// Configuration of touch support
#define GUI_SUPPORT_TOUCH    (0)  // Support a touch screen (req. win-manager)

// Default font
#define GUI_DEFAULT_FONT     &GUI_Font6x8

// Support a mouse
#define GUI_SUPPORT_MOUSE    0

// Use window manager
#define GUI_WINSUPPORT       1

// Memory device package available
#define GUI_SUPPORT_MEMDEV   1

// Enable use of device pointers
#define GUI_SUPPORT_DEVICES  1

#define GUI_MEMCPY(dst,src,size)   emWin_memcpy(dst,src,size)
extern void *emWin_memcpy(void *pDst, const void *pSrc, long size);

#endif
