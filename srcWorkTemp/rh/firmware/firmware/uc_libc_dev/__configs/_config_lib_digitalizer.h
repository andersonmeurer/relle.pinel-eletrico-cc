#ifndef __CONFIG_LIB_DIGITALIZER_H
#define __CONFIG_LIB_DIGITALIZER_H

// ###################################################################################################################
// CONFIGURAÇÕES DE INICIALIZAÇÃO DO SAA7113

//		Filtro antialiasing e AFC ativados.
//		Habilitado a detecção atumática de campo
//		Padrão ITU-R BT656 com controle de brilho, contraste e saturação para esse padrão
//		Processamento de croma cam largura de banda nominal (800Khz)


// ************************
// VALORES PADRÕES DOS REGS
// ************************
#define SAA711x_AIN_DEFAULT 	1 // COMPOSITE 1
		// 0 = COMPOSITE 0
		// 1 = COMPOSITE 1
		// 2 = COMPOSITE 2
		// 3 = COMPOSITE 3
		// 6 = SVIDEO 0
		// 7 = SVIDEO 1
		// 8 = SVIDEO 2
		// 9 = SVIDEO 3

#define SAA711x_AOUT_DEFAULT 	1 // AOUT_AD1
		// 0 = AOUT_TESTE1
		// 1 = AOUT_AD1
		// 2 = AOUT_AD2
		// 3 = AOUT_TESTE2

#define SAA711x_BRIGHT_DEFAULT 		0x80 	// recomendado 0x80
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			// brightness level 		
	
#define SAA711x_CONTRAST_DEFAULT 	0x43 	// recomendado 0x47  - IP Video 0x43
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			// val: contrast gain
				// 0b10000000: -2 (inverse luminance)
			  	// 0b11000000: -1 (inverse luminance)
			  	// 0:	      0	 (luminance off)
			  	// 64->127:    1->1.999
			  	// 0b01000111: 1.109 (CCIR value)
	
#define SAA711x_SAT_DEFAULT		0x43	 	// recomendado 0x40	 - IP Video 0x43
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
	  		
	  		// val: chrominance saturation control gain
			  	// 0b10000000: -2 (inverse luminance)
			  	// 0b11000000: -1 (inverse luminance)
			  	// 64->127:    0->1.999
			  	// 0b01000000: 1.0 (CCIR value)
			
#define SAA711x_HUE_DEFAULT 	0x00		// recomendado 0x00
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			// val: chrominance hue control (phase). SIGNED
				// -128: -180 degrees
				// +127: +178.6 degrees

// ************************
// VALORES DOS REGS
// ************************
#define SAA711x_R_01_INC_DELAY	0x08		// recomendado 0x08
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		 	// bit0:	disable increment delay
		 	// other: delay value (NOTE 000 is max, 111 is min)

#define SAA711x_R_02_AINPUT_CTRL_1 0xF0		// recomendado 0xcX - IP Video 0xF0
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	FUSE1 	FUSE0 	GUDL0 	GUDL1 	MODE3 	MODE2 	MODE1 	MODE0
		//	1		1		0		0		x		x		x		x
		
			// MODE select: operational mode
				// 0000: CVBS from analog input A11 (automatic gain)
			 	// 0001: CVBS from analog input A12 (automatic gain)
			 	// 0010: CVBS from analog input A21 (automatic gain)
			 	// 0011: CVBS from analog input A22 (automatic gain)
			// GUDL1
				// val: update hysterisis for 9 bit gain: +/- 0/1/2/3 LSB 
			// FUSE select: amplifier & anti-alias activation
			   // 00: desactivated
			   // 01: desactivated
			   // 10: amplifier active
			   // 11: amplifier & anti-alias both active

