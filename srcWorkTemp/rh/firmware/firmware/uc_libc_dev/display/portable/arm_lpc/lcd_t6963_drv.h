#ifndef __LCD_T6963_DRV_H
#define __LCD_T6963_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_display.h"
#include "delay.h"

#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
	void lcd_write(u8 reg, u8 cd);
	#define lcd_write_command(reg)	lcd_write(reg, 1)
	#define lcd_write_data(reg)		lcd_write(reg, 0)

	u8 lcd_read(u8 cd);
	#define lcd_read_command()		lcd_read(1)
	#define lcd_read_data()			lcd_read(0)
	
	void lcd_config_ports(void);
#elif #if (LCD_CONNECTION == LCD_CONNECTION_IOMAP)
	// ACESSO DIRETO IOMAP RAM
	static volatile u8
		*pLcdG_Command = (u8 *) LCD_ADDR_CMD, 	// read status and write command
		*pLcdG_Data = (u8 *) LCD_ADDR_DATA; 	// read last address data and write data
#else
	#error NÃO FOI DEFINIDO O TIPO DE CONEXÃO DO LPC. VEJA _CONFIG_LCD.H
#endif


#endif
