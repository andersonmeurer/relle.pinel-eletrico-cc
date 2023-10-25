#include "buffer_word.h"

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o buffer com a posição de memória apontada pelo vetor
// Parametros: 	Endereço da estrutura do buffer, endereço do vetor de dados e o tamanho do vetor
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferW_Init(buffer_word_t* buffer, u16 *start, u16 size) {
	// set start pointer of the buffer
	buffer->dataptr = start;
	buffer->size = size;
	// initialize index and length
	buffer->dataindex = 0;
	buffer->datalength = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o primeiro caractere do buffer e retira-o do buffer
// Parametros: 	Ponteiro da estrutura do buffer
// Retorna: 	Primeiro caractere do buffer
//------------------------------------------------------------------------------------------------------------------
u16 bufferW_GetFromFront(buffer_word_t* buffer) {
	u16 data = 0;
	
	// check to see if there's data in the buffer
	if(buffer->datalength)	
	{
		// get the first character from buffer
		data = buffer->dataptr[buffer->dataindex];
		// move index down and decrement length
		buffer->dataindex++;
		if(buffer->dataindex >= buffer->size)	
		{
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength--;
	}
	
	return data;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Elimina do buffer N bytes
// Parametros: 	Ponteiro para estrutura do buffer e quantidades de bytes
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferW_DumpFromFront(buffer_word_t* buffer, u16 numbytes) {
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
// Descrição: 	Retorna o caractere apontado pelo indece
// Parametros:	Ponteiro para estrutura do buffer e o indice do dado
// Retorna: 	O caractere apontado pelo indece
//------------------------------------------------------------------------------------------------------------------
u16 bufferW_GetAtIndex(buffer_word_t* buffer, u16 index) {
	// return character at index in buffer
	return buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Adiciona o caractere no fim do buffer
// Parametros: 	Ponteiro para estrutura do buffer e o dados a ser gravados
// Retorna: 	Retone pdPASS se o dados for gravado com sucesso ou retorana com código de erro
//------------------------------------------------------------------------------------------------------------------
int bufferW_AddToEnd(buffer_word_t* buffer, u16 data) {
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
// Descrição: 	Verifica se o buffer esta cheio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna true se o buffer não está cheio
//------------------------------------------------------------------------------------------------------------------
int bufferW_IsNotFull(buffer_word_t* buffer) {
	// check to see if the buffer has room
	// return true if there is room
	return (buffer->datalength < buffer->size);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Verifica se o buffer esta vazio
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Retorna true se o buffer está vazio
//------------------------------------------------------------------------------------------------------------------
int bufferW_IsEmpty(buffer_word_t* buffer) {
	return (buffer->datalength == 0);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de recepção
// Parametros: 	Ponteiro para estrutura do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void bufferW_Clear(buffer_word_t* buffer) {
	buffer->datalength = 0;
}
