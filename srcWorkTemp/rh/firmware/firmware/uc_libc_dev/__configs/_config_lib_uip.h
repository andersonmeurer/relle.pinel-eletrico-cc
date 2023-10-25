#ifndef	__CONFIG_LIB_UIP_H
#define __CONFIG_LIB_UIP_H

#include "_config_lib_net.h"
#include "uc_libdefs.h"

// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************

// #################################################################################################################################################
// DETERMINAR O TIPO DA NIC E ENDEREÇO MAC
#define UIP_USE_NIC 		UIP_USE_NIC_ENC28J60 	// UIP_USE_NIC_RTL8019, UIP_USE_NIC_ENC28J60 ou  UIP_USE_NIC_AX88796

// ###################################################################################################################################################
// TAMANHO DO BUFFER DO PACOTE UIP
//	O buffer do pacote IP não pode ser menor que 60 bytes e não pode ser maior que 1520 bytes
//	Buffer menor requer menos processamento do TCP
#define UIP_BUFSIZE    		ETHERNET_MAX_FRAME_LEN

// ###################################################################################################################################################
// NÚMERO MÁXIMO DE CONEXÃO UDP
#define UIP_UDP_CONNS 		1 //10

// ###################################################################################################################################################
// NÚMERO MÁXIMO DE CONEXÃO TCP
//	Número máximo de conexão TCP aberta simultania. Como cada conexão TCP são alocadas estáticamente, e cada alocação ocupa +- 30 bytes,
//	Então quanto menos conexão menor será a quantidade de RAM alocada
#define UIP_CONNS 	2 //10

// ###################################################################################################################################################
// NÚMERO MÁXIMO DE PORTA TCP DE ESCUTA
//	Cada porta TCP requer 2 bytes de RAM
#define UIP_LISTENPORTS 	1 //20

// ###################################################################################################################################################
// NÚMERO MÁXIMO DE ENTRADA NA TABELA ARP
//	Esta opção deve ser ajusta para numero alto se este nodo uIP terá muitas conexões
#define UIP_ARPTAB_SIZE 	4

// ###################################################################################################################################################
// NÚMERO MÁXIMO TEMPO TABELA ARP
// 	O maxium tempo da tabela ARP entradas medido em 10ths de segundos
// 	O valor 120 corresponde a 20 minutos (padrão BSD)
#define UIP_ARP_MAXAGE 120

// ###################################################################################################################################################
// SUPORTE A BROADCAST
//	Este flag configura o IP a suportar broadcast. Ista função é somente útil junto com o UDP
//		0 = desliga o suporte
//		1 = liga o suporte
#define UIP_BROADCAST	1	// UIP_UDP deve ser 1

// ###################################################################################################################################################
// SUPORTE AO DHCP
// 		Este flag configura o uIP a suportar DHCP. Ista função é somente útil se UIP_UDP = 1
//		0 = desliga o suporte
//		1 = liga o suporte
#define ENABLE_DHCP 	0	// UIP_UDP deve ser 1

// ###################################################################################################################################################
// SUPORTE AO UDP
//		0 = desliga o suporte
//		1 = liga o suporte
#define UIP_UDP 		1

// ###################################################################################################################################################
// SUPORTE AO CHECKSUM UDP
//		0 = desliga o suporte checksums 
//		1 = liga o suporte checksums 
#define UIP_UDP_CHECKSUMS   1 //0

// ###################################################################################################################################################
// SUPORTE A ESTATISTICA uIP
//	Determina se suporte a estatistica deve ou não ser compilada
//	Essa estatistica é útil para depurar e mostrar resultados para o usuário
//		0 = desliga o suporte
//		1 = liga o suporte
#define UIP_STATISTICS      1

