/*
PARAMETROS DOS SISTEMAS DE CORES	

Parametros Basicos    				NTSC M			PAL M			PAL B-G-H  		PAL I  			PAL D  			PAL N  			SECAM  B-G-H   	SECAM D-K-K1-L		
linhas / campo 						525/60			525/60			625/50 			625/50 			625/50 			625/50 			625/50  		625/50	
freqüência vertical 				60 Hz			60 Hz			50 Hz 			50 Hz 			50 Hz 			50 Hz 			50 Hz.  		50 Hz
freqüência horizontal 				15.734 kHz		15.734 kHz		15.625 kHz 		15.625 kHz 		15.625 kHz 		15.625 kHz 		15.625 kHz.  	15.625 kHz.
freq. da subportadora de color 		3.579545 MHz	3.575611 MHz.	4.433618 MHz 	4.433618 MHz. 	4.433618 MHz. 	3.582056 MHz. 
freq. da portadora de audio (FM) 	4.5 MHz 		4.5 MHz.		5.5 MHz. 		6.0 MHz. 		6.5 MHz. 		4.5 MHz.		5.5 MHz (AM)  	6.5 MHz (AM)  
largura da faixa de video 			4.2 MHz			4.2 MHz.		5.0 MHz 		5.5 MHz. 		6.0 MHz. 		4.2 MHz. 		5.0 MHz  	6.0 MHz.

*/

#include <string.h>
#include "saa7113.h"

#if (DIGITALIZER_USE_DEBUG == pdON)
	#include "stdio_uc.h"
#endif

static void saa711x_init_values(struct saa711x_t *dev);
static int saa711x_set_v4lctrl( struct saa711x_t *dev, struct v4l2_control *ctrl);
static int saa711x_set_size(struct saa711x_t *dev, int width, int height);
static void saa711x_set_v4lstd( struct saa711x_t *dev, v4l2_std_id std);
static v4l2_std_id saa711x_get_v4lstd(struct saa711x_t *dev);

// ####################################################################################################################
// ####################################################################################################################
// TABELAS DE INCIALIZAÇÕES DO DECODER

// TABELA DE VALORES DE INICIALIZAÇÃO DO SAA7113
//	Valores padrões para: 
//		Filtro antialiasing e AFC ativados.
//		Habilitado a detecção atumática de campo
//		Padrão ITU-R BT656 com controle de brilho, contraste e saturação para esse padrão
//		Processamento de croma cam largura de banda nominal (800Khz)
// VALORES ESTÃO NO _config_lib_digitalizer.h
static const u8 SAA7113_INIT[] = {
	R_01_INC_DELAY,	SAA711x_R_01_INC_DELAY, 
	R_02_AINPUT_CTRL_1, SAA711x_R_02_AINPUT_CTRL_1,
	R_03_AINPUT_CTRL_2,	SAA711x_R_03_AINPUT_CTRL_2,
	R_04_AINPUT_CTRL_3,	SAA711x_R_04_AINPUT_CTRL_3,
	R_05_AINPUT_CTRL_4, SAA711x_R_05_AINPUT_CTRL_4,
	R_06_H_SYNC_START, SAA711x_R_06_H_SYNC_START,
	R_07_H_SYNC_STOP, SAA711x_R_07_H_SYNC_STOP,
	
	R_08_SYNC_CTRL, SAA711x_R_08_H_SYNC_CTRL,
	R_09_LUMA_CTRL,	SAA711x_R_09_LUMA_CTRL,
	
	R_0A_LUMA_BRIGHT_CTRL, SAA711x_BRIGHT_DEFAULT,
	R_0B_LUMA_CONTRAST_CTRL, SAA711x_CONTRAST_DEFAULT,
	R_0C_CHROMA_SAT_CTRL, SAA711x_SAT_DEFAULT,
	R_0D_CHROMA_HUE_CTRL, SAA711x_HUE_DEFAULT,
	
	R_0E_CHROMA_CTRL_1, SAA711x_R_0E_CHROMA_CTRL_1,
	R_0F_CHROMA_GAIN_CTRL, SAA711x_R_0F_CHROMA_GAIN_CTRL,
	R_10_FORMAT_DELAY_CTRL, SAA711x_R_10_FORMAT_DELAY_CTRL,
	R_11_OUTPUT_CTRL_1, SAA711x_R_11_OUTPUT_CTRL_1,
	R_12_OUTPUT_CTRL_2,	SAA711x_R_12_OUTPUT_CTRL_2,
	
	R_13_OUTPUT_CTRL_3,	(SAA711x_R_13_OUTPUT_CTRL_3&0xfc),

	
	// R_15_VGATE_START			// ver SAA711X_CFG_XXHZ_VIDEO	
	// R_16_VGATE_STOP			// ver SAA711X_CFG_XXHZ_VIDEO	
	// R_17_VGATE_MSB			// ver SAA711X_CFG_XXHZ_VIDEO	
	// R_40_AC1							// ver SAA711X_CFG_XXHZ_VIDEO	
	// R_41_LCR_BASE  a R_41_LCR_BASE + 22 = 0xFF valor padrão nos regs // DESLIGA VBI
	
	R_58_PROGRAM_FRAMING_CODE, SAA711x_R_58_PROGRAM_FRAMING_CODE,
	
	//	R_59_H_OFF_FOR_SLICER  				// ver SAA711X_CFG_XXHZ_VIDEO
	//	R_5A_V_OFF_FOR_SLICER 				// ver SAA711X_CFG_XXHZ_VIDEO	
	//	R_5B_FLD_OFF_AND_MSB_FOR_H_AND_V_OFF, 0x03,	// ver SAA711X_CFG_XXHZ_VIDEO	
	
	R_5E_SDID, SAA711x_R_5E_SDID,
	
	0x00, 0x00
};

// ########################################################################################################################
// REGISTRADOR DE STATUS
// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		 D0
//	INTL 	HLVLN 	FIDT 	GLIMT 	GLIMB 	WIPA 	COPRO 	RDCAP
		// RDCAP/CODE
			// (OLDSB = 0) RDCAP ready for capture (all internal loops locked); active HIGH
			// (OLDSB = 1) CODE color signal in accordance with selected standard has been detected active HIGH
		// COPRO/SLTCA 
			// (OLDSB = 0) COPRO copy protected source detected according to Macrovision version up to 7.01 
			// (OLDSB = 1) SLTCA slow time constant active in WIPA mode; active HIGH
		// WIPA white peak loop is activated; active HIGH
		// GLIMB gain value for active luminance channel is limited [min (bottom)]; active HIGH
		// GLIMT gain value for active luminance channel is limited [max (top)]; active HIGH
		// FIDT identification bit for detected field frequency
			// 0 50 Hz
			// 1 60 Hz
		// HLVLN/HLCK 
			// HLVLN (OLDSB = 0) status bit for horizontal/vertical loop
				// 0 locked
				// 1 unlocked	
			// HLCK (OLDSB = 1) status bit for locked horizontal frequency; LOW = locked, HIGH = unlocked
				// 0 locked
				// 1 unlocked			
		// INTL status bit for interlace detection
			// 0 non-interlaced
			// 1 interlaced

