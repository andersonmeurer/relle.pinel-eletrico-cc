#include "lcd_drv.h"

#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
void lcd_config_ports(void)	{
	LCD_PORTDATA_DIR &= ~(LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como entrada
	LCD_PORTCTRL_DIR |= (LCD_PIN_RS|LCD_PIN_WR|LCD_PIN_E); // Pinos de controle como saida
	LCD_PORTCTRL_CLR |= LCD_PIN_E;

//	while(1) {
//		LED1_OFF = LED1; //lcd_read(0);
//		delay_ms(LCD_DELAY_HOME);
//		LED1_ON = LED1; //lcd_read(1);
//		delay_ms(LCD_DELAY_HOME);
//	}
}


// LÊ NO LCD
u8 lcd_read(u8 rs) {
	u8 data = 0;

	LCD_PORTCTRL_SET = LCD_PIN_WR;			// WR = 1;
	if (rs)	LCD_PORTCTRL_SET = LCD_PIN_RS;	// RS = 1;
	else	LCD_PORTCTRL_CLR = LCD_PIN_RS;	// RS = 0;
	LCD_PORTCTRL_SET = LCD_PIN_E;			// E = 1
		
	delay_us(LCD_DELAY);
		
	if (LCD_PORTDATA_IN&LCD_PIN_D7)		data|=0x80;
	if (LCD_PORTDATA_IN&LCD_PIN_D6)		data|=0x40;
	if (LCD_PORTDATA_IN&LCD_PIN_D5)		data|=0x20;
	if (LCD_PORTDATA_IN&LCD_PIN_D4)		data|=0x10;
	if (LCD_PORTDATA_IN&LCD_PIN_D3)		data|=0x08;
	if (LCD_PORTDATA_IN&LCD_PIN_D2)		data|=0x04;
	if (LCD_PORTDATA_IN&LCD_PIN_D1)		data|=0x02;
	if (LCD_PORTDATA_IN&LCD_PIN_D0)		data|=0x01;

	LCD_PORTCTRL_CLR = LCD_PIN_E;			// E = 0
				
	return data;
}

// ESCREVE NO LCD
void lcd_write(u8 reg, u8 rs) {
	LCD_PORTDATA_DIR |= (LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como saida
	LCD_PORTCTRL_CLR = LCD_PIN_WR;			// WR = 0;
		
	if (rs)	LCD_PORTCTRL_SET = LCD_PIN_RS;	// RS = 1;
	else	LCD_PORTCTRL_CLR = LCD_PIN_RS;	// RS = 0;
				
	// Atualizar barramento do LCD com o o valor do registrador
	if (reg&0x80) LCD_PORTDATA_SET = LCD_PIN_D7; else LCD_PORTDATA_CLR = LCD_PIN_D7;
	if (reg&0x40) LCD_PORTDATA_SET = LCD_PIN_D6; else LCD_PORTDATA_CLR = LCD_PIN_D6;
	if (reg&0x20) LCD_PORTDATA_SET = LCD_PIN_D5; else LCD_PORTDATA_CLR = LCD_PIN_D5;
	if (reg&0x10) LCD_PORTDATA_SET = LCD_PIN_D4; else LCD_PORTDATA_CLR = LCD_PIN_D4;
	if (reg&0x08) LCD_PORTDATA_SET = LCD_PIN_D3; else LCD_PORTDATA_CLR = LCD_PIN_D3;
	if (reg&0x04) LCD_PORTDATA_SET = LCD_PIN_D2; else LCD_PORTDATA_CLR = LCD_PIN_D2;
	if (reg&0x02) LCD_PORTDATA_SET = LCD_PIN_D1; else LCD_PORTDATA_CLR = LCD_PIN_D1;
	if (reg&0x01) LCD_PORTDATA_SET = LCD_PIN_D0; else LCD_PORTDATA_CLR = LCD_PIN_D0;
		
	LCD_PORTCTRL_SET = LCD_PIN_E;	// E = 1
	delay_us(LCD_DELAY);
	LCD_PORTCTRL_CLR = LCD_PIN_E;	// E = 0
	LCD_PORTDATA_DIR &= ~(LCD_PIN_D7|LCD_PIN_D6|LCD_PIN_D5|LCD_PIN_D4|LCD_PIN_D3|LCD_PIN_D2|LCD_PIN_D1|LCD_PIN_D0); // Pinos dos dados como entrada
}

#endif
