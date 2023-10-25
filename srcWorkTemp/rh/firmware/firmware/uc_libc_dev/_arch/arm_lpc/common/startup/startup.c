#include "_config_cpu_.h"
#include "vic.h"

#if (STARTUP_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

#if defined(RUN_MODE1) || defined(RUN_MODE2) || defined (RUN_MODEBOOT1) || defined (RUN_MODEBOOT2)
//extern u32 _stack;		// Endereço inicial da RAM para as STACKS. Topo da RAM
extern u32 _etext;		// Endereço final da memória de programa. Contendo as secções .text(código executável) e .rodata (contantes)
extern u32 _data;		// Endereço inicial da área de RAM da secção .data (variáveis inicializadas)
extern u32 _edata;		// Endereço final da área de RAM da secção .data
#endif
extern u32 _bss;		// Endereço inicial da área de RAM da secção .bss (variáveis não inicializadas)
extern u32 _ebss;		// Endereço final da área de RAM da secção .bss

extern int main (void);
extern void arm_Init (void);
#if defined(RUN_MODEBOOT2) || (RUN_MODEBOOT1)
extern void boot_Check(void);
#endif

#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
void ramext_Init (void);
#endif


// ESTA FUNÇÃO É CHAMADA PELO VETOR DE INTERRUPÇÃO RESET
void _start (void);
void _start (void) {
	// DESABILITA TODAS AS INTERRUPÇÕES
	irq_Disable();

	// -------------------------------------------------------------------------------------------------------------------------
	// SE ESTAMOS USANDO O BOOTLOADER É PRECISO CHAMAR A ROTINA DE VERIFICAÇÃO
	//	Se é para é para executar o aplicativo do usuário ou não.
	//   pois, o aplicativo do usuário deve ser executado com todos os registradores do padrão do reset
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(RUN_MODEBOOT2) || (RUN_MODEBOOT1)
	boot_Check();
	#endif
	// -------------------------------------------------------------------------------------------------------------------------
	// CONFIGURA PROCESSADOR ARM
	// 	As configurações do arm deve ser feita antes, isto porque em algumas configurações abaixo
	//	 necessitam de delays para inicializações baseadas na frequencia do processador
	// -------------------------------------------------------------------------------------------------------------------------
	arm_Init();

	// -------------------------------------------------------------------------------------------------------------------------
	// CONFIGURA A RAM EXTERNA SE FOR USADA
	// 	As configurações das rams externas deve ser feitas antes de atualizar as secções .data e .bss,
	//	 isto porque podemos alocar essas secções na ram externa
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
	ramext_Init();
	#endif

	// -------------------------------------------------------------------------------------------------------------------------
	// AJUSTANDO OS STACKS DE CADA MODO DE OPERAÇÃO DO PROCESSADOR ARM
	// -------------------------------------------------------------------------------------------------------------------------

	u32 *pSrc;
	u32 *pDest;

	// -------------------------------------------------------------------------------------------------------------------------
	// ATRIBUI OS VALORES INICIAIS DAS VARIÁVEIS DA SECÇÃO .data
	// -------------------------------------------------------------------------------------------------------------------------
	#if defined(RUN_MODE1) || defined(RUN_MODE2) || defined (RUN_MODEBOOT1) || defined (RUN_MODEBOOT2)
	pSrc  = &_etext; 		// Captura o endereço final da secção .text e .rodata. Onde contém os valores da variáveis inicializadas
	pDest = &_data;			// Captura o endereço inicial da área de RAM da secção .data (variáveis inicializadas)
	while(pDest < &_edata)	// Faça até o endereço final da área de RAM da secção .data
		*pDest++ = *pSrc++;
	#endif

	// -------------------------------------------------------------------------------------------------------------------------
	// LIMPA A ÁREA DE RAM DAS VARIÁVEIS NÃO INICIALIZADAS
	// -------------------------------------------------------------------------------------------------------------------------
	pDest = &_bss; 			// Captura o endereço inicial da área de RAM da secção .bss (variáveis não inicializadas)
	while(pDest < &_ebss)	// Faça até o endereço final da área de RAM da secção .bss
		*pDest++ = 0;		// Limpa o endereço de memória


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
