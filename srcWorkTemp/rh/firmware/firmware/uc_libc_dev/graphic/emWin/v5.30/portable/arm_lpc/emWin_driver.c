// TODO
// fazer distinção dos tipos de displays e touch

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "_config_lib_emwin.h"
#include <stddef.h> /* for size_t */

#if (GUI_SUPPORT_TOUCH > 0)
#include "tsc2046.h"
#endif

// Configuration checking
#if !defined(cortexm3)
#error Esta CPU não suporta esse recurso gráfico
#endif

#if !defined(USE_EXT_SDRAM)
#error O controlador gráfico requer sdram
#endif

#ifndef VRAM_ADDR
#define VRAM_ADDR 0 // TBD by customer: This has to be the frame buffer start address
#endif

#ifndef XSIZE_PHYS
#error Não foi definido o tamanho X físico do display
#endif

#ifndef YSIZE_PHYS
#error Não foi definido o tamanho Y físico do display
#endif

#ifndef COLOR_CONVERSION
#error Não foi definido o tipo de conversão de cores
#endif

#ifndef DISPLAY_DRIVER
#error Não foi definido o display
#endif

#ifndef NUM_VSCREENS
#define NUM_VSCREENS 1
#else

#if (NUM_VSCREENS <= 0)
#error At least one screeen needs to be defined!
#endif

#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
#error Virtual screens e multiplos buffers não são permitidos
#endif


// ###########################################################################################################################
// ###########################################################################################################################
// DRIVERS LCD
// ###########################################################################################################################
#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

#define tft_CursorEnable()	CRSR_CTRL |= 1
#define tft_CursorDisable()	CRSR_CTRL &= ~1
#define tft_Enable()		LCD_CTRL |= (1 << 0)
#define tft_Disable()		LCD_CTRL &= ~(1 << 0)
#define tft_PowerON()		LCD_CTRL |= (1 << 11)
#define tft_PowerOff()		LCD_CTRL &= ~(1 << 11)
#define tft_BaclLightOn()	TFT_BACKLIGHT_SET = TFT_BACKLIGHT_NUMPIN
#define tft_BaclLightOff() 	TFT_BACKLIGHT_CLR = TFT_BACKLIGHT_NUMPIN


// TFT MODELO XYZ SEI LÁ
//horizontal
#define TFT_H_PULSE          	256
#define TFT_H_FRONT_PORCH      	10
#define TFT_H_BACK_PORCH       	10

//vertical
#define TFT_V_PULSE           	45
#define TFT_V_FRONT_PORCH      	10
#define TFT_V_BACK_PORCH       	10

static void tft_Init(void) {
	PCONP |= PCLCD;											// Ligar o TFT no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	// Consultar datasheet página 113
	IOCON_P1_20 = 7; //VD[10] = G[0]
	IOCON_P1_21 = 7; //VD[11] = G[1]
	IOCON_P1_22 = 7; //VD[12] = G[2]
	IOCON_P1_23 = 7; //VD[13] = G[3]
	IOCON_P1_24 = 7; //VD[14] = G[4]
	IOCON_P1_25 = 7; //VD[15] = G[5]
	IOCON_P1_26 = 7; //VD[20] = B[1]
	IOCON_P1_27 = 7; //VD[21] = B[2]
	IOCON_P1_28 = 7; //VD[22] = B[3]
	IOCON_P1_29 = 7; //VD[23] = B[4]
	IOCON_P2_06 = 7; //VD[04] = R[1]
	IOCON_P2_07 = 7; //VD[05] = R[2]
	IOCON_P2_08 = 7; //VD[06] = R[3]
	IOCON_P2_09 = 7; //VD[07] = R[4]
	IOCON_P2_12 = 5; //VD[03] = R[0]
	IOCON_P2_13 = 7; //VD[19] = B[0]

	//controle
	IOCON_P2_02 = 7; //TFT_DCLK
	IOCON_P2_04 = 7; //TFT_ENAB

	// INIT GLCD COTROLLER (DATASHEET PG 288)
	tft_CursorDisable(); // Disable cursor
	LCD_CTRL = TFT_CONFIG;

	// INIT PIXEL CLOCK
	LCD_CFG = TFT_CLKDIV;
	LCD_POL =
			(0<<13) |							// IPC = 0 clk polarity
			((XSIZE_PHYS - 1) << 16) |  		// CPL Clocks per line.
			(1<<26); 							// BCD = 1  bypass internal clk divider

	// INIT HORIZONTAL TIMING
	LCD_TIMH =
		(((XSIZE_PHYS / 16) - 1) << 2) |
		((TFT_H_PULSE - 1) << 8) |
		(TFT_H_FRONT_PORCH << 16) |
		(TFT_H_BACK_PORCH << 24);

	// INIT VERTICAL TIMING
	LCD_TIMV =
		(YSIZE_PHYS - 1) |
		(TFT_V_PULSE << 10) |
		(TFT_V_FRONT_PORCH << 16) |
		(TFT_V_BACK_PORCH << 24);

	LCD_UPBASE = VRAM_ADDR & ~7UL; // Upper Panel Frame Base Address register
	LCD_LPBASE = VRAM_ADDR & ~7UL; // Lower Panel Frame Base Address register

	tft_Enable();
	delay_ms(100);
	tft_PowerON();
	delay_ms(100);

	#if (LCD_USE_DEBUG == pdON)
		//mostra o clock
		plog("TFT: Video clock: %d Hz"CMD_TERMINATOR, F_CPU/(LCD_CFG+1));
		//mostra o frame rate
		plog("TFT: Frame rate: %d fps"CMD_TERMINATOR, (F_CPU/C_GLCD_CLK_PER_FRAME)/(LCD_CFG+1));
	#endif

	TFT_BACKLIGHT_DIR |= TFT_BACKLIGHT_NUMPIN; 					// Colocar o pino de controle do backlight para saida
	tft_BaclLightOn();
}

