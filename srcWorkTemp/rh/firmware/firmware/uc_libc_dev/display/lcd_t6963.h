#ifndef __LCD_T6963_H
#define __LCD_T6963_H

#include "lcd_t6963_drv.h"

#define LCD_MODE_GRAPHIC 				0x00	// Indicador para executar ações para modo grafico
#define LCD_MODE_TEXT  					0x01	// Indicador para executar ações para modo texto
#define LCD_ON 							0x01	
#define LCD_OFF							0x00	

// para FS  = 1 = 6 pixels
// Size(T) =  BytesPorLinha * LCDHeight/6 
// Size(G) =  BytesPorLinha * LCDHeight 
// AddrEnd = (AddrHome + Size(x) ) - 1
// Size(x) = (AddrEnd - AddIni) + 1

#define LCD_PIXEL_BY_BYTE 				6
#define LCD_WIDTH 						128		// Largura, em dots, do display
#define LCD_HEIGHT						64		// Altura, em dots, do display
#define LCD_WIDTH_MAX 					LCD_WIDTH + 4
#define LCD_ADDR_TEXT_HOME   	  		0x0000  // Endereço de inicio na RAM do display para o texto
#define LCD_TEXT_AREA          			0x0016  // 128/6 = 21.33 = 22 = 16h - Bytes por linha - Cada byte corresponde a 6 pixels (FS = 1)
#define LCD_TEXT_SIZE         			0x00EB  // BytesPorLinha * LCDHeight/6 = 22*64/6 = 235 = EBh
#define LCD_ADDR_GRAPHIC_HOME  			0x00EB  // Endereço de inicio na RAM do display para o grafico
#define LCD_GRAPHIC_AREA  			   	0x0016  // 128/6 = 21.33 redondar 22 = 16h - Bytes por linha - Cada byte corresponde a 6 pixels (FS = 1)
#define LCD_GRAPHIC_SIZE  			   	0x0580  // BytesPorLinha * LCDHeight = 22*64 = 1408 = 580h
#define LCD_PAGE_MAX 					3		// numero de páginas na RAM do display- Usar 3 com CG ou 4 paginas sem CG

#define LCD_CG_HOME 					0x1800	// Endereco do gerador de caractere na RAM do display
#define LCD_CG_SIZE 					0x0800	// Endereco do gerador de caractere na RAM do display
#define LCD_CG_OFFSET  					0x03	// Offset do gerador de caractere na RAM do display, este para que o display possa trabalhar


// 						RAM 8K
//  		-----------------------------	--
//  0x0000  | Area para texto           |	|
//          | ponteiro para 128 pos de  |	|
//  0x00EA  | caractere na ROM ou RAM   |	|
//  		-----------------------------	|	Página 1	
//  0x00EB  | Area para Grafico 	 	|	|
//          | 		             		|	|
//  0x066A  |                           |	|
//  	    -----------------------------	--
//  0x066B  | Area para texto           |	|	
//          | ponteiro para 128 pos de  |	|
//  0x0754  | caractere na ROM ou RAM   |	|
//  		-----------------------------	|	Página 2
//  0x0755  | Area para Grafico 	 	|	|
//          | 		             		|	|
//  0x0CD4  |                           |	|
//   		-----------------------------	--
//  0xCD54  | Uso geral  		        |
//          | 		             		|
//          | 		             		|
//  0x17FF	|         			        |
//  	    -----------------------------
//  0x1800  | Formatos de Caracteres    |
//  	    |         		            |
//  0x1FFF  -----------------------------

#define LCD_CMD_SET_CURSOR   	          			0x21
#define LCD_CMD_SET_CG_OFFSET   		    		0x22
#define LCD_CMD_SET_POINT             				0x24

#define LCD_CMD_TEXT_HOME_ADDR         				0x40
#define LCD_CMD_TEXT_AREA                			0x41
#define LCD_CMD_GRAPHIC_HOME_ADDR    				0x42
#define LCD_CMD_GRAPHIC_AREA                		0x43

#define LCD_CMD_ROM_OR   		            	 	0x80
#define LCD_CMD_ROM_XOR   	            			0x81
#define LCD_CMD_ROM_AND   							0x83
#define LCD_CMD_ROM_TEXT 	  			  	    	0x84
#define LCD_CMD_RAM_OR    							0x88
#define LCD_CMD_RAM_XOR   							0x89
#define LCD_CMD_RAM_AND   		 					0x8B
#define LCD_CMD_RAM_TEXT    						0x8C

#define LCD_CMD_MODE_OFF       			   			0x90
#define LCD_CMD_MODE_GRAPHIC   		    			0x98
#define LCD_CMD_MODE_TEXT          					0x94
#define LCD_CMD_MODE_TEXT_GRAPHIC					0x94|0x98
#define LCD_CMD_MODE_CURSOR   		    			0x92
#define LCD_CMD_MODE_BLINK      					0x91


#define LCD_CMD_CURSOR_1LINE   						0xA0
#define LCD_CMD_CURSOR_8LINE   						0xA7

#define LCD_CMD_WRITE_DATA_AUTO    					0xB0
#define LCD_CMD_READ_DATA_AUTO   		        	0xB1
#define LCD_CMD_RESET_AUTO   						0xB2

#define LCD_CMD_WRITE_DATA_INC    		   			0xC0
#define LCD_CMD_READ_DATA_INC   					0xC1
#define LCD_CMD_WRITE_DATA_NOINC    	   			0xC4
#define LCD_CMD_READ_DATA_NOINC   					0xC5

#define LCD_STATUS0 								0x01	// 1 = Pronto   0 = Comando sendo executado
#define LCD_STATUS1									0x02	// 1 = Pronto   0 = Não capaz de ler e escreve 
#define LCD_STATUS2									0x04	// 1 = Pronto 	0 = Não pronto para leitura automatica
#define LCD_STATUS3									0x08	// 1 = Pronto 	0 = Não pronto para escrita automatica
#define LCD_STATUS5									0x20	// Operacao de controle -> 0 = Não Ok 1 = Ok
#define LCD_STATUS6									0x40	// Erro no comando de peek ou copy -> 0 = Sem erro  1 = erro
#define LCD_STATUS7									0x80	// Condição de blink -> 0 = Desligado 1 = normal

void lcd_Init(void);
void lcd_ClearText(u8 data);
void lcd_ClearGraphic(u8 data);
void lcd_WriteText(u8 x, u8 y, const char *data);
void lcd_BoxFill(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode);
void lcd_Pixel(u8 x, u8 y, u8 Mode);
void lcd_LineH(u8 xx1, u8 yy1, u8 length, u8 mode);
void lcd_LineV(u8 xx1, u8 yy1, u8 yy2, u8 mode);
void lcd_Box(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode);
void lcd_Refresh(u8 page);
void lcd_RefreshAuto(void);
void lcd_SetPage(u8 page);
void lcd_SetDisplay(u8 mode);
void lcd_Line(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode);

#if (LCD_USE_CIRCLE ==pdON)
	#define lcd_GradToRad (double)(M_PI / 180.0)
	void lcd_Circle(u8 xx1,  u8 yy1, u8 r, u8 Step, u8 mode);
#endif

#endif
