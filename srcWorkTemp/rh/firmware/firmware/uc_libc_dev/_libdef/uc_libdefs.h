#ifndef __UC_LIBDEFS_H
#define __UC_LIBDEFS_H

#define __VERSION "uC_libc2.8"

// ###################################################################################################################
// DIFINIÇÕES DOS BITS
// ###################################################################################################################
#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)
#define BIT_8 (1<<8)
#define BIT_9 (1<<9)
#define BIT_10 (1<<10)
#define BIT_11 (1<<11)
#define BIT_12 (1<<12)
#define BIT_13 (1<<13)
#define BIT_14 (1<<14)
#define BIT_15 (1<<15)
#define BIT_16 (1<<16)
#define BIT_17 (1<<17)
#define BIT_18 (1<<18)
#define BIT_19 (1<<19)
#define BIT_20 (1<<20)
#define BIT_21 (1<<21)
#define BIT_22 (1<<22)
#define BIT_23 (1<<23)
#define BIT_24 (1<<24)
#define BIT_25 (1<<25)
#define BIT_26 (1<<26)
#define BIT_27 (1<<27)
#define BIT_28 (1<<28)
#define BIT_29 (1<<29)
#define BIT_30 (1<<30)
#define BIT_31 (1<<31)

// ###################################################################################################################
// DEFINIÇÕES DE ESCOLHA DE DIPOSTIVOS NAS BIBLIOTECAS
// ###################################################################################################################
// PADRÃO DE ENTRADA E SAIDA
#define STDIO_USE_UART0	0
#define STDIO_USE_UART1	1
#define STDIO_USE_UART2	2
#define STDIO_USE_UART3	3
#define STDIO_USE_LCD	4

// LIBS DOS DISPLAYS
#define LCD_CONNECTION_GPIO		0				// Acesso ao display é via porta IO
#define LCD_CONNECTION_IOMAP  	1				// Acesso ao display é mapeada em memória
#define glcdCONNECTION_GPIO		0				// Acesso ao display é via porta IO
#define glcdCONNECTION_IOMAP  	1				// Acesso ao display é mapeada em memória

// TIPOS DE NIC E ORDEM DO BYTE USADA PELA PILHA TCPI/IP UIP
#define UIP_USE_NIC_RTL8019  	0
#define UIP_USE_NIC_ENC28J60 	1
#define UIP_USE_NIC_AX88796 	2

#define UIP_LITTLE_ENDIAN  3412
#define UIP_BIG_ENDIAN     1234

// ###################################################################################################################
// DEFINIÇÕES DE MACROS
// ###################################################################################################################
#define PI		3.14159265359
#ifndef _BV
#define _BV(n)              (1 << (n))
#endif
#define __BIT_MASK(A) 		(1<<(A))

#define set_bit(x,y) 		(x |= (y)) 					// Seta  o bit y no byte x
#define clear_bit(x,y) 		(x &= (~y)) 				// Limpa o bit y no byte x
#define check_bit(x,y) 		(x & (y)) 					// Checa o bit y no byte x

// Macros de minimo maximo e absoluto
#ifndef min
#define min(a,b)			((a<b)?(a):(b))
#define max(a,b)			(((a)>(b))?(a):(b))
#define constrain(x,a,b)	((x<a)?a:(x>b)?b:x)
#endif
#define arrsizeof(x) 		((sizeof (x))/(sizeof (x [0])))	

#define abs_my(x)			(((x)>0)?(x):(-x))

#define __builtin_expect(x, expected_value) (x)
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define percentage(value, per) 	(n32)(((n64)value * per)/100) //Traz a porcentagem desejada do valor inteiro

// Transforma byte em caractere ascii, os valores ascii deve ser mostrado no formato %c
#define byte2ascii_UpperNibble(x) ((x>>4) < 10) ? (x>>4) + 48 : (x>>4) + 55   			// upper nibble
#define byte2ascii_LowerNibble(x) ((x&0x0f) < 10) ? (x&0x0f) + 48 : (x&0x0f) + 55  		// lower nibble

#if defined(CPU_AVR)	
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond
#define us2cnt(us) ((u16)((u32)(us) / (1000000L / (float)((u32)F_CPU / 1L)))) // Converte microsegundos em valor de contadores
// Mudar e testar estados dos bits