static void _SetDisplayOrigin(int x, int y) {
  	#if USE_24BPP_MODE
  	#define PIXEL_WIDTH  4
	#else
  	#define PIXEL_WIDTH  2
	#endif

  	(void)x;
  	// Set start address for display data and enable LCD controller
  	LCD_UPBASE = VRAM_ADDR + (y * YSIZE_PHYS * PIXEL_WIDTH);  // Needs to be set, before LCDC is enabled
}

// ###########################################################################################################################
// ###########################################################################################################################
// FUNÇÕES GUI_X DE INCIALIZAÇÃO
// ###########################################################################################################################

// ---------------------------------------------------------------------------------------------------------------------------
// Purpose:  Called during the initialization process in order to set up the available memory for the GUI.
// ---------------------------------------------------------------------------------------------------------------------------
void GUI_X_Config(void) {
  	static U32* aMemory = (u32*)EMWIN_ADDR;
  	// Assign memory to emWin
  	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
 	// Set default font
  	GUI_SetDefaultFont(GUI_FONT_6X8);}

// ---------------------------------------------------------------------------------------------------------------------------
// Purpose: Called during the initialization process in order to set up the  display driver configuration.
// ---------------------------------------------------------------------------------------------------------------------------
void LCD_X_Config(void) {
  	// At first initialize use of multiple buffers on demand
  	#if (NUM_BUFFERS > 1)
	GUI_MULTIBUF_Config(NUM_BUFFERS);
  	#endif

  	// Set display driver and color conversion for 1st layer
  	GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);

  	// Display driver configuration, required for Lin-driver
  	if (LCD_GetSwapXY()) {
    	LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    	LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  	} else {
    	LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    	LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  	}
  	LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);

	// Set user palette data (only required if no fixed palette is used)
  	#if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  	#endif
}


// ---------------------------------------------------------------------------------------------------------------------------
//* Purpose:
//*   This function is called by the display driver for several purposes.
//*   To support the according task the routine needs to be adapted to
//*   the display controller. Please note that the commands marked with
//*   'optional' are not cogently required and should only be adapted if
//*   the display controller supports these features.
//*
//* Parameter:
//*   LayerIndex - Index of layer to be configured
//*   Cmd        - Please refer to the details in the switch statement below
//*   pData      - Pointer to a LCD_X_DATA structure
//*
//* Return Value:
//*   < -1 - Error
//*     -1 - Command not handled
//*      0 - Ok
// ---------------------------------------------------------------------------------------------------------------------------
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  	int r;
  	LCD_X_SETORG_INFO* pSetOrg;

	(void) LayerIndex;

  	switch (Cmd) {
  	case LCD_X_INITCONTROLLER: {
		// Called during the initialization process in order to set up the
		// display controller and put it into operation. If the display
		// controller is not initialized by any external routine this needs
		// to be adapted by the customer...

		tft_Init();

		#if (GUI_SUPPORT_TOUCH > 0)
		//#define TOUCH_BOARD_SWAPXY 1 // para TFT 3.2inch-RPi-LCD-B-Schematic SwapXY = 1
		#define TOUCH_BOARD_SWAPXY 0 // para TFT 7inch


		tsc2046_Init();
		tsc2046_SwapXY(0); // para TFT 7"
		tsc2046_LcdSize(XSIZE_PHYS, YSIZE_PHYS);
		tsc2046_SetArea(0, 0, YSIZE_PHYS, XSIZE_PHYS);

	    GUI_TOUCH_SetOrientation(0);
	    GUI_TOUCH_Calibrate(GUI_COORD_X, 15, 785, 3801, 250); // para TFT 7inch SwapXY = 0
	    GUI_TOUCH_Calibrate(GUI_COORD_Y, 15, 465, 3725, 215); // para TFT 7inch SwapXY = 0

		#endif //#if (GUI_SUPPORT_TOUCH > 0)
		return 0;
  	}
	case LCD_X_SETORG: {
		// Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
		pSetOrg = (LCD_X_SETORG_INFO *)pData;
		_SetDisplayOrigin(pSetOrg->xPos, pSetOrg->yPos);
		return 0;
	}
  	default: r = -1; break;
  	}
  	return r;
}

