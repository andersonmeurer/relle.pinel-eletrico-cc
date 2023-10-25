#include <math.h>
#include "lcd_t6963.h"
#include "lcd_fonts_5x4.h"
//#include "lcd_fonts_5x6.h" // Não está funcionando

#if (LCD_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

/*
	Primeiro desenhamos na memmória principal da CPU
	Uma vez atualizada setamos a página de memória do display e fazemos a transferencia para lá
*/

static u8 screen[LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE];		 					// Memória espelho para atualizar o LCD
static u8 *addr_screenXY(u8 x, u8 y, u8 mode);								// Apontador na memória espelho
static u8 *addr_screenText 		= (u8 *) &screen;							// Endereço inicio da memória de texto
static u8 *addr_screenGaphic	= (u8 *) &screen + LCD_TEXT_SIZE;			// Endereço inicio da memória grafica

static u8 last_page  = 1; 													// Guarda a útima página atualizada no LCD

void lcd_WriteData(u8 Data);
void lcd_WriteCommand(u8 Data);
u8 lcd_ReadSta(void);
void lcd_SetAddress(u16 Addr);
void lcd_WriteDataAuto(u8 Data);
void lcd_Clear(u8 data, u8 mode);
u16 lcd_Pow(u16 Base, u16 Exp);
u8 lcd_Format(u8 x,u8 Seq);
u8 lcd_Bit(u8 Data);

// ###################################################################################################################
// ###################################################################################################################
// ROTINAS DE CONTROLE
// ###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------------
// Inicializa display  e portas do microcontrolador referentes ao display
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Init(void)	{
	u16 p;
	
	#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
		lcd_config_ports();	
	#endif

   	last_page  = 1;
   	lcd_SetPage(1);
   	
    //Ajusto area da linha horizontal grafica multipla de 8 (8 bits)
    lcd_WriteData(LCD_GRAPHIC_AREA);
    lcd_WriteData(0x00);
    lcd_WriteCommand(LCD_CMD_GRAPHIC_AREA); 

     //Ajusto area da linha horizontal texto por char 
    lcd_WriteData(LCD_TEXT_AREA);
    lcd_WriteData(0x00);
    lcd_WriteCommand(LCD_CMD_TEXT_AREA);
       
    // Carrega fonte para a RAM CG do display
	lcd_SetAddress(LCD_CG_HOME);
	lcd_WriteCommand(LCD_CMD_WRITE_DATA_AUTO);
	
	p = 0;	
	while (p < sizeof(lcd_fonts))	{	
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
		lcd_WriteDataAuto(lcd_fonts[p++]);
             
        lcd_WriteDataAuto(0x00);  // Oitava linha do char de 5x dever ser vazia    
    }
    
  	lcd_WriteCommand(LCD_CMD_RESET_AUTO);    
    
    //Aponto para o endereco de RAM do display para os caracteres lidos
    lcd_WriteData(LCD_CG_OFFSET);
    lcd_WriteData(0x00);
    lcd_WriteCommand(LCD_CMD_SET_CG_OFFSET);
   	
    // Exibir texto da RAM com operacao XOR no grafico
    lcd_WriteCommand(LCD_CMD_RAM_XOR);
        
   	#if (LCD_USE_DEBUG == pdON)
	plog("LCD: addr screen 0x%lx"CMD_TERMINATOR, screen);
	plog("LCD: addr_screenText 0x%x"CMD_TERMINATOR, addr_screenText);
	plog("LCD: addr_screenGaphic 0x%x"CMD_TERMINATOR, addr_screenGaphic);
	plog("LCD: Carregando char. tabelas %u chars"CMD_TERMINATOR, sizeof(lcd_fonts));
	#endif
}


// -----------------------------------------------------------------------------------------------------------------------
// RETORNA O ENDEREÇO DA SCREEN APONTADO PELA COLUNA E LINHA DESEJADA
// -----------------------------------------------------------------------------------------------------------------------
u8 *addr_screenXY(u8 x, u8 y, u8 mode)	{	
	if (mode == LCD_MODE_GRAPHIC)	return (u8*) ((y * LCD_GRAPHIC_AREA) + x/LCD_PIXEL_BY_BYTE + addr_screenGaphic);
	else 							return (u8*) ((y * LCD_TEXT_AREA) + x + addr_screenText);
}

