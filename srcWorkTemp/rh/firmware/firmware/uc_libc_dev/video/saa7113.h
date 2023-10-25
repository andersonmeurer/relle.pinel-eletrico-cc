#ifndef SAA711x_H
#define SAA711x_H

#include "_config_cpu_.h"
#include "_config_lib_digitalizer.h"
#include "_config_cpu_digitalizer.h"
#include "videodev.h"
#include "v4l2-common.h"
#include "_config_cpu_i2c.h"

#if (SAA711x_USE_I2C == 0)
	#include "i2c0.h"
	#define saa711x_i2c_Send  		i2c0_WriteBuffer
	#define saa711x_i2c_Receive  	i2c0_ReadBuffer	
#elif (SAA711x_USE_I2C == 1)
	#include "i2c1.h"
	#define saa711x_i2c_Send 		i2c1_WriteBuffer
	#define saa711x_i2c_Receive 	i2c1_ReadBuffer
#else
	#error NÃO FOI DEFINIDO QUAL O BARRAMENTO I2C NO _config_cpu_i2c.h
#endif

 struct saa711x_t {	
	enum v4l2_chip_ident ident;	// ID do decoder
	v4l2_std_id std;			// padrão da imagem
	u8 addr_dev;				// Endereço I2C do decoder
	u8 sts;					// Status do saa
	u8 sts_old;				// Status do saa modo antigo para manter compatibilidade
	u32 input;					// entrada de video
	u32 output;					// saida do video digital

	u32 bright;
	u32 contrast;
	u32 hue;
	u32 sat;
	u32 width;
	u32 height;
};



// SAA7113/4/5 HW inputs
#define SAA711x_COMPOSITE0 		0
#define SAA711x_COMPOSITE1 		1	
#define SAA711x_COMPOSITE2 		2
#define SAA711x_COMPOSITE3 		3
#define SAA711x_SVIDEO0    		6
#define SAA711x_SVIDEO1    		7
#define SAA711x_SVIDEO2    		8
#define SAA711x_SVIDEO3    		9

#define SAA711x_AOUT_TESTE1    	0
#define SAA711x_AOUT_AD1    	1
#define SAA711x_AOUT_AD2    	2
#define SAA711x_AOUT_TESTE2    	3

// REGISTRADORES
#define R_00_CHIP_VERSION                             0x00

// Video Decoder - Frontend part
#define R_01_INC_DELAY                                0x01
#define R_02_AINPUT_CTRL_1                            0x02
#define R_03_AINPUT_CTRL_2                            0x03
#define R_04_AINPUT_CTRL_3                            0x04
#define R_05_AINPUT_CTRL_4                            0x05

// Video Decoder - Decoder part
#define R_06_H_SYNC_START                             0x06
#define R_07_H_SYNC_STOP                              0x07
#define R_08_SYNC_CTRL                                0x08
#define R_09_LUMA_CTRL                                0x09
#define R_0A_LUMA_BRIGHT_CTRL                         0x0a
#define R_0B_LUMA_CONTRAST_CTRL                       0x0b
#define R_0C_CHROMA_SAT_CTRL                          0x0c
#define R_0D_CHROMA_HUE_CTRL                          0x0d
#define R_0E_CHROMA_CTRL_1                            0x0e
#define R_0F_CHROMA_GAIN_CTRL                         0x0f
#define R_10_FORMAT_DELAY_CTRL                        0x10
#define R_11_OUTPUT_CTRL_1                            0x11
#define R_12_OUTPUT_CTRL_2                            0x12
#define R_13_OUTPUT_CTRL_3                            0x13
#define R_13_MODE_STS                            	  0x13
#define R_15_VGATE_START		                      0x15
#define R_16_VGATE_STOP                               0x16
#define R_17_VGATE_MSB                   			  0x17

#define R_1F_STATUS_BYTE_2_VD_DEC                     0x1f

// General purpose VBI data slicer part
#define R_40_AC1 			                          0x40
#define R_41_LCR_BASE                                 0x41
#define R_58_PROGRAM_FRAMING_CODE                     0x58
#define R_59_H_OFF_FOR_SLICER                         0x59
#define R_5A_V_OFF_FOR_SLICER                         0x5a
#define R_5B_FLD_OFF_AND_MSB_FOR_H_AND_V_OFF          0x5b
#define R_5E_SDID                                     0x5e
#define R_60_DR						                  0x60
#define R_61_LN_1                   				  0x61
#define R_62_LN_2    				                  0x62

int saa711x_Init(struct saa711x_t *dev);
int saa711x_command( struct saa711x_t *dev,  unsigned int cmd, void *arg);
int saa711x_has_reg(struct saa711x_t *dev, const u8 reg);
int saa711x_read(struct saa711x_t *dev,  u8 reg);
int saa711x_write(struct saa711x_t *dev, u8 reg, u8 value);
int saa711x_write_block(struct saa711x_t *dev, const u8 *regs);

#endif
