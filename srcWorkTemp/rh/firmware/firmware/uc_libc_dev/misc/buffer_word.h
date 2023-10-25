#ifndef __BUFFER_W_H
#define __BUFFER_W_H

#include "_config_cpu_.h"

// BUFFER WORD
// Este buffer guarda dois bytes em cada posição, diferente do buffer.c que guarda um byte por posição

//-----------------------------------------------------------------------------------------------------------------
// Estrutura do buffer
typedef volatile struct buffer_word_s	{
	volatile u16 *dataptr;				// Endereço fisico da memória onde o buffer é armazenado
	volatile u16 size;					// Tamnaho de alocação do buffer
	volatile u16 datalength;			// Tamanho do dado armazenado
	volatile u16 dataindex;				// Indece do inicio do dado no buffer
} buffer_word_t;

void bufferW_Init(buffer_word_t* buffer, u16 *start, u16 size);
u16 bufferW_GetFromFront(buffer_word_t* buffer);
void bufferW_DumpFromFront(buffer_word_t* buffer, u16 numbytes);
u16 bufferW_GetAtIndex(buffer_word_t* buffer, u16 index);
int bufferW_AddToEnd(buffer_word_t* buffer, u16 data);
int bufferW_IsNotFull(buffer_word_t* buffer);
int bufferW_IsEmpty(buffer_word_t* buffer);
void bufferW_Clear(buffer_word_t* buffer);

#endif

