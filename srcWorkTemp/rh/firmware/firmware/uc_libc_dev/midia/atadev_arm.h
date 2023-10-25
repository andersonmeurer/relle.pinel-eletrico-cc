/*! \file atadev.h \brief IDE-ATA interface device driver. */
//*****************************************************************************
//
// File Name	: 'atadev.h'
// Title		: IDE-ATA interface device driver
// Author		: Pascal Stang
// Date			: 8/15/2004
// Revised		: 8/15/2004
// Version		: 0.1
// Target MCU	: ARMmini LPC210x
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef ATADEV_H
#define ATADEV_H

#include "global.h"
#include "membus.h"
#include "device.h"
#include "atadevconf.h"

// typedefs and structures

// ATA block device
extern const DevBlock_t DevBlockAta;

// Prototypes
int atadevInit(void);
int atadevReset(void);

u08  atadevReadReg(u16 reg);
void atadevWriteReg(u16 reg, u08 data);

int atadevReadData(u08 *buffer, int numBytes);
int atadevWriteData(u08 *buffer, int numBytes);

#endif
