#ifndef __CONFIG_CPU_NIC_H
#define __CONFIG_CPU_NIC_H

#include "_config_cpu_.h"

// ###################################################################################################################
// RTL8019AS
#define RTL_RESET_PORT 	PORTD
#define RTL_RESET_DDR 	DDRD
#define RTL_RESET_PIN 	PD1

// CONFIGURAÇÕES PARA MODO GPIO
#define RTL_CONNECTION_GPIO				0
	#define RTL_DATA_PORT 	PORTA
	#define RTL_DATA_IN 	PINA
	#define RTL_DATA_DDR 	DDRA
	#define RTL_D0_PIN		PA0
	#define RTL_D1_PIN		PA1
	#define RTL_D2_PIN		PA2
	#define RTL_D3_PIN		PA3
	#define RTL_D4_PIN		PA4
	#define RTL_D5_PIN		PA5
	#define RTL_D6_PIN		PA6
	#define RTL_D7_PIN		PA7
	
	#define RTL_ADDR_PORT 	PORTC
	#define RTL_ADDR_DDR 	DDRC
	#define RTL_A0_PIN		PC0
	#define RTL_A1_PIN		PC1
	#define RTL_A2_PIN		PC2
	#define RTL_A3_PIN		PC3
	#define RTL_A4_PIN		PC4
	
	#define RTL_CTRL_PORT 	PORTG
	#define RTL_CTRL_DDR 	DDRG
	#define RTL_RD_PIN		PG1
	#define RTL_WR_PIN		PG0

// CONFIGURAÇÕES PARA MODO RAM // DEVE HABILITAR SRE NO MCUCR PARA ACESSO A RAM EXTERNA
#define RTL_CONNECTION_IOMAP			2	// Acesso a NIC é mapeada dentro da memória nas linhas de endereço A0 a A4, requer latch nos.
	#define RTL_IOMAP_OFFSET 0x8300
	
#define RTL_CONNECTION_IOMAP_HIGHADDR	3 	// Acesso a NIC é mapeada dentro da memória nas linhas de endereço A8 a A12, não requer latch.
	#define RTL_IOMAP_OFFSET_HIGHADDR 0x2000

// AJUSTA O TIPO DE ACESSO A RTL
#define RTL_CONNECTION RTL_CONNECTION_IOMAP_HIGHADDR		
//						RTL_CONNECTION_GPIO  			Acesso a NIC via porta GPIO
//						RTL_CONNECTION_IOMAP 			Acesso a NIC como RAM através da linha de endereço baixo
//						RTL_CONNECTION_IOMAP_HIGHADDR	Acesso a NIC como RAM através da linha de endereço alta


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

// The ENC28J60_RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated
#define ENC28J60_RXSTART_INIT     0x0
#define ENC28J60_RXSTOP_INIT      (0x1FFF-0x0600-1)//  4 pacotes de tamanho máximo ) 
#define ENC28J60_TXSTART_INIT     (0x1FFF-0x0600)
#define ENC28J60_TXSTOP_INIT      0x1FFF		// 1 pacote de tamanho máximo ) 

#endif
