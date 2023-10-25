#include "rtl8019.h"

static u8 nic_counter_overrun = 0;

// Ponteiros de auxilio na captura do pacote de recepção
static u8 nextPage;				// Aponta para a próxima página do pacote na FIFO de recepção
static u16 currentRetreiveAddress;	// Aponta para a próximo endereço do byte a ser capturado na FIFO de recepção

void rtl8019_Overrun(void);

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a interface para a NIC, incializa os buffers e a NIC
// Parametros: 	Endereço MAC da NIC. mac0 Mais significativo
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
int rtl8019_Init(u8 mac0, u8 mac1, u8 mac2, u8 mac3, u8 mac4, u8 mac5) {
	// INCIALIZA NIC
	#if !defined(SIM_SKYEYE) 
		rtl8019_SetupPorts();											// Configura a interface para a NIC
		HARD_RESET_RTL();												// Emite um reset via hardware
	#endif
	
	rtl8019_Write(RTL_REG_ISR, rtl8019_Read(RTL_REG_ISR) );			// Emite um reset via software
	
	#if !defined(SIM_SKYEYE) 
		nic_Delay(NIC_DELAY_INIT);
	#endif
	
    // PÁGINA 3 
    rtl8019_Write(RTL_REG_CR, (RTL_PS1|RTL_PS0|RTL_RD2|RTL_STOP));	// Selecionar página 3, DMA desligada e NIC parada
    rtl8019_Write(RTL_REG_EECR, (RTL_EEM1|RTL_EEM0)); 				// Hablita escrita nos registradores de configurções. É necessário fazer isso antes de acessar os registradores CONFIGs
    rtl8019_Write(RTL_REG_CONFIG2, 0x20);							// Selecionando detecção automático do tipo de rede (Teste para 10 Base-T ativado)
    rtl8019_Write(RTL_REG_CONFIG3, 0x10);							// Plug and Play desativado, configura para half duplex, LED0 = LINK, LEDs1/2 to RX e TX, desabilita sleep e powerdown
    
    #if !defined(SIM_SKYEYE) 
    	nic_Delay(NIC_DELAY_SKYEYE);
    #endif
    	
    rtl8019_Write(RTL_REG_EECR, 0);									// Desablita escrita nos registradores de configurções.

	// PÁGINA 0
    rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_STOP));  				// Selecionar página 0, DMA desligada e NIC parada
	
	#if !defined(SIM_SKYEYE) 
		nic_Delay(NIC_DELAY_SKYEYE_2);      						// make sure nothing is coming in or going out
	#endif
	
	rtl8019_Write(RTL_REG_DCR, RTL_DCR_INIT);  						// Configurando registrador de dados. FIFO e modo de transferencia da CPU para NIC
	rtl8019_Write(RTL_REG_RBCR0, 0x00);								// Indica ao DMA a quantidade de bytes para transferencia
	rtl8019_Write(RTL_REG_RBCR1, 0x00);								// Indica ao DMA a quantidade de bytes para transferencia
	rtl8019_Write(RTL_REG_RCR, RTL_RCR_INIT);						// Ajustando filtro de recepção de pacotes
	rtl8019_Write(RTL_REG_TPSR, RTL_TPSR_INIT);						// Aponta na FIFO o começo do buffer de transmissão
	rtl8019_Write(RTL_REG_TCR, RTL_LB0);							// RTL operando em loopback interno e geração de CRC para recepção
	rtl8019_Write(RTL_REG_PSTART, RTL_PSTART_INIT);					// Aponta na FIFO o começo do buffer de recepção
	rtl8019_Write(RTL_REG_PSTOP, RTL_PSTOP_INIT);					// Aponta na FIFO o fim do buffer de recepção
	rtl8019_Write(RTL_REG_BNRY, RTL_PSTART_INIT);					// Indica o endereço da próxima página a ser lida pelo host
			
	//PÁGINA 1
	rtl8019_Write(RTL_REG_CR, (RTL_PS0|RTL_RD2|RTL_STOP));			// Selecionar página 1, DMA desligado e NIC parada
	
	#if !defined(SIM_SKYEYE) 
		nic_Delay(NIC_DELAY_SKYEYE_2);								// make sure nothing is coming in or going out
	#endif
	
	rtl8019_Write(RTL_REG_CURR, RTL_PSTART_INIT);					// Aponta para a primeira página do pacote recebido
	rtl8019_Write(RTL_REG_PAR0+0, mac0);							// Ajusta o endereço MAC (Byte mais significativo)
	rtl8019_Write(RTL_REG_PAR0+1, mac1);							// Ajusta o endereço MAC
	rtl8019_Write(RTL_REG_PAR0+2, mac2);							// Ajusta o endereço MAC
	rtl8019_Write(RTL_REG_PAR0+3, mac3);							// Ajusta o endereço MAC
	rtl8019_Write(RTL_REG_PAR0+4, mac4);							// Ajusta o endereço MAC
	rtl8019_Write(RTL_REG_PAR0+5, mac5);							// Ajusta o endereço MAC
	
	// Inicializa seq para NE2000
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_STOP));					// Selecionar página 0, DMA desligada e NIC parada
	rtl8019_Write(RTL_REG_DCR, RTL_DCR_INIT);						// Configurando registrador de dados. FIFO e modo de transferencia da CPU para NIC
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));					// Selecionar página 0, DMA desligada e NIC trabalhando
	rtl8019_Write(RTL_REG_ISR, 0xFF);								// Limpa todos os bits de flags de interrupção da NIC
	rtl8019_Write(RTL_REG_IMR, RTL_IMR_INIT);						// Configura as interrupções
	rtl8019_Write(RTL_REG_TCR, 0);									// RTL operando em modo normal e geração de CRC para recepção e transmissão
	
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));					// Selecionar página 0, DMA desligada e NIC trabalhando
	
	nic_counter_overrun = 0;
	
	return pdPASS;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o cabo 10-base-T esta conectado a placa de rede