// ###################################################################################################################################################
// SUPORTE PARA ABRIR CONEXÃO TCP
//	Se a aplicação que está rodando no uIP não necessita abrir um conexão de saida TCP, 
//	então esse suporte deve ser desligado para reduzir o código compilado
//		0 = Exclui o suporte para abrir conexão TCP
//		1 = Inclui o suporte para abrir conexão TCP
#define UIP_ACTIVE_OPEN 	1

// ###################################################################################################################################################
// TTL
//	O TTL (time to live) do pacote IP emitido pelo uIP
// 	This should normally not be changed.
 #define UIP_TTL         64

// ###################################################################################################################################################
// SUPORTE A REMONTAGEM DO PACOTE IP
//	uIP suporta momtagem de pacotes fragmentados dos pacotes IP. 
//	Porém, este suporte consomente muita RAM para o buufer de montagem e o código de montagem ocupa aproximadamente 700 bytes
//	O buffer de montagem é do mesmo tamanho do ip_buf buffer (configurado pelo UIP_BUFSIZE).
#define UIP_REASSEMBLY 	0

// ###################################################################################################################################################
// TEMPO MÁXIMO DE UM FRAGMENTO
//	Tempo máximo para que um fragmento IP deve esperar no buffer antes que seja abandonado
#define UIP_REASS_MAXAGE 40

// ###################################################################################################################################################
// ATRIBUIÇÃO DE ENDEREÇO IP AO PING
// 	uIP uses a "ping" packets for setting its own IP address if this
//	option is set. If so, uIP will start with an empty IP address and
// 	the destination IP address of the first incoming "ping" (ICMP echo)
// 	packet will be used for setting the hosts IP address.
// 	note This works only if UIP_FIXEDADDR is 0.
#define UIP_PINGADDRCONF 0

// ###################################################################################################################################################
// SUPORTE AO DADOS URGENTE TCP
//	Determina se o suporte aos dados urgente TCP seja compilado ao uIP
// 	Dados urgentes (out-of-band data) é um recurso raramente utilizado TCP que muito raramente seria exigido
#define UIP_URGDATA      0

// ###################################################################################################################################################
// TAMANHO DO CABEÇALHO 
//	Este é o offset onde o cabeçalho do IP pode ser encontrado no buffer uip_buf
//		14 = para pacotes Ethernet
//		0 = para SLIP
#define UIP_LLH_LEN		14

// ###################################################################################################################################################
// TAMANHO DA JANELA DE RECEPÇÃO
//	This is should not be to set to more than  UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
// 	Should be set low (i.e., to the size of the uip_buf buffer) is the
// 	application is slow to process incoming data, or high (32768 bytes)
// 	if the application processes data quickly.
#define UIP_TCP_MSS     (UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN)
#define UIP_RECEIVE_WINDOW UIP_TCP_MSS

// ###################################################################################################################################################
// FIXAR MAC
//	Especifica se o módulo ARP uIP deve ser, ou não, compilado com um endereço MAC fixo.
//	Se a configurção for 0, a macro uip_setethaddr() pode ser usado para especificar o MAC em tempo de execução
//		0 = Não fixa o MAC
//		1 = Fixa o MAC
#define UIP_FIXEDETHADDR 0

	// Valores para fixar o MAC
	#define UIP_ETHADDR0    0x00  // O primeiro octeto do MAC se UIP_FIXEDETHADDR é 1
	#define UIP_ETHADDR1    0xbd  // O segundo 	octeto do MAC se UIP_FIXEDETHADDR é 1
	#define UIP_ETHADDR2    0x3b  // O terceiro octeto do MAC se UIP_FIXEDETHADDR é 1
	#define UIP_ETHADDR3    0x33  // O quarto 	octeto do MAC se UIP_FIXEDETHADDR é 1
	#define UIP_ETHADDR4    0x05  // O quinto 	octeto do MAC se UIP_FIXEDETHADDR é 1
	#define UIP_ETHADDR5    0x71  // O sexto 	octeto do MAC se UIP_FIXEDETHADDR é 1

// ###################################################################################################################################################
// FIXAR IP

