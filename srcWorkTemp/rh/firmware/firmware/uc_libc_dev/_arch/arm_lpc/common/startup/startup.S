// Este startup é usado somenete nos ARM7 com exceção do LPC2468
// Isto porque usando o startup.c as chamadas de funções de dentro de cada função trava o uC

 		.global main
		.global exit
		.extern arm_Init
		.extern irq_Disable
		.extern vic_Init
		
		#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
		.extern ramext_Init
		#endif

		#ifdef RTOS_UCLINUX
		.global func_irq_swi
		#endif

		// SÍMBOLOS DO LINKER SCRIPT
		.global _stack					// Endereço inicial da RAM para as STACKS. Topo da RAM
		.global _etext					// Endereço final da memória de programa. Contendo as secções .text(código executável) e .rodata (contantes)
		.global _data					// Endereço inicial da área de RAM da secção .data (variáveis inicializadas)
		.global _edata					// Endereço final da área de RAM da secção .data
		.global _bss					// Endereço inicial da área de RAM da secção .bss (variáveis não inicializadas)
		.global _ebss					// Endereço final da área de RAM da secção .bss

		// CONSTANTES DO STARTUP
		.set  MODE_USR, 0x10            // Modo user
		.set  MODE_FIQ, 0x11            // Modo fiq
		.set  MODE_IRQ, 0x12            // Modo irq
		.set  MODE_SVC, 0x13            // Modo supervisor
		.set  MODE_ABT, 0x17            // Modo abort
		.set  MODE_UND, 0x1B            // Modo undefined
		.set  MODE_SYS, 0x1F            // Modo system

		.equ  IRQ_DISABLE, 0x80         // Quando o bit (I) é setado, IRQ é dasabilitado
		.equ  FIQ_DISABLE, 0x40         // Quando o bit (F) é setado, FIQ é dasabilitado

		.text
		.code 32
		.align 0

// ##########################################################################################################################
// PROGRAMA STARTUP QUE SERÁ EXECUATDO NA EXCEÇÃO RESET
// ##########################################################################################################################
        .global _start
        .func   _start

#ifdef RTOS_UCLINUX
_swiirq:
swiirq:
        ldr   r10,=func_irq_swi
        mov   lr,pc
        bx    r10                       // enter main()
#endif

_start:
// -------------------------------------------------------------------------------------------------------------------------
// DESABILITA TODAS AS INTERRUPÇÕES
// -------------------------------------------------------------------------------------------------------------------------
		#if defined (RTOS_NONE)
		bl irq_Disable
		#endif

// -------------------------------------------------------------------------------------------------------------------------
// CONFIGURA PROCESSADOR ARM
// 		AS CONFIGURAÇÕES DO ARM DEVE SER FEITA ANTES, ISTO PORQUE EM ALGUMAS CONFIGURAÇÕES ABAIXO PRECISAMOS DO TEMPOS 
//			CALCULADOS PELA FREQUENCIA DO PROCESSADOR
// -------------------------------------------------------------------------------------------------------------------------
		bl	arm_Init

// -------------------------------------------------------------------------------------------------------------------------
// CONFIGURA PROCESSADOR ARM
// 	As configurações do arm deve ser feita antes, isto porque em algumas configurações abaixo
//	 necessitam de delays para inicializações baseadas na frequencia do processador
// -------------------------------------------------------------------------------------------------------------------------
		#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
		// CONFIGURA SRAM e sdram caso for lpc2468
    	bl 	ramext_Init   		
		#endif

