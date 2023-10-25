#ifndef __LCD_DRV_H
#define __LCD_DRV_H

#include <util/delay.h>
#include "_config_cpu_.h"
#include "_config_cpu_display.h"

#define delay_ms _delay_ms

void lcd_config_ports(void);
void lcd_write(u8 reg, u8 rs);
#define lcd_write_command(reg)	lcd_write(reg, 0)
#define lcd_write_data(reg)		lcd_write(reg, 1)

u8 lcd_read(u8 rs);
#define lcd_read_command()		lcd_read(0)
#define lcd_read_data()			lcd_read(1)	


	
#endif
