#include "checksum.h"

// ################################################################################################################
// Descrição:	Calcula o ChecSum do buffer
// Parametros:	Ponteiro do buffer do frame, tamanho do buffer e o ponteiro da var para retorno do checksum com a substração
// Retorna: 	CS do frame
// ################################################################################################################
u8 checksum(u8 *buffer, u8 nbytes) {
	u8 byte_count;
	u8 *p_buffer;
	u16 sum = 0;

	for (byte_count=0, p_buffer=buffer; byte_count<nbytes; byte_count++, p_buffer++) {
		sum += *p_buffer;
	}
	
	sum %= 256;
	
	return sum;
}

/*
// ################################################################################################################
// Descrição:	Calcula o ChecSum do buffer
// Parametros:	Ponteiro do buffer do frame, tamanho do buffer e o ponteiro da var para retorno do checksum com a substração
// Retorna: 	CS do frame
// ################################################################################################################
u8 checksum(u8 *buffer, u8 nbytes, u8 *sum_ret) {
	u8 byte_count;
	u8 *p_buffer;
	u16 sum = 0;

	for (byte_count=0, p_buffer=buffer; byte_count<nbytes; byte_count++, p_buffer++) {
		sum += *p_buffer;
	}
	
	sum %=256;
	*sum_ret = 256 - sum;
	
	return sum;
}
*/

