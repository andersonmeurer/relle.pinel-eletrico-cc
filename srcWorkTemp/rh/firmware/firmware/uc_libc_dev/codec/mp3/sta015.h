#ifndef _STA015_H
#define _STA015_H

#include "sta015_drv.h"

int sta_Init (void);
void sta_ResetSoftware (void);
void sta_DecoderPlay (void);
void sta_DecoderStop (void);
void sta_DecoderResume (void);
void sta_DecoderPause (void);
void sta_SetVolume(u8 volume, s8 balance);
void sta_SetTone (s8 bass_enh, u16 bass_freq, s8 treble_enh, u16 treble_freq);
bool sta_DataReq(void);
void sta_SendAudio(u8 data);
void sta_GetMp3Info(u16 *bitrate, u8 *samp_freq, u8 *mode);
u16 sta_GetAverageBitrate(void);

//#define sta_DataReq						sta_dara_req

#define STA_I2C_ADDR_DEVICE 		    0x86 	// Endereço do STA no BUS I2C  //STA_I2C_DEV						0x86
#define STA_I2C_IDENT 	    			0xac	// Valor ID do STA //STA_IDENT						0xAC


// Endereços dos registradores do STA acessados via I2C
#define STA_REG_VERSION                	0x00
#define STA_REG_ID				      	0x01
#define STA_REG_PLLCTL                 	0x05
#define STA_REG_PLLCTL_M	        	0x06
#define STA_REG_PLLCTL_N	            0x07
#define STA_REG_REQPOL	                0x0c
#define STA_REG_SCLKPOL		        	0x0d
#define STA_REG_ERRORCODE				0x0f
#define STA_REG_RESETSOFT			    0x10
#define STA_REG_PLAY					0x13
#define STA_REG_MUTE					0x14
#define STA_REG_CMDINT					0x16
#define STA_REG_DATAREQENABLE		    0x18
#define STA_REG_SYNCSTS					0x40
#define STA_REG_ANC_COUNT_LOW			0x41
#define STA_REG_ANC_COUNT_HIGH        	0x42
#define STA_REG_HEAD_H					0x43
#define STA_REG_HEAD_M					0x44
#define STA_REG_HEAD_L					0x45
#define STA_REG_DLA						0x46
#define STA_REG_DLB						0x47
#define STA_REG_DRA						0x48
#define STA_REG_DRB						0x49
#define STA_CHIP_MODE					0x4d
#define STA_REG_MFSDF_441              	0x50
#define STA_REG_PLLFRAC_441_L          	0x51
#define STA_REG_PLLFRAC_441_H          	0x52
#define STA_REG_PCMDIVIDER				0x54
#define STA_REG_PCMCONF					0x55
#define STA_REG_PCMCROSS				0x56
#define STA_REG_ANC_DATA1				0x59
#define STA_REG_ANC_DATA2             	0x5a
#define STA_REG_ANC_DATA3             	0x5b
#define STA_REG_ANC_DATA4             	0x5c
#define STA_REG_ANC_DATA5 	        	0x5d
#define STA_REG_MFSDF					0x61
#define STA_REG_DAC_CLK_MODE			0x63
#define STA_REG_PLLFRAC_L				0x64
#define STA_REG_PLLFRAC_H				0x65
#define STA_REG_FRAME_CNT_L				0x67
#define STA_REG_FRAME_CNT_M				0x68
#define STA_REG_FRAME_CNT_H				0x69
#define STA_REG_AVERAGE_BITRATE			0x6a
#define STA_REG_SOFTVERSION				0x71
#define STA_REG_RUN						0x72
#define STA_REG_TREBLE_FREQUENCY_LOW	0x77
#define STA_REG_TREBLE_FREQUENCY_HIGH	0x78
#define STA_REG_BASS_FREQUENCY_LOW		0x79
#define STA_REG_BASS_FREQUENCY_HIGH    	0x7a
#define STA_REG_TREBLE_ENHANCE 			0x7b
#define STA_REG_BASS_ENHANCE			0x7c
#define STA_REG_TONE_ATTEN				0x7d

#define MIN_VOLUME_ATTENUATION			0
#define MAX_VOLUME_ATTENUATION			96
#define MIN_TONE_ATTENUATION			0
#define MAX_TONE_ATTENUATION			96
#define MIN_BASS_FREQUENCY				100
#define MAX_BASS_FREQUENCY				500
#define MIN_BASS_ENHANCE				-12	// -18dB in 1.5 dB steps
#define MAX_BASS_ENHANCE				+12	// +18dB in 1.5 dB steps
#define MIN_TREBLE_FREQUENCY			1000
#define MAX_TREBLE_FREQUENCY			5000
#define MIN_TREBLE_ENHANCE				-12	// -18dB in 1.5 dB steps
#define MAX_TREBLE_ENHANCE				+12	// +18dB in 1.5 dB steps
#define SOFTMUTE_VOLUME_CHANGE			20

#endif
