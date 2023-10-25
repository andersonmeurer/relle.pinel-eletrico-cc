#ifndef __TFT_H
#define __TFT_H

#include "_config_cpu_.h"
#include "_config_lib_tft.h"

#if !defined(cortexm3)
#error ARM não suporta esse recurso
#endif

#if !defined(USE_EXT_SDRAM)
#error O controlador gráfico requer sdram
#endif

#define tft_CursorEnable()	CRSR_CTRL |= 1
#define tft_CursorDisable()	CRSR_CTRL &= ~1
#define tft_Enable()		LCD_CTRL |= (1 << 0)
#define tft_Disable()		LCD_CTRL &= ~(1 << 0)
#define tft_PowerON()		LCD_CTRL |= (1 << 11)
#define tft_PowerOff()		LCD_CTRL &= ~(1 << 11)
#define tft_BaclLightOn()	TFT_BACKLIGHT_SET = TFT_BACKLIGHT_NUMPIN
#define tft_BaclLightOff() 	TFT_BACKLIGHT_CLR = TFT_BACKLIGHT_NUMPIN

void tft_Init(void);
void tft_Frame(u32 base);
#if (TFT_USE_CURSOR > 0)
void tft_LoadCursor(void);
void tft_CursorPos(int x, int y);
#endif

#endif
