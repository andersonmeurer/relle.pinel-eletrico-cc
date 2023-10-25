/*
	Utiliza-se o barramento i2c para acessar os registradores e o barrmanento SPI para mandar o bitstream mp3 para o sta
	SOMENTE TOCA MUSICA COM RAZÃO MP3 256. estudar mais as devisões
	FAZER UMA MACRO PARA LER O FORMATO DO MP3 E RECALCULAR OS plls
*/

/*! \file sta013.c \brief STMicroelectronics STA013 MP3 player driver. */
//*****************************************************************************
//
// File Name	: 'sta013.c'
// Title		: STMicroelectronics STA013 MP3 player driver
// Author		: Pascal Stang
// Created		: 10/22/2000
// Revised		: 7/11/2003
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "sta015.h"
#include "mp3_data.h"

#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

#if (STA_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static u8 sta_ReadReg (u8 reg);
static void sta_WriteReg  (u8 reg, u8 data);
static void sta_Update(void);

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o decoder MP3 sta015
// Parametros: 	Ponteiro da variável para guardar o ID do STA
// Retorna: 	Retorna pdPASS se a inicialização ocorreu com sucesso, senão retorna pdFAIL
//------------------------------------------------------------------------------------------------------------------
int sta_Init (void) {
	u8 sta_id;

	#if (STA_USE_DEBUG == pdON)
	plog("STA init"CMD_TERMINATOR);
	#endif

   	sta_SetupPorts();

	#if (STA_USE_DEBUG == pdON)
	plog("STA send cmd get ID"CMD_TERMINATOR);
	#endif

    sta_id = sta_ReadReg(STA_REG_ID);
    
    
	// LÊ REGISTRO DE IDENTIFICADOR DO STA
	if (sta_id != STA_I2C_IDENT)	{
		#if (STA_USE_DEBUG == pdON)
		plog("STA init fail"CMD_TERMINATOR);
		#endif

		return pdFAIL;
	}

	#if (STA_USE_DEBUG == pdON)
	plog("STA update DSP"CMD_TERMINATOR);
	#endif
	sta_Update();
	
	#if (STA_USE_DEBUG == pdON)
	plog("STA reset software"CMD_TERMINATOR);
	#endif

	sta_ResetSoftware();											// Reajusta software do DSP no STA
	#if (STA_USE_DEBUG == pdON)
	plog("STA init ok id = 0x%x"CMD_TERMINATOR, sta_id);
	#endif

	return pdPASS;
}

