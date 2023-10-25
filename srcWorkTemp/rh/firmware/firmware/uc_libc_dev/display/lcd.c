#include "lcd.h"

// HABILITANDO LCD E = 1
// ACESSANDO COMANDO  RS = 0
// ACESSANDO DATA     RS = 1
// GRAVANDO NO LCD WR = 0
// LENDO NO LCD WR = 1


// TESTADO PARA OS LCDs 16x1 16x2 16x4 20x2 e 20x4
// D7  D6  D5  D4  D3  D2  D1  D0
// 0   0   0   0   0   0   0   1 		Limpa o display e zera o contador de endereços da DD RAM
// 0   0   0   0   0   0   1   X 		Zera o contador de endereços da DD RAM, faz com que o display reinicie o deslocamento da posição original. Não altera o conteúdo da DD RAM
// 0   0   0   0   0   1   ID  SF 		Define a direção de movimento do cursor e especifica a direção de deslocamento dos caracteres no display 
//											ID: 0 – decrementa, 1 – incrementa contador de endereços 
//											SF: 0 – não acompanha, 1– acompanha o delocamento do display
// 0   0   0   0   1   DA  CA  BL 		Definições diversas 
//											DA: 0 – deliga, 1 – liga display 
//											CA: BL
//											0	0	Desliga cursor
//											0	1	Cursor OVR (Cursor cheio)
//											1	0	Cursor INS (Cursor underline)
//											1	1	Cursor piscando ( alternando entre OVR e INS)
// 0   0   0   1   SC  RL  X   X 		Movimento de caracteres e cursor no display sem alterar o conteúdo da DD RAM
//											SC: 0 – move o cursor, 1 – desloca os caracteres
//											RL: 0 – desloca para a esquerda, 1 – desloca para a direita
// 0   0   1   DL  NL  FC  X   X 		Define o comprimento de dados da interface
//											DL: 0 – 4-bits, 1 – 8-bits
//											NL: 0 – 1 linha, 1 – 2 linhas
//											FC: 0 – matriz 5x7, 1 – matriz 5x10
// 0   1   A5  A4  A3  A2  A1  A0 		Armazena o endereço especificado no contador de endereços da CG RAM. Os dados são escritos ou lidos da CG RAM após este comando.
// 1   A6  A5  A4  A3  A2  A1  A0 		Armazena o endereço especificado no contador de endereços da DD RAM Os dados são escritos ou lidos da DD RAM após este comando.

// Endereço para exibição dos dados
// Linha	LCD	16 colunas	LCD	20 colunas
// 1 		0x00 - 0x0F		0x00 - 0x13
// 2		0x40 - 0x4F		0x40 - 0x53
// 3 		0x14 - 0x23		0x14 - 0x27
// 4        0x54 - 0x63		0x54 - 0x67
// OBS: O endereço é incrementado automaticamente após escrever o dados no LCD


// Tempos
// Esperar 20ms após ligar display antes de começar enviar os comandos
// Comandos 0x01 e 0x02 levam para ser processado 1.64ms os demais 40us

void lcd_busy(void);

//-----------------------------------------------------------------------------------------------------------------------
// INCIALIZA O DISPLAY
//-----------------------------------------------------------------------------------------------------------------------
void lcd_init(void)	{
	lcd_config_ports();
	delay_ms(20);
	
	//lcd_command(0x2C);						// Config display working, 4Bit interface, 2lines, 5x10 dot format
	lcd_command(0x3C);							// Config display working, 8Bit interface, 2lines, 5x10 dot format
	
	lcd_set_addr_data(0);
	lcd_set_cursor(LCD_CURSOR_BLINK);
	lcd_command(LCD_CLEAR);
	lcd_move_cursor(LCD_CURSOR_HOME);
}

