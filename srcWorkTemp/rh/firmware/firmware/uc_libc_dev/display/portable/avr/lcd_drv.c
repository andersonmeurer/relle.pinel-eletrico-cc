#include "lcd_drv.h"


void lcd_config_ports(void)	{
	sbi(LCD_PORT_CTRL_CONFIG, LCD_PIN_RW); // Pino RW como saida
	
	#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
		sbi(LCD_PORT_CTRL_CONFIG, LCD_PIN_E); // Pino E como saida
		sbi(LCD_PORT_CTRL_CONFIG, LCD_PIN_RS); // Pino RS como saida
		
		cbi(LCD_PORT_CTRL_OUT, LCD_PIN_E); // Desabilitando o LCD
	#endif
}

// LÊ NO LCD
u8 lcd_read(u8 rs) {
	sbi(LCD_PORT_CTRL_OUT, LCD_PIN_RW); // acessando para leitura WR = 1
	
	#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
		LCD_PORT_DATA_CONFIG = 0x0;			// Porta de dados como entrada
		
		if (rs)	 sbi(LCD_PORT_CTRL_OUT, LCD_PIN_RS); // se é acesso para dados
		else	 cbi(LCD_PORT_CTRL_OUT, LCD_PIN_RS); // se é acesso para comandos
		
		sbi(LCD_PORT_CTRL_OUT, LCD_PIN_E); 	// Habilitando o LCD	
		delay_ms(LCD_DELAY);
		u8 val = LCD_PORT_DATA_IN;		// Captura dados do barramento
		cbi(LCD_PORT_CTRL_OUT, LCD_PIN_E); 	// Desabilitando o LCD
		
		return val;
	#else										// IOMAP
		if (rs)	 return *(volatile u8 *) LCD_ADDR_DATA; // se é acesso para dados
		else	 return *(volatile u8 *) LCD_ADDR_CMD;  // se é acesso para comandos
	#endif
}

// ESCREVE NO LCD
void lcd_write(u8 reg, u8 rs) {
	cbi(LCD_PORT_CTRL_OUT, LCD_PIN_RW); // acessadno para escrita WR = 0		
	
	#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
		LCD_PORT_DATA_CONFIG = 0xFF;					// Porta de dados como saida
		if (rs)	 sbi(LCD_PORT_CTRL_OUT, LCD_PIN_RS); 	// se é acesso para dados
		else	 cbi(LCD_PORT_CTRL_OUT, LCD_PIN_RS); 	// se é acesso para comandos
	
		sbi(LCD_PORT_CTRL_OUT, LCD_PIN_E); // Habilitando o LCD
		LCD_PORT_DATA_OUT = reg;
		delay_ms(LCD_DELAY);
		cbi(LCD_PORT_CTRL_OUT, LCD_PIN_E); // Desabilitando o LCD		
	
	#else										// IOMAP
		if (rs)	 *(volatile u8 *) LCD_ADDR_DATA = reg;// se é acesso para dados
		else	 *(volatile u8 *) LCD_ADDR_CMD = reg; // se é acesso para comandos
	#endif
}
