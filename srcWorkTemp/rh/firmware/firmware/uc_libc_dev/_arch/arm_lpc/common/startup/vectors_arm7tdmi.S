		.text
		.code 32
		.align 0

        .global _vector
        .func   _vector

_vector:
		b     _start							// Exceção de reset. 					Chamar a função _start
		ldr   pc, _undef_addr					// Exceção indefinida. 					Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _swi_addr						// Exceção de interrupção por software. Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _pabt_addr					// Exceção prefetch abort.				Carrega PC com endereço da função que tratará essa exceção
		ldr   pc, _dabt_addr					// Exceção data abort.					Carrega PC com endereço da função que tratará essa exceção
		nop										// Exceção reservada para Checksum
		ldr   pc, _irq_addr						// Exceção IRQ. 						Carrega PC com endereço da função contida no registrador VicVectAddr
		ldr   pc, _fiq_addr						// Exceção FIQ.							Carrega PC com endereço da função que tratará essa exceção

// DEFININDOS ENDEREÇOS DE SALTO
#if defined(RTOS_NONE)
	_undef_addr: .word 	vectorUndefHandler      	// Captura o endereço da função que trata instruções indefinidas
	_swi_addr:   .word 	vectorSwiHandler       		// Captura o endereço da função que trata a interrupção por software
	_pabt_addr:  .word 	vectorPrefetchAbortHandler  // Captura o endereço da função que trata prefetch abort
	_dabt_addr:  .word 	vectorDataAbortHandler      // Captura o endereço da função que trata data abort
	_irq_addr:   .word 	vectorIrqHandler          	// Captura o endereço da função que trata interrupção IRQ
	_fiq_addr:   .word 	vectorFiqHandler 			// Captura o endereço da função que trata interrupção FIQ
#elif defined(RTOS_UCLINUX)
	_undef_addr: .word 	0x81000004                	// Captura o endereço da função que trata instruções indefinidas
	_swi_addr:   .word 	0x81000008                	// Captura o endereço da função que trata a interrupção por software
	_pabt_addr:  .word 	0x8100000C                	// Captura o endereço da função que trata prefetch abort
	_dabt_addr:  .word 	0x81000010                	// Captura o endereço da função que trata data abort
	_irq_addr:   .word 	0x81000018                	// Captura o endereço da função que trata interrupção IRQ
	_fiq_addr:   .word 	0x8100001C                	// Captura o endereço da função que trata interrupção rápida
#elif defined(FREE_RTOS)
	_undef_addr: .word 	vectorUndefHandler      	// Captura o endereço da função que trata instruções indefinidas
	_swi_addr:   .word 	vPortYieldProcessor       	// Captura o endereço da função que trata a interrupção por software
	_pabt_addr:  .word 	vectorPrefetchAbortHandler  // Captura o endereço da função que trata prefetch abort
	_dabt_addr:  .word 	vectorDataAbortHandler      // Captura o endereço da função que trata data abort
	_irq_addr:   .word 	vectorIrqHandler          	// Captura o endereço da função que trata interrupção IRQ
	_fiq_addr:   .word 	vectorFiqHandler 			// Captura o endereço da função que trata interrupção FIQ
#endif
		.end
