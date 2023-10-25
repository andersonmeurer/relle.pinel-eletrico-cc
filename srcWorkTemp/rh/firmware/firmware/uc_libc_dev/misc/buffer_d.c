#include "buffer_d.h"

#if (BUFFER_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif


// BUFFER DIMENSIONAL
// Este buffer guarda uma cadeia de bytes em cada posição, diferente do buffer.c que guarda um byte por posição

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o buffer com a posição de memória apontada pelo vetor
// Parametros: 	Endereço da estrutura do buffer, endereço do vetor de dados,??? e offset de bytes
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferD_Init(buffer_d_t *buffer, u8 *start, u16 size, u16 offset) {
	buffer->dataptr = start;							// Aponta o inicio de memória alocada para o buffer
	buffer->size = size*offset;							// Indica o tamanho do buffer dimensional
	buffer->offset = offset;							// Especifica o deslocamento de bytes no buffer buffer[tam][offset]
	buffer->dataindex = 0;								// Indica o endereço do primeiro byte ainda não lido
	buffer->datalength = 0;								// Indica a quantidade de bytes no buffer

	#if (BUFFER_USE_DEBUG == pdON)
	plog("BUF: addr buffer 0x%x"CMD_TERMINATOR, buffer);
	plog("BUF: addr data buffer 0x%x"CMD_TERMINATOR, start);
	plog("BUF: size buf %u * %u = %u"CMD_TERMINATOR, size, buffer->offset, buffer->size);
	#endif
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Adiciona os bytes do buffer para o BufferD
// Parametros: 	Ponteiros para estrutura do buffer e do bufferdata
// Retorna: 	Retorna pdPASS se o dados for gravado com sucesso ou código de erro
//------------------------------------------------------------------------------------------------------------------
int bufferD_AddToEnd(buffer_d_t* buffer, u8 *bufferdata) {
	u16 i;

	if(buffer->datalength+buffer->offset <= buffer->size) {		// Checa se existe espaço no buffer
		for(i=0; i<buffer->offset; i++) {						// Faz para tudo o bufferdata
			// Salva o byte no fim do buffer. O operado mod é para continuar a partir do endereço de inicio, pois é um buffer circulars
			buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = bufferdata[i];
			buffer->datalength++;								// Incrementa a quantidade de bytes
		}
		return pdPASS;											// Retorna com sucesso
	}
	else 
		return errBUFFER_FULL;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o conteúdo do bufferD para para um buffer de retorno.
// Parametros: 	Ponteiro da estrutura do bufferD e o ponteiro do bufferdata para ser prenchidos.
// Retorna: 	Retona pdPASS caso leitura foi executada, ou código de erro
//------------------------------------------------------------------------------------------------------------------
int bufferD_GetFromFront(buffer_d_t* buffer, u8 *bufferdata) {
	u16 i;
	u8 data;

	if(buffer->datalength) {									// Checa se existem bytes no bufferD
		for(i=0; i<buffer->offset; i++) {						// Faz para todo o bufferdata
			data = buffer->dataptr[buffer->dataindex];			// Capturo o primeiro byte que chegou que não foi lido
			bufferdata[i]= data;								// Atribuo o byte lido do bufferD para o bufferdata
			buffer->dataindex++;								// Movo o ponteiro para o próximo byte ainda não lido

			if(buffer->dataindex >= buffer->size)				// Verifico se chegou ao fim do endereço do bufferD
				buffer->dataindex %= buffer->size;				// Retorno ao inicio do bufferD + offset de deslocamento do byte

			buffer->datalength--;								// Diminuo a quantidade de bytes no bufferD
		}
		
		return pdPASS;
	}
	
	return errBUFFER_EMPTY;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Verifica se o buffer esta cheio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer não está cheio, senão retoirna pdFALSE
//------------------------------------------------------------------------------------------------------------------
int bufferD_IsNotFull(buffer_d_t* buffer) {
	if (buffer->datalength < buffer->size)
			return pdTRUE;
	else	return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Verifica se o buffer esta vazio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer está vazio, senão retorna pdFALSE
//------------------------------------------------------------------------------------------------------------------
int bufferD_IsEmpty(buffer_d_t* buffer) {
	if (buffer->datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de recepção
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferD_Clear(buffer_d_t* buffer) {
	buffer->datalength = 0;
}
