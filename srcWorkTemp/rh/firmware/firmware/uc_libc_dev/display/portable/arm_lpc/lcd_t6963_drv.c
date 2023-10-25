#include "lcd_t6963_drv.h"

#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
// ESCREVE NO LCD
void lcd_write(u8 reg, u8 cd) {
	LCD_PORT_CLR = LCD_PIN_WR;			// WR = 0;
				
	if (cd)	LCD_PORT_SET = LCD_PIN_CD;	// CD = 1;	grava comando
	else	LCD_PORT_CLR = LCD_PIN_CD;	// CD = 0;	grava dados
				
	// Atualizar barramento do LCD com o o valor do registrador
	if (reg&0x80)		LCD_PORT_SET = LCD_PIN_D7;
	else				LCD_PORT_CLR = LCD_PIN_D7;
	if (reg&0x40)		LCD_PORT_SET = LCD_PIN_D6;
	else				LCD_PORT_CLR = LCD_PIN_D6;
	if (reg&0x20)		LCD_PORT_SET = LCD_PIN_D5;
	else				LCD_PORT_CLR = LCD_PIN_D5;
	if (reg&0x10)		LCD_PORT_SET = LCD_PIN_D4;
	else				LCD_PORT_CLR = LCD_PIN_D4;
	if (reg&0x08)		LCD_PORT_SET = LCD_PIN_D3;
	else				LCD_PORT_CLR = LCD_PIN_D3;
	if (reg&0x04)		LCD_PORT_SET = LCD_PIN_D2;
	else				LCD_PORT_CLR = LCD_PIN_D2;
	if (reg&0x02)		LCD_PORT_SET = LCD_PIN_D1;
	else				LCD_PORT_CLR = LCD_PIN_D1;
	if (reg&0x01)		LCD_PORT_SET = LCD_PIN_D0;
	else				LCD_PORT_CLR = LCD_PIN_D0;
	
	LCD_PORT_CLR = LCD_PIN_CE;	// E = 0 Habilita display
	delay_us(LCD_DELAY);
	LCD_PORT_SET = LCD_PIN_CE;	// E = 1 Desabilita display

	LCD_PORT_SET = LCD_PIN_WR;			// WR = 1
}
	
// LÊ NO LCD
u8 lcd_read(u8 cd) {
	u8 data = 0;
			
	LCD_PORT_DIR &= ~(LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como entrada		
	LCD_PORT_CLR = LCD_PIN_RD;			// RD = 0;

	if (cd)	LCD_PORT_SET = LCD_PIN_CD;	// CD = 1; Lê comando
	else	LCD_PORT_CLR = LCD_PIN_CD;	// CD = 0; Lê dado
			
	LCD_PORT_CLR = LCD_PIN_E;			// E = 0 Habilita display
		
	delay_us(LCD_DELAY);
		
	if (LCD_PORT_IN&LCD_PIN_D7)		data|=0x80;
	if (LCD_PORT_IN&LCD_PIN_D6)		data|=0x40;
	if (LCD_PORT_IN&LCD_PIN_D5)		data|=0x20;
	if (LCD_PORT_IN&LCD_PIN_D4)		data|=0x10;
	if (LCD_PORT_IN&LCD_PIN_D3)		data|=0x08;
	if (LCD_PORT_IN&LCD_PIN_D2)		data|=0x04;
	if (LCD_PORT_IN&LCD_PIN_D1)		data|=0x02;
	if (LCD_PORT_IN&LCD_PIN_D0)		data|=0x01;
			
	LCD_PORT_SET = LCD_PIN_CE;	// E = 1 Desabilita display
	LCD_PORT_SET = LCD_PIN_RD;	// RD = 1;
	LCD_PORT_DIR |= (LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como saida
				
	return data;
}


void lcd_config_ports(void)	{
	LCD_PORT_DIR |= (LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como saida
	LCD_PORT_DIR |= (LCD_PIN_RD|LCD_PIN_WR|LCD_PIN_CE|LCD_PIN_CD); 	// Pinos de controle como saida
	LCD_PORT_SET = LCD_PIN_CE|LCD_PIN_RD|LCD_PIN_WR|LCD_PIN_CD;		// #CE= 1 #WR=1 #RD=1 #CD=1
}

#endif