// -------------------------------------------------------------------------------------------------------------------
// DESCRIÇÃO: 	AJUSTA O TIPO DE CURSOR
// Parametros:	( 	
//					LCD_CURSOR_OFF  - NÃO MOSTRA O CURSOR /
//					LCD_CURSOR_OVR  - CURSOR PARA SOBRESCREVER - CURSO CHEIO /
//					LCD_CURSOR_INS  - CURSOR PARA INSERÇÃO - CURSO LINHA
//				) |
//				LCD_CURSOR_BLINK - CURSOR PISCANDO
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void lcd_set_cursor(u8 set) {
	lcd_command(0xC|set);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Move o cursor
// Parametros:	LCD_CURSOR_HOME / LCD_CURSOR_LEFT / LCD_CURSOR_RIGHT
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void lcd_move_cursor(u8 move) {	
	switch (move) {
		case LCD_CURSOR_HOME: 		lcd_command(0x03); delay_ms(LCD_DELAY_HOME); break; // Coloca cursor em HOME
		case LCD_CURSOR_LEFT: 		lcd_command(0x11); break; // Move cursor para esquerda
		case LCD_CURSOR_RIGHT: 		lcd_command(0x14); break; // Move cursor para direita
	}		
}

//-----------------------------------------------------------------------------------------------------------------------
// MANDA COMANDOS PARA O DISPLAY
//-----------------------------------------------------------------------------------------------------------------------
void lcd_command(u8 command) {
	lcd_busy();
	lcd_write_command(command);
}

//-----------------------------------------------------------------------------------------------------------------------
// LIMPA A TELA DO DISPLAY
//-----------------------------------------------------------------------------------------------------------------------
void lcd_clear(void) {
	lcd_command(LCD_CLEAR);
	delay_ms(LCD_DELAY_HOME);
}

//-----------------------------------------------------------------------------------------------------------------------
// LIMPA A LINHA DESEJADA Y=(0..Y)
//-----------------------------------------------------------------------------------------------------------------------
void lcd_clear_line(u8 y, const u8 addr_end, const u8 addr_offset) {
	u8 yy;
	
	lcd_goto_xy(0, y, addr_offset);
	
	//for (yy = y; yy <= y + 0x0F; yy++) { // Para 16 colunas
	//for (yy = y; yy <= y + 0x13; yy++) { // Para 20 colunas
	for (yy = y; yy <= y + addr_end; yy++) { // Para 20 colunas
		lcd_busy();
		lcd_write_data((u8)' ');
	}
	
	lcd_goto_xy(0, y, addr_offset);
}

//-----------------------------------------------------------------------------------------------------------------------
// SALTA PARA COLUNA=X (0..X) E LINHA=Y (0..Y)
//-----------------------------------------------------------------------------------------------------------------------
void lcd_goto_xy(u8 x, u8 y, const u8 addr_offset) {
	
	if (y < 2) 	lcd_set_addr_data((x) + (0x40*(y)));					// linhas 0 e 1
	else  		lcd_set_addr_data((x) + (0x40*(y) + addr_offset));		// Linhas 2 e 3
	
	/*
	if (y < 2) 	// linhas 0 e 1
		lcd_set_addr_data((x) + (0x40*(y)));
	else 			// Linhas 2 e 3
		lcd_set_addr_data((x) + (0x40*(y)+0x14));
	*/
	
}

//-----------------------------------------------------------------------------------------------------------------------
// APONTA PARA O ENDEREÇO DESEJADO
//-----------------------------------------------------------------------------------------------------------------------
void lcd_set_addr_data(u8 address)	{
	lcd_busy();
	lcd_write_command(0x80 | address);
}

//-----------------------------------------------------------------------------------------------------------------------
// ESCREVE ARRAY CHAR NO DISPLAY
//-----------------------------------------------------------------------------------------------------------------------
void lcd_put_string(const char *value)	{
	while(*value)	{
		lcd_busy();
		lcd_write_data(*value++);
	}	
}

//-----------------------------------------------------------------------------------------------------------------------
// RETORNA O ENDEREÇO DO ULTIMO DADO GRAVADO
//-----------------------------------------------------------------------------------------------------------------------
u8 lcd_last_addr_data(void)	{
	lcd_busy();
	return lcd_read_data();
}

//-----------------------------------------------------------------------------------------------------------------------
// FUNÇÃO UTILIZADA PELO RPRINTF
//-----------------------------------------------------------------------------------------------------------------------
int lcd_put_char(n16 ch) {
	lcd_busy();
	lcd_write_data((u8)ch);
	return (int)ch;												// Retorna com o caractere a ser TX
}

//-----------------------------------------------------------------------------------------------------------------------
// ESPERA ATÉ O DISPLAY ESTIVER DESOCUPADO
//-----------------------------------------------------------------------------------------------------------------------
void lcd_busy(void)	{
	while (lcd_read_command() & 0x80);; //if bit 7 = 1 display busy
}