// ####################################################################################################################
// ####################################################################################################################
// TABELAS DE AJUSTES PADRÕES
// ####################################################################################################################

// TABELAS DE VALORES DE RES PARA DECODER VIDEO 60HZ 525 Linhas (NTSC-M, NTSC-J PAL-M)
static const u8 SAA711X_CFG_60HZ_VIDEO[] = {
	R_15_VGATE_START, 0x00, //0x15, // Recomendado 0x0  // Subida do sinal
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	VSTA7	VSTA6	VSTA5	VSTA4	VSTA3	VSTA2	VSTA1	VSTA0
			// LSB VSTA  Start of VGATE pulse
		
	R_16_VGATE_STOP,0x00, // 0xFF,// Recomendado 0x0, // queda do sinal
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	VSTO7	VSTO6	VSTO5	VSTO4	VSTO3	VSTO2	VSTO1	VSTO0
			// LSB VSTO  Stop of VGATE pulse
		
	R_17_VGATE_MSB, 0x00,// Recomendado 0x0
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		 D0
		//	0		0		0		0		0		0		VSTO_MSB VSTA_MSB
			//	VSTA_MSB  bit: MSB of VSTA 
			//	VSTO_MSB  bit: MSB of VSTB 

	R_40_AC1, 0x82, // Recomendado 0x02 ,// No framing code errors allowed.  - IP Video 0x82
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		 D0
		//	FISET	HAM_N	FCE		HUNT_N	0		CLKSEL1 CLKSEL0	 0
		//	0		0		0		0		0		0		1		 0
		
			// CLKSEL Amplitude searching Data slicer clock selection
				// 00 Reserved
				// 01 13.5 MHz (default)										
				// 10 Reserved
				// 11 Reserved
			// HUNT_N Amplitude searching
				// 0 Amplitude searching active (default)						
				// 1 Amplitude searching stopped
			// FCE Framing code error
				// 0 One framing code error allowed								
				// 1 No framing code errors allowed
			// HAM_N Hamming check
				// 0 Hamming check for 2 bytes after framing code, 				
					// dependent on data type (default)
				// 1 No Hamming check
			// FISET Field size select
				// 0 50 Hz field rate											
				// 1 60 Hz field rate											

	R_59_H_OFF_FOR_SLICER, 0x54,// Recomendado 0x54
		// 	HOFF7 	HOFF6 	HOFF5	HOFF4	HOFF3	HOFF2	HOFF1	HOFF0
			// HOFF Horizontal offset. BITS mais siginificativos estão no reg 0x5B

	R_5A_V_OFF_FOR_SLICER, 0x0a, // Recomendado 0x07  - IP Video 0x0a
		//	VOFF7	VOFF6	VOFF5	VOFF4	VOFF3	VOFF2	VOFF1	VOFF0
			// VOFF Vertical offset BITS mais siginificativos estão no reg 0x5B
				
	R_5B_FLD_OFF_AND_MSB_FOR_H_AND_V_OFF, 0x83, // Recomendado 0x83
		// 	D7 		D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	FOFF					VOFF8	0		HOFF10 	HOFF9 	HOFF8
		//	1		0		0		0		0		0		1		1
			// HOFF Horizontal offset MSB
			// VOFF Vertical offset MSB									
			// FOFF Field offset
				// 0 no modification of internal field indicator
				// 1 Invert field indicator (even/odd; default)					
	
	0x00, 0x00
};

// TABELAS DE VALORES DE RES PARA DECODER VIDEO 50HZ
static const u8 SAA711X_CFG_50HZ_VIDEO[] = {	
	R_15_VGATE_START, 0x37,
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	VSTA7	VSTA6	VSTA5	VSTA4	VSTA3	VSTA2	VSTA1	VSTA0
			// LSB VSTA  Start of VGATE pulse
		
	R_16_VGATE_STOP, 0x16,
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	VSTO7	VSTO6	VSTO5	VSTO4	VSTO3	VSTO2	VSTO1	VSTO0
			// LSB VSTO  Stop of VGATE pulse
		
	R_17_VGATE_MSB, 0x99,
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		 D0
		//	0		0		0		0		0		0		VSTO_MSB VSTA_MSB
			//	VSTA_MSB  bit: MSB of VSTA 
			//	VSTO_MSB  bit: MSB of VSTB 

	R_40_AC1, 0x20, //0x02,
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		 D0
		//	FISET	HAM_N	FCE		HUNT_N	0		CLKSEL1 CLKSEL0	 0
		//	0		0		0		0		0		0		1		 0
		
			// CLKSEL Amplitude searching Data slicer clock selection
				// 00 Reserved
				// 01 13.5 MHz (default)										
				// 10 Reserved
				// 11 Reserved
			// HUNT_N Amplitude searching
				// 0 Amplitude searching active (default)						
				// 1 Amplitude searching stopped
			// FCE Framing code error
				// 0 One framing code error allowed								
				// 1 No framing code errors allowed
			// HAM_N Hamming check
				// 0 Hamming check for 2 bytes after framing code, 				
					// dependent on data type (default)
				// 1 No Hamming check
			// FISET Field size select
				// 0 50 Hz field rate											
				// 1 60 Hz field rate											

	R_59_H_OFF_FOR_SLICER, 0x47, // recomendado 0x54
		// 	HOFF7 	HOFF6 	HOFF5	HOFF4	HOFF3	HOFF2	HOFF1	HOFF0
			// HOFF Horizontal offset. BITS mais siginificativos estão no reg 0x5B

	R_5A_V_OFF_FOR_SLICER, 0x0a, //0x07, 
		//	VOFF7	VOFF6	VOFF5	VOFF4	VOFF3	VOFF2	VOFF1	VOFF0
			// VOFF Vertical offset BITS mais siginificativos estão no reg 0x5B
				// Valor minimo = 0 e valor máximo = 0x138
				//	0x07 for 50 Hz 625 lines input								
				//	0x0A for 60 Hz 525 lines input								
				
	R_5B_FLD_OFF_AND_MSB_FOR_H_AND_V_OFF, 0x83,
		// 	D7 		D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	FOFF					VOFF8	0		HOFF10 	HOFF9 	HOFF8
		//	1		0		0		0		0		0		1		1
			// HOFF Horizontal offset MSB
			// VOFF Vertical offset MSB									
			// FOFF Field offset
				// 0 no modification of internal field indicator
				// 1 Invert field indicator (even/odd; default)					
	
	0x00, 0x00
};