//				OBS: Isto funciona se no registrador Config2 esta configurado (PL0=PL1=0) para habilitar o teste de link 10BaseT
// Parametros: 	Nenhum
// Retorna: 	pdTRUE se o cabo 10-Base T esta conectado
//------------------------------------------------------------------------------------------------------------------
int rtl8019_CableConnected(void) {
	u8 cr;
	n8 return_code = pdTRUE;

	cr = rtl8019_Read(RTL_REG_CR);						// Capturo o estado atual do CR
	
	rtl8019_Write(RTL_REG_CR, (RTL_PS1|RTL_PS0|cr));	// Selecionar página 3 e manter o resto da configuração anterior
	
	if (rtl8019_Read(RTL_REG_CONFIG0)&RTL_BNC)
		return_code = pdFALSE;
	
	rtl8019_Write(RTL_REG_CR, cr);						// Restauro o registrador CR
	
	return return_code;
}

// #################################################################################################################
// #################################################################################################################
// TRANSMITINDO PACOTES
// #################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Determina o tamanho do frame. A NIC não pode mandar pacotes menor que 60 Bytes (Minimo permitido)
//				Caso o tamanho do pacote for menor que 60 bytes o resto será preenchido automaticamente
// Parametros: 	Tamanho do pacote
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_BeginPacketSend(u16 packetLength) {
	u16 sendPacketLength;
	
	// AJUSTA PARA O TAMANHO DO PACOTE CASO FOR MENOR QUE O MINIMO PERMITIDO (64BYTES)
	sendPacketLength = (packetLength>=ETHERNET_MIN_PACKET_LENGTH) ? packetLength : ETHERNET_MIN_PACKET_LENGTH ;
	
	// CASO AINDA EXISTIR UMA TRANSMISSÃO DE PACOTES, ESPERA PELA FINALIZAÇÃO
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));					// Selecionar página 0, DMA desligada e NIC trabalhando
	while( rtl8019_Read(RTL_REG_CR) & RTL_TXP );					// Espera pela finalização da transmissão do pacote
	
	//ATUALIZADO - Tirado do comentário
	rtl8019_Write(RTL_REG_TPSR, RTL_TPSR_INIT);						// Aponta na FIFO o começo do buffer de transmissão
	
	rtl8019_Write(RTL_REG_RSAR0, 0x00);								// Indica o endereço inicial na FIFO para transferencia de dados via DMA
	rtl8019_Write(RTL_REG_RSAR1, RTL_TPSR_INIT);					// Indica o endereço inicial na FIFO para transferencia de dados via DMA
	rtl8019_Write(RTL_REG_ISR, RTL_PTX);							// Limpa o flag de interrupção de transmissão de pacotes em erro
	rtl8019_Write(RTL_REG_RBCR0, (u8)(packetLength));			// Indica ao DMA a quantidade de bytes para transferencia
	rtl8019_Write(RTL_REG_RBCR1, (u8)(packetLength>>8));		// Indica ao DMA a quantidade de bytes para transferencia
	rtl8019_Write(RTL_REG_TBCR0, (u8)(sendPacketLength));		// Indica a quantidade de bytes a serem transmitidos
	rtl8019_Write(RTL_REG_TBCR1, (u8)((sendPacketLength)>>8));	// Indica a quantidade de bytes a serem transmitidos
	rtl8019_Write(RTL_REG_CR, (RTL_RD1|RTL_START));					// Selecionar página 0,  DMA modo de escrita e NIC trabalhando
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transfere para a NIC um buffer para transmissão de frame na rede
//					A função rtl8019_BeginPacketSend() deve ser chamada antes de mandar qualquer dados para transmissão
//					Muitas chamadas da função rtl8019_SendPacketData() pode ser feita para copiar diferentes buffers para transmissão
// Parametros:  Ponteiro do buffer local e seu tamanho
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_SendPacketData(u8 *localBuffer, u16 length) {
	u16 i;
	
	for(i=0;i<length;i++)									// Faça para todos os bytes do buffer local
		rtl8019_Write(RTL_REG_RDMAPORT, localBuffer[i]);		// Transfere o byte para a NIC via DMA
}
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Finalização da operação de um pacote de transmissão e a instrução para transmitir o frame na rede
// Parametros:  Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_EndPacketSend(void) {
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_TXP)); 			// Selecionar página 0 e transmite pacote na rede
	rtl8019_Write(RTL_REG_ISR, RTL_RDC); 					// Limpa o flag de interrupção da transferencia do DMA remoto
}