// ###########################################################################################################################
// ###########################################################################################################################
// FUNÇÕES GUI_X
// ###########################################################################################################################
// ---------------------------------------------------------------------------------------------------------------------------
//* Note:
//*     GUI_X_Init() is called from GUI_Init is a possibility to init
//*     some hardware which needs to be up and running before the GUI.
//*     If not required, leave this routine blank.
// ---------------------------------------------------------------------------------------------------------------------------
static volatile int OS_TimeMS;

void GUI_X_Init(void) {
	OS_TimeMS = 0;
}

int GUI_X_GetTime(void) {
  	return OS_TimeMS;
}

void GUI_X_Delay(int ms) {
  	int tEnd = OS_TimeMS + ms;
  	while ((tEnd - OS_TimeMS) > 0);
}

extern void GUI_X_SysTick_Handler(void);
void GUI_X_SysTick_Handler(void) {
  	OS_TimeMS++;                                // Increment counter
}

U32 GUI_X_GetTaskId(void) { return 0; }
void GUI_X_InitOS(void)    { }
void GUI_X_Unlock(void)    { }
void GUI_X_Lock(void)      { }
//void GUI_X_ExecIdle(void) { OS_Delay(1); }


// ###########################################################################################################################
// ###########################################################################################################################
// FUNÇÕES GUI_X LOGS
// ###########################################################################################################################

//Note:
//  Logging is used in higher debug levels only. The typical target
//  build does not use logging and does therefor not require any of
//  the logging routines below. For a release build without logging
//  the routines below may be eliminated to save some space.
//  (If the linker is not function aware and eliminates unreferenced
//  functions automatically)

#include "stdio_uc.h"
void GUI_X_Log     (const char *s) { plognp(s); }
void GUI_X_Warn    (const char *s) { plognp(s); }
void GUI_X_ErrorOut(const char *s) { plognp(s); }

//void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
//void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
//void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }


// ###########################################################################################################################
// ###########################################################################################################################
// FUNÇÕES GUI_X PARA TOUCH
// ###########################################################################################################################
#if (GUI_SUPPORT_TOUCH > 0)
static volatile s16 _TouchX, _TouchY;
extern void GUI_X_TouchTick(void);

// ---------------------------------------------------------------------------------------------------------------------------
// Esta função DEVE ser chamada pelo programa princioal, de preferncia a cada 1ms.
// 	ERRO: A posição do cursor não se atualiza, somente se utilizar gerenciador de janelas do emWin
// ---------------------------------------------------------------------------------------------------------------------------
void GUI_X_TouchTick(void) {
	tsc2046_Tick();

	// processar GUI_TOUCH_Exec a cada 100ms
	static int ct = 0;
	ct++;
	if (ct%10) return;

	s16 x, y;
	tsc2046_GetTouchCoord(&x, &y);
	_TouchX = x;
	_TouchY = y;
	GUI_TOUCH_Exec();
	//GUI_TOUCH_Exec();
}

// ---------------------------------------------------------------------------------------------------------------------------
//* Function decription:
//*   Called from GUI, if touch support is enabled.
//*   Switches on voltage on X-axis,
//*   prepares measurement for Y-axis.
//*   Voltage on Y-axis is switched off.
// ---------------------------------------------------------------------------------------------------------------------------
void GUI_TOUCH_X_ActivateX(void) {
}

// ---------------------------------------------------------------------------------------------------------------------------
//* Function decription:
//*   Called from GUI, if touch support is enabled.
//*   Switches on voltage on Y-axis,
//*   prepares measurement for X-axis.
//*   Voltage on X-axis is switched off.
// ---------------------------------------------------------------------------------------------------------------------------
void GUI_TOUCH_X_ActivateY(void) {
}

// ---------------------------------------------------------------------------------------------------------------------------
//* Function decription:
//*   Called from GUI, if touch support is enabled.
//*   Measures voltage of X-axis.
// ---------------------------------------------------------------------------------------------------------------------------
int GUI_TOUCH_X_MeasureX(void) {
  return _TouchX;
}

// ---------------------------------------------------------------------------------------------------------------------------
//* Function decription:
//*   Called from GUI, if touch support is enabled.
//*   Measures voltage of Y-axis.
// ---------------------------------------------------------------------------------------------------------------------------
int GUI_TOUCH_X_MeasureY(void) {
  return _TouchY;
}

#endif