// -----------------------------------------------------------------------------------------------------------------------
// Liga e desliga o display
// LCD_CMD_MODE_OFF / LCD_CMD_MODE_GRAPHIC / LCD_CMD_MODE_TEXT / LCD_CMD_MODE_TEXT_GRAPHIC / LCD_CMD_MODE_CURSOR / LCD_CMD_MODE_BLINK / 
// -----------------------------------------------------------------------------------------------------------------------
void lcd_SetDisplay(u8 mode)	{	
	lcd_WriteCommand(mode);
}
 
// -----------------------------------------------------------------------------------------------------------------------
// Ajusto o ponteiro da RAM do display para a posição desejada
// -----------------------------------------------------------------------------------------------------------------------
void lcd_SetAddress(u16 Addr)		{
	// Seto o ponterio da RAM do diplay para inicio da caixa a ser desenhada
    lcd_WriteData(Addr & 0xFF);
    lcd_WriteData(Addr >> 8);
    lcd_WriteCommand(LCD_CMD_SET_POINT);
}
   

// ###################################################################################################################
// ROTINAS ATUALIZAÇÃO
// ###################################################################################################################
// ###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------------
// Atuliza a RAM do display com a RAM principal
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Refresh(u8 Page)	{
	u16	AddrDisp;
	u8	*p = (u8 *) addr_screenText;
		
	if ( (Page > LCD_PAGE_MAX) || (Page == 0) )	return;
  	
	AddrDisp = LCD_ADDR_TEXT_HOME + (Page - 1) * (LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE);
	lcd_SetAddress(AddrDisp);
	lcd_WriteCommand(LCD_CMD_WRITE_DATA_AUTO);
	
    while (AddrDisp <= (u16)(LCD_ADDR_TEXT_HOME + (Page - 1) * (LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE)) + LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE)	   {
      	lcd_WriteDataAuto(*p++); 
        AddrDisp++;
    }
    
    lcd_WriteCommand(LCD_CMD_RESET_AUTO);
}

// -----------------------------------------------------------------------------------------------------------------------
// Atuliza a RAM do display com a RAM principal de forma automatica
// Quando  a utilma pagina foi 1 então atualiza pagina 2 senão atualiza pagina 1
// PARA QUE O REFRESH AUTO FUNCIONE, O MESMO DEVE SER CHAMADO ANTES DO SetPage()
// -----------------------------------------------------------------------------------------------------------------------
void lcd_RefreshAuto(void)	{
	if (last_page == 1)	{
		lcd_Refresh(2);
		lcd_SetPage(2);
	}	else 	{
		lcd_Refresh(1);
		lcd_SetPage(1);
	}	
}

// -----------------------------------------------------------------------------------------------------------------------
// Inicializa display  e portas do microcontrolador referentes ao display
// -----------------------------------------------------------------------------------------------------------------------
void lcd_SetPage(u8 Page)	{
	
	u16	Addr;
	
	last_page = Page; // Para guarda a ultima pagina no momento do refresh
	
	if ( (Page > LCD_PAGE_MAX) || (Page == 0) )	return;
  	
    // Ajusto inicio da RAM para texto
    Addr = LCD_ADDR_TEXT_HOME + (Page - 1) * (LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE);
    
    lcd_WriteData(Addr & 0xFF);
    lcd_WriteData(Addr >> 8);
    lcd_WriteCommand(LCD_CMD_TEXT_HOME_ADDR); 
    
    // Ajusto inicio da RAM para grafico
    Addr = LCD_ADDR_GRAPHIC_HOME + (Page - 1) * (LCD_TEXT_SIZE + LCD_GRAPHIC_SIZE);
    lcd_WriteData(Addr & 0xFF);
    lcd_WriteData(Addr >> 8);
    lcd_WriteCommand(LCD_CMD_GRAPHIC_HOME_ADDR);    
}



// ###################################################################################################################
// DRIVERS
// ###################################################################################################################
// ###################################################################################################################

