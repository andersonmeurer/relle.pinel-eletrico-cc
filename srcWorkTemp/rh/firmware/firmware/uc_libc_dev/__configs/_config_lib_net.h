#ifndef __CONFIG_LIB_NET_H
#define __CONFIG_LIB_NET_H

#define IP_TCP_HEADER_LENGTH			40
#define ETHERNET_MIN_PACKET_LENGTH		60 			// Pacote minimo permitido é de 64bytes. Aqui é desconsiderado o 4 bytes CRC pois é gerado pela placa e rede
#define ETHERNET_HEADER_LENGTH			14			// No barramento ethernet esse valor é fixo. Composto do End MAC destino, End MAC origem e o tipo
#define ETHERNET_TOTAL_HEADER_LENGTH	(IP_TCP_HEADER_LENGTH + ETHERNET_HEADER_LENGTH)
#define ETHERNET_MAX_FRAME_LEN        	1518

#endif
