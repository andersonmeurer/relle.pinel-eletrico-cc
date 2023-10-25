#include "_config_cpu_.h"
#include "vic.h"

#if (STARTUP_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

#if defined(RUN_MODE1) || defined(RUN_MODE2) || defined (RUN_MODEBOOT1) || defined (RUN_MODEBOOT2)
//extern u32 _stack;		// Endere�o inicial da RAM para as STACKS. Topo da RAM
extern u32 _etext;		// Endere�o final da mem�ria de programa. Contendo as sec��es .text(c�digo execut�vel) e .rodata (contantes)
extern u32 _data;		// Endere�o inicial da �rea de RAM da sec��o .data (vari�veis inicializadas)
extern u32 _edata;		// Endere�o final da �rea de RAM da sec��o .data
#endif
extern u32 _bss;		// Endere�o inicial da �rea de RAM da sec��o .bss (vari�veis n�o inicializadas)
extern u32 _ebss;		// Endere�o final da �rea de RAM da sec��o .bss

extern int main (void);
extern void arm_Init (void);
#if defined(RUN_MODEBOOT2) || (RUN_MODEBOOT1)
extern void boot_Check(void);
#endif

#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
void ramext_Init (void);
#endif


// ESTA FUN��O � CHAMADA PELO VETOR DE INTERRUP��O RESET
void _start (void);
void _start (void) {
	// DESABILITA TODAS AS INTERRUP��ES
	irq_Disable();

	// -------------------------------------------------------------------------------------------------------------------------
	// SE ESTAMOS USANDO O BOOTLOADER � PRECISO CHAMAR A ROTINA DE VERIFICA��O
	//	Se � para � para executar o aplicativo do usu�rio ou n�o.
	//   pois, o aplicativo do usu�rio deve ser executado com todos os registradores do padr�o do reset
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(RUN_MODEBOOT2) || (RUN_MODEBOOT1)
	boot_Check();
	#endif
	// -------------------------------------------------------------------------------------------------------------------------
	// CONFIGURA PROCESSADOR ARM
	// 	As configura��es do arm deve ser feita antes, isto porque em algumas configura��es abaixo
	//	 necessitam de delays para inicializa��es baseadas na frequencia do processador
	// -------------------------------------------------------------------------------------------------------------------------
	arm_Init();

	// -------------------------------------------------------------------------------------------------------------------------
	// CONFIGURA A RAM EXTERNA SE FOR USADA
	// 	As configura��es das rams externas deve ser feitas antes de atualizar as sec��es .data e .bss,
	//	 isto porque podemos alocar essas sec��es na ram externa
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
	ramext_Init();
	#endif

	// -------------------------------------------------------------------------------------------------------------------------
	// AJUSTANDO OS STACKS DE CADA MODO DE OPERA��O DO PROCESSADOR ARM
	// -------------------------------------------------------------------------------------------------------------------------

	u32 *pSrc;
	u32 *pDest;

	// -------------------------------------------------------------------------------------------------------------------------
	// ATRIBUI OS VALORES INICIAIS DAS VARI�VEIS DA SEC��O .data
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(RUN_MODE1) || defined(RUN_MODE2) || defined (RUN_MODEBOOT1) || defined (RUN_MODEBOOT2)
	pSrc  = &_etext; 		// Captura o endere�o final da sec��o .text e .rodata. Onde cont�m os valores da vari�veis inicializadas
	pDest = &_data;			// Captura o endere�o inicial da �rea de RAM da sec��o .data (vari�veis inicializadas)
	while(pDest < &_edata)	// Fa�a at� o endere�o final da �rea de RAM da sec��o .data
		*pDest++ = *pSrc++;
	#endif

	// -------------------------------------------------------------------------------------------------------------------------
	// LIMPA A �REA DE RAM DAS VARI�VEIS N�O INICIALIZADAS
	// -------------------------------------------------------------------------------------------------------------------------
	pDest = &_bss; 			// Captura o endere�o inicial da �rea de RAM da sec��o .bss (vari�veis n�o inicializadas)
	while(pDest < &_ebss)	// Fa�a at� o endere�o final da �rea de RAM da sec��o .bss
		*pDest++ = 0;		// Limpa o endere�o de mem�ria


	// -------------------------------------------------------------------------------------------------------------------------
	// CHAMA O PROGRAMA MAIN SEM PASSAGEM DE PARAMETROS
	// -------------------------------------------------------------------------------------------------------------------------
	main();

	// -------------------------------------------------------------------------------------------------------------------------
	// caso houver algum problema se sair do main cair aqui
	// -------------------------------------------------------------------------------------------------------------------------
	#if (STARTUP_USE_DEBUG == pdON)
	plognp(CMD_TERMINATOR "ERRO FATAL: saiu do programa principal (main) "CMD_TERMINATOR);
	#endif
	while(1);
}
