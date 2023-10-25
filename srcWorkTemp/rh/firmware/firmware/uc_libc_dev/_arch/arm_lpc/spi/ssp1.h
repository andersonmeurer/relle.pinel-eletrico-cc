#ifndef __SPP1_H 
#define __SPP1_H

#define usesSSP

#if defined (arm7tdmi)
#if !defined (LPC2468)
#error ESTE ARM NÃO SUPORTA SSP0
#endif
#endif

#include "_config_cpu_.h"

void ssp1_Init (u8 mode, u16 config);
void spi1_SetBitsPerTransfer(u16 bits);
u32 ssp1_SetClockFreq(u32 frequency);
int spp1_SetMode ( u8 mode );
u8 ssp1_WaitReady (void);
u8 ssp1_TransferByte (u8 c);
void ssp1_SendBlock(const u8 *pData, u32 size);
void ssp1_ReceiveBlock(u8 *pData, u32 size);

#endif