#define SAA711x_R_03_AINPUT_CTRL_2	0x03 	//recomendado 0x33 - IP Video 0x03
		// 	D7		D6		D5	 	D4	 	D3		D2		D1		D0
		// 	0		HLNRS	VBSL	WPOFF	HOLDG	GAFIX	GAI28	GAI18
		//	0		0		1		1		0		0		1		1
			// GAI18 GAI28
			// GAFIX select: gain control adjustment
		     	// 0 : gain controlled by MODE
		     	// 1 : gain is user programmable								
			// HOLDG select: automatic gain integration
			    // 0: active
			    // 1: freeze
			// WPOFF select: white peak off
			     // 0: white peak off active									
			     // 1: white peak off
			// VBSL select: AGC hold during vertical blanking period
			     // 0: short, AGC disabled during equal and serat. pulses)
			     // 1: long,  AGC disabled until start of video
			     // 	line 22 for NTSC, line 24 for PAL
			// HLNRS select: HL not referenced select
			     // 0: normal clamping if decoder is in unlocked state			
			     // 1: reference select if decoder is in unlocked state
			// 0 - deve ser escrito 0
		
#define SAA711x_R_04_AINPUT_CTRL_3 	0x00	// recomendado 0x00
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			 // val: gain control analog; static gain channel 1
			 // .    MSB set by GAI1_MSB
			 // . 0: ~ -3dB
			 // 127: ~ 0
			 // 511: ~ +6dB
		
#define SAA711x_R_05_AINPUT_CTRL_4 0x80	// recomendado 0x00 - 0x80 IP Video
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			 
			 // val: gain control analog; static gain channel 2
			 // .    MSB set by GAI2_MSB
			 // . 0: ~ -3dB
			 // 127: ~ 0
			 // 511: ~ +6dB

#define SAA711x_R_06_H_SYNC_START 0xe9	// recomendado 0xe9
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			// val: horizontal sync start (step size = 8/LLC)
			// .    valid range is limited to [-108;+108]
			// recommended: 0b11101001

#define SAA711x_R_07_H_SYNC_STOP 0x0d 	// recomendado 0x0d
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
			
			// val: horizontal sync stop (step size = 8/LLC)
			// .    valid range is limited to [-108;+108]
			// recommended: 0b00001101
		
#define SAA711x_R_08_H_SYNC_CTRL 0x88  	// Para 60Hz 525 Linha (NTSC-M, NTSC-J ou PAL-M Recomendado 0x98 - IP Video 0x88
										// Para 50Hz 625 Linha Recomendado 0x28
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	AUFD	FSEL	FOET	HTC1	HTC0	HPLL 	VNOI1 	VNOI0
		//	0		1		0		?		?		0		0		0
				
			// VNOI select: vertical noise reduction
			 	// 00: normal mode (recommended)									
			 	// 01: fast mode (for stable sources). AUFD must be disabled.
			 	// 10: free running mode (???)
			 	// 11: vertical noise reduction bypassed.
			// HPLL select: PLL mode
			 	// 0: closed														
			 	// 1: open, horizontal frequency fixed
			// HTC select: horizontal constant selection
			 	// 00: TV mode (poor quality signals)
			 	// 01: VTR mode (recommended with a detection control circuit		
			 	//    	is connected directly to SAA7113H
			 	// 10: reserved
			 	// 11: fast locking mode (recommended)								
			// FOET select: forced odd/even toggle
			 	// 0: odd/even signal toggles only with interlaced sources			
			 	// 1: odd/even signal toggles even if source in non interlaced		
	 		// FSEL select: field selection
			 	// 0: 50Hz, 625 lines									
			 	// 1: 60Hz, 525 lines												
			// AUFD select: automatic field (PAL/SECAM/NTSC) detection
			 	// 0: field state directly controlled via FSEL						
			 	// 1: Automatic field detection									

