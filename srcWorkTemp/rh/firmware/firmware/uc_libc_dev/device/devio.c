#include "devio.h"

// ###################################################################
// VARS DE CONTROLE DE ACESSO AOS DISPOSITIVOS
// ###################################################################
typedef struct {
	u8 id;
	devioctl ctrl;
}tDevIO;

static u8 devcount; 			// Quantidade de dispositivos registrados
static tDevIO deviolist[DEVIO_QTD_DEVICES];  	// Lista dos dispositivos registrados

// Rotina de registro de dispositivos
int devioreg(int id, devioctl ctrl) {
	if (devcount == DEVIO_QTD_DEVICES) return pdFAIL;
	deviolist[devcount].id = id;
	deviolist[devcount].ctrl = ctrl;
	devcount++;
	return pdPASS;
}

// Rotina para chamar funções de escrita e leitura de cada dispositivo
int devio(u8 dev, u8 op, void *data, u32 addr, u32 len) {
    u8 i;

    for (i=0; i<devcount; i++)
    	if (dev == deviolist[i].id)
    		return deviolist[i].ctrl(op, data, addr, len); // Chama a função devio_xxxx do dispositivo

    return pdFAIL; // devio: invalid
}

// ###################################################################
// FACILITADORES DE ACESSO A TIPOS DE DADOS SIMPLES
// ###################################################################
// gravação
void devwb(u8 dev, u32 addr, u8 data)  {
	devw(dev, &data, addr, 1);
}

void devww(u8 dev, u32 addr, u16 data) {
	devw(dev, &data, addr, 2);
}

void devwd(u8 dev, u32 addr, u32 data) {
	devw(dev, &data, addr, 4);
}

// leitura
u8 devrb(u8 dev, u32 addr) {
    u8 rd;
    if (devr(dev, &rd, addr, 1) == pdFAIL) return 0;
    return rd;
}

u16 devrw(u8 dev, u32 addr) {
    u16 rd;
    if (devr(dev, &rd, addr, 2) == pdFAIL) return 0;
    return rd;
}

u32 devrd(u8 dev, u32 addr) {
    u32 rd;
    if (devr(dev, &rd, addr, 4) == pdFAIL) return 0;
    return rd;
}

