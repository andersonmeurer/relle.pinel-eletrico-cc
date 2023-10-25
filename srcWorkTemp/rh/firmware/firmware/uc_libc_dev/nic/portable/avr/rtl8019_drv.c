#include "rtl8019_drv.h"

#if (RTL_CONNECTION == RTL_CONNECTION_GPIO)

u8 rtl8019_Read(u8 address) {
	u8 data = 0;

  	// ATAULIZANDO ENDEREÇO NO BUS
	if (address & 0x01)	sbi(RTL_ADDR_PORT, RTL_A0_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A0_PIN);
	if (address & 0x02)	sbi(RTL_ADDR_PORT, RTL_A1_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A1_PIN);
	if (address & 0x04)	sbi(RTL_ADDR_PORT, RTL_A2_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A2_PIN);
	if (address & 0x08)	sbi(RTL_ADDR_PORT, RTL_A3_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A3_PIN);
	if (address & 0x10)	sbi(RTL_ADDR_PORT, RTL_A4_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A4_PIN);

	// EMITINDO PULSO READ
	cbi(RTL_CTRL_PORT, RTL_RD_PIN);
	nop();
	if (RTL_DATA_IN & _BV(RTL_D0_PIN))	data += 1;
	if (RTL_DATA_IN & _BV(RTL_D1_PIN))	data += 2;
	if (RTL_DATA_IN & _BV(RTL_D2_PIN))	data += 4;
	if (RTL_DATA_IN & _BV(RTL_D3_PIN))	data += 8;
	if (RTL_DATA_IN & _BV(RTL_D4_PIN))	data += 16;
	if (RTL_DATA_IN & _BV(RTL_D5_PIN))	data += 32;
	if (RTL_DATA_IN & _BV(RTL_D6_PIN))	data += 64;
	if (RTL_DATA_IN & _BV(RTL_D7_PIN))	data += 128;
	
	// DESLIGANDO PULSO READ
	sbi(RTL_CTRL_PORT, RTL_RD_PIN);
	
	return data;
/*
   	u8 byte;
   
   	// assert the address, leaving the non-address pins intact
   	address |= (RTL_ADDRESS_PORT & ~RTL_ADDRESS_MASK);
   	RTL_ADDRESS_PORT = address;
   
   	// assert read
   	RTL_CONTROL_PORT &= ~_BV(RTL_CONTROL_READPIN);
   	nop();
   
   	// read in the data
   	byte = RTL_DATA_PIN;

   	// negate read
   	RTL_CONTROL_PORT |= _BV(RTL_CONTROL_READPIN);


   	return byte;
   	*/
}
	