// #################################################################################################################
// #################################################################################################################
// RECEBENDO PACOTES
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Configura a NIC para capturar um pacote
//					O tamanho de retorno é o tamanho de todos os dados do frame ethernet menos o CRC. Então o tamanho minimo será 60 bytes e não 64Bytes
//					O tamanho 0 indica que não há pacotes disponivel.
//					Uma chamada para rtl8019_BeginPacketRetreive() deve ser seguido pela chamada rtl8019_EndPacketRetreive(),
//					indiferentemente seo dados é capturado, a menos que o o retorno seja 0
// Parametros: 	Nenhum
// Retorna: 	O tamanho em bytes do pacote recebido
//------------------------------------------------------------------------------------------------------------------
u16 rtl8019_BeginPacketRetreive(void) {
	u8 i;
	u8 bnry;
	u8 pageheader[4];
	u16 rxlen;
	
	rtl8019_ProcessInterrupt();									// Analisa se houve um overflow na FIFO de recebimento de pacotes
	rtl8019_Write(RTL_REG_CR, (RTL_PS0|RTL_RD2|RTL_START));								// Selecionar página 1, DMA desligada e NIC trabalhando
	i = rtl8019_Read(RTL_REG_CURR);								// Lê o endereço da primeira págino do pacote recebido
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));								// Selecionar página 0, DMA desligada e NIC trabalhando
	bnry = rtl8019_Read(RTL_REG_BNRY) ;							// Lê o endereço da página a ser lida pelo host

	if( bnry == i )												// Checa se ainda existe pacotes para serem lidos no buffer
		return 0;												// Retorna caso não exista mais pacotes a serem lidos
	
	rtl8019_Write(RTL_REG_ISR, RTL_PRX);						// Limpa o flag de interrupção de chegada de pacotes sem erro
		
	if( (bnry >= RTL_PSTOP_INIT) || (bnry < RTL_PSTART_INIT) ) {// Se o ponteiro BNRY é inválido reinicie os valores de controle do buffer e descarta o pacote
		rtl8019_Write(RTL_REG_BNRY, RTL_PSTART_INIT); 			// Indica o endereço da próxima página a ser lida pelo host
		rtl8019_Write(RTL_REG_CR, (RTL_PS0|RTL_RD2|RTL_START));	// Selecionar página 1, DMA desligada e NIC trabalhando
		rtl8019_Write(RTL_REG_CURR, RTL_PSTART_INIT);			// Aponta para a primeira página do pacote recebido
		rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));			// Selecionar página 0, DMA desligada e NIC trabalhando	
		return 0;
	}

	// CAPTURAR OS 4 PRIMEIROS BYTES DE CONTROL DO PACOTE
    rtl8019_Write(RTL_REG_RBCR0, 4);								// Indica ao DMA a quantidade de bytes para transferencia
    rtl8019_Write(RTL_REG_RBCR1, 0);								// Indica ao DMA a quantidade de bytes para transferencia
    rtl8019_Write(RTL_REG_RSAR0, 0);								// Indica o endereço inicial na FIFO para transferencia de dados via DMA
    rtl8019_Write(RTL_REG_RSAR1, bnry);								// Indica o endereço inicial na FIFO para transferencia de dados via DMA
    rtl8019_Write(RTL_REG_CR, (RTL_RD0|RTL_START));					// Selecionar página 0, DMA para leitura e NIC trabalhando
	
	for(i=0;i<4;i++)
		pageheader[i] = rtl8019_Read(RTL_REG_RDMAPORT);			// Lê os bytes do DMA
	
    rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));							// Selecionar página 0, DMA desligada e NIC trabalhando
    for(i = 0; i <= 20; i++)
        if(rtl8019_Read(RTL_REG_ISR) & RTL_RDC)					// Checa se a transferencia do DMA remoto esta completa
            break;
    
    rtl8019_Write(RTL_REG_ISR, RTL_RDC);						// Limpa o flag de interrupção da transferencia do DMA remoto
	
	rxlen = (pageheader[ETHERNET_PACKET_LEN_H]<<8) + 			// Capturando o tamanho, em bytes, do pacote
		pageheader[ETHERNET_PACKET_LEN_L];	
	nextPage = pageheader[ETHERNET_PACKET_NEXT_PAGE];			// Aponta para a próxima página de 256 bytes
	currentRetreiveAddress = (bnry<<8) + 4;						// Aponta para o endereço do próximo byte a ser lido, considerando que já fora lido o 4 primeros bytes
	
	// Se a próxima página não é válida, o pacote ainda não esta pronto
	if( (nextPage >= RTL_PSTOP_INIT) || (nextPage < RTL_PSTART_INIT) )
		return 0;
    
    return rxlen-4;												// Retorna a quantida de bytes recebidos pela NIC menos o CRC
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o frame ethernet recebido pela NIC através do DMA
//					rtl8019_BeginPacketRetreive() deve ser chamado antes de capturar qualquer dado.
//					Muitas chamadas para rtl8019_RetreivePacketData() pode ser feita para copiar pacotes de dados de diferentes buffers
// Parametros: 	Ponteiro do buffer e seu tamanho para armazernar o frame ethernet recebido
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_RetreivePacketData(u8 *localBuffer, u16 length) {
	u16 i;
	
	// INCIALIZA O DMA PARA TRANSFERENCIA DE DADOS
    rtl8019_Write(RTL_REG_RBCR0, (u8)length);							// Indica ao DMA a quantidade de bytes para transferencia
    rtl8019_Write(RTL_REG_RBCR1, (u8)(length>>8)); 					// Indica ao DMA a quantidade de bytes para transferencia
    rtl8019_Write(RTL_REG_RSAR0, (u8)currentRetreiveAddress);			// Indica o endereço inicial na FIFO para transferencia de dados via DMA
    rtl8019_Write(RTL_REG_RSAR1, (u8)(currentRetreiveAddress>>8));	// Indica o endereço inicial na FIFO para transferencia de dados via DMA
    rtl8019_Write(RTL_REG_CR, (RTL_RD0|RTL_START));												// Selecionar página 0, DMA para leitura e NIC trabalhando
	
	for(i=0;i<length;i++)														// Faça para todo o buffer
		localBuffer[i] = rtl8019_Read(RTL_REG_RDMAPORT);						// Leia os bytes recebidos da rede através do DMA

    rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));											// Selecionar página 0, DMA desligada e NIC trabalhando
    for(i = 0; i <= 20; i++)
        if(rtl8019_Read(RTL_REG_ISR) & RTL_RDC)									// Checa se a transferencia do DMA remoto esta completa
            break;
    
    rtl8019_Write(RTL_REG_ISR, RTL_RDC);										// Limpa o flag de interrupção da transferencia do DMA
    
    currentRetreiveAddress += length;											// Aponta para o próximo endereço do byte a ser lido no DMA
        
    // Caso chegou no limite da FIFO de recepção
    // O próximo byte a ser lido é apartir do inicio da FIFO mais o deslocameno do endereço
    // pois é um buffer circular
    if( currentRetreiveAddress >= (u16)RTL_PSTOP_INIT<<8)			
    	currentRetreiveAddress = currentRetreiveAddress - (  ((u16)RTL_PSTOP_INIT<<8) - ((u16)RTL_PSTART_INIT<<8) );    																			
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Finalização da operação de captura de pacotes inciada pela função TL8019beginPacketRetreive().
//					O espaço do buffer da NIC usado para captura de pacotes é liberado
//					Um pacote pode ser removido do buffer sem ser lido pela rtl8019_EndPacketRetreive()
//					depois rtl8019_BeginPacketRetreive().
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_EndPacketRetreive(void) {
	u8 i;

    rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));							// Selecionar página 0, DMA desligada e NIC trabalhando
    for(i = 0; i <= 20; i++)
        if(rtl8019_Read(RTL_REG_ISR) & RTL_RDC)					// Checa se a transferencia do DMA esta completa
            break;

    rtl8019_Write(RTL_REG_ISR, RTL_RDC);						// Limpa o flag de interrupção da transferencia do DMA
    rtl8019_Write(RTL_REG_BNRY, nextPage);						// Indica o endereço da próxima página a ser lida pelo host
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê os registradores de interrupção analisando um overflow do buffer de recepção
//					A função não precisa ser chamada na resposta de uma interrupção. 
//					A função pode ser usada em forma de polling e a linha de interrupção não precisa ser usada.
//					Esta função deve ser chamada antes de tentar receber um pacote da NIC
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_ProcessInterrupt(void) {
	u8 byte = rtl8019_Read(RTL_REG_ISR);	// Lê os resgistradores de interrupção
	
	if( byte & RTL_OVW ) {						// Checa se houve um overflow no buffer de recepção da NIC
		rtl8019_Overrun();						// Executar o procedimento de overflow para não perder os dados já recebido e garantir a transmissão do pacote atual
	 	nic_counter_overrun++;
	 }
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
void rtl8019_ClearCountOverrun(void) {
	nic_counter_overrun = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros:
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
u8 rtl8019_CountOverrun(void) {
	return nic_counter_overrun;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Executar o procedimento de overflow do buffer para não perder os dados já recebido e garantir a transmissão do pacote atual
//					Esta função deve ser chamado antes de recuperando pacotes da NIC se há um buffer overrun
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void rtl8019_Overrun(void) {
	u8 data_L, resend;	

	data_L = rtl8019_Read(RTL_REG_CR);				// Captura o registrador CR com as condições atuais de funcionamento da RTL
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_STOP));	// Selecionar página 0, DMA desligada e NIC parada
	
	#if !defined(SIM_SKYEYE) 
		nic_Delay(NIC_DELAY_SKYEYE_2);				// Espera para completar qualquer transação na NIC em progresso
	#endif
	
	rtl8019_Write(RTL_REG_RBCR0, 0x00);				// Indica ao DMA a quantidade de bytes para transferencia
	rtl8019_Write(RTL_REG_RBCR1, 0x00);				// Indica ao DMA a quantidade de bytes para transferencia
		
	// ESTE PROCEDIMENTO CHECA SE EXISTE ALGUMA TRANSMISSÃO EM PROGRESSO QUANDO FOI EMITIDO UM COMANDO DE PARADA DA NIC.
	// SE EXISTE UMA TRANSMISSÃO EM PROGRESSO É PRECISO FAZER UMA RETRANSMISSÃO DO PACOTE, PARA QUE NÃO SEJA PERDIDO
	if(!(data_L & 0x04))							// Checa se não existia algum pacote sendo transmitido antes de desligar a NIC
		resend = 0;									// Não é necessário restransmissão de pacotes a rede
	else if(data_L & 0x04) {						// Checa se existe algum pacote a sendo transmitido
		data_L = rtl8019_Read(RTL_REG_ISR);			// Lê os resgistradores de interrupção
		if((data_L & 0x02) || (data_L & 0x08))		// Checa se o pacote foi transmitido sem erros ou a transmissão foi abortada devido a muitas colisões
	    	resend = 0;								// Não é necessário restransmissão de pacotes, no caso de colisões o sftware de alto nivel que se encarrega de retransmitir
	    else
	    	resend = 1;								// Indica que o pacote que estava sendo transmitindo não foi completta e é necessário fazer uma restransmissão
	}
	
	rtl8019_Write(RTL_REG_TCR, RTL_LB0); 			// RTL operando em loopback interno e geração de CRC para recepção
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));	// Selecionar página 0, DMA desligada e NIC trabalhando
	rtl8019_Write(RTL_REG_BNRY, RTL_PSTART_INIT);	// Indica o endereço da próxima página a ser lida pelo host
	rtl8019_Write(RTL_REG_CR, (RTL_PS0|RTL_RD2|RTL_START));	// Selecionar página 1, DMA desligada e NIC trabalhando
	rtl8019_Write(RTL_REG_CURR, RTL_PSTART_INIT);	// Aponta para a primeira página do pacote recebido
	
	// TIRADO DO COMETÁRIO ÚLTIMA ATUALIZAÇÃO
	rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_START));	// Selecionar página 0, DMA desligada e NIC trabalhando
	
	rtl8019_Write(RTL_REG_ISR, 0x10);				// Limpa a interrupção de overflow de recepção
	rtl8019_Write(RTL_REG_TCR, 0); 					// RTL operando em modo normal e geração de CRC para recepção e transmissão
	
    if(resend)
        rtl8019_Write(RTL_REG_CR, (RTL_RD2|RTL_TXP|RTL_START));	// Selecionar página 0, DMA desligada, comando para retransmitir pacote e iniciar a NIC

    rtl8019_Write(RTL_REG_ISR, 0xFF);				// Limpa todas as interrupções
}
