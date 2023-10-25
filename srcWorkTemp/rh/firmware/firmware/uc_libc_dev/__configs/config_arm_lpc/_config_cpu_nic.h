#ifndef __CONFIG_CPU_NIC_H
#define __CONFIG_CPU_NIC_H

#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURA��ES DA RTL8019AS

#define RTL_PORT_DIR GPIO1_DIR
#define RTL_PORT_CLR GPIO1_CLR
#define	RTL_PORT_SET GPIO1_SET
#define	RTL_PORT_IN  GPIO1_PIN
#define RTL_PIN_RST	(1<<31)

// CONFIGURA��ES PARA MODO GPIO
#define RTL_CONNECTION_GPIO			0				// Acesso a NIC por IO
	#define RTL_PIN_D0		(1<<16) 
	#define RTL_PIN_D1		(1<<17)
	#define RTL_PIN_D2		(1<<18)
	#define RTL_PIN_D3		(1<<19)
	#define RTL_PIN_D4		(1<<20)
	#define RTL_PIN_D5		(1<<21)
	#define RTL_PIN_D6		(1<<22)
	#define RTL_PIN_D7		(1<<23)
	#define RTL_PIN_A0		(1<<24)
	#define RTL_PIN_A1		(1<<25)
	#define RTL_PIN_A2		(1<<26)
	#define RTL_PIN_A3		(1<<27)
	#define RTL_PIN_A4		(1<<28)	
	#define RTL_PIN_RD		(1<<29)
	#define RTL_PIN_WR		(1<<30)
	
	#define RTL_DELAY_US 	1000

// CONFIGURA��ES PARA MODO RAM
#define RTL_CONNECTION_IOMAP  		1				// Acesso a NIC � mapeada dentro da mem�ria nas linhas de endere�o A0 a A4
	#define RTL_IOMAP_OFFSET 		0x82000000		// Endere�o de OFFSET para acessar a NIC

// AJUSTA O TIPO DE ACESSO A RTL
#define RTL_CONNECTION RTL_CONNECTION_GPIO		
//						RTL_CONNECTION_GPIO  Acesso a NIC via porta GPIO
//						RTL_CONNECTION_IOMAP Acesso a NIC como RAM


// VALORES INCIAIS DOS RESGISTRADORES DA RTL
// -------------------------------------------
#define RTL_RCR_INIT 	0x04 
	// Pacotes recebidos s�o armazenados no buffer ring
	// Somemte aceita pacotes se o endere�o MAC for igual aos registradores PAR[0..5]
	// Pacotes multicast s�o rejeitados
	// Pacotes broadcast s�o aceitos
	// Pacotes menores que 64 bytes ser�o rejeitados
	// Pacotes com CRC inv�lido ser�o rejeitados

#define RTL_DCR_INIT 	0x58
	// FIFO threshold: 8-bits
	// DMA: Transferencia a 8-bit e byte order 8086

#define RTL_IMR_INIT	0x11
	// Habilita a interrup��o de overflow de recep��o
	// Habilita a interrup��o de chegada de dados sem erros

// POSI��O DE MEM�RIA FIFO. Endere�o da RAM vai de 0x4000 a 0x8000
// A MEM�RIA ESTA DEVIVIDA EM BUFFER DE 256 BYTES CHAMADAS DE P�GINAS
//-------------------------------------------------------------------
#define RTL_TPSR_INIT  	0x40	// Endere�o inicial da p�gina para o buffer de transmiss�o ( 6 paginas cont�m 1536 bytes comporta 1 pacote de tamanho m�ximo ) 
#define RTL_PSTART_INIT 0x46	// Endere�o inicial da p�gina para o buffer de recep��o (26 p�ginas cont�m 6656 bytes comporta 4 pacotes de tamanho m�ximo ) 
#define RTL_PSTOP_INIT 	0x60	// Endere�o final da p�gina para o buffer de recep��o

// VALORES PADR�ES ETHERNET
//	 Pacote internet, o tamanho minimo e m�ximo deve ser de 64 e 1518 bytes, definido em:
//		6 bytes para o MAC destino
//		6 bytes para o MAC oriegm
//		2 bytes para o tipo
//		46 a 1500 bytes para os dados
//		4 bytes para o CRC

// Os primeiros 4 bytes de cada pacote recebido na NIC s�o para controle dos buffers do restante dos buffers
#define ETHERNET_PACKET_STATUS      0x00
#define ETHERNET_PACKET_NEXT_PAGE 	0x01
#define	ETHERNET_PACKET_LEN_L    	0x02
#define	ETHERNET_PACKET_LEN_H 	 	0x03



// ###################################################################################################################
// CONFIGURA��ES DO ENC28J60
#define ENC28J60_USE_INTERRUPT		pdOFF		// Define se a placa de rede vai usar a porta de interrup��o
	#define ENC28J60_USE_EINT			0			// Define qual pino de interrup��o da CPU que ser� usada para a NIC. 0 para EINT0, 1 para EINT1 e assim sucessivamente

// ORGANIZA��O DA MEM�RIA
#define ENC28J60_TXSTART	       	0x0000		// In�cio da mem�ria de transmiss�o da NIC
#define ENC28J60_RXSTART	       	0x0600		// Limite da mem�ria de transmiss�o e inicio da mem�ria de recep��o da NIC. Fornece 1536 bytes para TX (pelo menos 1 pacote)
#define ENC28J60_RXEND	         	0x1fff		// Fim da mem�ria de recep��o da NIC. Resto da 8K de ram da NIC = 6655 bytes (4 pacotes de 1518 bytes)



#endif
