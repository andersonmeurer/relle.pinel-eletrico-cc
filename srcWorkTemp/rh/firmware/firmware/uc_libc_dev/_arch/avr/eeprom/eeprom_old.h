#ifndef __EEPROM_H
#define __EEPROM_H

#include "_config_cpu_.h"

void eeprom_WriteByte(u16 Addr, u8 Data);
u8 eeprom_ReadByte(u16 Addr);
void eeprom_WriteBuffer(u16 AddrInit, const u8 *Buf, u16 Len);
void eeprom_ReadBuffer(u16 AddrInit, u8 *Buf, u16 Len);

#endif