// SOMENTE FUNCIONA COM VARIAVEIS
#define cbi(reg,bit)	reg &= ~(_BV(bit))
#define sbi(reg,bit)	reg |= (_BV(bit))
#define cli()			__asm__ __volatile__ ("cli" ::)
#define sei()			__asm__ __volatile__ ("sei" ::)
#else
#define us2cnt(us) ((u32)((float)(us) / (1000000L / (float)((u32)CCLK / 1L)))) // Converte microsegundos em valor de contadores em relação ao clock da CPU
#define us2cnt_Perif(us) ((u32)((float)(us) / (1000000L / (float)((u32)PCLK / 1L)))) // Converte microsegundos em valor de contadores em relação ao clock dos periféricos
#endif

#define nop()  __asm__ __volatile__ ("nop" ::)


// ###################################################################################################################
// DEFINIÇÕES DE TIPOS DE VARIÁVEIS USADAS NAS BIBLIOTECAS
// ###################################################################################################################

typedef volatile unsigned char 	reg8;
typedef volatile unsigned short reg16;
typedef volatile unsigned long 	reg32;

typedef unsigned char 			u8;
typedef unsigned short 			u16;
typedef unsigned long 			u32;
typedef unsigned long long		u64;

typedef char 					n8;
typedef short 					n16;
typedef long					n32;
typedef long long				n64;

typedef signed char				s8;
typedef signed short			s16;
typedef signed long				s32;
typedef signed long long		s64;

typedef unsigned int			uint;
typedef int						bool;

// não podemos usar como typedef por causa do erro de compilaçãos dos tipos
//	delclarados typedef unsigned portLONG portTickType;
// principalmente no freeRTOS
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long

#if defined(CPU_AVR)
	#define portSHORT		int
	#define portSTACK_TYPE	unsigned portCHAR
	#define __STRINGSIZE__ 		32
	typedef volatile unsigned short regCpu;
#elif defined(LINUX)
	#define __STRINGSIZE__ 		256
#else
	#define portSHORT		short
	#define portSTACK_TYPE	unsigned portLONG
	#define __STRINGSIZE__ 		64
	typedef volatile unsigned long regCpu;
#endif

// Defines the prototype to which task functions must conform.  Defined in this file to ensure the type is known before portable.h is included.
typedef void (*TaskFunction_t)( void * );

typedef const char* 			cpchar;
typedef char* 					pchar;
typedef char 					string[__STRINGSIZE__];

typedef struct {
	int x,y;
}tPoint;

// Retangulo pos X e Y, e com largura e altura
typedef struct {
	int x, y, w ,h;
}tRect;

// Retangulo com centro, largura e altura
typedef struct {
	int cx, cy, w ,h;
}tRectC;

#define RECTX_XI(r)		(r.cx-r.w/2)
#define RECTX_XF(r)		(r.cx+r.w/2)
#define RECTX_YI(r)		(r.cy-r.h/2)
#define RECTX_YF(r)		(r.cy+r.h/2)
#define RECT_XF(r)		(r.cx+r.w)
#define RECT_YF(r)		(r.cy+r.h)


// ###################################################################################################################
// CÓDIGO DAS FUNÇÕES DE RETORNO. O TIPO DE RETORNO DAS FUNÇÕES DEVE SER int
// ###################################################################################################################
// 		Todas as routinas de retorno é do tipo int
// UC_LIBC. Deve ser entre -65535 a 65536, isto por causa dos uC de 8 bits onde int é de 16 bits

// GERAL
#define pdTRUE				( 1 )
#define pdFALSE				( 0 )
#define pdPASS				( 1 )
#define pdOK				( 1 )
#define pdNO				( 0 )
#define pdFAIL				( 0 )
#define pdON 				( 1 )
#define pdOFF 				( 0 )
#define pdREADY				pdPASS
#define pdNOT_READY			pdFAIL
#define pdBUSY				pdFAIL


#define TIMEOUT_1SEC		1000000L
#define TIMEOUT_2SEC		2000000L
#define TIMEOUT_3SEC		3000000L
#define TIMEOUT_4SEC		4000000L
#define TIMEOUT_5SEC		5000000L
#define TIMEOUT_6SEC		6000000L
#define TIMEOUT_7SEC		7000000L
#define TIMEOUT_8SEC		8000000L
#define TIMEOUT_9SEC		9000000L

#ifndef EOF
#define EOF 	(int)(-1)
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

// -------------------------------------------------------------------------------------------------------------------
// CÓDIGOS DE ERROS
//		Se não haver erros retorne pdPASS

// FREE RTOS
typedef void (*pdTASK_CODE)( void * );
#define errQUEUE_EMPTY							(-1)
#define errQUEUE_FULL							(-1)
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY	(-2)
#define errNO_TASK_TO_RUN						(-3)
#define errQUEUE_BLOCKED						(-4)
#define errQUEUE_YIELD							(-5)
#define errQUEUE_CREATE_ERROR					(-6)
#define errSEMAPHORE_CREATE_ERROR				(-7)

