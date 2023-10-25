#ifndef __VIC_CORTERM3_H 
#define __VIC_CORTERM3_H

#include "_config_cpu_.h"

// NÃO É PRECISO ADICIONAR OS PROCEDIMENTO DE ENTRADA E SAIDA DO ISR,
// DEVIDO QUE O COMPILADOR ADICIONA OS PROCEDIMENTOS AUTOMATICAMENTE
// Deixar as macros abaixo para manter compativel com o arm7tdmi
#define ISR_EXIT
#define ISR_ENTRY

#define irq_Enable()               asm volatile ("cpsie i")
#define irq_Disable()              asm volatile ("cpsid i")

void nvic_enableIRQ(u8 numIRQ);
void nvic_disableIRQ(u8 numIRQ);
u8 nvic_getPendingIRQ(u8 numIRQ);
void nvic_setPendingIRQ(u8 numIRQ);
void nvic_clearPendingIRQ(u8 numIRQ);
u8 nvic_getActiveIRQ(u8 numIRQ);
void nvic_setPriority(s32 numIRQ, u8 priority);
void nvic_SetIntHandler(void (*func)(int) );
#endif