//--------------------------------------------------------------------------------------------------------------------------
// Inicializa o Decoder no STA
//--------------------------------------------------------------------------------------------------------------------------
void sta_ResetSoftware(void) {
	// Reset via software
	sta_WriteReg (STA_REG_RESETSOFT,		0x01);	// (0x10) 
	sta_WriteReg (STA_REG_RESETSOFT,		0x00);	// (0x10) 

	delay_ms(100); 									// Esperar x mili segundos para que o STA se ajuste
	
	sta_WriteReg (STA_REG_MUTE,				0x01);	// (0x14) Mudo
	
	// Configurando a saida PCM >> DAC CS4334
	sta_WriteReg (STA_REG_PCMDIVIDER,		0x01);	// (0x54) O_FAC = 256 ????????
//	sta_WriteReg (STA_REG_PCMCONF,			0x33);	// (0x55) bit MSB TX primeiro / Word alinhado a esquerda / LRCKT Polarizado copativel com I2S /
													// Formato I2S / Dado transmitido na subida de SCKT / modo 24 bits com 32 slots TX porque o CS4334 é de 24bits
	sta_WriteReg (STA_REG_PCMCONF,			0x31);	
	
	
	// Configurar PLL do STA
	// UTILIZAR O SOFTWARE CPLL.EXE PARA CALCULAR ESTAS VARIAVEIS para a frequencia do cristal usado no STA
	// Estes valores foram calculados para cristal a 14.7456MHZ e Ratio 256
	
	sta_WriteReg (STA_REG_PLLFRAC_441_H,	0x04);	// (0x52) Este registrador contém o valor fracionario para o PLL para a frequencia de referencia de 44.1 KHz
	sta_WriteReg (STA_REG_PLLFRAC_441_L,	0x00);	// (0x51) Este registrador contém o valor fracionario para o PLL para a frequencia de referencia de 44.1 KHz
	sta_WriteReg (STA_REG_PLLFRAC_H,		0x55);	// (0x65) Este registrador contém o valor fracionario para o PLL
	sta_WriteReg (STA_REG_PLLFRAC_L,		0x55);	// (0x64) Este registrador contém o valor fracionario para o PLL
	sta_WriteReg (STA_REG_MFSDF_441,		0x10);	// (0x50) Este registrador contém o valor para o PLL X para a frequencia de referencia de 44.1 KHz
	sta_WriteReg (STA_REG_MFSDF,			0x0F);	// (0x61) Este registrador contém o valor para o PLL X para a frequencia de referencia de 32 e 48 Khz de fluxo de bits de entrada
	
	// Configurar interfaces e polaridade dos pinos do STA	etc
	sta_WriteReg (STA_REG_PLLCTL_M,			0x0C); 	// (0x06) Registrador M de configuração do PLL pelo DSP	// Saida VCO habilitada / VCO fornece Clk do divisor S / VCO fornece Clk do divisor X / Update FRAC
	sta_WriteReg (STA_REG_PLLCTL_N,			0x00);	// (0x07) Registrador N de configuração do PLL pelo DSP
	sta_WriteReg (STA_REG_PLLCTL,			0xA1);  // (0x05) Pino Data_Req habilitado / XTO Habilitado / pino OCLK Habilitado / freq OSCK != clk system
	sta_WriteReg (STA_REG_SCLKPOL,			0x00);	//0x04// (0x0d) Os dados sao capturados na borda de descida do SCKR
	sta_WriteReg (STA_REG_REQPOL,			0x01);	// (0x0c) Requisito do sinal Data Req é feita no nivel alto
	sta_WriteReg (STA_REG_DATAREQENABLE,	0x04);	// (0x18) Data Req configurado para sinal de requisito
	sta_WriteReg (STA_REG_PLLCTL,			0xA1);  // (0x05) Pino Data_Req habilitado / XTO Habilitado / pino OCLK Habilitado / freq OSCK != clk system
	       
    //Modo de trabalho do Sta
    sta_WriteReg (STA_CHIP_MODE,  			0x00); 	// (0x4d) MP3 decoder    
	sta_DecoderStop();
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void sta_DecoderPlay (void)	{
	sta_WriteReg (STA_REG_RUN,  	0x01); 	// (0x72) Rodar DSP. Somente aciona logo após de ajustar os regs de config
	sta_WriteReg (STA_REG_MUTE,		0x00);	// mute output
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void sta_DecoderStop (void)	{
	sta_WriteReg (STA_REG_MUTE,		0x01);	// mute output
	sta_WriteReg (STA_REG_RUN,  	0x00); 	// (0x72) Rodar DSP. Somente aciona logo após de ajustar os regs de config
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void sta_DecoderResume (void) {
	sta_WriteReg (STA_REG_PLAY,		0x01);
	sta_WriteReg (STA_REG_MUTE, 	0x00);
}

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
void sta_DecoderPause (void) {
	sta_WriteReg (STA_REG_MUTE,		0x01);	// mute output
	sta_WriteReg (STA_REG_PLAY,		0x00);
}


//--------------------------------------------------------------------------------------------------------------------------
// Volume com escala de 0 a 100
// Balanço com escala de -100-0-100
//--------------------------------------------------------------------------------------------------------------------------
void sta_SetVolume(u8 volume, s8 balance) {
	char attenL, attenR;

	if( balance > 0) {	// balance to the left, attenuate right
		attenL = (100 - volume);
		attenR = (100 - volume) - (balance);
	} else {	// balance to the right, attenuate left
		attenL = (100 - volume) + (balance);
		attenR = (100 - volume);
	}
	// respect limits
	attenL = min(attenL, MAX_VOLUME_ATTENUATION);
	attenL = max(attenL, MIN_VOLUME_ATTENUATION);
	attenR = min(attenR, MAX_VOLUME_ATTENUATION);
	attenR = max(attenR, MIN_VOLUME_ATTENUATION);

	// set volume
	sta_WriteReg (STA_REG_DLA, attenL);
	sta_WriteReg (STA_REG_DLB, MAX_VOLUME_ATTENUATION);
	sta_WriteReg (STA_REG_DRA, attenR);
	sta_WriteReg (STA_REG_DRB, MAX_VOLUME_ATTENUATION);
	
	#if (STA_USE_DEBUG == pdON)
	plog("STA set volume %u"CMD_TERMINATOR, volume);
	plog("STA set balance %u"CMD_TERMINATOR, balance);
	plog("STA attenL %d"CMD_TERMINATOR, attenL);
	plog("STA attenR %d"CMD_TERMINATOR, attenR);
	#endif
}

//--------------------------------------------------------------------------------------------------------------------------
// Ajusta tom
// 	bass_enh 	entre -12 a +12
//	bass_freq	entre 0 a 65535
//	treble_enh	entre -12 a +12
//	treble_freq	entre 0 a 65535
//--------------------------------------------------------------------------------------------------------------------------
void sta_SetTone (s8 bass_enh, u16 bass_freq, s8 treble_enh, u16 treble_freq) {
	// set bass enhancement
	sta_WriteReg (STA_REG_BASS_FREQUENCY_LOW,  (bass_freq   ) & 0xFF );
	sta_WriteReg (STA_REG_BASS_FREQUENCY_HIGH, (bass_freq>>8) & 0xFF );
	
	// respect limits
	bass_enh = min(bass_enh, MAX_BASS_ENHANCE);
	bass_enh = max(bass_enh, MIN_BASS_ENHANCE);
	sta_WriteReg (STA_REG_BASS_ENHANCE, bass_enh);

	// set treble enhancement
	sta_WriteReg (STA_REG_TREBLE_FREQUENCY_LOW,  (treble_freq   ) & 0xFF );
	sta_WriteReg (STA_REG_TREBLE_FREQUENCY_HIGH, (treble_freq>>8) & 0xFF );
	// respect limits
	treble_enh = min(treble_enh, MAX_TREBLE_ENHANCE);
	treble_enh = max(treble_enh, MIN_TREBLE_ENHANCE);
	sta_WriteReg (STA_REG_TREBLE_ENHANCE, treble_enh);

	// set attentuation to avoid clipping
	sta_WriteReg ( STA_REG_TONE_ATTEN, max(bass_enh,treble_enh) );

	#if (STA_USE_DEBUG == pdON)
	plog("STA bass_freq %u"CMD_TERMINATOR, bass_freq);
	plog("STA set bass_enh %d"CMD_TERMINATOR, bass_enh);
	plog("STA treble_freq %u"CMD_TERMINATOR, treble_freq);
	plog("STA set treble_enh %d"CMD_TERMINATOR, treble_enh);
	#endif

}

//--------------------------------------------------------------------------------------------------------------------------
// Retorna true se o STA precisa de dados
//--------------------------------------------------------------------------------------------------------------------------
bool sta_DataReq(void)	{
	return sta_dara_req();
}

//--------------------------------------------------------------------------------------------------------------------------
// Manda os dados a porta do STA
//--------------------------------------------------------------------------------------------------------------------------
void sta_SendAudio(u8 data)	{
	#if (STA_USE_GPIO_BITSTREAM == pdON) //Utiliza os pinos GPIO para mandar bistream mp3 para o sta
		register u8	i;
	
		for (i = 0; i < 8; i++)	{	
			sta_set_clk(); //sbi(STA_SCKR_PORT, STA_SCKR);
			//delay_us(3);
			
        	if (data & 0x80)	sta_set_data(); //sbi(STA_SDI_PORT, STA_SDI);	// Se é bit 1 manda bit 1
    		else				sta_clr_data(); //cbi(STA_SDI_PORT, STA_SDI);	// Se é bit 0 manda bit 0
			
			sta_clr_clk(); //cbi(STA_SCKR_PORT, STA_SCKR);
			//delay_us(2);
  			data <<= 1;
  		}
  	#else
  		sta_TransferByte(data);
  	#endif
}      

void sta_GetMp3Info(u16 *bitrate, u8 *samp_freq, u8 *mode) {
	u8 headL, headM, headH;
	u8 mpegID, bitrateIndex, sampFreqIndex;
	
	// get the MP3 header info
	headH = sta_ReadReg (STA_REG_HEAD_H);
	headM = sta_ReadReg (STA_REG_HEAD_M);
	headL = sta_ReadReg (STA_REG_HEAD_L);

	// IDex:ID is in head[20:19]
	// 00 - MPEG2.5
	// 01 - reserved
	// 10 - MPEG2
	// 11 - MPEG1
	mpegID = (headH & 0x18)>>3;

	// sampling frequency is in head[11:10]
	sampFreqIndex = ((headM & 0x0C)>>2) | (mpegID<<2);

	// bitrate index is in head[15:12]
	bitrateIndex = ((headM & 0xF0)>>4) | ((mpegID & 0x01)<<4);
	//bitrateIndex = ((headM & 0xF0)>>4) | (1<<4);
	
	// mode is in head[7:6]
	// 00 - stereo
	// 01 - joint stereo
	// 10 - dual channel
	// 11 - single channel (mono)
	*mode = (headL & 0xC0)>>6;
	
	*bitrate =	2 * mp3_bitrates [bitrateIndex];
	*samp_freq = mp3_sampling_frequencies[sampFreqIndex];

/*
   header = (unsigned long)sta_ReadReg (STA_REG_HEAD_H) << 16 |
       (unsigned long)sta_ReadReg (STA_REG_HEAD_M) << 8  |
       (unsigned long)sta_ReadReg (STA_REG_HEAD_L);

//   hdr->word           = l;
//   hdr->emphasis       = l & 0x03;
//   hdr->isOriginal     = (l >> 2) & 0x01;
//   hdr->isCopyrighted  = (l >> 3) & 0x01;
//   hdr->modeExtension  = (l >> 4) & 0x03;
//   hdr->mode           = (l >> 6) & 0x03;
//   hdr->private        = (l >> 8) & 0x01;
//   hdr->padding        = (l >> 9) & 0x01;
//   hdr->frequencyIndex = (l >> 10) & 0x03;
//   hdr->bitrateIndex   = (l >> 12) & 0x0f;
//   hdr->protection     = (l >> 16) & 0x01;
//   hdr->layer          = (l >> 17) & 0x03;
//   hdr->ID             = (l >> 19) & 0x01;
//   hdr->ID_ex          = (l >> 20) & 0x01;
*/
}

u16 sta_GetAverageBitrate(void) {
	return (2 * sta_ReadReg (STA_REG_AVERAGE_BITRATE));
}

// ############################################################################################################################################
// ############################################################################################################################################
// FUNÇÕES INTERNAS
// ############################################################################################################################################
//--------------------------------------------------------------------------------------------------------------------------
// Lê o byte do registrador Reg
//--------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros: 	
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
static u8 sta_ReadReg(u8 reg)	{
	sta_WriteBuffer(STA_I2C_ADDR_DEVICE, &reg, 1);			// Mando o comando para leitura do registrador
	sta_ReadBuffer(STA_I2C_ADDR_DEVICE, &reg, 1);		// Retorna 1 byte
		
	return reg;
}

//--------------------------------------------------------------------------------------------------------------------------
// Grava o byte Data no registrador Reg
//--------------------------------------------------------------------------------------------------------------------------
static void sta_WriteReg (u8 reg, u8 data) {
	u8 buffer[2]; 
		
	buffer[0] = reg;
	buffer[1] = data;
	
	sta_WriteBuffer(STA_I2C_ADDR_DEVICE, buffer, 2);
}

//--------------------------------------------------------------------------------------------------------------------------
// Atualiza os registradores do STA com uma lista pré determinada
//--------------------------------------------------------------------------------------------------------------------------
static void sta_Update(void) {
	u16 count = 0;
	u8	flag = 0, addr, data;
		
	// Send Configuration file
	while (!flag)	  {
	  	addr = sta_update_data[count++];
	  	data = sta_update_data[count++];
	  		
	  	if ( (addr == 0xff) && (data == 0xff) ) flag = 1;
	  	else sta_WriteReg (addr, data);
	}
}



/*
        
unsigned long Sta_StsMp3(void)	{
	
    return ( ((unsigned long)sta_ReadReg(STA_REG_HEAD_H) << 16 ) | (sta_ReadReg(STA_REG_HEAD_M) << 8) | sta_ReadReg(STA_REG_HEAD_L));
}
*/



