#include "crc8.h"

#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0

u8	crc8 ( u8 *data_in, u16 number_of_bytes_to_read ) {
	u8	 crc;
	u16 loop_count;
	u8  bit_counter;
	u8  data;
	u8  feedback_bit;
	
	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++) {
		data = data_in[loop_count];
		
		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;
	
			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}
		
			data = data >> 1;
			bit_counter--;
		
		} while (bit_counter > 0);
	}
	
	return crc;
}


/*
// ################################################################################################################
// Descrição:	Calcula o CRC do buffer
// Parametros:	Ponteiro do buffer do frame e seu tamanho
// Retorna: 	CRC do frame
// ################################################################################################################
unsigned char CRC(unsigned char *Buffer, unsigned char nbytes) {
	unsigned char crc_reg =0xff, poly, byte_count, bit_count;
	unsigned char *byte_point;
	unsigned char bit_point;

	for (byte_count=0, byte_point=Buffer; byte_count<nbytes; ++byte_count, ++byte_point) {
		for (bit_count=0, bit_point=0x80 ; bit_count<8; ++bit_count, bit_point>>=1)	{
			if (bit_point & *byte_point) {	// case for new bit = 1
				if (crc_reg & 0x80)
					poly = 1;	// define the polynomial
				else
					poly = 0x1c;
				crc_reg = ( (crc_reg << 1) | 1) ^ poly;
			} else {		// case for new bit = 0
				poly = 0;
				if (crc_reg & 0x80)
					poly=0x1d;
				crc_reg= (crc_reg << 1) ^ poly;
			}
		}
	}
	
	return ~crc_reg;	// Return CRC
}
*/