void lcd_WriteData(u8 Data)			{  
    while ( (lcd_ReadSta() & (LCD_STATUS0 | LCD_STATUS1)) != (LCD_STATUS0 | LCD_STATUS1));
    
    lcd_write_data(Data); //*pLcdG_Data = Data;
}

void lcd_WriteDataAuto(u8 Data)		{
    while ( (lcd_ReadSta() & LCD_STATUS3) != LCD_STATUS3);
  
    lcd_write_data(Data); //*pLcdG_Data = Data;
}


// -----------------------------------------------------------------------------------------------------------------------
// Escreve comandos no display
// -----------------------------------------------------------------------------------------------------------------------
void lcd_WriteCommand(u8 Data)	{
    while ( (lcd_ReadSta() & (LCD_STATUS0 | LCD_STATUS1)) != (LCD_STATUS0 | LCD_STATUS1));

    lcd_write_command(Data);
}

// -----------------------------------------------------------------------------------------------------------------------
// Lê status do display
// -----------------------------------------------------------------------------------------------------------------------
u8 lcd_ReadSta(void)	{
    return lcd_read_command();
}




// ###################################################################################################################
// ###################################################################################################################
// ROTINAS PARA PLOTAGEM
// ###################################################################################################################
// -----------------------------------------------------------------------------------------------------------------------
// Devolve a base elevada a EXP
// -----------------------------------------------------------------------------------------------------------------------
u16 lcd_Pow(u16 Base, u16 Exp)	{
	u16
		Aux,
		t;
		
	if (Exp == 0) return 1;
	
	Aux = Base;
	for (t = 1; t < Exp; t++)
		Aux *= Base;
	
	return Aux;
}

// -----------------------------------------------------------------------------------------------------------------------
// Formata a sequencia de bits 1 de acordo com a coluna e sequencia Inc. EX para 6 pixels por byte
// Col = 5 Inc = 1 retorna 0000 0001b
// Col = 6 Inc = 1 retorna 0010 0000b
// Col = 6 Inc = 4 retorna 0011 1100b
// -----------------------------------------------------------------------------------------------------------------------
u8 lcd_Format(u8 x,u8 Seq)	{
	u8
		Data,
		t;
		
	Data = 0;
	for(t = x; t < x + Seq; t++)
		Data += (u8)lcd_Pow(2,lcd_Bit(t));
			
	return Data;
}

// -----------------------------------------------------------------------------------------------------------------------
// Retorna a potencia do bit da determinada coluna. EX;
//	bit			Colunas
//	5			0, 6,  12 .....
//	4			1, 7,  13 .....	
//	3			2, 8,  14 .....
//	2			3, 9,  15 .....
//  1			4, 10, 16 .....
//	0			5, 11, 17 .....
// -----------------------------------------------------------------------------------------------------------------------
u8 lcd_Bit(u8 Data)	{
	return (LCD_PIXEL_BY_BYTE - 1) - (Data % LCD_PIXEL_BY_BYTE);
}

// -----------------------------------------------------------------------------------------------------------------------
// limpa áreas da RAM do display referente ao grafico e texto
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Clear(u8 data, u8 mode)	{
	u8	*addr;
	u8	*addrLimit;
		        
    if (mode == LCD_MODE_TEXT)	{
    	addr = (u8 *)addr_screenText;
    	addrLimit = (u8 *) addr_screenText + LCD_TEXT_SIZE;	
	}	else	{
		addr = (u8 *) addr_screenGaphic;
		addrLimit = (u8 *) addr_screenGaphic + LCD_GRAPHIC_SIZE;	
	}
	    	
    while (addr < addrLimit) *addr++ = data;
}

// -----------------------------------------------------------------------------------------------------------------------
// Limpa a área de RAM do display referente ao grafico
// -----------------------------------------------------------------------------------------------------------------------
void lcd_ClearGraphic(u8 data)		{
	lcd_Clear(data, LCD_MODE_GRAPHIC);
}

// -----------------------------------------------------------------------------------------------------------------------
// Limpa a área de RAM do display referente ao texto
// -----------------------------------------------------------------------------------------------------------------------
void lcd_ClearText(u8 data)		{
	lcd_Clear(data, LCD_MODE_TEXT);
}

