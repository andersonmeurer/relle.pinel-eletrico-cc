#include "glcd_drv.h"

static void glcd_DevSelectChip(u8 chip);
static void glcd_DevUnSelectChip(u8 chip);

static void glcd_DevSelectChip(u8 chip) {
	if (chip==1)
			glcdPORTCS_SET |= glcdPIN_CS1;
	else  	glcdPORTCS_SET |= glcdPIN_CS0;
}

static void glcd_DevUnSelectChip(u8 chip) {
	if (chip==1)
			glcdPORTCS_CLR |= glcdPIN_CS1;
	else  	glcdPORTCS_CLR |= glcdPIN_CS0;
}

// INIT PORTS
void glcd_ConfigPorts(void) {
	glcdPORTDATA_DIR &= ~(glcdPIN_D7|glcdPIN_D6|glcdPIN_D5|glcdPIN_D4|glcdPIN_D3|glcdPIN_D2|glcdPIN_D1|glcdPIN_D0); // Pinos dos dados como entrada
	glcdPORTCTRL_DIR |= (glcdPIN_RS|glcdPIN_WR|glcdPIN_E); 		// Pinos de controle como saida
	glcdPORTCTRL_CLR |= (glcdPIN_RS|glcdPIN_WR|glcdPIN_E);		// Desabilita o pino

	glcdPORTCS_DIR |= (glcdPIN_CS1|glcdPIN_CS0); 				// Pinos de seleção como saida
	glcdPORTCS_CLR |= (glcdPIN_CS1|glcdPIN_CS0); 				// Desabilita os pinos
}

void glcd_WaitReady(u8 chip) {
	while((glcd_ReadCommand(chip) == glcdBUSY_FLAG)); // Espera até que o display esteja pronto para receber dados ou comandos
}

// LÊ NO LCD
u8 glcd_Read(u8 chip, u8 rs) {
	u8 data = 0;
	glcd_DevSelectChip(chip);

	glcdPORTCTRL_SET = glcdPIN_WR;			// R/#W = 1;
	if (rs)	glcdPORTCTRL_SET = glcdPIN_RS;	// RS = 1;
	else	glcdPORTCTRL_CLR = glcdPIN_RS;	// RS = 0;
	glcdPORTCTRL_SET = glcdPIN_E;			// E = 1

	delay_us(glcdDELAY_RW);

	if (glcdPORTDATA_IN&glcdPIN_D7)		data|=0x80;
	if (glcdPORTDATA_IN&glcdPIN_D6)		data|=0x40;
	if (glcdPORTDATA_IN&glcdPIN_D5)		data|=0x20;
	if (glcdPORTDATA_IN&glcdPIN_D4)		data|=0x10;
	if (glcdPORTDATA_IN&glcdPIN_D3)		data|=0x08;
	if (glcdPORTDATA_IN&glcdPIN_D2)		data|=0x04;
	if (glcdPORTDATA_IN&glcdPIN_D1)		data|=0x02;
	if (glcdPORTDATA_IN&glcdPIN_D0)		data|=0x01;

	glcdPORTCTRL_CLR = glcdPIN_E;			// E = 0
	glcd_DevUnSelectChip(chip);

	return data;
}

// ESCREVE NO LCD
void glcd_Write(u8 reg, u8 chip, u8 rs) {
	glcd_WaitReady(chip);

	glcd_DevSelectChip(chip);
	glcdPORTDATA_DIR |= (glcdPIN_D7|glcdPIN_D6|glcdPIN_D5|glcdPIN_D4|glcdPIN_D3|glcdPIN_D2|glcdPIN_D1|glcdPIN_D0); // Pinos dos dados como saida
	glcdPORTCTRL_CLR = glcdPIN_WR;			// WR = 0;

	if (rs)	glcdPORTCTRL_SET = glcdPIN_RS;	// RS = 1;
	else	glcdPORTCTRL_CLR = glcdPIN_RS;	// RS = 0;

	// Atualizar barramento do LCD com o o valor do registrador
	if (reg&0x80) glcdPORTDATA_SET = glcdPIN_D7; else glcdPORTDATA_CLR = glcdPIN_D7;
	if (reg&0x40) glcdPORTDATA_SET = glcdPIN_D6; else glcdPORTDATA_CLR = glcdPIN_D6;
	if (reg&0x20) glcdPORTDATA_SET = glcdPIN_D5; else glcdPORTDATA_CLR = glcdPIN_D5;
	if (reg&0x10) glcdPORTDATA_SET = glcdPIN_D4; else glcdPORTDATA_CLR = glcdPIN_D4;
	if (reg&0x08) glcdPORTDATA_SET = glcdPIN_D3; else glcdPORTDATA_CLR = glcdPIN_D3;
	if (reg&0x04) glcdPORTDATA_SET = glcdPIN_D2; else glcdPORTDATA_CLR = glcdPIN_D2;
	if (reg&0x02) glcdPORTDATA_SET = glcdPIN_D1; else glcdPORTDATA_CLR = glcdPIN_D1;
	if (reg&0x01) glcdPORTDATA_SET = glcdPIN_D0; else glcdPORTDATA_CLR = glcdPIN_D0;

	glcdPORTCTRL_SET = glcdPIN_E;	// E = 1
	delay_us(glcdDELAY_RW);
	glcdPORTCTRL_CLR = glcdPIN_E;	// E = 0
	glcdPORTDATA_DIR &= ~(glcdPIN_D7|glcdPIN_D6|glcdPIN_D5|glcdPIN_D4|glcdPIN_D3|glcdPIN_D2|glcdPIN_D1|glcdPIN_D0); // Pinos dos dados como entrada
	glcd_DevUnSelectChip(chip);
}