#define SAA711x_R_09_LUMA_CTRL	0x10	// recomendado 0x01 - IP Video 0x10
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	BYPS 	PREF	BPSS1	BPSS0	VBLB	UPTCV	APER1	APER0
		//	0		0		0		0		0		0		0		1
		
			// APER select: luminance aperture factor
			  	// 00: factor = 0													
			  	// 01: factor = 0.5													
			  	// 10: factor = 0.5
			  	// 11: factor = 1
			// UPTCV select: update time for analog AGC value
			  	// 0: horizontal update (once a line)								
			  	// 1: vertical update (once per field)
		  	// VBLB select: vertical blanking luminance bypass
			  	// 0: active luminance processing									
			  	// 1: chrominance trap and peaking stage are disabled during
			  	// .  VBI lines determined by VREF=0
			// BPSS select: aperture band-pass (centre frequency)
			  	// 00: 4.1 MHz														
			  	// 01: 3.8 Mhz, (not to be used with bypass chrominance trap)
			  	// 01: 2.6 Mhz, (not to be used with bypass chrominance trap)
			  	// 01: 2.9 Mhz, (not to be used with bypass chrominance trap)
			// PREF bool: prefilter active
			// BYPS Chrominance trap bypass:
			  	// 0: chrominance trap active; default for CVBS mode				
			  	// 1: chrominance trap bypassed; default for S-Video !!!!
	

#define SAA711x_R_0E_CHROMA_CTRL_1 0x01 	// recomendado 0x01
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	CDT0	CSTD2 	CSTD1 	CSTD0 	DCCF	FCTC	CHBW1	CHBW0
		// 	0		x		x		x		0		0		0		1			
			
			// CHBW select: chrominance bandwidth
			 	// 00: small bandwidth (~ 620 KHz)
			 	// 01: nominal bandwidth (~ 800 KHz)							
			 	// 10: medium bandwidth (~ 920 Mhz)
			 	// 11: wide bandwidth (~ 1000 Mhz)								
			// FCTC select: fast colour time constant
			 	// 0: nominal time constant										
			 	// 1: fast time constant										
			// DCCF select: disable chrominance comb filter
			 	// 0: filter on (during lines determined by VREF=1)				
			 	// 1: filter permanently off
			// CSTD select: colour standard definition
			  	// 000: PAL BGHIN	 / NTSC-M or NTSC-Japan						
			  	//     for Japan: brightness=0x95, contrast=0x48
			  	// 001: NTSC 4.43 (50 Hz) / PAL 4.43 (60 Hz)
			  	// 010: combination PAL-N / NTSC 4.43 (60Hz)
			  	// 011: NTSC-N		 / PAL M
			  	// 101: SECAM		 / reserved
			  	// xxx: other values should not be set (SECAM ME ?)
		 	// CDT0 select: clear DTO
			 	// 0: disabled													
			 	// 1: every time CDTO carrier is set, the internal subcarrier
			 	//   DTO phase is reset to 0 (degrees) and the RTCO output
			 	//   generates a logic 0 at time slot 68... blah blah

				
#define SAA711x_R_0F_CHROMA_GAIN_CTRL 0x2b 	// recomendado 0x2a	 - IP Video 0x2b
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	ACGC	CGAIN6	CGAIN5	CGAIN4	CGAIN3	CGAIN2	CGAIN1	CGAIN0
		//	0		0		1		0		1		0		1		0
		
			// CGAIN val: chrominance gain control 0.5 -> 7.5
			  	// 0b0100100: nominal gain
			// ACGC select: automatic gain control
			  	// 0: on														
			  	// 1: programmable gain via CGAIN
			
#define SAA711x_R_10_FORMAT_DELAY_CTRL 0x08  // recomendado 0x00  - IP Video 0x08
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	OFTS1 	OFTS0 	HDEL1 	HDEL0 	VRLN 	YDEL2 	YDEL1 	YDEL0
		//	0		0		0		0		0		0		0		0
		
			// YDEL val: luminance delay compensation: -4 -> +3 
			// VRLN select: VREF pulse position and length (ITU number in par.)
			 	// 0 60Hz: len=240 f1=19(22)->258(261) f2=282(285)->521(524)
			 	// 0 50Hz: len=286 f1=24->309 f2=337->622
			 	// 1 60Hz: len=242 f1=19(21)->259(262) f2=281(284)->522(525)
			 	// 1 50Hz: len=288 f1=23->310 f2=336->623
		    // HDEL val: fine position of HS: 0->3 
			// OFTS select: output format selection
			 	// 00: standard ITU 656 format									
			 	// 01: V-flag in SAV/EAV generated by VREF
			 	// 10: V-flag in SAV/EAV generated by data type
			 	// 11: reserved
			 	
