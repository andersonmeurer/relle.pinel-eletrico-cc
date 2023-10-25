#ifndef __PLL_CDCE_H
#define __PLL_CDCE_H


#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"

#if (CDCE_USE_I2C == 0)
	#include "i2c0.h"
	#define cdce_i2c_Send  		i2c0_WriteBuffer
	#define cdce_i2c_Receive  	i2c0_ReadBuffer	
#elif (CDCE_USE_I2C == 1)
	#include "i2c1.h"
	#define cdce_i2c_Send 		i2c1_WriteBuffer
	#define cdce_i2c_Receive 	i2c1_ReadBuffer
#else
	#error NÃO FOI DEFINIDO QUAL O BARRAMENTO I2C NO _config_device.h
#endif

// ENDEREÇO DO CDCE ESCRAVO NO BARRAMENTO I2C
#define I2C_CDCE_ADDR0				0xD8
#define I2C_CDCE_ADDR1				0xDA
#define I2C_CDCE_ADDR2				0xDC
#define I2C_CDCE_ADDR3				0xDE

// TIPOS DE ACESSO AO CDCE
#define CDCE_BYTE_ACCESS			0x80
#define CDCE_BLOCK_ACCESS			0x00
	
// COMANDOS DE CONTROLE DO CDCE
#define CDCE_M1						0x01
	#define CDCE_M1_REG					0x02
	#define CDCE_M1_SOURCE_FIN			0x00
	#define CDCE_M1_SOURCE_PLL1			0x80
	#define CDCE_M1_MASK				0x80
		
#define CDCE_XCSEL					0x02
	#define CDCE_XCSEL_REG				0x05
	#define CDCE_XCSEL_MASK				0xF8

#define CDCE_MUX1					0x10
#define CDCE_MUX2					0x11
#define CDCE_MUX3					0x12
#define CDCE_MUX4					0x13
	#define CDCE_MUX1_REG				0x14
	#define CDCE_MUX2_REG				0x24
	#define CDCE_MUX3_REG				0x34
	#define CDCE_MUX4_REG				0x44
	#define CDCE_MUX_MASK				0x80
	#define CDCE_MUX_SOURCE_FIN			0x80
	#define CDCE_MUX_SOURCE_PLL			0

#define CDCE_PDIV1					0x03
	#define CDCE_REG_PDIV1_MSB			0x02
	#define CDCE_REG_PDIV1_LSB			0x03
	#define CDCE_PDIV1_MASK_MSB			0x03

#define CDCE_PDIV2					0x20
#define CDCE_PDIV4					0x21
#define CDCE_PDIV6					0x22
#define CDCE_PDIV8					0x23
#define CDCE_PDIV3					0x40
#define CDCE_PDIV5					0x41
#define CDCE_PDIV7					0x42
#define CDCE_PDIV9					0x43
	#define CDCE_PDIV2_REG				0x16
	#define CDCE_PDIV3_REG				0x17
	#define CDCE_PDIV4_REG				0x26
	#define CDCE_PDIV5_REG				0x27
	#define CDCE_PDIV6_REG				0x36
	#define CDCE_PDIV7_REG				0x37
	#define CDCE_PDIV8_REG				0x46
	#define CDCE_PDIV9_REG				0x47
	#define CDCE_PDIV_MASK				0x7F


// FUNÇÕES
int cdce_Init(u8 addr_device);
int cdce_ReadReg(u8 addr_device, u8 addr_reg, u8 *data);
int cdce_WriteReg(u8 addr_device, u8 addr_reg, u8 data);
int cdce_Config ( u8 addr_device, u8 cmd, u16 action );

#if (CDCE_USE_DEBUG == pdON)
int cdce_ViewRegsPLL (u8 addr_device, u8 pll );
#endif

#endif
