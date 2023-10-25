#ifndef __INTEL_HEX_H
#define __INTEL_HEX_H

#include "_config_cpu_.h"

int IntelHexInterpret (u8 *flineHEX, u8 *flen, u16 *faddrOffSet, u8 *fbuffer);
s16 _hex2byte(u8 *s);

#endif
