#ifndef __TLC5917_H
#define __TLC5917_H

#include "tlc5917_drv.h"

void tlc5917_Init ( void );
void tlc5917_EnableOutput ( void );
void tlc5917_DesableOutput ( void );
void tlc5917_SetOutput( u8 st );
u8 tlc5917_StateOutput ( void );
int tlc5917_RegCalcBits(u16 r_ext, u8 iout, u8 *cmBit, u8 *hcBit, u8 *ccBits);
void tlc5917_WriteLed(u8 *stBits, u8 nBytes);
void tlc5917_WriteCurrent(u8 *stBits, u8 nBytes);
void tlc5917_ReadStatus(u8 *stBits, u8 nBytes);

#endif
