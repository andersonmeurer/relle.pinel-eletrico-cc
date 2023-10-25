#ifndef __SPP0_H 
#define __SPP0_H

#if defined (arm7tdmi)
#if !defined (LPC2468) && !defined (LPC2138) && !defined (LPC2148)
#error ESTE ARM NÃO SUPORTA SSP0
#endif
#endif

#include "_config_cpu_.h"

void ssp0_Init (u8 mode, u16 config);
void spi0_SetBitsPerTransfer(u16 bits);
u32 ssp0_SetClockFreq(u32 frequency);
int spp0_SetMode ( u8 mode );
u8 ssp0_WaitReady (void);
u8 ssp0_TransferByte (u8 c);
void ssp0_SendBlock(const u8 *pData, u32 size);
void ssp0_ReceiveBlock(u8 *pData, u32 size);

#endif
