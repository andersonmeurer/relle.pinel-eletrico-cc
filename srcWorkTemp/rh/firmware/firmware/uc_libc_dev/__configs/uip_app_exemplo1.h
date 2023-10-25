#ifndef __UIP_APP_H
#define __UIP_APP_H

#include "_config_cpu_.h"
//#include "psock.h"
#include "buffer_d.h"

#define NET_USE_DEBUG pdOFF

#define BUFFER_TX_SIZE				10						// Tamanho do buffer circular de transmissão de dados na rede
#define BUFFER_RX_SIZE				BUFFER_TX_SIZE			// Tamanho do buffer circular de recepção de dados na rede

// Dos 1500 bytes de dados do pacote ethernet, somente 1446 bytes são para a nossa aplicação,
// pois os primeiros 54 bytes são para o cabeçalho IP da rede
#define PROTOCOL_BUFFER_SIZE		1446

#if (PROTOCOL_BUFFER_SIZE > 1446) || (PROTOCOL_BUFFER_SIZE < 6)
#error O PROTOCOL_BUFFER_SIZE deve ser entre 7 a 1446 bytes
#endif

#define LED1_DIR		GPIO0_DIR
#define LED1_CLR		GPIO0_CLR
#define LED1_SET		GPIO0_SET
#define LED1			(1<<6)

#define LED2_DIR		GPIO0_DIR
#define LED2_CLR		GPIO0_CLR
#define LED2_SET		GPIO0_SET
#define LED2			(1<<7)

#define SENSOR1_DIR		GPIO1_DIR
#define SENSOR1_PIN		GPIO1_PIN
#define SENSOR1			(1<<8)

#define SENSOR2_DIR		GPIO2_DIR
#define SENSOR2_PIN		GPIO2_PIN
#define SENSOR2			(1<<12)

#define sensor1()		(SENSOR1_PIN & SENSOR1)?(pdTRUE):(pdFALSE)
#define sensor2()		(SENSOR2_PIN & SENSOR2)?(pdTRUE):(pdFALSE)
#define led1_On() 		LED1_SET = LED1
#define led1_Off() 		LED1_CLR = LED1
#define led2_On() 		LED2_SET = LED2
#define led2_Off() 		LED2_CLR = LED2


// TIPO PACOTE DO PROTOCOLO DE APLICAÇÃO
#define PROTOCOL_DATA_SIZE PROTOCOL_BUFFER_SIZE-6
// -6 por causa do u8 device, u8 cmd, u16 seg, u16 size; sobrando o resto para o campo de dados

struct ppa_t {
	u8 device;
	u8 cmd;
	u16 seg;
	u16 size;
	u8 data[PROTOCOL_DATA_SIZE];
} __attribute__ ((packed)); // 	Especifica ao compilador para não alinhar a estrutura.

// NÚMERO DOS DISPOSITIVO
#define ID_LED1          100
#define ID_LED2          101
#define ID_SENSOR1       150
#define ID_SENSOR2       151
#define ID_SDRAM         200

// COMANDOS
#define DEVICE_OFF       0
#define DEVICE_ON        1

#define COMMAND_READ     0
#define COMMAND_WRITE    1

void net_TCP(void);
void net_UDP(void);
#define UIP_APPCALL     	net_TCP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos TCP
#define UIP_UDP_APPCALL  	net_UDP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos UDP

enum stsCnx_e {DISCONNECTED, CONNECTED};

// Esta estrutura vai ser anexada ao vetor de conexão uip_conn e é obrigatório declarar, mesmo que não conter dados de controle.
//	Esta estrutura de dados guarda o estado da aplicação em cada conexão.
//	Muito útil quando se trabalha com múltiplas conexões.
typedef struct app_state {
	buffer_d_t fifoRX;						// Estrutura da FIFO de recepção de pacotes de dados
	u8 *fifoRXdata; 						// Região de memória de armazenamento dos bytes da FIFO de recepção
	u8 *packetRX;  							// Pacotes de dados de recepção
	u16 packetRXcount;						// Quantidade de pacotes recebidos por esta conexão

	buffer_d_t fifoTX;						// Estrutura da FIFO de transmissão de pacotes de dados
  	u8 *fifoTXdata; 						// Região de memória de armazenamento dos bytes da FIFO de transmissão
  	u8 *packetTX;  							// Pacote de dados para transmissão
  	u16 packetTXcount;						// Quantidade de pacotes transmitido por esta conexão

  	struct ppa_t *ppa;						// Ponteiro do protocolo de aplicação e vai apontar para bufferWork
  	u8 *bufferWork;							// Buffer de trabalho
  	enum stsCnx_e stsCnx;					// Estado da conexão
  	u8 flagTXok;							// flag de transmissão do pacote, é para esperar o ack do destino para continuar a transissão dos próximos pacotes
  	u8 sdramFlagTx;
  	u16 sdramSeg;
  	u32 sdramAddr;
  	u32 sdramSize;
} uip_tcp_appstate_t;

void net_Init(void);




#endif
