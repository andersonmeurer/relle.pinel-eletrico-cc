#ifndef __BUFFER_H
#define __BUFFER_H

#include "_config_cpu_.h"

//-----------------------------------------------------------------------------------------------------------------
// ESTRUTURA DO BUFFER
typedef volatile struct buffer_s {
	u8 *dataptr;		// Endereço fisico da memória onde o buffer é armazenado
	u16 size;			// Tamnaho de alocação do buffer
	u16 datalength;		// Tamanho do dado armazenado
	u16 dataindex;		// Indece do inicio do dado no buffer
} buffer_t;

void buffer_Init(buffer_t *buffer, u8 *start, u16 size);
u8 buffer_GetFromFront(buffer_t* buffer);
void buffer_DumpFromFront(buffer_t* buffer, u16 numbytes);
u8 buffer_GetAtIndex(buffer_t* buffer, u16 index);
int buffer_AddToEnd(buffer_t* buffer, u8 data);
int buffer_IsNotFull(buffer_t* buffer);
int buffer_IsEmpty(buffer_t* buffer);
void buffer_Clear(buffer_t* buffer);

#endif

