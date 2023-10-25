#ifndef __LCD20X4_H
#define __LCD20X4_H

#include "lcd.h"

#define LCD_MAX_COL				20
#define LCD_MAX_ROW				4

#define lcd_Init				lcd_init
#define lcd_SetCursor			lcd_set_cursor
#define lcd_MoveCursor			lcd_move_cursor
#define lcd_Command				lcd_command
#define lcd_Clear				lcd_clear
#define lcd_ClearLine(y)		lcd_clear_line(y, 0x13, 0x14) 	// 0x13 = Endereço final da coluna, 0x14 idem abaixo
#define lcd_GotoXY(x,y)			lcd_goto_xy(x, y ,0x14)			// 0x14 = Offset da terceira e quarta linha
#define lcd_SetAddressData		lcd_set_addr_data
#define lcd_PutString			lcd_put_string
#define lcd_LastAddressData		lcd_last_addr_data
#define lcd_PutChar				lcd_put_char
	
#endif