// These configuration options can be used for setting the IP address
// settings statically, but only if UIP_FIXEDADDR is set to 1. The
// configuration options for a specific node includes IP address,
// netmask and default router as well as the Ethernet address. The
// netmask, default router and Ethernet address are appliciable only
// if uIP should be run over Ethernet.

//	Especifica se o uIP deve ser, ou não, compilado com um endereço IP fixo.
//	Se não vai usar o IP fixo deve-se usar as macros uip_sethostaddr(), uip_setdraddr() e uip_setnetmask() para configurar os IPs
//		0 = Não fixa os endereços IPs
//		1 = Fixa o os endereços IPs
#define UIP_FIXEDADDR    0

	// IP
	#define UIP_IPADDR0		192 	// O primeiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_IPADDR1		168 	// O segundo  octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_IPADDR2		1 		// O terceiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_IPADDR3		100 	// O quarto	  octeto do IP se UIP_FIXEDADDR é 1
	// GATEWAY
	#define UIP_DRIPADDR0	192 	// O primeiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_DRIPADDR1	168 	// O segundo  octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_DRIPADDR2	1		// O terceiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_DRIPADDR3	1		// O quarto	  octeto do IP se UIP_FIXEDADDR é 1
	// MASK
	#define UIP_NETMASK0	255		// O primeiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_NETMASK1	255		// O segundo  octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_NETMASK2	255		// O terceiro octeto do IP se UIP_FIXEDADDR é 1
	#define UIP_NETMASK3	0		// O quarto	  octeto do IP se UIP_FIXEDADDR é 1



// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************

// #################################################################################################################################################
// ORDEM DO BYTE DA CPU
//	A ordem do byte da arquitetura da CPU onde na qual o uIP será executado
//		Família motorola é BIG_ENDIAN e Intel é LITTLE_ENDIAN
#define UIP_BYTE_ORDER     	UIP_LITTLE_ENDIAN 	//	UIP_LITTLE_ENDIAN ou LITTLE_ENDIAN

// ###################################################################################################################################################
// The initial retransmission timeout counted in timer pulses.
// This should not be changed.
#define UIP_RTO         3

// ###################################################################################################################################################
// The maximum number of times a segment should be retransmitted
// before the connection should be aborted.
// This should not be changed.
#define UIP_MAXRTX      8

// ###################################################################################################################################################
// The maximum number of times a SYN segment should be retransmitted
// before a connection request should be deemed to have been unsuccessful.
// This should not need to be changed.
#define UIP_MAXSYNRTX      5

// ###################################################################################################################################################
// Quanto tempo uma conexão deve permanecer no estado TIME_WAIT.
//	Esta configuração não tem opção real implicação, e que deveria ser deixado intocado
#define UIP_TIME_WAIT_TIMEOUT 120

// ###################################################################################################################################################
// ESTRUTURA DE DADOS PARA A ESTATISTICA uIP
//	Define o tipo de dados para armazenar a estatistica do uIP
// All of these should be changed to suit your project.
typedef unsigned short uip_stats_t;


// ###################################################################################################################################################
// DETERMINAR NÚMERO DE CLOCKS DO UIP GERADOS EM UM SEGUNDO
#if defined(FREE_RTOS)
	#include "_config_lib_freertos.h"
	#define CLOCK_CONF_SECOND 		configTICK_RATE_HZ
#else
	#define CLOCK_CONF_SECOND 		100
#endif


// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ###################################################################################################################################################
// ANALISE
#if (UIP_BUFSIZE < 60) || ((UIP_BUFSIZE > 1520))
#error ETHERNET_MAX_FRAME_LEN FORA DOS LIMITES ajuste entre 60 a 1520 no _config_lib_net.h
#endif



// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ***************************************************************************************************************************************************
// ###################################################################################################################################################
// APLICAÇÃO
#include "uip_app.h"

#if ENABLE_DHCP > 0
#include "dhcpc.h"
#endif

#endif