// -----------------------------------------------------------------------------------------------------------------------
// Desenha um pixel no display
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Pixel(u8 x, u8 y, u8 mode)	{
 	u8 DataAux;
 	u8 Data;

 	if ( (x >= LCD_WIDTH) || (y >= LCD_HEIGHT) ) return;

	Data = *addr_screenXY(x, y, LCD_MODE_GRAPHIC);
	DataAux = lcd_Format(x,1);
	
	if (mode == LCD_ON)
		*addr_screenXY(x, y, LCD_MODE_GRAPHIC) = Data | DataAux;
	else
		*addr_screenXY(x, y, LCD_MODE_GRAPHIC) = Data & ~DataAux;
}


// -----------------------------------------------------------------------------------------------------------------------
// Cria uma linha da posicao xx1, yy1 até xx2, yy2
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Line(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode)	{
 	n16
 		x,y,
 		count,
 		xs,ys,
 		xm,ym;

 	x  = (n16)xx1; 
 	y  = (n16)yy1;
 	xs = (n16)xx2 - (n16)xx1; 
 	ys = (n16)yy2 - (n16)yy1;
 
 	if (xs < 0)  	xm=-1; 
 	else 
 		if(xs > 0) 	xm = 1; 
 		else  		xm = 0;
 
 	if (ys < 0) 	ym = -1;
 	else 
 		if(ys > 0) 	ym = 1; 
 		else 		ym=0;

 	if	(xs < 0) xs = -xs;
 	if	(ys < 0) ys = -ys;

 	lcd_Pixel((u8)x,(u8)y, mode);

 	//<45º
 	if(xs > ys)  {
   		count = -(xs / 2);

   		while(x != xx2 )    {
    		count = count + ys;
     		x = x + xm;

     		if(count>0)	{
       			y = y + ym;
       			count = count - xs;
      		}

     		lcd_Pixel((u8)x,(u8)y, mode);
   		}

	}  else   {
		 // >=45º
    	count=-(ys/2);

    	while(y != yy2)	 	{
      		count = count + xs;
      		y = y + ym;

      		if(count>0)	{
        		x=x+xm;
        		count=count-ys;
       		}

      		lcd_Pixel((u8)x,(u8)y, mode);
    	}
   	}
}

// -----------------------------------------------------------------------------------------------------------------------
// Cria uma linha vertical da coluna xx1 da linha yy1 até linha yy2
// -----------------------------------------------------------------------------------------------------------------------
void lcd_LineV(u8 xx1, u8 yy1, u8 yy2, u8 mode)	{
	u8
		Data,
		y;
		
	if ( (xx1 >= LCD_WIDTH) || (yy1 >= LCD_HEIGHT) || (yy2 >= LCD_HEIGHT) || (yy1 > yy2) ) 
		return;
		
	for (y = yy1; y <= yy2; y++)	{
		Data = *addr_screenXY(xx1, y, LCD_MODE_GRAPHIC);
    	
    	if (mode == LCD_ON)	Data = Data | lcd_Format(xx1, 1);
    	else					Data = Data & ~lcd_Format(xx1, 1);
    	
    	*addr_screenXY(xx1, y, LCD_MODE_GRAPHIC) = Data;
    }
}

// -----------------------------------------------------------------------------------------------------------------------
// Cria uma linha horizontal da coluna xx1 da linha yy1 até coluna xx2
// -----------------------------------------------------------------------------------------------------------------------
void lcd_LineH(u8 xx1, u8 yy1, u8 xx2, u8 mode)	{
	u8
		DataAux,
		Seq,
		Data,
		x;
	
	if ( (xx1 >= LCD_WIDTH) || (xx2 >= LCD_WIDTH) || (yy1 >= LCD_HEIGHT) || (xx1 > xx2) ) 
		return;
		
    Seq = LCD_PIXEL_BY_BYTE;
    	
    for (x = xx1; x <= xx2; x += Seq)	{
    	if ( (lcd_Bit(x) == (LCD_PIXEL_BY_BYTE - 1)) && (x + (LCD_PIXEL_BY_BYTE - 1) <= xx2))	{
    		Seq = LCD_PIXEL_BY_BYTE;
    			
    		if (mode == LCD_ON)	    DataAux = 0xFF;
    		else 						DataAux = 0x00;

    	} else	{
			
    		if (x + lcd_Bit(x) <= xx2)	 		Seq = lcd_Bit(x) + 1;
    		else						    	Seq = lcd_Bit(x) - lcd_Bit(xx2) + 1;
				
			Data = lcd_Format(x, Seq);
				
    		DataAux = *addr_screenXY(x, yy1, LCD_MODE_GRAPHIC);
    			
    		if (mode == LCD_ON)	DataAux = DataAux | Data; 		//Para pixel ligado
    		else	    			DataAux = DataAux & (~Data);	//para pixel desligado   
    	}
    		
    	*addr_screenXY(x, yy1, LCD_MODE_GRAPHIC) = DataAux;
    }
}
	
