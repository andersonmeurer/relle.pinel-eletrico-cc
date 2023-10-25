#ifndef __LCD16X4_H
#define __LCD16X4_H

#include "lcd.h"

#define LCD_MAX_COL				16
#define LCD_MAX_ROW				4

#define lcd_Init				lcd_init
#define lcd_SetCursor			lcd_set_cursor
#define lcd_MoveCursor			lcd_move_cursor
#define lcd_Command				lcd_command
#define lcd_Clear				lcd_clear
#define lcd_ClearLine(y)		lcd_clear_line(y, 0x0F, 0x10) 	// 0x0F = Endereço final da coluna, 0x10 idem abaixo
#define lcd_GotoXY(x,y)			lcd_goto_xy(x, y ,0x10)			// 0x10 = Offset da terceira e quarta linha
#define lcd_SetAddressData		lcd_set_addr_data
#define lcd_PutString			lcd_put_string
#define lcd_LastAddressData		lcd_last_addr_data
#define lcd_PutChar				lcd_put_char
	
#endif
