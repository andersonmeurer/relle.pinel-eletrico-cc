#ifndef __CONFIG_TFT_H
#define __CONFIG_TFT_H

#include "_config_lib_.h"

#define TOUCH_LIMIT_AD			50	// limite do valor AD que identfica que o touch fora precionado

#define TFT_USE_BACKLIGHT 		pdON
#define TFT_USE_CURSOR 			1 // define o tipo de cursor. 0, desligar o recurso de touch e cursor
#define TFT_USE_ROTATE			TFT_ROTATE_NONE // TFT_ROTATE_UD / TFT_ROTATE_NONE

#define TFT_H_SIZE 				800
#define TFT_V_SIZE 				600 // 480 //600

//horizontal
#define TFT_H_PULSE          	256
#define TFT_H_FRONT_PORCH      	10
#define TFT_H_BACK_PORCH       	10

//vertical
#define TFT_V_PULSE           	45
#define TFT_V_FRONT_PORCH      	10
#define TFT_V_BACK_PORCH       	10

//geral
//#define C_GLCD_CLK_PER_LINE     (TFT_H_SIZE + TFT_H_PULSE + TFT_H_FRONT_PORCH + TFT_H_BACK_PORCH)
//#define C_GLCD_LINES_PER_FRAME  (TFT_V_SIZE + C_GLCD_V_PULSE + TFT_V_FRONT_PORCH + TFT_V_BACK_PORCH)
//#define C_GLCD_CLK_PER_FRAME	(C_GLCD_CLK_PER_LINE * C_GLCD_LINES_PER_FRAME)

#endif
