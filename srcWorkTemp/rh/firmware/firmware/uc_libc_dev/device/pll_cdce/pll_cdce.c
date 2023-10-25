#include "pll_cdce.h"

#if (CDCE_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif


int cdce_Init(u8 addr_device) {	
	u8 reg_value;
	int ret;
			
	#if (CDCE_USE_DEBUG == pdON)	
	plog("CDCE send cmd get ID"CMD_TERMINATOR);
	#endif
	
	if ( (ret = cdce_ReadReg(addr_device, 0x00, &reg_value)) != pdPASS)
		return ret;
			
	#if (CDCE_USE_DEBUG == pdON)
	plog("CDCE ID = 0x%x"CMD_TERMINATOR, reg_value);
	
	u8 x;
	for (x = 1; x <= 6; x++) {
		if ( (ret = cdce_ReadReg(addr_device, x, &reg_value)) != pdPASS)
			return ret;
		plog("CDCE REG 0x%x = 0x%x"CMD_TERMINATOR, x, reg_value);
	}
	
	#endif
	
	return pdPASS;
}


//int cdce_ConfigRegs ( u8 addr_device, u8 cmd ) {
//}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê uma registrador do PLL CDCE949
// Parametros: 	Endereço do dispositivo, offset do registrador e o ponteiro para retorno do byte lido
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int cdce_ReadReg(u8 addr_device, u8 addr_reg, u8 *data) {
	n8 return_code;
	
	addr_reg |= CDCE_BYTE_ACCESS;  // Anexa ao endereço do registrador o tipo de acesso em bytes
	
	return_code = cdce_i2c_Send(addr_device, &addr_reg, 1);
	
	if (return_code == pdPASS)
		return_code = cdce_i2c_Receive(addr_device, data, 1);
		
	return return_code;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve dados no registrador desejado
// Parametros: 	Endereço do dispositivo, offset do registrador e o dado a ser gravado ao registrador
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int cdce_WriteReg(u8 addr_device, u8 addr_reg, u8 data) {
	u8 buffer[2]; 
		
	buffer[0] = CDCE_BYTE_ACCESS | addr_reg;
	buffer[1] = data;
				
	return cdce_i2c_Send(addr_device, buffer, 2);
}



//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emite comandos para configuração dos registradores
// Parametros: 	Endereço do dispositivo
//				Comando a ser emitidos e ação ou valores a ser gravados
//					Comando CDCE_M1			ação 	CDCE_M1_SOURCE_FIN / CDCE_M1_SOURCE_PLL1
//					Comando CDCE_MUX[4:1] 	ação 	CDCE_MUX_SOURCE_FIN / CDCE_MUX_SOURCE_PLL
//					Comando CDCE_XCSEL 		valores de 0 a 20
//					Comando CDCE_PDIV1 		valores de 0 a 0x3ff
//					Comando CDCE_PDIV[9:2] 	valores de 0 a 0x7f
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int cdce_Config ( u8 addr_device, u8 cmd, u16 action ) {
	int ret = pdPASS;
	u8 aux;
	
	if ( (cmd & 0xF0) == 0) {																	// Checa se é comando genérico
		switch (cmd) {
			case CDCE_PDIV1: {																	// Checa se é divisor da saida y1
				// SALVA AS CONFIGURAÇÕES QUE NÃO PERTENCEM AO PDIV1
				if ( (ret = cdce_ReadReg(addr_device, CDCE_REG_PDIV1_MSB, &aux)) != pdPASS)		// Lê o byte do registrador
					return ret;
				
				aux &=~ CDCE_PDIV1_MASK_MSB;													
				aux |= ((u8) ((action & 0xFF00) >> 8)) & CDCE_PDIV1_MASK_MSB;
													
				#if (CDCE_USE_DEBUG == pdON)
				plog("CDCE WR PDIV1 MSB. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_REG_PDIV1_MSB, aux);
				plog("CDCE WR PDIV1 LSB. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_REG_PDIV1_LSB, (u8)action);
				#endif
					
				ret = cdce_WriteReg(addr_device, CDCE_REG_PDIV1_MSB, aux);						// Grava o valor mais significativo do PDIV1
				ret = cdce_WriteReg(addr_device, CDCE_REG_PDIV1_LSB, (u8)action);				// Grava o valor menos significativo do PDIV1

				break;
			}
		
			case CDCE_XCSEL: {
				// SALVA AS CONFIGURAÇÕES QUE NÃO PERTENCEM AO XCSEL
				if ( (ret = cdce_ReadReg(addr_device, CDCE_XCSEL_REG, &aux)) != pdPASS)
					return ret;

				aux &=~ CDCE_XCSEL_MASK;
				aux |= ((u8) (action << 3)) & CDCE_XCSEL_MASK;
				
				#if (CDCE_USE_DEBUG == pdON)
				plog("CDCE WR XCSEL. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_XCSEL_REG, aux);
				#endif
			
				ret = cdce_WriteReg(addr_device, CDCE_XCSEL_REG, aux);

				break;
			}

			case CDCE_M1: {
				// SALVA AS CONFIGURAÇÕES QUE NÃO PERTENCEM AO M1
				if ( (ret = cdce_ReadReg(addr_device, CDCE_M1_REG, &aux)) != pdPASS)
					return ret;
			
				aux &=~ CDCE_M1_MASK;
				aux |= action & CDCE_M1_MASK;

				#if (CDCE_USE_DEBUG == pdON)
				plog("CDCE WR M1. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_M1_REG, aux);
				#endif
			
				ret = cdce_WriteReg(addr_device, CDCE_M1_REG, aux);
			
				break;
			}
		}
	
	// CMD MUX[4:1]
	} else if ( cmd & 0x10) {																	// Checa se é comando MUX dos PLLS
		// SALVA AS CONFIGURAÇÕES QUE NÃO PERTENCEM AO MUX
		if ( (ret = cdce_ReadReg(addr_device, CDCE_MUX1_REG+0x10*(cmd&0xf), &aux)) != pdPASS)
			return ret;
			
		aux &=~ CDCE_MUX_MASK;
		aux |= action & CDCE_MUX_MASK;
			
		#if (CDCE_USE_DEBUG == pdON)
		plog("CDCE WR MUX. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_MUX1_REG+0x10*(cmd&0xf), aux);
		#endif
			
		ret = cdce_WriteReg(addr_device, CDCE_MUX1_REG+0x10*(cmd&0xf), aux);		
	
	// PDIV2,4,6,8
	} else if ( cmd & 0x20) {																	// Checa se é comando PDIV
		// SALVA AS CONFIGURAÇÕES QUE NÃO PERTENCEM AO PDIV
		if ( (ret = cdce_ReadReg(addr_device, CDCE_PDIV2_REG+0x10*(cmd&0xf), &aux)) != pdPASS)
			return ret;
			
		aux &=~ CDCE_PDIV_MASK;			
		aux |= (action & CDCE_PDIV_MASK);

		#if (CDCE_USE_DEBUG == pdON)
		plog("CDCE PDIV. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_PDIV2_REG+0x10*(cmd&0xf), aux);
		#endif
			
		ret = cdce_WriteReg(addr_device, CDCE_PDIV2_REG+0x10*(cmd&0xf), aux);
	
	// PDIV3,5,7,9
	} else if ( cmd & 0x40) {																	// Checa se é comando PDIV
		#if (CDCE_USE_DEBUG == pdON)
			plog("CDCE PDIV. reg=0x%x - action = 0x%x"CMD_TERMINATOR, CDCE_PDIV3_REG+0x10*(cmd&0xf), (u8)action);
			#endif
			
			ret = cdce_WriteReg(addr_device, CDCE_PDIV3_REG+0x10*(cmd&0xf), (u8)action);
	}
	
	return ret;
}

#if (CDCE_USE_DEBUG == pdON)
//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve dados no registrador desejado
// Parametros: 	Endereço do dispositivo, offset do registrador e o dado a ser gravado ao registrador
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int cdce_ViewRegsPLL (u8 addr_device, u8 pll ) {
	u8 x, reg_value;
	int ret;

	for ( x = pll; x < pll+0x10; x++) {
		if ( (ret = cdce_ReadReg(addr_device, x, &reg_value)) != pdPASS)
			return ret;
		plog("CDCE REG 0x%x = 0x%x"CMD_TERMINATOR, x, reg_value);
	}

	return pdPASS;	
}
#endif

