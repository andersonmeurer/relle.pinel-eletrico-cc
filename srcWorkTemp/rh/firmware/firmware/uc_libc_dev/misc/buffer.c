#include "buffer.h"

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa o buffer com a posi��o de mem�ria apontada pelo vetor
// Parametros: 	Endere�o da estrutura do buffer, endere�o do vetor de dados e o tamanho do vetor
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void buffer_Init(buffer_t *buffer, u8 *start, u16 size) {
	// set start pointer of the buffer
	buffer->dataptr = start;
	buffer->size = size;
	// initialize index and length
	buffer->dataindex = 0;
	buffer->datalength = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o primeiro caractere do buffer e retira-o do buffer
// Parametros: 	Ponteiro da estrutura do buffer
// Retorna: 	Primeiro caractere do buffer
//------------------------------------------------------------------------------------------------------------------
u8 buffer_GetFromFront(buffer_t *buffer) {
	u8 data = 0;
	
	// check to see if there's data in the buffer
	if(buffer->datalength) {
		// get the first character from buffer
		data = buffer->dataptr[buffer->dataindex];
		// move index down and decrement length
		buffer->dataindex++;
		if(buffer->dataindex >= buffer->size) {
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength--;
	}
	
	return data;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Elimina do buffer N bytes
// Parametros: 	Ponteiro para estrutura do buffer e quantidades de bytes
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void buffer_DumpFromFront(buffer_t* buffer, u16 numbytes) {
	// dump numbytes from the front of the buffer
	// are we dumping less than the entire buffer?
	if(numbytes < buffer->datalength) {
		// move index down by numbytes and decrement length by numbytes
		buffer->dataindex += numbytes;
		if(buffer->dataindex >= buffer->size) {
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength -= numbytes;
	} else {
		// flush the whole buffer
		buffer->datalength = 0;
	}
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o caractere apontado pelo indece
// Parametros:	Ponteiro para estrutura do buffer e o indice do dado
// Retorna: 	O caractere apontado pelo indece
//------------------------------------------------------------------------------------------------------------------
u8 buffer_GetAtIndex(buffer_t* buffer, u16 index) {
	// return character at index in buffer
	return buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Adiciona o caractere no fim do buffer
// Parametros: 	Ponteiro para estrutura do buffer e o dados a ser gravados
// Retorna: 	pdPASS se o dados for gravado com sucesso ou erro de acesso
//------------------------------------------------------------------------------------------------------------------
int buffer_AddToEnd(buffer_t* buffer, u8 data) {
	// make sure the buffer has room
	if(buffer->datalength < buffer->size) {
		// save data byte at end of buffer
		buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
		// increment the length
		buffer->datalength++;
		// return success
		return pdPASS;
	} else 
		return errBUFFER_FULL;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Verifica se o buffer esta cheio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer n�o est� cheio
//------------------------------------------------------------------------------------------------------------------
int buffer_IsNotFull(buffer_t* buffer) {
	// check to see if the buffer has room
	// return true if there is room
	if (buffer->datalength < buffer->size)
		return pdTRUE;
	else
		return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Verifica se o buffer esta vazio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna pdTRUE se o buffer est� vazio
//------------------------------------------------------------------------------------------------------------------
int buffer_IsEmpty(buffer_t *buffer) {
	if (buffer->datalength == 0)
		return pdTRUE;
	else
		return pdFALSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de recep��o
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void buffer_Clear(buffer_t* buffer) {
	buffer->datalength = 0;
}

