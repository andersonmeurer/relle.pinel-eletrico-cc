#ifndef __BUFFER_D_H
#define __BUFFER_D_H

#include "_config_cpu_.h"

//-----------------------------------------------------------------------------------------------------------------
// ESTRUTURA DO BUFFER DIMENSIONAL [TAMANHO][OFFSET]
typedef volatile struct buffer_d_s	{
	volatile u8 *dataptr;			// Endereço incial de memória alocada para o buffer
	volatile u32 size;				// Tamanho do buffer alocado
	volatile u16 offset;			// Especifica o deslocamento de bytes no buffer buffer[tam][offset]
	volatile u32 datalength;		// Tamanho do dado armazenado
	volatile u32 dataindex;			// Indece do inicio do dado no buffer para gravar novos dados

} buffer_d_t;

void bufferD_Init(buffer_d_t *buffer, u8 *start, u16 size, u16 offset);
int bufferD_AddToEnd(buffer_d_t *buffer, u8 *bufferdata);
int bufferD_GetFromFront(buffer_d_t *buffer, u8 *bufferdata);
int bufferD_IsNotFull(buffer_d_t *buffer);
int bufferD_IsEmpty(buffer_d_t *buffer);
void bufferD_Clear(buffer_d_t *buffer);

#endif