#define errMALLOC_ERROR							(-8)
#define errREG_EMPTY							(-9)

#define errBUFFER_FULL							(-10)
#define errBUFFER_EMPTY							(-11)
#define errBUFFER_NO_EXIST						(-12)

#define errPASSWORD_INVALID						(-13)

#define errI2C_NO_RESPONSE						(-20)
#define errI2C_ERROR							(-21)
#define errI2C_DEVICE_BUSY						(-22)
#define errI2C_DEVICE_NO_DATA					(-23)

#define errSPI_DEVICE_NO_DATA					(-30)
#define errSPI_ERROR_SET_MODE					(-31)
#define errSPP0_ERROR_SET_MODE					(-31)
#define errSPP1_ERROR_SET_MODE					(-31)
	
#define errADC_LOST_DATA						(-40)

#define errTIMEROUT								(-50)

#define errCLI_EXED_ARGS						(-60)
#define errCLI_MANY_ARGS						(-61)
#define errCLI_FEW_ARGS							(-62)
#define errCLI_CMD_NOT_FOUND					(-63)

#define errMODEM_CANCEL_REMOTE					(-70)
#define errMODEM_USER_CANCEL					(-71)
#define errMODEM_SYNC							(-72)
#define errMODEM_MANY_RETRANSMISSION			(-73)
#define errMODEM_XINIT 							(-74)
#define errMODEM_NACK 							(-75)
#define errMODEM_FILE_EMPTY 					(-76)
#define errMODEM_NO_RESPONSE					(-77)
#define errMODEM_ACK							(-78)
#define errMODEM_CHECKSUM						(-79)
#define errMODEM_TIMEROUT						(-80)
#define	errMODEM_MEM_OVERFLOW					(-81)
#define errMODEM_UNKNOWN						(-82)
#define errMODEM_COMMAND_INVALID				(-83)

#define errSPI_FLASH_SEFTEST					(-90)
#define errSPI_FLASH_WRITE						(-91)
#define errSPI_FLASH_ERASE						(-92)

#define errIAP_COMMAND_INVALID         			(-93)
#define errIAP_SRC_ADDR			      			(-95)
#define errIAP_DST_ADDR			       			(-96)
#define errIAP_SRC_ADDR_NOT_MAPPED 				(-97)
#define errIAP_DST_ADDR_NOT_MAPPED 				(-98)
#define errIAP_COUNT			       			(-99)
#define errIAP_SECTOR_INVALID       			(-90)
#define errIAP_SECTOR_NOT_BLANK     			(-100)
#define errIAP_SECTOR_NOT_PREPARED 				(-101)
#define errIAP_COMPARE		        			(-102)
#define errIAP_BUSY                 			(-103)

#define errIAP_ADDR_INVALID           			(-104)
#define errIAP_SIZE_INVALID          			(-105)
#define errIAP_PARAM_INVALID           			(-106)
#define errIAP_NO_SAFE_REGION          			(-107)

#define errINTEL_HEX_DELIMITER 					(-108)
#define errINTEL_HEX_CHECKSUM 					(-109)
#define errINTEL_HEX_NUMBER_INVALID				(-110)
#define errINTEL_HEX_COMMAND_INVALID			(-111)

#define errNIC_NO_RESPONSE						(-112)

// MODBUS
#define errMODBUS_BUFRX_INCOMPLETE				(-120)
#define errMODBUS_BUFRX_OVERFLOW 				(-121)
#define errMODBUS_CRC 							(-122)
#define	errMODBUS_TX 							(-123)
#define errMODBUS_TIMEOUT 						(-124)
#define errMODBUS_BUFFER_OVERFLOW 				(-125)
#define errMODBUS_LENPACKET 					(-126)
#define errMODBUS_ID 							(-127)
#define errMODBUS_ADDR 							(-128)
#define errMODBUS_CMD 							(-129)
#define errMODBUS_EXCEPTION						(-130)
#define errMODBUS_LEN 							(-131)
#define errMODBUS_VALUE 						(-132)
#define errMODBUS_OPEN_UART 					(-133)
#define errMODBUS_BUSY 							(-134)

// USB HOST
#define errUSB_TD_FAIL              			(-150)
#define errUSB_DEVICE_NO_PRESENT   				(-151)

// USB HOST MASS STORE
#define errUSB_MS_CMD_FAILED       	   			(-152)
#define errUSB_MS_BAD_CONFIGURATION   			(-153)
#define errUSB_MS_NO_INTERFACE   				(-154)