#define SAA711x_R_11_OUTPUT_CTRL_1 0x8 	// recomendado 0x0c - IP Video 0x0c
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	GPSW1	CM99	GPSW0 	HLSEL 	OEYC 	OERT 	VIPB 	COLO
		//	0		0		0		0		1		1		0		0
		
			// COLO bool: enable colour 
				// 0:															
				// 1: Color forced on
			// VIPB select: YUV decoder bypassed
			  	// 0: processed data to VPO output								
			  	// 1: ADC data to VPO output depending on mode settings
			// OERT select: output enable real-time
			  	// 0: RTS0, RTS1, RTCO high inpedance inputs					
			  	// 1: RTS0, RTS1, RTCO active if RTSE[13:10] = 0000				
			// OEYC select: output enable YUV data
			  	// 0: VPO high impedance
			  	// 1: output VPO active or controlled by RTS1					
			// HLSEL elect: selection of horizontal lock indicator for RTS0,
			  	// .	  RTS1
			  	// 0: standard horizontal lock indicator (low-passed)			
			  	// 1: fast lock indicator (use recommended for hi-perf signals	
			// GPSW0 bool: general purpose switch on RTS0 if RTSE0=0x0010
				// 0: clear pin RTS1											
				// 1: set pin RTS1
			// CM99 select: CM99 compatibility to SAA7199
			  	// 0: default value												
			  	// 1: to be set only if SAA7199 present
			// GPSW1 bool: general purpose switch on RTS1 if RTSE1=0x0010 
				// 0: clear pin RTS1											
				// 1: set pin RTS1
		
#define SAA711x_R_12_OUTPUT_CTRL_2	0x01 	//0xd7, // recomendado 0x01  - IP Video 0xb7
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		// 	RTSE13	RTSE12	RTSE11	RTSE10	RTSE03	RTSE02	RTSE01	RTSE00
		//	1		1		0		0		0		1		1		1
		
			// RTSE0x select: RTS0 output control:
			// RTSE1x select: RTS1 output control:
			  	
			  	// 0000 - 3-state, RTS used as dot input
			  	// 0011 - horizontal lock:
			  		//  HLSEL = 0: standard horizontal lock indicator
			  		//  HLSEL = 1: fast mode (not for VCRs)
			  	// 0010 - Pino de saida ajudtado pelo reg 0x11 bit GPSWx
			  	// 0111 - Indica dados validos no VPO
			  	// 1011 - Pulso vertical VS
			  	// 1100 - Pulso vertical V123
			  	// 1101 - Pino acionado pelo VGATE. Valores de inicio e fim do VGATE config nos regs 0x15 a 0x17
			
#define SAA711x_R_13_OUTPUT_CTRL_3	0x11  // - IP Video 0x11
		// 	D7	 	D6	 	D5	 	D4	 	D3		D2		D1		D0
		//	ADLSB	0		0		OLDSB	FIDP	0		AOSL1	AOSL0  			(AOSLx = SAA711x_AOUT_DEFAULT)
		//	0		0		0		0		0		0		x		x
		
			// AOSL select: AOUT connected to:
			  	// 00 - internal test point 1											
			  	// 01 - input AD1
			  	// 02 - input AD2
			  	// 03 - internal test point 2
			// FIDP select: field ID polarity if RTS set to 0x1111:
			  	// 0 - default
			  	// 1 - inverted
			// OLDSB  bool: old status byte (compatibility) 
			// ADLSB A2D output bits on VPO in bypass mode (VIPB=1, test)
		

#define SAA711x_R_58_PROGRAM_FRAMING_CODE 	0x40 // recomendado 0x0 - IP Video 0x40
		// 	FC7	 	FC6		FC5	 	FC4	 	FC3		FC2		FC1		FC0
			// FC Framing code for programmable data types
	
		
#define SAA711x_R_5E_SDID 	0x00	 //0x35, // recomendado 0x0
		// 	0		0		SDID5 	SDID4	SDID3 	SDID2	SDID1	SDID0
			// SDID codes
	
#endif
