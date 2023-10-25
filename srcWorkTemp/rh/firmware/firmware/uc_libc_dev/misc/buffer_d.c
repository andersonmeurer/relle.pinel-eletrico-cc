#include "buffer_d.h"

#if (BUFFER_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif


// BUFFER DIMENSIONAL
// Este buffer guarda uma cadeia de bytes em cada posi��o, diferente do buffer.c que guarda um byte por posi��o

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa o buffer com a posi��o de mem�ria apontada pelo vetor
// Parametros: 	Endere�o da estrutura do buffer, endere�o do vetor de dados,??? e offset de bytes
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferD_Init(buffer_d_t *buffer, u8 *start, u16 size, u16 offset) {
	buffer->dataptr = start;							// Aponta o inicio de mem�ria alocada para o buffer
	buffer->size = size*offset;							// Indica o tamanho do buffer dimensional
	buffer->offset = offset;							// Especifica o deslocamento de bytes no buffer buffer[tam][offset]
	buffer->dataindex = 0;								// Indica o endere�o do primeiro byte ainda n�o lido
	buffer->datalength = 0;								// Indica a quantidade de bytes no buffer

	#if (BUFFER_USE_DEBUG == pdON)
	plog("BUF: addr buffer 0x%x"CMD_TERMINATOR, buffer);
	plog("BUF: addr data buffer 0x%x"CMD_TERMINATOR, start);
	plog("BUF: size buf %u * %u = %u"CMD_TERMINATOR, size, buffer->offset, buffer->size);
	#endif
}


//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Adiciona os bytes do buffer para o BufferD
// Parametros: 	Ponteiros para estrutura do buffer e do bufferdata
// Retorna: 	Retorna pdPASS se o dados for gravado com sucesso ou c�digo de erro
//------------------------------------------------------------------------------------------------------------------
int bufferD_AddToEnd(buffer_d_t* buffer, u8 *bufferdata) {
	u16 i;

	if(buffer->datalength+buffer->offset <= buffer->size) {		// Checa se existe espa�o no buffer
		for(i=0; i<buffer->offset; i++) {						// Faz para tudo o bufferdata
			// Salva o byte no fim do buffer. O operado mod � para continuar a partir do endere�o de inicio, pois � um buffer circulars
			buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = bufferdata[i];
			buffer->datalength++;								// Incrementa a quantidade de bytes
		}
		return pdPASS;											// Retorna com sucesso
	}
	else 
		return errBUFFER_FULL;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� o conte�do do bufferD para para um buffer de retorno.
// Parametros: 	Ponteiro da estrutura do bufferD e o ponteiro do bufferdata para ser prenchidos.
// Retorna: 	Retona pdPASS caso leitura foi executada, ou c�digo de erro
//------------------------------------------------------------------------------------------------------------------
int bufferD_GetFromFront(buffer_d_t* buffer, u8 *bufferdata) {
	u16 i;
	u8 data;

	if(buffer->datalength) {									// Checa se existem bytes no bufferD
		for(i=0; i<buffer->offset; i++) {						// Faz para todo o bufferdata
			data = buffer->dataptr[buffer->dataindex];			// Capturo o primeiro byte que chegou que n�o foi lido
			bufferdata[i]= data;								// Atribuo o byte lido do bufferD para o bufferdata
			buffer->dataindex++;								// Movo o ponteiro para o pr�ximo byte ainda n�o lido

			if(buffer->dataindex >= buffer->size)				// Verifico se chegou ao fim do endere�o do bufferD
				buffer->dataindex %= buffer->size;				// Retorno ao inicio do bufferD + offset de deslocamento do byte

			buffer->datalength--;								// Diminuo a quantidade de bytes no bufferD
		}
		
		return pdPASS;
	}
	
	return errBUFFER_EMPTY;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Verifica se o buffer esta cheio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer n�o est� cheio, sen�o retoirna pdFALSE
//------------------------------------------------------------------------------------------------------------------
int bufferD_IsNotFull(buffer_d_t* buffer) {
	if (buffer->datalength < buffer->size)
			return pdTRUE;
	else	return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Verifica se o buffer esta vazio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer est� vazio, sen�o retorna pdFALSE
//------------------------------------------------------------------------------------------------------------------
int bufferD_IsEmpty(buffer_d_t* buffer) {
	if (buffer->datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de recep��o
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferD_Clear(buffer_d_t* buffer) {
	buffer->datalength = 0;
}