// ADXL345
#define errADXL345_NO_CONNECTED					(-200) // Não está conectado
#define errADXL345_BAD_ARG						(-201) // bad method argument
#define errADXL345_READ_ERROR 					(-202) // problem reading accel

// DEVICES
#define errDEVICE_NOT_FOUND						(-300)
#define errDEVICE_NOT_READY						(-301)
#define errDEVICE_VALUE_NOT_READY				(-302)
#define errDEVICE_VALUE_INVALID					(-303)
#define errDEVICE_VALUE_OUT_OF_RANGE			(-304)

// DISCOS
#define errDISK_INIT							(-400)
#define errDISK_MOUNT							(-401)
#define errDISK_FILE_OPEN						(-402)
#define errDISK_FILE_READ						(-403)

// ISP
#define errISP_PROGRAM_TOO_LARGE				(-500)
#define errISP_NO_RESPONSE						(-501)
#define errISP_SYNCHRONIZED						(-502)
#define errISP_SET_OSCILATOR					(-503)
#define errISP_UNLOCK							(-504)
#define errISP_DEVICE_TYPE						(-505)
#define errISP_DEVICE_CHIP_VARIANT				(-506)
#define errISP_ERASE_FLASH_ALL					(-507)
#define errISP_PREPARE_TO_PROG					(-508)
#define errISP_WRITE_RAM						(-509)
#define errISP_WRITE_FLASH						(-510)
#define errISP_CHECSUM							(-511)
#define errISP_SEND_BYTES						(-512)

// ADXL345
#define errPCA9685_NO_CONNECTED					(-600) // Não está conectado


// -------------------------------------------------------------------------------------------------------------------
// CPU_ARM_LPC
#define errIRQ_INSTALL							( -1000 )
#define errIRQ_SLOT_FULL						( -1001 )
#define errIRQ_SLOT_OUT_OF_RANGE				( -1002 )


// ###################################################################################################################
// CONTROLE DE DISCO DE ARMAZENAMENTOS DE DADOS
// ###################################################################################################################

// TIPO DE DISCO DE ARMAZENAMENTO DE DADOS
typedef enum {
  	DISK_TYPE_MMC = 0, 	// MMC ver 3
  	DISK_TYPE_SD1,		// SD ver 1
  	DISK_TYPE_SD2,		// SD ver 2
  	DISK_TYPE_SDC,		// SD
  	DISK_TYPE_SD_BLOCK,	// Endereçamen to por blocos
  	DISK_TYPE_UNKNOWN,
} disk_type_t;

// ESTRUTURA PARA GUARDAR DADOS ESPCIFICOS DO CARTÃO EM USO
typedef struct {
  	u32  block_number;
  	u32  block_size;
  	disk_type_t disk_type;
 	bool write_protect;
} tDiskStatus;


// ###################################################################################################################
// CORES
// ###################################################################################################################
typedef unsigned long 			tColor;

#define RGB24(r,g,b)			(tColor)((u8)r|((u8)g<<8)|((u8)b<<16))
#define RGB16(r,g,b)			(tColor)((u8)((b>>3)&0x1F)|((u8)((g>>2)&0x3F)<<5)|((u8)((r>>3)&0x1F)<<11))

// ###################################################################################################################
// TFT e TOUCH
// ###################################################################################################################
#define TFT_ROTATE_NONE		0 	// Não rotacionar TFT
#define TFT_ROTATE_UD 		1 	// Rotacionar TFT 180º deixando a tela de cabeça para baixo;
//#define TFT_ROTATE_CW		// Rotacionar TFT no sentido horário (90º);
//#define TFT_ROTATE_CCW - Rotacionar no sentido anti-horário (90º).

#define TOUCH_USE_AD 			0 	// Utiliza a porta AD do uC para leitura do touchscreen
#define TOUCH_USE_TSC2046 		1	// Utiliza o CI TSC2046 para leitura do touchscreen VIA SPI


// ###################################################################################################################
// DISPOSITIVOS DE CONTROLE REMOTO INFRAVERMELHO
// ###################################################################################################################
#define IRRC_JAPANESE 	0 // Padrão recomendado: Atende aparelhos da york, controle remotor universais
#define IRRC_DUGOLD 	1 // Modelo do controle da placa universal dugold da dufrio
#define IRRC_SAMSUNG 	2 // Aparelhos da samsung e electrolux

// ###################################################################################################################
// DISPOSITIVOS STDIO
// ###################################################################################################################
#define STDIO_LPRINTF	1
#define STDIO_DEBUG		2
#define STDIO_ERROR		3
#define STDIO_DEV1 		4
#define STDIO_DEV2 		5
#define STDIO_DEV3 		6
#define STDIO_DEV4 		7

#endif
