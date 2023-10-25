#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include "modbus.h"

void modbus_SlaveInit(
	int slaveAddr,
	int(*puts_func)(u8* buffer, u16 count),
	int(*getc_func)(u8* ch),
	int(*byte_available_func)(void),
	void(*flushRX_func)(void)
);

void modbus_SlaveSetID(int slaveAddr);

void modbus_SlaveAppendFunctions(
	int (*readregs_func)(uint addrInit, u8* query, uint count),
	int (*writereg_func)(uint addr, u16 value),
	int (*writeregs_func)(uint addrInit, u8* query, uint count)
);

int modbus_SlaveProcess(void);
void modbus_SlaveTick(void);

#endif