// -------------------------------------------------------------------------------------------------------------------------
// AJUSTANDO OS STACKS DE CADA MODO DE OPERAÇÃO DO PROCESSADOR ARM
// -------------------------------------------------------------------------------------------------------------------------
		ldr   r0, =_stack
		
		// STACK PARA MODO FIQ
		msr   CPSR_c, #MODE_FIQ|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0
		sub   r0, r0, #FIQ_STACK_SIZE
		
		// STACK PARA MODO IRQ
		msr   CPSR_c, #MODE_IRQ|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0
		sub   r0, r0, #IRQ_STACK_SIZE
		
		// STACK PARA MODO SUPERVISOR
		msr   CPSR_c, #MODE_SVC|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0
		sub   r0, r0, #SVC_STACK_SIZE
		
		// STACK PARA MODO INDEFINIDO
		msr   CPSR_c, #MODE_UND|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0
		sub   r0, r0, #UND_STACK_SIZE
		
		// STACK PARA MODO ABORTO
		msr   CPSR_c, #MODE_ABT|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0
		sub   r0, r0, #ABT_STACK_SIZE
		
		// STACK PARA MODO DE SISTEMA E USUÁRIO
		msr   CPSR_c, #MODE_SYS|IRQ_DISABLE|FIQ_DISABLE
		mov   sp, r0

// -------------------------------------------------------------------------------------------------------------------------
// ATRIBUI OS VALORES INICIAIS DAS VARIÁVEIS DA SECÇÃO .data
// -------------------------------------------------------------------------------------------------------------------------
#if defined(RUN_MODE1) || defined(RUN_MODE2) || defined (RUN_MODEBOOT1) || defined (RUN_MODEBOOT2)
        ldr   r0,=_etext               			// Endereço final da secção .text e .rodata. Onde contém os valores da variáveis inicializadas
        ldr   r1,=_data                			// Endereço inicial da área de RAM da secção .data (variáveis inicializadas)
        ldr   r2,=_edata                		// Endereço final da área de RAM da secção .data
_copy_data:      
		cmp   r1,r2                     		 
        ldrne r3,[r0],#4                		
        strne r3,[r1],#4
        bne   _copy_data                    	
#endif

// -------------------------------------------------------------------------------------------------------------------------
// LIMPA A ÁREA DE RAM DAS VARIÁVEIS NÃO INICIALIZADAS
// -------------------------------------------------------------------------------------------------------------------------
        ldr   r0, =0                     		// Pega o valor 0
        ldr   r1, =_bss         				// Endereço inicial da área de RAM da secção .bss (variáveis não inicializadas)
        ldr   r2, =_ebss           				// Endereço final da área de RAM da secção .bss
_clear_bss:      
		cmp   r1,r2                    		 	// check if data to clear
        strne r0,[r1],#4                		// clear 4 bytes
        bne   _clear_bss                   		// loop until done

// -------------------------------------------------------------------------------------------------------------------------
// INICAR O MODO SUPERVISOR PARA O FREERTOS
// -------------------------------------------------------------------------------------------------------------------------
		#if defined (FREE_RTOS) && defined (arm7tdmi)
		// TODO - ainda não testada, pode ser que a instrução assember não foi convertida de .S para .C
		// 	Desejamos que a cpu inicie no modo supervisor. A maioria dos OS tempo real tantos os processos do kernel como as tarefas
		//		das aplicações são executadas no modo supervisor
		msr   CPSR_c, #MODE_SVC|IRQ_DISABLE|FIQ_DISABLE
		#endif

// -------------------------------------------------------------------------------------------------------------------------
// INICIALIZA OS CANAIS IRQ VETORADO PARA O ARM7TDMI
// -------------------------------------------------------------------------------------------------------------------------
		#if defined (arm7tdmi)
		bl vic_Init
		#endif

// -------------------------------------------------------------------------------------------------------------------------
// CHAMA O PROGRAMA MAIN SEM PASSAGEM DE PARAMETROS
// -------------------------------------------------------------------------------------------------------------------------		
        mov   r0,#0                    			// Sem argumentos (argc = 0)
        mov   r1,r0
        mov   r2,r0
        mov   fp,r0                     		// null frame pointer
        mov   r7,r0                     		// null frame pointer for thumb
        ldr   r10,=main
        mov   lr,pc
   
        bx    r10                       		// Entra no código C (main()). Utilize  a instrução BX para nunca voltar
        										// Utilize BLX (?) se voce deseja usar o c++

		.size   _start, . - _start
        .endfunc
	
		.end
