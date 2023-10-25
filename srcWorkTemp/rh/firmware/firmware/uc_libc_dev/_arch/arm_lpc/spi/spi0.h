#ifndef __SPI0_H
#define __SPI0_H

/* Para cortexm3 utilize spp */

#define usesSPI

#include "_config_cpu_.h"

#if defined (cortexm3)
#error CORTEX-M3 não tem suporte a spi, utilize spp
#endif
void spi0_Init(u32 frequency, u8 mode, u16 config);
#if defined(SPI0_BITS_MASK)
void spi0_SetBitsPerTransfer(u16 bits);
#endif
u32 spi0_SetClockFreq(u32 frequency);
int spi0_SetMode ( u8 mode );
u8 spi0_TransferByte(u8 data);

#endif
