#ifndef __GLCD_DRV_H
#define __GLCD_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_display.h"
#include "delay.h"


// -----------------------------------------------------------------------------------------------------------
// PARAMETROS PARA O PADRÃO KS0108 >> MGD12864A
#define glcdDELAY_RW	1 // 1us. Os tempos são em ns, porém o delay minimo nas libs é de 1us
#define glcdBUSY_FLAG 	0x80
// -----------------------------------------------------------------------------------------------------------


#if (glcdCONNECTION == glcdCONNECTION_GPIO)
	void glcd_Write(u8 reg, u8 chip, u8 rs);
	#define glcd_WriteCommand(reg, chip)	glcd_Write(reg, chip, 0)
	#define glcd_WriteRAM(reg, chip)		glcd_Write(reg, chip, 1)

	u8 glcd_Read(u8 chip, u8 rs);
	#define glcd_ReadCommand(chip)			glcd_Read(chip, 0)
	#define glcd_ReadRAM(chip)	 			glcd_Read(chip, 1)

	void glcd_ConfigPorts(void);
	void glcd_WaitReady(u8 chip);
#elif (glcdCONNECTION == glcdCONNECTION_IOMAP)
	#error NÃO FOI IMPLEMENTADO ESTE RECURSO
//	// ACESSO DIRETO IOMAP RAM
//	static volatile u8
//		*pLcdG_Command = (u8 *) glcdADDR_CMD, 	// read status and write command
//		*pLcdG_Data = (u8 *) glcdADDR_DATA; 	// read last address data and write data
#else
	#error NÃO FOI DEFINIDO O TIPO DE CONEXÃO DO LPC. VEJA _CONFIG_LCD.H
#endif


#endif
