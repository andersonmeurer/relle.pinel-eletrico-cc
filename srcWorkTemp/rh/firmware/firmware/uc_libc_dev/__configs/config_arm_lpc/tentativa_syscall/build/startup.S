		.extern main
	
		#ifdef RTOS_UCLINUX
			.extern func_irq_swi
		#endif

		// SÍMBOLOS DO LINKER SCRIPT
		.extern _stack					// Endereço inicial da RAM para as STACKS. Topo da RAM
		.extern _etext					// Endereço final da memória de programa. Contendo as secções .text(código executável) e .rodata (contantes)
		.extern _data					// Endereço inicial da área de RAM da secção .data (variáveis inicializadas)
		.extern _edata					// Endereço final da área de RAM da secção .data
		.extern _bss					// Endereço inicial da área de RAM da secção .bss (variáveis não inicializadas)
		.extern _ebss					// Endereço final da área de RAM da secção .bss

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
// VETORES DE EXCEÇÕES
// 	O vetor de exeções deve estar localizadao em um dos 4 locais, FLASH, SRAM interma, BOOT ou memória externa.
// 		o local é especificado no link script junto com registrador MEMAP do ARM
// ##########################################################################################################################
        .global _vector
        .func   _vector

_vector:
		b     _start							// Exceção de reset. 					Chamar a função _start
		ldr   pc, _undef_addr					// Exceção indefinida. 					Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _swi_addr						// Exceção de interrupção por software. Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _pabt_addr					// Exceção prefetch abort.				Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _dabt_addr					// Exceção data abort.					Carrega PC com endereço da função que tratará essa exceção
		nop										// Exceção reserveda
	#if defined(LPC2468)
		ldr   pc, [pc, #-0x120]					// Exceção IRQ. 						Carrega PC com endereço da função contida no registrador VicVectAddr
	#else
		ldr   pc, [pc, #-0xFF0]					// Exceção IRQ. 						Carrega PC com endereço da função contida no registrador VicVectAddr
	#endif
		ldr   pc, _fiq_addr						// Exceção FIQ.							Carrega PC com endereço da função que tratará essa exceção

// DEFININDOS ENDEREÇOS DE SALTO
#if defined(RTOS_NONE)
	_undef_addr: .word 	__default_handler         // Captura o endereço da função padrão
	_swi_addr:   .word 	__default_handler         // Captura o endereço da função padrão
	_pabt_addr:  .word 	__default_handler         // Captura o endereço da função padrão
	_dabt_addr:  .word 	__default_handler         // Captura o endereço da função padrão
	_fiq_addr:   .word 	__default_handler         // Captura o endereço da função padrão
#elif defined(RTOS_UCLINUX)
	_undef_addr: .word 	0x81000004                // Captura o endereço da função que trata instruções indefinidas
	_swi_addr:   .word 	0x81000008                // Captura o endereço da função que trata a interrupção por software
	_pabt_addr:  .word 	0x8100000C                // Captura o endereço da função que trata prefetch abort
	_dabt_addr:  .word 	0x81000010                // Captura o endereço da função que trata data abort
	_fiq_addr:   .word 	0x8100001C                // Captura o endereço da função que trata interrupção rápida
#elif defined(FREE_RTOS)
	_undef_addr: .word 	__default_handler         // Captura o endereço da função padrão
	_swi_addr:   .word 	vPortYieldProcessor       // Captura o endereço da função que trata a interrupção por software
	_pabt_addr:  .word 	__default_handler         // Captura o endereço da função padrão
	_dabt_addr:  .word 	__default_handler         // Captura o endereço da função padrão
	_fiq_addr:   .word 	__default_handler 		  // Captura o endereço da função padrão
#endif

// FUNÇÂO PADRÂO DE SALTO
__default_handler:  b .

        .size _vector, . - _vector
        .endfunc


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
// CONFIGURA A RAM EXTERNA SE FOR USADA
// -------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXT_STATIC_RAM
	#if defined(LPC2468)
	
	#else	// PARA O LPC22xx
        .set BCFG0_OFS,    0x00        // BCFG0 Offset
        .set BCFG1_OFS,    0x04        // BCFG1 Offset
        .set BCFG2_OFS,    0x08        // BCFG2 Offset
        .set BCFG3_OFS,    0x0C        // BCFG3 Offset

		// AJUSTA OS PINOS PARA INTERFACEAR COM A RAM
		ldr     R0, =PINSEL_ADDR
       	ldr     R1, =PINSEL_VALUE
        str     R1, [R0]
 
 		// CONFIGURA O BARRAMENTO
 		ldr     R0, =BCFG_BASE

        ldr     R1, =BCFG0_VALUE
        str     R1, [R0, #BCFG0_OFS]
        ldr     R1, =BCFG1_VALUE
        str     R1, [R0, #BCFG1_OFS]
        ldr     R1, =BCFG2_VALUE
        str     R1, [R0, #BCFG2_OFS]
        ldr     R1, =BCFG3_VALUE
        str     R1, [R0, #BCFG3_OFS]
	#endif
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
#ifdef ROM_RUN
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
//
// -------------------------------------------------------------------------------------------------------------------------
#ifndef RTOS_UCLINUX
		// 	Desejamos que a cpu inicie no modo supervisor. 
		// 	O sistema operacional irá para modo system quando iniciar a primeira tarefa
		msr   CPSR_c, #MODE_SVC|IRQ_DISABLE|FIQ_DISABLE
#endif

// -------------------------------------------------------------------------------------------------------------------------
// CHAMA O PROGRAMA MAIN SEM PASSAGEM DE PARAMETROS
// -------------------------------------------------------------------------------------------------------------------------
		#ifdef USE_MY_SYSCALL
			bl	syscalls_Init					// Incializa syscalls newlib
		#endif
		
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
