// #################################################################################################################
// #################################################################################################################
//  Module Name:       NIC Driver Interface for uIP-AVR Port
//  
//  Created By:        Louis Beaudoin (www.embedded-creations.com)
//
//  Original Release:  November 16, 2003 
//
//  Module Description:  
//  Provides three functions to interface with a NIC driver
//  These functions can be called directly from the main uIP control loop
//  to send packets from uip_buf and uip_appbuf, and store incoming packets to
//  uip_buf
//
// #################################################################################################################
#include "uip_nic.h"
#include "_config_cpu_nic.h"

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa a o controlador ethernet
// Parametros: 	Endereço MAC
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
int nic_Init(struct uip_eth_addr mac) {
	uip_setethaddr(mac);
	return NICInit(mac.addr[0], mac.addr[1], mac.addr[2], mac.addr[3], mac.addr[4], mac.addr[5]);
}

#if (UIP_USE_NIC == UIP_USE_NIC_ENC28J60)
// #################################################################################################################
// #################################################################################################################
// DRIVER ENC28J60
// #################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite para rede o pacote contido no buffer uip_buf e uip_appdata
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void nic_Send(void) {
	enc28j60_PacketSend(uip_buf, uip_len);							// Transfere para a NIC um buffer para transmissão de frame na rede
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Através de um polling no main a NIC é consultada se chegou algum pacote ou um overflow na recepção.
//					Se um novo pacote existe e ele cabe no uip_buf ele é cpaturado.
//					pacotes maior que o buffer são descartados
// Parametros: 	Nenhum
// Retorna: 	O tamando do pacote recebido. O valo 0 indica que nenhum pacote foi recebido
//------------------------------------------------------------------------------------------------------------------
u16 nic_Poll(void) {
	return enc28j60_PacketReceive(uip_buf, UIP_BUFSIZE);	
}

#else // FIM ENC28j60
// #################################################################################################################
// #################################################################################################################
// DRIVER PARA RTL E AX
// #################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite para rede o pacote contido no buffer uip_buf e uip_appdata
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void nic_Send(void) {
	NICBeginPacketSend(uip_len);									// Determina o tamanho do frame a ser transmitido
	
																	// send packet, using data in uip_appdata if over the IP+TCP header size
	if( uip_len <= ETHERNET_TOTAL_HEADER_LENGTH ) {							// Se existir dados (uip_appdata) transmite, senão só transmite o cabeçalho
   		NICSendPacketData(uip_buf, uip_len);						// Transfere para a NIC um buffer para transmissão de frame na rede
	} else 	{
    	uip_len -= ETHERNET_TOTAL_HEADER_LENGTH;
    	NICSendPacketData(uip_buf, ETHERNET_TOTAL_HEADER_LENGTH);			// Transfere para a NIC um buffer para transmissão de frame na rede
		NICSendPacketData((u8 *)uip_appdata, uip_len);	// Transfere para a NIC um buffer para transmissão de frame na rede
	}

	NICEndPacketSend();												// Finalização da operação de um pacote de transmissão e a instrução para transmitir o frame na rede
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Através de um polling no main a NIC é consultada se chegou algum pacote ou um overflow na recepção.
//					Se um novo pacote existe e ele cabe no uip_buf ele é cpaturado.
//					pacotes maior que o buffer são descartados
// Parametros: 	Nenhum
// Retorna: 	O tamando do pacote recebido. O valo 0 indica que nenhum pacote foi recebido
//------------------------------------------------------------------------------------------------------------------
u16 nic_Poll(void) {
	u16 packet_len;

	packet_len = NICBeginPacketRetreive();			// Configura a NIC para capturar um pacote e retorna o tamanho do pacote recebido

	if( !packet_len )									// Retona se não existe pacotes recebidos da rede
  		return 0;

	if( packet_len > UIP_BUFSIZE ) {					// Discarta pacotes recebidos maior que o buffer
  		NICEndPacketRetreive();							// Finalização da operação de captura de pacotes 
   		return 0;
	}

	NICRetreivePacketData(uip_buf, packet_len);		// Lê o frame ethernet recebido pela NIC para o buffer do UIP
	NICEndPacketRetreive();								// Finalização da operação de captura de pacotes 
		
	return packet_len;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
void nic_ClearCountOverrun(void) {
	NICClearCountOverrun();
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
u8 nic_CountOverrun(void) {
	return NICCountOverrun();
}

n8 nic_CableConnected(void) {
	return NICCableConnected();
}

#endif