// -----------------------------------------------------------------------------------------------------------------------
// Cria uma caixa vazia
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Box(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode)	{
	lcd_LineV(xx1,yy1,yy2,mode);
	lcd_LineV(xx2,yy1,yy2,mode);
	lcd_LineH(xx1,yy1,xx2,mode);
	lcd_LineH(xx1,yy2,xx2,mode);	
}

// -----------------------------------------------------------------------------------------------------------------------
// Cria uma caixa cheia
// -----------------------------------------------------------------------------------------------------------------------
void lcd_BoxFill(u8 xx1, u8 yy1, u8 xx2, u8 yy2, u8 mode)	{
	u8
		y;
		
	if ( (yy1 >= LCD_HEIGHT) || (yy2 >= LCD_HEIGHT) || (yy1 > yy2) ) 
		return;
		
	for (y = yy1; y <= yy2; y++)
		lcd_LineH(xx1, y, xx2, mode);
}



#if (LCD_USE_CIRCLE == pdON)
// -----------------------------------------------------------------------------------------------------------------------
// Cria uma circulo no cetro xx1, yy1  de raio r mosntado em passos step
// -----------------------------------------------------------------------------------------------------------------------
void lcd_Circle(u8 xx1,  u8 yy1, u8 r, u8 Step, u8 mode)	{
 	u8 
 		i;
 	
 	n16 
 		x,y;

 if (Step == 0) 
 	Step = 1;
 	
 for(i = 0; i <= 90; i += Step)		{
   	x = (n16) ((double)r * cos((double)i * lcd_GradToRad));
   	y = (n16) ((double)r * sin((double)i * lcd_GradToRad));
   	lcd_Pixel((u8)(xx1+x),(u8)(yy1+y),mode);
   	lcd_Pixel((u8)(xx1-x),(u8)(yy1+y),mode);
   	lcd_Pixel((u8)(xx1+x),(u8)(yy1-y),mode);
   	lcd_Pixel((u8)(xx1-x),(u8)(yy1-y),mode);
  }
}
#endif


// -----------------------------------------------------------------------------------------------------------------------
// Escreve na area de texto na RAM principal
// -----------------------------------------------------------------------------------------------------------------------
void lcd_WriteText(u8 x, u8 y, const char *data)	{
	u8 ch;

   	#if (LCD_USE_DEBUG == pdON)
	u8 count = 0;
	plog("LCD: Imprimir %s"CMD_TERMINATOR, data);
	#endif

	while (*data) {
		ch = *data++;

		if ( (ch >= 'a') && (ch <= 'z') )    	ch -= 64;
    	// LETRAS MAISCULAS COM ACENTOS
    	else if ( (ch >= 192) && (ch <= 221) ) 	ch -= 128;
    	// LETRAS MINUSCULAS COM ACENTOS
    	else if ( (ch >= 224) && (ch <= 253) ) 	ch -= 160;
    	else if (ch == '{')    					ch = 7;
    	else if (ch == '}')			    		ch = 0x3E;
    	else						    		ch -= 32;
    	
		*addr_screenXY(x++, y, LCD_MODE_TEXT) = ch;		
		
		#if (LCD_USE_DEBUG == pdON)
		count++;
		#endif
	}
	
	#if (LCD_USE_DEBUG == pdON)
	plog("LCD: Imprimiu %u chars"CMD_TERMINATOR, count);
	#endif

}
