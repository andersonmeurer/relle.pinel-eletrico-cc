#ifndef __CONFIG_CPU_NIC_H
#define __CONFIG_CPU_NIC_H

#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURAÇÕES DA RTL8019AS

#define RTL_PORT_DIR GPIO1_DIR
#define RTL_PORT_CLR GPIO1_CLR
#define	RTL_PORT_SET GPIO1_SET
#define	RTL_PORT_IN  GPIO1_PIN
#define RTL_PIN_RST	(1<<31)

// CONFIGURAÇÕES PARA MODO GPIO
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

// CONFIGURAÇÕES PARA MODO RAM
#define RTL_CONNECTION_IOMAP  		1				// Acesso a NIC é mapeada dentro da memória nas linhas de endereço A0 a A4
	#define RTL_IOMAP_OFFSET 		0x82000000		// Endereço de OFFSET para acessar a NIC

// AJUSTA O TIPO DE ACESSO A RTL
#define RTL_CONNECTION RTL_CONNECTION_GPIO		
//						RTL_CONNECTION_GPIO  Acesso a NIC via porta GPIO
//						RTL_CONNECTION_IOMAP Acesso a NIC como RAM


// VALORES INCIAIS DOS RESGISTRADORES DA RTL
// -------------------------------------------
#define RTL_RCR_INIT 	0x04 
	// Pacotes recebidos são armazenados no buffer ring
	// Somemte aceita pacotes se o endereço MAC for igual aos registradores PAR[0..5]
	// Pacotes multicast são rejeitados
	// Pacotes broadcast são aceitos
	// Pacotes menores que 64 bytes serão rejeitados
	// Pacotes com CRC inválido serão rejeitados

#define RTL_DCR_INIT 	0x58
	// FIFO threshold: 8-bits
	// DMA: Transferencia a 8-bit e byte order 8086

#define RTL_IMR_INIT	0x11
	// Habilita a interrupção de overflow de recepção
	// Habilita a interrupção de chegada de dados sem erros

// POSIÇÃO DE MEMÓRIA FIFO. Endereço da RAM vai de 0x4000 a 0x8000
// A MEMÓRIA ESTA DEVIVIDA EM BUFFER DE 256 BYTES CHAMADAS DE PÁGINAS
//-------------------------------------------------------------------
#define RTL_TPSR_INIT  	0x40	// Endereço inicial da página para o buffer de transmissão ( 6 paginas contém 1536 bytes comporta 1 pacote de tamanho máximo ) 
#define RTL_PSTART_INIT 0x46	// Endereço inicial da página para o buffer de recepção (26 páginas contém 6656 bytes comporta 4 pacotes de tamanho máximo ) 
#define RTL_PSTOP_INIT 	0x60	// Endereço final da página para o buffer de recepção

// VALORES PADRÕES ETHERNET
//	 Pacote internet, o tamanho minimo e máximo deve ser de 64 e 1518 bytes, definido em:
//		6 bytes para o MAC destino
//		6 bytes para o MAC oriegm
//		2 bytes para o tipo
//		46 a 1500 bytes para os dados
//		4 bytes para o CRC

// Os primeiros 4 bytes de cada pacote recebido na NIC são para controle dos buffers do restante dos buffers
#define ETHERNET_PACKET_STATUS      0x00
#define ETHERNET_PACKET_NEXT_PAGE 	0x01
#define	ETHERNET_PACKET_LEN_L    	0x02
#define	ETHERNET_PACKET_LEN_H 	 	0x03



// ###################################################################################################################
// CONFIGURAÇÕES DO ENC28J60
#define ENC28J60_USE_INTERRUPT		pdOFF		// Define se a placa de rede vai usar a porta de interrupção
	#define ENC28J60_USE_EINT			0			// Define qual pino de interrupção da CPU que será usada para a NIC. 0 para EINT0, 1 para EINT1 e assim sucessivamente

// ORGANIZAÇÃO DA MEMÓRIA
#define ENC28J60_TXSTART	       	0x0000		// Início da memória de transmissão da NIC
#define ENC28J60_RXSTART	       	0x0600		// Limite da memória de transmissão e inicio da memória de recepção da NIC. Fornece 1536 bytes para TX (pelo menos 1 pacote)
#define ENC28J60_RXEND	         	0x1fff		// Fim da memória de recepção da NIC. Resto da 8K de ram da NIC = 6655 bytes (4 pacotes de 1518 bytes)



#endif
