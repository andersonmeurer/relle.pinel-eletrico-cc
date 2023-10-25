#ifndef __UTILS_H
#define __UTILS_H

#include "_config_cpu_.h"
#include "_config_lib_utils.h"

//double xatof(char *s); // arrumar a conversão do número negativo, pois passa para positivo

#if (UTIL_USE_DUMP == pdON)
#ifdef CPU_AVR
void put_dump (const u8 *buff, showAddrOffSet, int cnt);
void put_dump16 (const u8 *buff, u16 showAddrOffSet, int cnt, u8 mask );
#else
void put_dump (const u8 *buff, u32 showAddrOffSet, int cnt);
void put_dump16 (const u8 *buff, u32 showAddrOffSet, int cnt, u8 mask );
#endif
void buffer_dump(u8 *buff, int cnt, u8 mask);
#endif

#if (UTIL_USE_XATOI == pdON)
int xatoi (char **str, long *res);
#endif

#if (UTIL_USE_XITOA == pdON)
void xitoa (char* str, long val);
#endif

#if (UTIL_USE_DATETIME == pdON)
void minutesToHours(uint min, uint* h, uint* m);
void hoursToMinutes(uint h, uint m, uint* min);
#endif

#if (UTIL_USE_POT == pdON)
u32 pot(uint base, uint exp);
#endif

#if (UTIL_USE_GET_NUMBER == pdON)
u32 get_Number(u8 *enter_data, u8 enter_data_index);
#endif

#if (UTIL_USE_ASCII2BYTE == pdON)
u8 ascii2byte (u8 *val);
#endif

#if (UTIL_USE_HEX2BYTE == pdON)
s16 hex2byte(char c);
#endif

#if (UTIL_USE_PERCENT_INT == pdON)
//u8 percent (u32 value, u32 value_max);
int percent (u32 value, u32 value_max, u8 scale, u8 *p);
#endif

#if (UTIL_USE_ACCESS_BUF_LE == pdON)
u32 read_LE32u (volatile u8 *pmem);
void write_LE32u (volatile u8 *pmem, u32 val);
u16 read_LE16u (volatile u8 *pmem);
void write_LE16u (volatile u8 *pmem, u16 val);
#endif

#if (UTIL_USE_ACCESS_BUF_BE == pdON)
u32 read_BE32u (volatile u8 *pmem);
void write_BE32u (volatile u8 *pmem, u32 val);
u16 read_BE16u (volatile u8 *pmem);
void write_BE16u (volatile u8 *pmem, u16 val);
#endif

#endif