void rtl8019_Write(u8 address, u8 data) {
  	// PORTA GPIO COMO SAIDA
  	RTL_DATA_DDR  |= (_BV(RTL_D0_PIN) | _BV(RTL_D1_PIN) | _BV(RTL_D2_PIN) | _BV(RTL_D3_PIN) | _BV(RTL_D4_PIN) | _BV(RTL_D5_PIN) | _BV(RTL_D6_PIN) | _BV(RTL_D7_PIN) );
  	
  	// ATAULIZANDO ENDEREÇO NO BUS
	if (address & 0x01)	sbi(RTL_ADDR_PORT, RTL_A0_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A0_PIN);
	if (address & 0x02)	sbi(RTL_ADDR_PORT, RTL_A1_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A1_PIN);
	if (address & 0x04)	sbi(RTL_ADDR_PORT, RTL_A2_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A2_PIN);
	if (address & 0x08)	sbi(RTL_ADDR_PORT, RTL_A3_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A3_PIN);
	if (address & 0x10)	sbi(RTL_ADDR_PORT, RTL_A4_PIN);
	else				cbi(RTL_ADDR_PORT, RTL_A4_PIN);
	
	// ATAULIZANDO DADOS NO BUS
	if (data & 0x01)	sbi(RTL_DATA_PORT, RTL_D0_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D0_PIN);
	if (data & 0x02)	sbi(RTL_DATA_PORT, RTL_D1_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D1_PIN);
	if (data & 0x04)	sbi(RTL_DATA_PORT, RTL_D2_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D2_PIN);
	if (data & 0x08)	sbi(RTL_DATA_PORT, RTL_D3_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D3_PIN);
	if (data & 0x10)	sbi(RTL_DATA_PORT, RTL_D4_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D4_PIN);
	if (data & 0x20)	sbi(RTL_DATA_PORT, RTL_D5_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D5_PIN);
	if (data & 0x40)	sbi(RTL_DATA_PORT, RTL_D6_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D6_PIN);
	if (data & 0x80)	sbi(RTL_DATA_PORT, RTL_D7_PIN);
	else				cbi(RTL_DATA_PORT, RTL_D7_PIN);
	
	// EMITINDO PULSO WRITE
	cbi(RTL_CTRL_PORT, RTL_WR_PIN);
	
	nop();
	
	// DESLIGANDO PULSO WRITE
  	sbi(RTL_CTRL_PORT, RTL_WR_PIN);
  	
	// PINOS PARA A BUS DATA COMO ENTRADA
	RTL_DATA_DDR  &= ~(_BV(RTL_D0_PIN) | _BV(RTL_D1_PIN) | _BV(RTL_D2_PIN) | _BV(RTL_D3_PIN) | _BV(RTL_D4_PIN) | _BV(RTL_D5_PIN) | _BV(RTL_D6_PIN) | _BV(RTL_D7_PIN) );
		
	// PINOS DA BUS DATA EM PULLUP
	// RTL_DATA_PORT |=  (_BV(RTL_D0_PIN) | _BV(RTL_D1_PIN) | _BV(RTL_D2_PIN) | _BV(RTL_D3_PIN) | _BV(RTL_D4_PIN) | _BV(RTL_D5_PIN) | _BV(RTL_D6_PIN) | _BV(RTL_D7_PIN) );
		
  	
  /*
   	// assert the address, leaving the non-address pins intact
   	address |= (RTL_ADDRESS_PORT & ~RTL_ADDRESS_MASK);
   	RTL_ADDRESS_PORT = address;
    
	// set data bus as output and place data on bus
   	RTL_DATA_DDR = 0xFF;
   	RTL_DATA_PORT = data;
    
	// toggle write pin
   	RTL_CONTROL_PORT &= ~_BV(RTL_CONTROL_WRITEPIN);
   	nop();
   	RTL_CONTROL_PORT |= _BV(RTL_CONTROL_WRITEPIN);

    
	// set data port back to input with pullups enabled
   	RTL_DATA_DDR = 0x00;
   	RTL_DATA_PORT = 0xFF;
   	*/
}

#endif

void rtl8019_SetupPorts(void) {
	#if (RTL_CONNECTION == RTL_CONNECTION_GPIO)							// Configura caso a interface seja IO
		// PINOS PARA A BUS ADDR COMO SAIDA
		RTL_ADDR_DDR |= (_BV(RTL_A0_PIN) | _BV(RTL_A1_PIN) | _BV(RTL_A2_PIN) | _BV(RTL_A3_PIN) | _BV(RTL_A4_PIN));
		
		// PINOS PARA A BUS DATA COMO ENTRADA
		RTL_DATA_DDR  &= ~(_BV(RTL_D0_PIN) | _BV(RTL_D1_PIN) | _BV(RTL_D2_PIN) | _BV(RTL_D3_PIN) | _BV(RTL_D4_PIN) | _BV(RTL_D5_PIN) | _BV(RTL_D6_PIN) | _BV(RTL_D7_PIN) );
		
		// PINOS DA BUS DATA EM PULLUP
		//RTL_DATA_PORT |=  (_BV(RTL_D0_PIN) | _BV(RTL_D1_PIN) | _BV(RTL_D2_PIN) | _BV(RTL_D3_PIN) | _BV(RTL_D4_PIN) | _BV(RTL_D5_PIN) | _BV(RTL_D6_PIN) | _BV(RTL_D7_PIN) );
		
		// PINOS PARA A BUS DE CONTROLE COMO SAIDA
		RTL_CTRL_DDR |= (_BV(RTL_RD_PIN) | _BV(RTL_WR_PIN));
		
		// DESABILITANDO OS PINOS DE CONTROLE
		RTL_CTRL_PORT |= (_BV(RTL_RD_PIN) | _BV(RTL_WR_PIN));
	#endif

	RTL_RESET_DDR |= _BV(RTL_RESET_PIN);				// enable output pin for Resetting the RTL
	cbi(RTL_RESET_PORT, RTL_RESET_PIN);					// RESET desativados da nic
}