// #####################################################################################################################################
// #####################################################################################################################################
// I2C
// #####################################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o registrador no saa7113
// Parametros: 	Parametro 1: Ponteiro da estrutura do decoder
//				Parametro 2: Endereço do registrador
// Retorna: 	Valor lido do registrador ou -1 sinalizando erro de leitura
//------------------------------------------------------------------------------------------------------------------
int saa711x_read(struct saa711x_t *dev,  u8 reg) {
	u8 buffer_rx[1];
	u8 buffer_tx[1];
		
	buffer_tx[0] = reg;														// Captura o endereço do registrador a ser acessado
		
	if ( saa711x_i2c_Send(dev->addr_dev, buffer_tx, 1) == pdPASS) {			// Mando o comando para leitura do registrador e checa se o destino reconheceu (ACK) o comando
		if ( saa711x_i2c_Receive(dev->addr_dev, buffer_rx, 1) == pdPASS) 		// Leio o registrador e checo se o pedido de leitura foi reconhecido (ACK) pelo destino
			return buffer_rx[0];											// Se a leitura foi reconhecida pelo destino retorna o valor lido senãi retorna -1 sinalizando erro
	}
	
	return -1;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve no registrador
// Parametros: 	Parametro 1: Ponteiro da estrutura do decoder
//				Parametro 2: Endereço do registrador				
//				Parametro 3: Valor a ser escrito do reg
// Retorna: 	O código da execução de acesso ao BUS I2C
//------------------------------------------------------------------------------------------------------------------
int saa711x_write(struct saa711x_t *dev, u8 reg, u8 value) {
	u8 temp[] = {reg, value};	
	return saa711x_i2c_Send(dev->addr_dev, temp, 2);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve uma sequencia de dados now registradores
// Parametros: 	Parametro 1: Ponteiro da estrutura do decoder
//				Parametro 2: Ponteiro para a lista de dados
// Retorna: 	0 se gravação feita com sucesso, senão retorna -1
//------------------------------------------------------------------------------------------------------------------
int saa711x_write_block(struct saa711x_t *dev, const u8 *regs) {
	u8 reg, data;

	while (*regs != 0x00) {
		reg = *(regs++);
		data = *(regs++);

		// According with datasheets, reserved regs should be   filled with 0 - seems better not to touch on they  
		if (saa711x_has_reg(dev, reg)) {
			if (saa711x_write(dev, reg, data) != pdPASS)
				return -1;
		} 
		#if (DIGITALIZER_USE_DEBUG == pdON)
		else {
			plog("SAA - Tentando acessar o reg reservado 0x%X"CMD_TERMINATOR, reg);
		}
		#endif
	}
	return 0;
}


// ####################################################################################################################################
// ####################################################################################################################################
// UTILITÁRIOS
// ####################################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o registrador pode ser escrito no decoder. Devido que os regs reservado não podem ser escritos
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
//				Parametro 2: registrador para testes
// Retorna:		Retorna 0 se o registrador do parametro 2 for reservado
//				Retorna 1 se o registrador do parametro 2 pode ser escrito
//------------------------------------------------------------------------------------------------------------------
int saa711x_has_reg(struct saa711x_t *dev __attribute__((unused)), const u8 reg) {
	if (unlikely((reg >= 0x3b && reg <= 0x3f) || reg == 0x5c || reg == 0x5f ||
	    reg == 0xa3 || reg == 0xa7 || reg == 0xab || reg == 0xaf || (reg >= 0xb5 && reg <= 0xb7) ||
	    reg == 0xd3 || reg == 0xd7 || reg == 0xdb || reg == 0xdf || (reg >= 0xe5 && reg <= 0xe7) ||
	    reg == 0x82 || (reg >= 0x89 && reg <= 0x8e)))
		return 0;

	return reg != 0x14 && (reg < 0x18 || reg > 0x1e) && (reg < 0x20 || reg > 0x3f) &&
		       reg != 0x5d && reg < 0x63;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializo valores dos regs do decoder
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
static void saa711x_init_values(struct saa711x_t *dev) {
	
	#if (DIGITALIZER_USE_DEBUG == pdON)
	plog("SAA - Escrevendo valores iniciais nos regs do decoder"CMD_TERMINATOR);
	#endif
		
	// ESCVREVENDO VALORES PADRÕES DOS REGISTRADORES
	saa711x_write_block(dev, SAA7113_INIT);		// Escrevo valores iniciais nos regs do SAA7113	
	saa711x_set_v4lstd(dev, V4L2_STD_NTSC);		// Ajusta o padrão NTSC de imagem para digitalização
	
	// ESCREVENDO ENTRADA E SAÍDA DE VIDEO
	struct v4l2_routing route;
	dev->input = NULL;
	dev->output = NULL;	
	route.input = SAA711x_AIN_DEFAULT;
	route.output = SAA711x_AOUT_DEFAULT;		
	saa711x_command(dev, VIDIOC_INT_S_VIDEO_ROUTING, &route);
	dev->input = route.input;
	dev->output = route.output;
	
	// ESCREVENDO ELEMENTOS DE IMAGENS
	struct v4l2_control control;

	control.id = V4L2_CID_BRIGHTNESS;
	control.value = SAA711x_BRIGHT_DEFAULT;
	saa711x_command(dev, VIDIOC_S_CTRL, &control);

	control.id = V4L2_CID_SATURATION;
	control.value = SAA711x_SAT_DEFAULT;
	saa711x_command(dev, VIDIOC_S_CTRL, &control);

	control.id = V4L2_CID_CONTRAST;
	control.value = SAA711x_CONTRAST_DEFAULT;
	saa711x_command(dev, VIDIOC_S_CTRL, &control);

	control.id = V4L2_CID_HUE;
	control.value = SAA711x_HUE_DEFAULT;
	saa711x_command(dev, VIDIOC_S_CTRL, &control);

	dev->bright = SAA711x_BRIGHT_DEFAULT;
	dev->sat = SAA711x_SAT_DEFAULT;
	dev->contrast = SAA711x_CONTRAST_DEFAULT;
	dev->hue = SAA711x_HUE_DEFAULT;
	
}

// ####################################################################################################################################
// ####################################################################################################################################
// CONTROLE
// ####################################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o controle de brilho, contraste, saturação e cor da imagem
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
//				Parametro 2: Ponteiro da estrutura de controle
// Retorno:		Retorna 0 se operação executado com sucesso
//				Retorna -EINVAL se o tipo de controle não existe
//				Retorna -ERANGE se o valor de controle está fora dos limites
//------------------------------------------------------------------------------------------------------------------
static int saa711x_set_v4lctrl( struct saa711x_t *dev, struct v4l2_control *ctrl) {	

	switch (ctrl->id) {
	
	// CONTROLE DE BRILHO
	case V4L2_CID_BRIGHTNESS:
		//if (ctrl->value < 0 || ctrl->value > 255) {
		if ( ctrl->value > 255 ) {
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Ajuste invalido para o briho %d"CMD_TERMINATOR, ctrl->value);
			#endif
			return -ERANGE;
		}

		dev->bright = ctrl->value;										// Guarda o valor da mudança na estrutura do decoder
		saa711x_write(dev, R_0A_LUMA_BRIGHT_CTRL, dev->bright);			// Escreve o controle no devido registrador do decoder
		break;

	// CONTROLE DE CONTRASTE
	case V4L2_CID_CONTRAST:
		//if (ctrl->value < 0 || ctrl->value > 127) {
		if ( ctrl->value > 127) {
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Ajuste invalido para o contrates %d"CMD_TERMINATOR, ctrl->value);
			#endif
			return -ERANGE;
		}

		dev->contrast = ctrl->value;									// Guarda o valor da mudança na estrutura do decoder
		saa711x_write(dev, R_0B_LUMA_CONTRAST_CTRL, dev->contrast);		// Escreve o controle no devido registrador do decoder
		break;

	// CONTROLE DE SATURAÇÃO
	case V4L2_CID_SATURATION:
		//if (ctrl->value < 0 || ctrl->value > 127) {
		if ( ctrl->value > 127) {
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Ajuste invalido para a saturacao %d"CMD_TERMINATOR, ctrl->value);
			#endif
			return -ERANGE;
		}

		dev->sat = ctrl->value;											// Guarda o valor da mudança na estrutura do decoder
		saa711x_write(dev, R_0C_CHROMA_SAT_CTRL, dev->sat);				// Escreve o controle no devido registrador do decoder
		break;

	// CONTROLE DE COR
	case V4L2_CID_HUE:
		if ( ctrl->value > 255) {
		//if (ctrl->value < -127 || ctrl->value > 127) {
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Ajuste invalido para a cor %d"CMD_TERMINATOR, ctrl->value);
			#endif
			return -ERANGE;
		}

		dev->hue = ctrl->value;											// Guarda o valor da mudança na estrutura do decoder
		saa711x_write(dev, R_0D_CHROMA_HUE_CTRL, dev->hue);				// Escreve o controle no devido registrador do decoder
		break;

	default:
		return -EINVAL;	// Retorna erro avisando que o comando de controle não foi reconhecido
	}

	return 0;
}
/*
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o valor do controle de brilho, contraste, saturação e cor da imagem
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
//				Parametro 2: Ponteiro da estrutura de controle
// Retorno:		Retorna 0 se operação executado com sucesso
//				Retorna -EINVAL se o tipo de controle não existe
//------------------------------------------------------------------------------------------------------------------

static int saa711x_get_v4lctrl( struct saa711x_t *dev, struct v4l2_control *ctrl) {
	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		ctrl->value = dev->bright;
		break;
	case V4L2_CID_CONTRAST:
		ctrl->value = dev->contrast;
		break;
	case V4L2_CID_SATURATION:
		ctrl->value = dev->sat;
		break;
	case V4L2_CID_HUE:
		ctrl->value = dev->hue;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
*/
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Verifica a validade do tamanho da imagem
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
// 				Parametro 2 e 3: largura e espessura da imagem a ser decodificada
// Retorno:		Retorna 0 se operação executado com sucesso
//				Retorna erro -EINVAL indicando fora dos limites da imagem para digitalização
//------------------------------------------------------------------------------------------------------------------
static int saa711x_set_size(struct saa711x_t *dev, int width, int height) {
	int is_50hz = dev->std & V4L2_STD_625_50;
	int Vsrc = is_50hz ? 576 : 480;

	// CHECA O LIMITE DA IMAGEM
	if ( (width < 1) || (width > 1440) || (height < 1) || (height > Vsrc) || (width != 720) || (height != Vsrc) ) {
		#if (DIGITALIZER_USE_DEBUG == pdON)
		plog("SAA - Resolucao %ux%u incorreta"CMD_TERMINATOR, width, height);
		#endif
	
		return -EINVAL;
	}
	
	dev->width = width;
	dev->height = height;

	#if (DIGITALIZER_USE_DEBUG == pdON)
	plog("SAA - Decoder digitaliza na resolucao %ux%u"CMD_TERMINATOR,width, height);
	#endif

	return 0;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o tipo de imagem a ser digitalizada
// Parametros:	Parametro 1: Ponteiro para a estrutura do decoder
//				Parametro 2: Padrão de sistema de imagems
//					padrão V4L2_STD_525_60
//						V4L2_STD_PAL_M 		 	V4L2_STD_PAL_60 		V4L2_STD_NTSC_M
//						V4L2_STD_NTSC_M_JP 	 	V4L2_STD_NTSC_M_KR		V4L2_STD_NTSC_443
//					padrão V4L2_STD_625_50
//						V4L2_STD_PAL_B 			V4L2_STD_PAL_B1 		V4L2_STD_PAL_G
//						V4L2_STD_PAL_D			V4L2_STD_PAL_D1  		V4L2_STD_PAL_K 
//						V4L2_STD_PAL_H			V4L2_STD_PAL_I 			V4L2_STD_PAL_N
//						V4L2_STD_PAL_Nc 		V4L2_STD_SECAM_B 		V4L2_STD_SECAM_G 
//						V4L2_STD_SECAM_H		V4L2_STD_SECAM_D 		V4L2_STD_SECAM_K
//						V4L2_STD_SECAM_K1 		V4L2_STD_SECAM_L		
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
static void saa711x_set_v4lstd( struct saa711x_t *dev, v4l2_std_id std) {
	if (std == dev->std)
		return;

	dev->std = std;

	// This works for NTSC-M, SECAM-L and the 50Hz PAL variants.
	// O SAA7113 NÃO TEM ESCALER ENTÃO SÓ DIGITALIZA TAMANHO FIXO, OU SEJA, TAMANHO DE 720x480 A 60HZ OU 720x576 A 50HZ
	if (std & V4L2_STD_525_60) {										// Checa se o padrão 525 60Hz
		#if (DIGITALIZER_USE_DEBUG == pdON)
		plog("SAA - Ajuste decoder para o padrao de 60Hz (NTSC-M, NTSC-J ou PAL-M"CMD_TERMINATOR);
		#endif
		saa711x_write_block(dev, SAA711X_CFG_60HZ_VIDEO);				// Ajusta registradores do decoder para trabalhar com video a 60hz
		saa711x_set_size(dev, 720, 480);								// Verifica a validade do tamanho da imagem
	} else { // V4L2_STD_625_50
		#if (DIGITALIZER_USE_DEBUG == pdON)
		plog("SAA - Ajuste decoder para o padrao de 50Hz"CMD_TERMINATOR);
		#endif
		saa711x_write_block(dev, SAA711X_CFG_50HZ_VIDEO);				// Ajusta registradores do decoder para trabalhar com video a 50hz
		saa711x_set_size(dev, 720, 576);								// Verifica a validade do tamanho da imagem
	}

	// Register 0x0E - Bits D6-D4 on NO-AUTO mode (SAA7111 and SAA7113 doesn't have auto mode)
	//	    50 Hz / 625 lines           60 Hz / 525 lines
	//	000 PAL BGDHI (4.43Mhz)         NTSC M (3.58MHz)
	//	001 NTSC 4.43 (50 Hz)           PAL 4.43 (60 Hz)
	//	010 Combination-PAL N (3.58MHz) NTSC 4.43 (60 Hz)
	//	011 NTSC N (3.58MHz)            PAL M (3.58MHz)
	//	100 reserved                    NTSC-Japan (3.58MHz)
	 
	u8 reg = saa711x_read(dev,  R_0E_CHROMA_CTRL_1) & 0x8f;

	if (std == V4L2_STD_PAL_M) {
		reg |= 0x30;
	} else if (std == V4L2_STD_PAL_N) {
		reg |= 0x20;
	} else if (std == V4L2_STD_PAL_60) {
		reg |= 0x10;
	} else if (std == V4L2_STD_NTSC_M_JP) {
		reg |= 0x40;
	} else if (std & V4L2_STD_SECAM) {
		reg |= 0x50;
	}
	
	saa711x_write(dev, R_0E_CHROMA_CTRL_1, reg);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o padrão utilizado para decodificar a imagem
// Parametros:	Ponteiro para a estrutura do decoder
// Retorna 		Padrão de decodificação
//------------------------------------------------------------------------------------------------------------------
static v4l2_std_id saa711x_get_v4lstd(struct saa711x_t *dev) {
	return dev->std;
}


// ####################################################################################################################################
// ####################################################################################################################################
// COMANDO
// ####################################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emite comandos de controle para o saa7113
// Parametros:	Parametro 1: Ponteiro da estrutura do saa7113
//				Parametro 2: tipo de comando
//				Parametro 3: Lista de argumentos do comando
//					NULL - quando não necessário
// Retorna:		Código de execução do comando
//------------------------------------------------------------------------------------------------------------------
int saa711x_command( struct saa711x_t *dev,  unsigned int cmd, void *arg ) {
	switch (cmd) {

	/*
	#if (DIGITALIZER_USE_DEBUG == pdON)
	// ----------------------------------------------------------------------------
	// BEGUB
	// ----------------------------------------------------------------------------
	// COMANDO PARA VISUALIZAR OS VALORES DOS REGISTRADORES
	case VIDIOC_LOG_DECODER_DUMP: {
		int i, j, data;
		plog("SAA - Registradores do SAA7113"CMD_TERMINATOR);
		j = 0;
		for (i = 0; i < 0x89; i ++) {
			if (saa711x_has_reg(dev, i)) {
				data = saa711x_read(dev, i);
				plog("[%02x = %02x] ",i, data);
				j++;
				if ( j == 8) {
					j = 0;
					plog(CMD_TERMINATOR);
				}
			}			
		}
		break;
	}

	//COMANDO PARA VER STATUS DO DECODER	
	#endif
*/					
	// ----------------------------------------------------------------------------
	// INICIALIA DECODER VALORES PADRÕES
	// ----------------------------------------------------------------------------
	case VIDIOC_STATUS:
		// select mode 0 ( new mode )
		saa711x_write(dev, R_13_MODE_STS, (saa711x_read(dev,  R_13_MODE_STS) & 0xef));
		dev->sts = saa711x_read(dev,  R_1F_STATUS_BYTE_2_VD_DEC);

		// select mode 1 ( old mode )
		saa711x_write(dev, R_13_MODE_STS, (saa711x_read(dev,  R_13_MODE_STS)) | 0x10);
		dev->sts_old = saa711x_read(dev,  R_1F_STATUS_BYTE_2_VD_DEC);
		
		//int signalOk;
	
		//reg1f = saa711x_read(dev,  R_1F_STATUS_BYTE_2_VD_DEC);
	//signalOk = (reg1f & 0xc1) == 0x81;
	//plog("SAA - reg 0x1f : 0x%x"CMD_TERMINATOR, reg1f);
	//plog("SAA - Sinal de video: %s"CMD_TERMINATOR, signalOk ? "ok" : "ruim");
	//plog("SAA - Frequencia : %s"CMD_TERMINATOR, (reg1f & 0x20) ? "60 Hz" : "50 Hz");
		break;

	case VIDIOC_INT_INIT: {
		saa711x_init_values(dev); // Incializo valores dos regs do decoder
		#if (DIGITALIZER_USE_DEBUG == pdON)
			plog("SAA - Init valores"CMD_TERMINATOR);
		#endif
		break;
	}
	
	// ----------------------------------------------------------------------------
	// AJUSTE DE IMAGEM
	// ----------------------------------------------------------------------------
	//  V4L2_CID_BRIGHTNESS
	//	V4L2_CID_CONTRAST:
	//	V4L2_CID_SATURATION:
	//	V4L2_CID_HUE:
	
	// CAPTURA PARAMETROS DE COR, SATURAÇÃO, BRILHO E CONTRASTE DA IMAGEM
	//case VIDIOC_G_CTRL:
		//return saa711x_get_v4lctrl(dev, (struct v4l2_control *)arg);

	// AJUSTA PARAMETROS DE COR, SATURAÇÃO, BRILHO E CONTRASTE DA IMAGEM
	case VIDIOC_S_CTRL:
		return saa711x_set_v4lctrl(dev, (struct v4l2_control *)arg);

	// ----------------------------------------------------------------------------
	// AJUSTA O PADRÃO DA IMAGEM A SER DIGITALIZADA
	// ----------------------------------------------------------------------------
	//  V4L2_STD_PAL_M
	//	V4L2_STD_NTSC       (NTSC_M NTSC_M_JP NTSC_M_KR)
	//	V4L2_STD_PAL_BG		(PAL_B PAL_B1 PAL_G)
	//	V4L2_STD_PAL_DK		(PAL_D PAL_D1 PAL_K)
	//	V4L2_STD_PAL		(PAL_BG	PAL_DK PAL_H PAL_I)
	//	V4L2_STD_SECAM_DK   (SECAM_D	 SECAM_K	SECAM_K1)
	//	V4L2_STD_SECAM		(SECAM_B)
	case VIDIOC_G_STD:
		*(v4l2_std_id *)arg = saa711x_get_v4lstd(dev);
		break;

	case VIDIOC_S_STD:
		saa711x_set_v4lstd(dev, *(v4l2_std_id *)arg);
		break;
	// ----------------------------------------------------------------------------
	// ENTRADA DE VIDEO
	// ----------------------------------------------------------------------------
	// 		INPUT	SAA711x_COMPOSITE0 	SAA711x_COMPOSITE1 	SAA711x_COMPOSITE2 	SAA711x_COMPOSITE3 SAA711x_COMPOSITE4	SAA711x_COMPOSITE5 
	//				SAA711x_SVIDEO0		SAA711x_SVIDEO1		SAA711x_SVIDEO2 	SAA711x_SVIDEO3

	// RETORNA QUAL A ENTRADA DE VIDEO
	case VIDIOC_INT_G_VIDEO_ROUTING:	{
		struct v4l2_routing *route = arg;

		route->input = dev->input;
		route->output = dev->output;
		break;
	}

	// SELECIONA A ENTRADA E SAIDA DE VIDEO
	case VIDIOC_INT_S_VIDEO_ROUTING: {
		struct v4l2_routing *route = arg;

		if ( (route->input > SAA711x_SVIDEO3) || (route->output > SAA711x_AOUT_TESTE2) ) {
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Não existem mais entradas"CMD_TERMINATOR);
			#endif

			return -EINVAL; // Não existe mais entradas
		}

		// AJUSTA ENRADA DE VIDEO
		if (dev->input != route->input) { 			// Somente processa se a mudança for a diferente do valor atual
			#if (DIGITALIZER_USE_DEBUG == pdON)
				plog("SAA - Ajustando entrada de %s %u"CMD_TERMINATOR, (route->input >= SAA711x_SVIDEO0) ? "S-Video" : "video composto", route->input+1);
			#endif
		
			// AJUSTA A ENTRADA DE VIDEO
			dev->input = route->input;

			// select mode  
			saa711x_write(dev, R_02_AINPUT_CTRL_1,
			      (saa711x_read(dev,  R_02_AINPUT_CTRL_1) & 0xf0) |
			       dev->input);

			// bypass chrominance trap for S-Video modes  
			saa711x_write(dev, R_09_LUMA_CTRL,
			      (saa711x_read(dev,  R_09_LUMA_CTRL) & 0x7f) |
			       (dev->input >= SAA711x_SVIDEO0 ? 0x80 : 0x0));
		}
		
		// AJUSTA SAIDA DE VIDEO
		if (dev->output != route->output) {	// Somente processa se a mudança for a diferente do valor atual
			dev->output = route->output;
			#if (DIGITALIZER_USE_DEBUG == pdON)
				switch (route->output) {
					case SAA711x_AOUT_TESTE1:
						plog("SAA - Saida de video canal: teste point 1"CMD_TERMINATOR);
						break;
					case SAA711x_AOUT_AD1:
						plog("SAA - Saida de video canal: AD1"CMD_TERMINATOR);
						break;
					case SAA711x_AOUT_AD2:
						plog("SAA - Saida de video canal: AD2CMD_TERMINATOR");
						break;
					default: 
						plog("SAA - Saida de video canal: teste point 2"CMD_TERMINATOR);
				}
			#endif

			// select mode
			saa711x_write(dev, R_13_OUTPUT_CTRL_3,
			      (saa711x_read(dev,  R_13_OUTPUT_CTRL_3) & 0xfc) | dev->output);
		}
		break;
	}
	
	// ----------------------------------------------------------------------------
	// COMANDO DESCONHECIDO
	// ----------------------------------------------------------------------------	
	default:
		return -EINVAL;
	}

	return 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo o decoder saa7113
// Parametros:	Ponteiro para a estrutura do decoder
// Retorna:		
//------------------------------------------------------------------------------------------------------------------
int saa711x_Init(struct saa711x_t *dev) {	
	int	i;
	char name[17];
	u8 chip_id;
	
	// ----------------------------------------------------------------------------
	// CHECA SE O CHIP DECODER FAZ PARTE DO SAA711X
	// ----------------------------------------------------------------------------
	for (i = 0; i < 0x0f; i++) {
		saa711x_write(dev, 0, i);
		name[i] = (saa711x_read(dev,  0) & 0x0f) + '0';
		if (name[i] > '9')
			name[i] += 'a' - '9' - 1;
	}
	name[i] = '\0';

	saa711x_write(dev, 0, 5);
	chip_id = saa711x_read(dev,  0) & 0x0f;
	
	if (memcmp(name, "1f711", 5)) {	// Checa se o decoder faz parte do saa711x
		#if (DIGITALIZER_USE_DEBUG == pdON)
			plog("SAA - O CI encontrado nao pertence a familia SAA711x. Encontrado %s"CMD_TERMINATOR, name);
		#endif
		return pdFAIL;
	}

	#if (DIGITALIZER_USE_DEBUG == pdON)
		plog("SAA - Encontrado SAA711%d (%s)"CMD_TERMINATOR, chip_id, name);
	#endif
	
	// ----------------------------------------------------------------------------
	// INICIALIZA REGISTRADORES DO DECODER
	// ----------------------------------------------------------------------------
	
	dev->ident = V4L2_IDENT_SAA7113;
	saa711x_init_values(dev); // Incializo valores dos regs do decoder

	return pdPASS;
}




/*






COMANDOS

	// ----------------------------------------------------------------------------
	// 
	// ----------------------------------------------------------------------------
	case VIDIOC_S_FMT:
		return saa711x_set_v4lfmt(dev, (struct v4l2_format *)arg);

	case VIDIOC_G_FMT:
		return saa711x_get_v4lfmt(dev, (struct v4l2_format *)arg);
	

	// ----------------------------------------------------------------------------
	// 
	// ----------------------------------------------------------------------------
	case VIDIOC_INT_DECODE_VBI_LINE:
		saa711x_decode_vbi_line( dev, arg);
		break;

	case VIDIOC_INT_G_VBI_DATA:	{
		struct v4l2_sliced_vbi_data *data = arg;

		// Note: the internal field ID is inverted for NTSC,  so data->field 0 maps to the saa7115 even field, 
		// whereas for PAL it maps to the saa7115 odd field.  
		switch (data->id) {
		case V4L2_SLICED_WSS_625:
			if (saa711x_read(dev,  0x6b) & 0xc0)
				return -EIO;
			data->data[0] = saa711x_read(dev,  0x6c);
			data->data[1] = saa711x_read(dev,  0x6d);
			return 0;
		case V4L2_SLICED_CAPTION_525:
			if (data->field == 0) {
				// CC  
				if (saa711x_read(dev,  0x66) & 0x30)
					return -EIO;
				data->data[0] = saa711x_read(dev,  0x69);
				data->data[1] = saa711x_read(dev,  0x6a);
				return 0;
			}
			// XDS  
			if (saa711x_read(dev,  0x66) & 0xc0)
				return -EIO;
			data->data[0] = saa711x_read(dev,  0x67);
			data->data[1] = saa711x_read(dev,  0x68);
			return 0;
		default:
			return -EINVAL;
		}
		break;
	}









// Decode the sliced VBI data stream as created by the saa7115.
//   The format is described in the saa7115 datasheet in Tables 25 and 26
//   and in Figure 33.
//   The current implementation uses SAV/EAV codes and not the ancillary data
//   headers. The vbi->p pointer points to the R_5E_SDID byte right after the SAV
//   code.  
static void saa711x_decode_vbi_line( struct saa711x_t *dev, struct v4l2_decode_vbi_line *vbi) {
	static const char vbi_no_data_pattern[] = {
		0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0
	};

	u8 *p = vbi->p;
	u32 wss;
	int id1, id2;   // the ID1 and ID2 bytes from the internal header  

	vbi->type = 0;  // mark result as a failure  
	id1 = p[2];
	id2 = p[3];
	// Note: the field bit is inverted for 60 Hz video  
	if (dev->std & V4L2_STD_525_60)
		id1 ^= 0x40;

	// Skip internal header, p now points to the start of the payload  
	p += 4;
	vbi->p = p;

	// calculate field and line number of the VBI packet (1-23)  
	vbi->is_second_field = ((id1 & 0x40) != 0);
	vbi->line = (id1 & 0x3f) << 3;
	vbi->line |= (id2 & 0x70) >> 4;

	// Obtain data type  
	id2 &= 0xf;

	// If the VBI slicer does not detect any signal it will fill up the payload buffer with 0xa0 bytes.  
	if (!memcmp(p, vbi_no_data_pattern, sizeof(vbi_no_data_pattern)))
		return;

	// decode payloads  
	switch (id2) {
	case 1:
		vbi->type = V4L2_SLICED_TELETEXT_B;
		break;
	case 4:
		if (!saa711x_odd_parity(p[0]) || !saa711x_odd_parity(p[1]))
			return;
		vbi->type = V4L2_SLICED_CAPTION_525;
		break;
	case 5:
		wss = saa711x_decode_wss(p);
		if (wss == -1)
			return;
		p[0] = wss & 0xff;
		p[1] = wss >> 8;
		vbi->type = V4L2_SLICED_WSS_625;
		break;
	case 7:
		if (saa711x_decode_vps(p, p) != 0)
			return;
		vbi->type = V4L2_SLICED_VPS;
		break;
	default:
		return;
	}
}
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:	Ponteiro para a estrutura do decoder
// Retorna 		
//------------------------------------------------------------------------------------------------------------------
static int saa711x_get_v4lfmt(struct saa711x_t *dev, struct v4l2_format *fmt) {
	static u16 lcr2vbi[] = {
		0, V4L2_SLICED_TELETEXT_B, 0,	// 1  
		0, V4L2_SLICED_CAPTION_525,		// 4  
		V4L2_SLICED_WSS_625, 0,			// 5  
		V4L2_SLICED_VPS, 0, 0, 0, 0,	// 7  
		0, 0, 0, 0
	};
	
	struct v4l2_sliced_vbi_format *sliced = &fmt->fmt.sliced;
	int i;

	if (fmt->type != V4L2_BUF_TYPE_SLICED_VBI_CAPTURE)
		return -EINVAL;
	
	memset(sliced, 0, sizeof(*sliced));
	
	// done if using raw VBI  
	if (saa711x_read(dev,  R_80_GLOBAL_CTRL_1) & 0x10)
		return 0;
	
	for (i = 2; i <= 23; i++) {
		u8 v = saa711x_read(dev, i - 2 + R_41_LCR_BASE);

		sliced->service_lines[0][i] = lcr2vbi[v >> 4];
		sliced->service_lines[1][i] = lcr2vbi[v & 0xf];
		sliced->service_set |=	sliced->service_lines[0][i] | sliced->service_lines[1][i];
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:	Ponteiro para a estrutura do decoder
// Retorna 		
//------------------------------------------------------------------------------------------------------------------
static int saa711x_set_v4lfmt(struct saa711x_t *dev, struct v4l2_format *fmt) {
	if (fmt->type == V4L2_BUF_TYPE_SLICED_VBI_CAPTURE) {
		saa711x_set_lcr(dev, &fmt->fmt.sliced);
		return 0;
	}
	if (fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	// Verifica a validade do tamanho da imagem
	return saa711x_set_size(dev, fmt->fmt.pix.width,fmt->fmt.pix.height);
}

static int saa711x_odd_parity(u8 c) {
	c ^= (c >> 4);
	c ^= (c >> 2);
	c ^= (c >> 1);

	return c & 1;
}
static int saa711x_decode_wss(u8 * p) {
	static const int wss_bits[8] = {
		0, 0, 0, 1, 0, 1, 1, 1
	};
	u8 parity;
	int wss = 0;
	int i;

	for (i = 0; i < 16; i++) {
		int b1 = wss_bits[p[i] & 7];
		int b2 = wss_bits[(p[i] >> 3) & 7];

		if (b1 == b2)
			return -1;
		wss |= b2 << i;
	}
	parity = wss & 15;
	parity ^= parity >> 2;
	parity ^= parity >> 1;

	if (!(parity & 1))
		return -1;

	return wss;
}


static int saa711x_decode_vps(u8 * dst, u8 * p) {
	static const u8 biphase_tbl[] = {
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xc3, 0x4b, 0x43, 0xc3, 0x87, 0x0f, 0x07, 0x87,
		0x83, 0x0b, 0x03, 0x83, 0xc3, 0x4b, 0x43, 0xc3,
		0xc1, 0x49, 0x41, 0xc1, 0x85, 0x0d, 0x05, 0x85,
		0x81, 0x09, 0x01, 0x81, 0xc1, 0x49, 0x41, 0xc1,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xc2, 0x4a, 0x42, 0xc2, 0x86, 0x0e, 0x06, 0x86,
		0x82, 0x0a, 0x02, 0x82, 0xc2, 0x4a, 0x42, 0xc2,
		0xc0, 0x48, 0x40, 0xc0, 0x84, 0x0c, 0x04, 0x84,
		0x80, 0x08, 0x00, 0x80, 0xc0, 0x48, 0x40, 0xc0,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
	};
	int i;
	u8 c, err = 0;

	for (i = 0; i < 2 * 13; i += 2) {
		err |= biphase_tbl[p[i]] | biphase_tbl[p[i + 1]];
		c = (biphase_tbl[p[i + 1]] & 0xf) | ((biphase_tbl[p[i]] & 0xf) << 4);
		dst[i / 2] = c;
	}
	return err & 0xf0;
}
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:	Ponteiro para a estrutura do decoder
// Retorna 		
//------------------------------------------------------------------------------------------------------------------
// setup the sliced VBI lcr registers according to the sliced VBI format  
static void saa711x_set_lcr( struct saa711x_t *dev, struct v4l2_sliced_vbi_format *fmt) {
	int is_50hz = (dev->std & V4L2_STD_625_50);
	u8 lcr[24];
	int i, x;

	// saa7113/7114/7118 VBI support are experimental  
	if (!saa711x_has_reg(dev, R_41_LCR_BASE))
		return;

	for (i = 0; i <= 23; i++)
		lcr[i] = 0xff;

	if (fmt->service_set == 0) {
		// raw VBI  
		if (is_50hz)
			for (i = 6; i <= 23; i++)
				lcr[i] = 0xdd;
		else
			for (i = 10; i <= 21; i++)
				lcr[i] = 0xdd;
	} else {
		// sliced VBI  
		// first clear lines that cannot be captured  
		if (is_50hz) {
			for (i = 0; i <= 5; i++)
				fmt->service_lines[0][i] =
					fmt->service_lines[1][i] = 0;
		}
		else {
			for (i = 0; i <= 9; i++)
				fmt->service_lines[0][i] =
					fmt->service_lines[1][i] = 0;
			for (i = 22; i <= 23; i++)
				fmt->service_lines[0][i] =
					fmt->service_lines[1][i] = 0;
		}

		// Now set the lcr values according to the specified service  
		for (i = 6; i <= 23; i++) {
			lcr[i] = 0;
			for (x = 0; x <= 1; x++) {
				switch (fmt->service_lines[1-x][i]) {
					case 0:
						lcr[i] |= 0xf << (4 * x);
						break;
					case V4L2_SLICED_TELETEXT_B:
						lcr[i] |= 1 << (4 * x);
						break;
					case V4L2_SLICED_CAPTION_525:
						lcr[i] |= 4 << (4 * x);
						break;
					case V4L2_SLICED_WSS_625:
						lcr[i] |= 5 << (4 * x);
						break;
					case V4L2_SLICED_VPS:
						lcr[i] |= 7 << (4 * x);
						break;
				}
			}
		}
	}

	// write the lcr registers  
	for (i = 2; i <= 23; i++) {
		saa711x_write(dev, i - 2 + R_41_LCR_BASE, lcr[i]);
	}

	// enable/disable raw VBI capturing  
	saa711x_write_block(dev, fmt->service_set == 0 ? saa711x_cfg_vbi_on : saa711x_cfg_vbi_off);
}

// USADO PARA ATIVAR VBI
static const u8 saa711x_cfg_vbi_on[] = {
	R_80_GLOBAL_CTRL_1, 0x00,			// reset tasks  
	R_88_POWER_SAVE_ADC_PORT_CTRL, 0xd0,		// reset scaler  
	R_80_GLOBAL_CTRL_1, 0x30,			// Activate both tasks  
	R_88_POWER_SAVE_ADC_PORT_CTRL, 0xf0,		// activate scaler  
	R_87_I_PORT_I_O_ENA_OUT_CLK_AND_GATED, 0x01,	// Enable I-port output  

	0x00, 0x00
};

// USADO PARA DESATIVAR VBI
static const u8 saa711x_cfg_vbi_off[] = {
	R_80_GLOBAL_CTRL_1, 0x00,			// reset tasks  
	R_88_POWER_SAVE_ADC_PORT_CTRL, 0xd0,		// reset scaler  
	R_80_GLOBAL_CTRL_1, 0x20,			// Activate only task "B"  
	R_88_POWER_SAVE_ADC_PORT_CTRL, 0xf0,		// activate scaler  
	R_87_I_PORT_I_O_ENA_OUT_CLK_AND_GATED, 0x01,	// Enable I-port output  

	0x00, 0x00
};

//static void saa711x_decode_vbi_line( struct saa711x_t *dev, struct v4l2_decode_vbi_line *vbi);
//static int saa711x_odd_parity(u8 c);
//static int saa711x_decode_wss(u8 * p);
//static int saa711x_decode_vps(u8 * dst, u8 * p);
//static int saa711x_get_v4lfmt(struct saa711x_t *dev, struct v4l2_format *fmt);
//static int saa711x_set_v4lfmt(struct saa711x_t *dev, struct v4l2_format *fmt);
//static void saa711x_set_lcr( struct saa711x_t *dev, struct v4l2_sliced_vbi_format *fmt);


























case DECODER_ENABLE_OUTPUT: {
		int *iarg = arg;

		SAA7113H_DEBUG_PRINTK ("DECODER_ENABLE_OUTPUT: %d\r\n", *iarg);

	
		 * RJ: If output should be disabled (for playing videos), we also need
		 * a open PLL. The input is set to 0 (where no input source is
		 * connected), although this is not necessary.
		 * If output should be enabled, we have to reverse the above.
	
		if (*iarg) {
			// Lock horizontal PLL 
			(0x08).HPLL = 0;

			 //Enable real-time output 
			(0x11).OERT = 1;

			// Enable YUV output data 
			(0x11).OEYC = 1;

			/ Handle non-interlaced sources 
			(0x08).FOET = 1;
      
		} else {
			// Unlock horizontal PLL 
			(0x08) HPLL = 1;

			// Disable real-time output 
			(0x11).OERT = 0;

			// Disable YUV output data 
			(0x11).OEYC = 0;
		}

		// Enable the correct output pins 
		(0x12).RTSE0 = 0;
		(0x12).RTSE1 = 2;

		break;
	}


// Autodetection of colour standard 
static void saa7113h_detection (struct saa711x_t *dev) {
	int n, idx, old, detected;

	// detect 50/60Hz 
	saa711x_write(dev, R_08_SYNC_CTRL, ( saa711x_read(dev,  R_08_SYNC_CTRL) ) | 0x80);
	


	// stay here a little eternity 
//	set_current_state (TASK_UNINTERRUPTIBLE);
//	schedule_timeout (HZ/10);
//	set_current_state (TASK_RUNNING);


	
	saa7113h_read (client, (0x1F));

	// set explicitely the detected frequency 
	idx = (0x1F).FIDT ? 1 : 0;
	(0x08).AUFD = SAA7113H_AUFD_FIXED;
	(0x08).FSEL = (0x1F).FIDT;
	saa7113h_write (client, (0x08));

	// save old colour standard 
	saa7113h_read (client, (0x0E));
	old = (0x0E).CSTD;

	// set old status byte 
	(0x13).OLDSB = 1;
	saa7113h_write (client, (0x13));
  
	for (detected = n = 0; n < 8; n++) {
		if (saa7113h_standard[idx][n].standard != -1) {
			// set colour standard 
			(0x0E).CSTD = saa7113h_standard[idx][n].standard;
			saa7113h_write (client, (0x0E));

			// stay here a little eternity 
			set_current_state (TASK_UNINTERRUPTIBLE);
			schedule_timeout (HZ/10);
			set_current_state (TASK_RUNNING);

			// check status byte 
			saa7113h_read (client, (0x1F));
			if ((0x1F).RDCAP) {
				DPRINTK (" %s", saa7113h_standard[idx][n].name);
				detected++;
				break;
			}
		}
	}
  
	// restore old colour standard 
	if (!detected) {
		(0x0E).CSTD = old;
		saa7113h_write (client, (0x0E));
	}

	// set default status byte 
	(0x13).OLDSB = 0;
	saa7113h_write (client, (0x13));

}


*/
