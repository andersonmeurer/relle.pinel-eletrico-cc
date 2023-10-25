


# MAKEFILE PARA AMBIENTE CYGWYN E COM COMPILADOR GNUARM 4.1.2 - NÃO REENTRANTE
# ============================================================================


#############################################################################################################################################
# Linhas de comandos do MAKE
#
# make all ou make		= Compila o software
# make dump				= Exibe informações das secções de memória
# make lib				= Cria a biblioteca estática (.a) das bibliotecas usadas pelos software
# make clean ou c 		= Limpa o software e biblioteca estática (.a) compilados
#############################################################################################################################################

##########################################################################################################################################
########### ESPECEFICA QUAL PROMPT O SISTEMA SERÁ COMPILADO  >> CYGWIN | WINDOWS
	
	PROMTP_TYPE = CYGWIN

###########################################################################################################################################
########### TIPO DE PROCESSADOR E DE COMPILAÇÃO
	
	#### TIPO DO ARM  					>> LPC2294 | LPC2468
	SUBMDL = LPC2294

	#### UTILIZAÇÃO DE SIMULADOR 		>> SIM_NONE | SIM_SKYEYE
 	SIMULATOR = SIM_NONE
	
	#### UTILIZAÇÃO RTOS 				>> RTOS_NONE | RTOS_UCLINUX | FREE_RTOS
	RTOS = RTOS_NONE

	####  UTILIZAÇÃO DE PILHA TCP/IP	>> TCP_STACK_NONE | TCP_STACK_UIP
	TCP_STACK = TCP_STACK_NONE
	
	#### MY_SYSCALL						>> NOT_USE | USE_MY_SYSCALL # subistitui alguma funções do compialdor como _open, _close ....
	MY_SYSCALL = USE_MY_SYSCALL
	
	#### CRIA IMAGEM 					>> ROM_RUN ## Cria imagem ROM (final)  | RAM_RUN	## Cria imagem RAM (depuração)
	RUN_MODE = ROM_RUN		

############################################################################################################################################
########### DRIERTÓRIOS DAS BIBLIOTECAS

	ifeq ($(PROMTP_TYPE), WINDOWS)
		PATH_UC_LIBS 	= D:/_libs/uC_libc1.1
	else
		PATH_UC_LIBS 	= /cygdrive/d/_libs/uC_libc1.1
	endif

	PATH_ARM_LIBS		= $(PATH_UC_LIBS)/_arch/arm_lpc
	PATH_UIP_LIBS		= $(PATH_UC_LIBS)/uip/uip-v1.0-b
	PATH_FREERTOS_LIBS	= $(PATH_UC_LIBS)/free_rtos/v5.2.0

###########################################################################################################################################
########### ARQUIVOS C A SEREM COMPILADOS
########### 	As dependencias são geradas automaticamente
########### 	SRCARM são fontes do prorcessador ARM em C
########### 	SRC são fontes gerais em C
	
	EXTRAINCDIRS = 	# ./diretório
	
	TARGET  = main
	MAINSRC = $(TARGET).c

	ifeq ($(MY_SYSCALL), USE_MY_SYSCALL)
		SRC = $(PATH_UC_LIBS)/newlib/syscalls.c
	endif
		
# -------------------------------------------------------------------------------
# DISPOSITIVOS ARM

	SRCARM = $(PATH_ARM_LIBS)/commom/arm.c
	SRCARM += $(PATH_ARM_LIBS)/commom/vic.c

#	SRCARM += $(PATH_ARM_LIBS)/uart/uart0_poll.c
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart1_poll.c
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart2_poll.c						# (Somente no LPC2400)
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart3_poll.c						# (Somente no LPC2400)
	SRCARM += $(PATH_ARM_LIBS)/uart/uart0_irq.c							# requer vic.c e buffer.c
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart1_irq.c							# requer vic.c e buffer.c
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart2_irq.c							# requer vic.c e buffer.c (Somente no LPC2400)
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart3_irq.c							# requer vic.c e buffer.c (Somente no LPC2400)
#	SRCARM += $(PATH_ARM_LIBS)/i2c/i2c0.c
#	SRCARM += $(PATH_ARM_LIBS)/spi/spi0.c
#	SRCARM += $(PATH_ARM_LIBS)/adc/adc0.c
#	SRCARM += $(PATH_ARM_LIBS)/timer/timer0.c
#	SRCARM += $(PATH_ARM_LIBS)/timer/timer1.c
#	SRCARM += $(PATH_ARM_LIBS)/timer/sys_time.c							# NÃO USE COM SISTEMA OPERACIONAL
#	SRCARM += $(PATH_ARM_LIBS)/timer/delay_tick.c						# requer sis_time.c caso não é utilizado com nenhum sistema operacional
#	SRCARM += $(PATH_ARM_LIBS)/timer/delay.c
#	SRCARM += $(PATH_ARM_LIBS)/timer/rtc.c								# requer vic.c caso usar interrupção
#	SRCARM += $(PATH_ARM_LIBS)/eint/eint0.c								# requer vic.c
#	SRCARM += $(PATH_ARM_LIBS)/eint/eint1.c								# requer vic.c
#	SRCARM += $(PATH_ARM_LIBS)/eint/eint2.c								# requer vic.c
#	SRCARM += $(PATH_ARM_LIBS)/eint/eint3.c								# requer vic.c
	
# -------------------------------------------------------------------------------
# GERENCIADORES DE ACESSO A RECURSOS PELAS APLICAÇÕES NO FREE RTOS

#	SRCARM += $(PATH_ARM_LIBS)/uart/uart0_task.c						# requer uart0_irq.c e rprintf.c caso usá-lo
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart1_task.c						# requer uart1_irq.c e rprintf.c caso usá-lo
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart2_task.c						# requer uart1_irq.c e rprintf.c caso usá-lo
#	SRCARM += $(PATH_ARM_LIBS)/uart/uart3_task.c						# requer uart1_irq.c e rprintf.c caso usá-lo

# -------------------------------------------------------------------------------
# CLI - INTERPRETADOR DE COMANDOS
	
	SRC += $(PATH_UC_LIBS)/cli/cli.c									# requer syscalls.c

# -------------------------------------------------------------------------------
# MISC & STDIO

	SRC += $(PATH_UC_LIBS)/stdio/rprintf.c
	SRC += $(PATH_UC_LIBS)/misc/buffer.c
#	SRC += $(PATH_UC_LIBS)/misc/buffer_d.c
#	SRC += $(PATH_UC_LIBS)/misc/buffer_word.c

# -------------------------------------------------------------------------------
# SISTEMA DE ARQUIVOS

#	SRC += $(PATH_UC_LIBS)/fs/fat.c										# requer algum driver de dispositivo de armazenamento (mmc ou ata ...)
#	SRC += $(PATH_UC_LIBS)/fs/fat_browser.c								# requer fat.c

# -------------------------------------------------------------------------------
# MEMÓRIAS

#	SRC += $(PATH_UC_LIBS)/memory/i2c_eeprom.c							# requer i2c[x].c
#	SRC += $(PATH_UC_LIBS)/memory/spi_eeprom.c							# requer spi[x].c
#	SRC += $(PATH_UC_LIBS)/memory/mmc.c									# requer mmc_drv.c spi[x].c
#	SRC += $(PATH_UC_LIBS)/memory/portable/arm_lpc/mmc_drv.c

# -------------------------------------------------------------------------------
# SENSORES

#	SRC += $(PATH_UC_LIBS)/sensor/ds1621.c								# requer i2c[x].c

# -------------------------------------------------------------------------------
# DISPLAYS

#	SRC += $(PATH_UC_LIBS)/display/lcd.c								# requer lcd_drv.c e delay.c
#	SRC += $(PATH_UC_LIBS)/display/portable/arm_lpc/lcd_drv.c			# requer delay.c
#	SRC += $(PATH_UC_LIBS)/display/lcd_t6963.c							# requer lcd_t6963_drv.c
#	SRC += $(PATH_UC_LIBS)/display/portable/arm_lpc/lcd_t6963_drv.c		# requer delay.c

# -------------------------------------------------------------------------------
# CRC

#	SRC += $(PATH_UC_LIBS)/crc/rc4.c
#	SRC += $(PATH_UC_LIBS)/crc/crc8.c
#	SRC += $(PATH_UC_LIBS)/crc/checksum.c

# -------------------------------------------------------------------------------
# NICS

#	SRC += $(PATH_UC_LIBS)/nic/enc28j60.c								# requer delay.c ou delay_tick.c
#	SRC += $(PATH_UC_LIBS)/nic/rtl8019.c								# requer rtl8019_drv.c e delay.c
#	SRC += $(PATH_UC_LIBS)/nic/portable/arm_lpc/rtl8019_drv.c

# -------------------------------------------------------------------------------
# UIP

	### UIP BASICO. REQUER PARA TODAS AS APLICAÇÕES UIP
#	SRC += $(PATH_UIP_LIBS)/uip_timer.c									# requer uip_clock_arch.c						# NÃO USE COM SISTEMA OPERACIONAL
#	SRC += $(PATH_UIP_LIBS)/portable/arm_lpc/uip_clock_arch.c									 						# NÃO USE COM SISTEMA OPERACIONAL
#	SRC += $(PATH_UIP_LIBS)/uip_nic.c									# requer ax88796.c ou enc28j60.c ou rtl8019.c
#	SRC += $(PATH_UIP_LIBS)/uip.c										# requer uip_nic.c
#	SRC += $(PATH_UIP_LIBS)/uip_arp.c									# requer uip.c

	### UIP WEBSERVER
#	SRC += $(PATH_UIP_LIBS)/memb.c
#	SRC += $(PATH_UIP_LIBS)/psock.c
#	SRC += $(PATH_UIP_LIBS)/webserver/httpd.c
#	SRC += $(PATH_UIP_LIBS)/webserver/httpd-fs.c
#	SRC += $(PATH_UIP_LIBS)/webserver/httpd-cgi.c
#	SRC += $(PATH_UIP_LIBS)/webserver/http-strings.c

# -------------------------------------------------------------------------------
# FREE_RTOS
####	OBS. O TICK DO SISTEMA UTILIZA O TIMER0 DO ARM E PRIORIDADE 0

#	SRC += $(PATH_FREERTOS_LIBS)/queue.c								# requer heap_x.c
#	SRC += $(PATH_FREERTOS_LIBS)/tasks.c								# requer heap_x.c list.c
#	SRC += $(PATH_FREERTOS_LIBS)/croutine.c								# requer queue.c
#	SRC += $(PATH_FREERTOS_LIBS)/list.c
#	SRC += $(PATH_FREERTOS_LIBS)/portable/arm_$(SUBMDL)/port.c			# requer vic.c. OBS: O TICK do freeRTOS utiliza o timer0 do ARM
#	SRC += $(PATH_FREERTOS_LIBS)/portable/arm_$(SUBMDL)/port_isr.c

	#### SELECIONE UM DOS GERENCIADOR DE MEMÓRIA
#	SRC += $(PATH_FREERTOS_LIBS)/memmang/heap_1.c						# Gerenciador de memória com somente maloc(). Uso ideal para aplicações que cria listas, semafóros e tarefas uma só vez. É afetado pela definição configTOTAL_HEAP_SIZE
#	SRC += $(PATH_FREERTOS_LIBS)/memmang/heap_2.c						# Gerenciador de memória com maloc( e free(). Esse método é usado em aplicações onde as tarefas, filas e semáforos são repetidamente criados e destruídos. É afetado pela definição configTOTAL_HEAP_SIZE
#	SRC += $(PATH_FREERTOS_LIBS)/memmang/heap_3.c						# Este gerenciador utiliza as funções malloc() e free() padrões das bibliotecas LIBC. Este tipo de gerenciamento não é afetado pela definição configTOTAL_HEAP_SIZE, mas sim pelo linker script

# -------------------------------------------------------------------------------
# CODEC JPEG 
		
	### JPEG COMPRESS
#	SRC += $(PATH_UC_LIBS)/jcinit.c		# Initialization: determines which other modules to use
#	SRC += $(PATH_UC_LIBS)/jcmaster.c  	# Master control: setup and inter-pass sequencing logic
#	SRC += $(PATH_UC_LIBS)/jcmainct.c  	# Main buffer controller (preprocessor => JPEG compressor)
#	SRC += $(PATH_UC_LIBS)/jcprepct.c  	# Preprocessor buffer controller
#	SRC += $(PATH_UC_LIBS)/jccoefct.c  	# Buffer controller for DCT coefficient buffer
#	SRC += $(PATH_UC_LIBS)/jccolor.c	# Color space conversion
#	SRC += $(PATH_UC_LIBS)/jcsample.c	# Downsampling
#	SRC += $(PATH_UC_LIBS)/jcdctmgr.c  	# DCT manager (DCT implementation selection & control)
#	SRC += $(PATH_UC_LIBS)/jfdctint.c	# Forward DCT using slow-but-accurate integer method
#	SRC += $(PATH_UC_LIBS)/jfdctfst.c  	# Forward DCT using faster, less accurate integer method
#	SRC += $(PATH_UC_LIBS)/jfdctflt.c	# Forward DCT using floating-point arithmetic
#	SRC += $(PATH_UC_LIBS)/jchuff.c 	# Huffman entropy coding for sequential JPEG
#	SRC += $(PATH_UC_LIBS)/jcphuff.c	# Huffman entropy coding for progressive JPEG
#	SRC += $(PATH_UC_LIBS)/jcmarker.c	# JPEG marker writing
#	SRC += $(PATH_UC_LIBS)/jdatadst.c	# Data destination manager for stdio output
#	SRC += $(PATH_UC_LIBS)/jmemnobs.c 	# Uitiliza a memória principal
		
	### JPEG COMPRESS/DECOMPRESS
#	SRC += $(PATH_UC_LIBS)/jerror.c		# Standard error handling routines (application replaceable).
#	SRC += $(PATH_UC_LIBS)/jmemmgr.c	# System-independent (more or less) memory management code.
#	SRC += $(PATH_UC_LIBS)/jutils.c		# Miscellaneous utility routines.
	
	### JPEG APP
#	SRC += $(PATH_UC_LIBS)/jcapimin.c	# Application program interface: core routines for compression
#	SRC += $(PATH_UC_LIBS)/jcapistd.c	# Application program interface: standard compression.
#	SRC += $(PATH_UC_LIBS)/cdjpeg.c		# Utility routines used by all programs
#	SRC += $(PATH_UC_LIBS)/jcomapi.c	# Application program interface routines common to compression and decompression
#	SRC += $(PATH_UC_LIBS)/jcparam.c	# Compression parameter setting helper routines
#	SRC += $(PATH_UC_LIBS)/rdbmp.c		# BMP file input
	

###########################################################################################################################################
########### ARQUIVOS ASM A SEREM COMPILADOS
########### 	ASRCARM são fontes do prorcessador ARM em assembler
########### 	SSRC são fontes gerais em assembler
###########		ASCRSTARTUP é fonte assembler de inicialização do processador.

	ASRC = 
	ASRCARM = 
	ASCRSTARTUP = build/startup.S

###########################################################################################################################################
########### DEFINIÇÃO DE TODOS OS ARQUIVOS OBJETOS E LISTAS
########### 	OBS: TEM QUE SER LOGO DEPOIS DAS DECLARAÇÕES DAS FONTES. SENÃO AS REGRAS DE COMPILAÇÃO NÃO SERÃO ATUALIZADAS CORRETAMENTE

	# DEFINIÇÃO DE TODOS ARQUIVOS OBJETOS
	COBJ      	= $(SRC:.c=.o) 
	AOBJ      	= $(ASRC:.S=.o)
	COBJARM   	= $(SRCARM:.c=.o)
	AOBJARM   	= $(ASRCARM:.S=.o)
	MAINOBJ		= $(MAINSRC:.c=.o) 
	STARTUPOBJ	= $(ASCRSTARTUP:.S=.o)
	
	LST = $(ASRC:.S=.lst) $(ASRCARM:.S=.lst) $(SRC:.c=.lst) $(SRCARM:.c=.lst) $(MAINSRC:.c=.lst) $(ASCRSTARTUP:.S=.lst)


###########################################################################################################################################
########### TAMANHO DAS STACKS PARA CADA MODO DO PROCESSADOR 	
###########		NÃO PRECISA DEFINIR O TAMANHO DO STACK DO USUÁRIO/SISTEMA, É CALCULADA AUTOMATICAMENTE NO STARTUP
	
	ifeq ($(RTOS), FREE_RTOS)
		UND_STACK_SIZE = 0x4					# Tamanho da STACK para modo indefinido. Caso não usar colocar 0
		SVC_STACK_SIZE = 0x1500					# Tamanho da STACK para modo supervisor. Caso não usar colocar 0
		ABT_STACK_SIZE = 0x4					# Tamanho da STACK para modo aborto. Caso não usar colocar 0
		FIQ_STACK_SIZE = 0x4					# Tamanho da STACK para modo FIQ. Caso não usar colocar 0
		IRQ_STACK_SIZE = 0x1500					# Tamanho da STACK para modo IRQ. Caso não usar colocar 0
	else
		UND_STACK_SIZE = 0x4					# Tamanho da STACK para modo indefinido. Caso não usar colocar 0
		SVC_STACK_SIZE = 0x4					# Tamanho da STACK para modo supervisor. Caso não usar colocar 0
		ABT_STACK_SIZE = 0x4					# Tamanho da STACK para modo aborto. Caso não usar colocar 0
		FIQ_STACK_SIZE = 0x4					# Tamanho da STACK para modo FIQ. Caso não usar colocar 0
		IRQ_STACK_SIZE = 0x100					# Tamanho da STACK para modo IRQ. Caso não usar colocar 0
	endif

###########################################################################################################################################
########### RAM EXTERNA
	
	#### USO DE RAM ESTÁTICA EXTERNA		>> NO_EXT_STATIC_RAM | USE_EXT_STATIC_RAM
	EXT_RAM = NO_EXT_STATIC_RAM

	#### CONFIGURAÇÕES PARA USO DE RAM EXTÁTICA EXTERNA
	PINSEL_ADDR = 0xE002C014					# Endereço do registrador PINSEL2, resposável de configurar os pinos para acesso a RAM externa
	PINSEL_VALUE = 0x0F814920					# Valor a ser escrito no reg PINSEL2. 
														# Habilitado pinos: A[23:0], D[31:0], CS[0..3], WE, OE, BLS[3:0]
														# Desabilitados: Trace port e debug port
		# Bits 	27 	26 	25		P3.23/A23/XCLK and P3.22:2/A2.22:2
		#		1	0	0		A11:2 são linhas de endereços e demais GPIO
		#		1	0	1		A15:2 são linhas de endereços e demais GPIO
		#		1	1	0		A19:2 são linhas de endereços e demais GPIO
		#		1	1	1		A23:2 são linhas de endereços e demais GPIO
		# Bit24: 0 = GPIO / 1 = A1
		# Bit23: 0 = GPIO / 1 = A0
		# Bit3: 0 >> P1.25:16 = GPIO   / 1 >> P1.25:16 = Trace port
		# Bit2: 0 >> P1.36:26 = GPIO   / 1 >> P1.36:26 = Debug port
		
		
	BCFG_BASE = 0xFFE00000						# Endereço do registrador de configuração de modo de acesso a RAM externa
	
	BCFG0_VALUE = 0x1C00FFEF					# Banco 0 (CS0) Endereço entre 0x8000_0000 a 0x80FF_FFFF 
	BCFG1_VALUE = 0x20000420					# Banco 1 (CS1) Endereço entre 0x8100_0000 a 0x81FF_FFFF													
	BCFG2_VALUE = 0x0000FFEF 					# Banco 2 (CS2) Endereço entre 0x8200_0000 a 0x82FF_FFFF 														
	BCFG3_VALUE = 0x0000FBEF				  	# Banco 3 (CS3) Endereço entre 0x8300_0000 a 0x83FF_FFFF 
														
		#	   |-----|------|  |--|--|-------------------------------------|   |------------------|--|---------------|-|-----------| 
		# BITS 	31 30 29 28		27 26 25 24		23 22 21 20		19 18 17 16		15 14 13 12		11 10 9 8		7 6 5 4		3 2 1 0 														
		#		

		# Bits [31:30]: 		Sempre 0
		# Bits [29:28] 	MW:  	00 = Barramento de 8 bits / 01 = Barramento de 16 bits / 10 = Barramento de 32 bits /  11 = Reservado
		# Bit 27 		BM: 	1 = Habilita o banco de memória para BURST ROM 		
		# Bit 26 		WP: 	1 = Proteção de escrita no banco;		
		# Bits [25:16]: 		Não sei e reservado. Colocar 0
		# Bits [15:11] 	WST2: 	Controla o tamanho de acesso para escrita
		# Bit 10 		RBLE: 	Habilita o uso do pinos BLS para acessar bytes em dispositivos não 8 bits. 
								# 0 = para dispositivo de 8 bits  / 1 = para dispositivos de 16 ou 32 bits 
		# Bits [9:5] 	WST1: 	Controla o tamanho de acesso de leitura;
		# Bit 4			    : 	Reservado = 0
		# Bits [3:0] 	IDCY: 	Controla o número mínimo de ciclos ocioso entre as operações de escrita e leitura

		# Valores sugeridos
			# 0x1C00FFEF: Acesso a dispositivo de 16 bits somente de leitura de velocidade lenta: Ex: Flash
				# Bus de 16 bits, acesso com BUSRT, protegido contra gravação, acesso de leitura e esccrita o mais lento possivel
			# 0x10000420: Acesso a dispositivo rápido RW de 16 bits. Ex: Ram estática rápida
				# Bus de 16 bits, Sem BUSRT ROM, sem proteção de escrita, sem intervalo entre escrita e leituras (RAM rápida), Tamnanho escrita = 0, tamanho leitura = 1
			# 0x1000FEFF: Acesso a dispositivo rápido RW de 16 bits. Ex: Ram estática lenta
				# Bus de 16 bits, Sem BUSRT ROM, sem proteção de escrita, sem intervalo entre escrita e leituras (RAM rápida), Tamnanho escrita = 0, tamanho leitura = 1
			# 0x0000FFEF:  Acesso dispositivo lento de 8 bits: Ex: RTL8019
				# Bus 8 bits, sem BUSRT ROM, sem proteção de escrita, tamanho de acesso de leitura e escrita e intervalo entre elas o mais lento possivel
			# 0x20000420: Acesso a dispositivo rápido RW de 32 bits. Ex: Ram estática rápida
				# Bus 32 bits, Sem BUSRT ROM, sem proteção de escrita, sem intervalo entre escrita e leituras (RAM rápida), Tamnanho escrita = 0, tamanho leitura = 1

############################################################################################################################################
########### OPÇÕES DE BIBLIOTECAS
	
	ifeq ($(RTOS), FREE_RTOS)
		MATH_LIB = -lm					# FREE RTOS utiliza
	else
		MATH_LIB = -lm
	endif

############################################################################################################################################
########### FORMATO DE DEPURAÇÃO
########### 	FORMATOS NATIVOS DO ARMGCC -g SÃO dwarf-2 [PADRÃO] or stabs. ARM (extended) COFF REQUER stabs, plus an arm-objcopy run.

#	DEBUG = stabs
	DEBUG = dwarf-2
#	DEBUG =

############################################################################################################################################
########### NIVEL DE OTIMIZAÇÃO, PODE SER [0, 1, 2, 3, S]. 
########### 	0 = turn off optimization. s = optimize for size.
###########		(Note: 3 is not always the best optimization level. See arm-libc FAQ.)

	OPT = s

############################################################################################################################################
########### FLAG DO COMPILADOR PARA AJUSTAR O NIVEL DE PADRÃO DO COMPILADOR C
	
	#####   c89   = "ANSI" C | gnu89 = c89 plus GCC extensions | c99 = ISO C99 standard (not yet fully implemented) | gnu99 = c99 plus GCC extensions
	CSTANDARD = -std=gnu99

############################################################################################################################################
########### CONFIGURAÇÕES DE MODO DE INSTRUÇÕES E PROCESSADORES
	
	#### CFLAGS
	CDEFS =  -D $(RUN_MODE)
	CDEFS += -D $(SIMULATOR)
	CDEFS += -D $(RTOS)
	CDEFS += -D CPU_ARM_LPC
	CDEFS += -D $(SUBMDL)

	#### ASFLAGS
	ADEFS =  -D $(RUN_MODE)
	ADEFS += -D $(SIMULATOR)
	ADEFS += -D $(RTOS)
	ADEFS += -D $(SUBMDL)
	ADEFS += -D $(MY_SYSCALL)
	ADEFS += -D CPU_ARM_LPC
	ADEFS += -D UND_STACK_SIZE=$(UND_STACK_SIZE)
	ADEFS += -D ABT_STACK_SIZE=$(ABT_STACK_SIZE)
	ADEFS += -D FIQ_STACK_SIZE=$(FIQ_STACK_SIZE)
	ADEFS += -D IRQ_STACK_SIZE=$(IRQ_STACK_SIZE)
	ADEFS += -D SVC_STACK_SIZE=$(SVC_STACK_SIZE)
	ADEFS += -D $(EXT_RAM)
	ADEFS += -D PINSEL_ADDR=$(PINSEL_ADDR)
	ADEFS += -D PINSEL_VALUE=$(PINSEL_VALUE)
	ADEFS += -D BCFG_BASE=$(BCFG_BASE)
	ADEFS += -D BCFG0_VALUE=$(BCFG0_VALUE)
	ADEFS += -D BCFG1_VALUE=$(BCFG1_VALUE)
	ADEFS += -D BCFG2_VALUE=$(BCFG2_VALUE)
	ADEFS += -D BCFG3_VALUE=$(BCFG3_VALUE)


############################################################################################################################################
########### OPÇÕES DO ASSEMBLER
###########  	-Wa,...:   tell GCC to pass this to the assembler.
###########  	-ahlms:    create listing
###########  	-gstabs:   have the assembler create line number information; note that
###########             for use in COFF files, additional information about filenames
###########             and function names needs to be present in the assembler source
###########             files -- see arm-libc docs [FIXME: not yet described there]
###########  	-g$(DEBUG): have the assembler create line number information

	ASFLAGS = $(ADEFS)
	ASFLAGS += -Wa, -g$(DEBUG)
	ASFLAGS += -mcpu=arm7tdmi
	ASFLAGS += -I.
	ASFLAGS += -Wa,-adhlns=$(<:.S=.lst),-g$(DEBUG)
	      	    
############################################################################################################################################
########### OPÇÕES DO COMPILADOR C
###########  -g*:          generate debugging information
###########  -O*:          optimization level
###########  -f...:        tuning, see GCC manual and arm-libc documentation
###########  -Wall...:     warning level
###########  -Wa,...:      tell GCC to pass this to the assembler.
###########    -adhlns...: create assembler listing
	
	CFLAGS = -mcpu=arm7tdmi
	CFLAGS += -g$(DEBUG)
	CFLAGS += -O$(OPT)
	CFLAGS += $(CDEFS)
	CFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) 
	CFLAGS += $(CSTANDARD)
		
	CFLAGS += -I.
	CFLAGS += -I $(PATH_ARM_LIBS)
	CFLAGS += -I $(PATH_ARM_LIBS)/commom
	CFLAGS += -I $(PATH_ARM_LIBS)/uart
	CFLAGS += -I $(PATH_ARM_LIBS)/adc
	CFLAGS += -I $(PATH_ARM_LIBS)/eint
	CFLAGS += -I $(PATH_ARM_LIBS)/timer
	CFLAGS += -I $(PATH_ARM_LIBS)/spi
	CFLAGS += -I $(PATH_ARM_LIBS)/i2c
	CFLAGS += -I $(PATH_UC_LIBS)/_libdef
	CFLAGS += -I $(PATH_UC_LIBS)/misc
	CFLAGS += -I $(PATH_UC_LIBS)/fs
	CFLAGS += -I $(PATH_UC_LIBS)/cli
	CFLAGS += -I $(PATH_UC_LIBS)/crc
	CFLAGS += -I $(PATH_UC_LIBS)/sensor
	CFLAGS += -I $(PATH_UC_LIBS)/stdio
	CFLAGS += -I $(PATH_UC_LIBS)/memory
	CFLAGS += -I $(PATH_UC_LIBS)/memory/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/display
	CFLAGS += -I $(PATH_UC_LIBS)/display/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/nic
	CFLAGS += -I $(PATH_UC_LIBS)/nic/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/keyboard
	CFLAGS += -I $(PATH_UIP_LIBS)
	CFLAGS += -I $(PATH_UIP_LIBS)/portable/arm_lpc
	CFLAGS += -I $(PATH_UIP_LIBS)/webserver
	CFLAGS += -I $(PATH_FREERTOS_LIBS)/include
	CFLAGS += -I $(PATH_FREERTOS_LIBS)/portable/arm_$(SUBMDL)
	CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
	
	#	-Wall					# Habilita todos os avisos mais uteis para todas as circuntâncias
	
	# ( Ideal para novos projetos )
	#	-Wshadow 				# Avisa se uma variavel esta espelhada em outro lugar
	#	-Wpointer-arith 		# Avisa sobre qualquer coisa que depende  do "size of" do tipo de função ou do void
	#	-Wcast-qual 			# Avisa sempre que um ponteiro venha a remover o qualificador do tipo do destino. Ex (const char*) para (char*)
	#	-Wcast-align 			# Avisa sempre que um ponteiro seja requiro a ser alinhado com o tipo do destino. Ex (char*) para (int*)
	#	-Wstrict-prototypes		# Avisa se uma função foi declarada ou definida sem especificar os tipos dos argumentos
	
	# ( opcionais )
	#	-Wundef					# Diretivas que não foram declaradas Ex: #if DERITIVA 
	#	-Wimplicit 				# Avisa quando uma declaração não especifica um tipo
	#	-Wmissing-declarations	# Avisa se uma função global foi definida sem uma declaração prévia
	#	-Wmissing-prototypes	# Avisa se uma função global foi definida sem uma declaração prévia do protótipo
	#	-Wnested-externs 		# Avisa se uma declaração "extern" é encontrada sem uma função
	#	-Wswitch				# Avisar sempre que uma declaração switch tem um índice de tipo enumerado e carece de um caso para um ou mais dos códigos de que o chamado enumeração
	#	-Wredundant-decls 		# Avisa se existir declarações redundantes
	#	-Wreturn-type 
	#	-Wunused	
	#	-Wunreachable-code		# Avisa quando trechos ou códigos que nunca serão executados
	#	-Wsign-compare			# Avisa quando existir uma comparação entre signed e unsigned
	#	-fpack-struct 			# Empacota todas as estruturas juntas sem espaçamento. O código binário gerado é incopativel com outros binários. Não utilizar como padrão
	#	-funsigned-char 		# Permite que o tipo char seja como unsigned char. Não utilizar como padrão
	#	-funsigned-bitfields 	# Um bit é considerado como unsigned. Não utilizar como padrão
	#	-fshort-enums 			# Enumeradores com menores bytes alocado. Incopativel com outros binários. Não utilizar como padrão
	# 	-Wbad-function-cast		# Avisa quando uma chamada de função tem (cast) de diferente tipo

	CFLAGS += -Wall
	CFLAGS += -Wpointer-arith -Wbad-function-cast -Wshadow 
	CFLAGS += -Wimplicit
	CFLAGS += -Wsign-compare -Wextra
	CFLAGS += -Wswitch -Wreturn-type -Wunused
	CFLAGS += -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations

	ifeq ($(TCP_STACK), TCP_STACK_NONE)
	 	CFLAGS += -Wcast-align 
	endif
		
	ifneq ($(RTOS), FREE_RTOS)
		ifeq ($(TCP_STACK), TCP_STACK_NONE)
	 		CFLAGS += -Wcast-qual
		endif

		CFLAGS += -Wnested-externs -Wredundant-decls
	else		
		CFLAGS += -fno-strict-aliasing
	endif

###########################################################################################################################################
########### FLAGS DO LINKADOR
###########		-nostartfiles:	Diz ao GCC para não usar o startup padrão
###########  		-Wl,...:		Diz ao GCC para passar os argumento abaixo ao linker
###########    		-Map:      	Cria o mapa da memória
###########    		--cref:    	Adiciona a referencia cruzada ao mapa de memória

	# BUSCA O LINKER SCRIPT DEPENDENDO DO TIPO DE SELEÇÃO DA MEMÓRIA
	ifeq ($(RUN_MODE),RAM_RUN)
		LDSCRIPT = build/$(SUBMDL)-RAM.ld
	else 
		LDSCRIPT = build/$(SUBMDL)-ROM.ld
	endif

	#LDFLAGS = -T$(LDSCRIPT)
	LDFLAGS = -T$(LDSCRIPT) -Xlinker -o$(TARGET).elf
	LDFLAGS += -nostartfiles -Wl,-Map=$(TARGET).map,--cref,-nostdlib
	LDFLAGS += $(MATH_LIB)

############################################################################################################################################
############# ALVO PADRÃO PARA COMPILAÇÃO
all: begin build size end ok

#### o build foi arranjado dessa maneira para que possamos usar funções newlib sem necessitar usar um compilador não reentrante, 
####	subistituindo somente algumas funções do newlib tais como _open, _exit, _close
build: $(COBJ) $(AOBJ) $(COBJARM) $(AOBJARM) elf hex bin lss sym
hex: $(TARGET).hex
bin: $(TARGET).bin
elf: $(TARGET).elf
lss: $(TARGET).lss 
sym: $(TARGET).sym

LIBNAME=$(TARGET)_lib.a
lib: begin build_lib end ok_lib
build_lib: $(LIBNAME)

############################################################################################################################################
########## EXIBE INFORMAÇÕES DAS SECÇÕES DE MEMÓRIA

dump:
	@echo
	@$(OBJDUMP) -h $(TARGET).elf
	
############################################################################################################################################
########## DISPLAY COMPILER VERSION INFORMATION

gccversion : 
	@$(CC) --version

############################################################################################################################################
########## APAGA ARQUIVOS COMPILADOS

c: clean
clean: begin clean_list finished end 

clean_list:
	@echo
	@echo $(MSG_CLEANING)
	@$(REMOVE) $(COBJ)
	@$(REMOVE) $(AOBJ)
	@$(REMOVE) $(STARTUPOBJ)
	@$(REMOVE) $(COBJARM)
	@$(REMOVE) $(AOBJARM)
	@$(REMOVE) $(SRC:.c=.s)
	@$(REMOVE) $(SRC:.c=.d)
	@$(REMOVE) $(MAINSRC:.c=.d)
	@$(REMOVE) $(SRCARM:.c=.s)
	@$(REMOVE) $(SRCARM:.c=.d)
	@$(REMOVE) $(LST)
	@$(REMOVE) $(TARGET).map
	@$(REMOVE) $(TARGET).a
	@$(REMOVE) $(TARGET).o
	@$(REMOVE) $(TARGET).elf
	@$(REMOVE) $(TARGET).obj
	@$(REMOVE) $(TARGET).hex
	@$(REMOVE) $(TARGET).bin
	@$(REMOVE) $(TARGET).a90
	@$(REMOVE) $(TARGET).sym
	@$(REMOVE) $(TARGET).lnk
	@$(REMOVE) $(TARGET).lss
#	@$(REMOVE) .dep/*
	@$(REMOVEDIR) .dep
	@$(REMOVE) $(LIBNAME)

############################################################################################################################################
########### MOSTRA OS TAMANHOS DO ARQUIVO

HEXSIZE = $(SIZE) --target=ihex $(TARGET).hex
ELFSIZE1 = $(SIZE) -x -A $(TARGET).elf
ELFSIZE2 = $(SIZE) -x -B $(TARGET).elf

size:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE); $(ELFSIZE1); 2>/dev/null; echo; fi
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE); $(ELFSIZE2); 2>/dev/null; echo; fi

############################################################################################################################################
############ EXIBE MENSAGENS DE INCIO E FIM DA COMPILAÇÃO

begin:
	@echo
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo
	@echo $(MSG_END)
	@echo

ok:
	@echo
	@echo $(MSG_ERRORS_NONE)

ok_lib:
	@echo
	@echo $(MSG_ERRORS_NONE)


#############################################################################################################################################
########### DEFINIÇÃO DE MENSAGENS

MSG_LPC21_RESETREMINDER = "You may have to bring the target in bootloader-mode now."
MSG_ERRORS_NONE = "ERRORS: NONE"
MSG_BEGIN = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  BEGIN  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
MSG_END = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   END   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
MSG_SIZE = "SIZE:"
MSG_FLASH = "CREATING LOAD FILE FOR FLASH:"
MSG_EXTENDED_LISTING = "CREATING EXTENDED LISTING:"
MSG_SYMBOL_TABLE = "CREATING SYMBOL TABLE:"
MSG_LINKING = "LINKING:"
MSG_COMPILING = "CC:"
MSG_COMPILING_ARM = "CC ARM:"
MSG_ASSEMBLING = "AS:"
MSG_ASSEMBLING_ARM = "AS ARM:"
MSG_CLEANING = "CLEANING PROJECT:"
MSG_FORMATERROR = "Can not handle output-format"
MSG_CREATING_LIBRARY = "CREATING LIBRARY:"

###########################################################################################################################################
########### DEFINIÇÃO DE COMANDOS E PROGRAMAS	

SHELL 		= sh
AS			= arm-elf-gcc
AR			= arm-elf-ar
CC 			= arm-elf-gcc
OBJCOPY 	= arm-elf-objcopy
OBJDUMP 	= arm-elf-objdump
SIZE 		= arm-elf-size
NM 			= arm-elf-nm
RN			= mv
REMOVE 		= rm -f
REMOVEDIR 	= rm -rf
COPY 		= cp
	
############################################################################################################################################
##########* REGRAS IMPLICITAS

# 	LINK: CRIA O EXECUTAVEL ELF DOS ARQUIVOS OBJETOS
# COLOCAMOS O MAIN SEPARADO PARA QUE O elf TENHA ALGUMA DEPENDECIA E TAMBÉM PELO MESMO MOTIVO MENECIONADOS DO build:
#		TAMBÉM O STARTUP ESTÁ SEPARADO PARA QUE O LINKADO ALOQUE CORRETAMENTE ELE AO PROGRAMA
#.SECONDARY : $(TARGET).elf
#.PRECIOUS : $(MAINOBJ) $(COBJ) $(AOBJ) $(COBJARM) $(AOBJARM) $(TARGET).a $(STARTUPOBJ) $(LDSCRIPT)
#%.elf: $(MAINOBJ) $(COBJ) $(AOBJ) $(COBJARM) $(AOBJARM) $(TARGET).a $(STARTUPOBJ) $(LDSCRIPT)
#	@echo
#	@echo $(MSG_LINKING) $@
#	@$(CC) $(CFLAGS) $(MAINOBJ) $(TARGET).a $(STARTUPOBJ) $(LDFLAGS) --output $@

.SECONDARY : $(TARGET).elf
.PRECIOUS : $(MAINOBJ) $(COBJ) $(AOBJ) $(COBJARM) $(AOBJARM) $(TARGET).a $(STARTUPOBJ) $(LDSCRIPT)
$(TARGET).elf :	 $(MAINOBJ) $(COBJ) $(AOBJ) $(COBJARM) $(AOBJARM) $(TARGET).a $(STARTUPOBJ) $(LDSCRIPT)
%.elf: 
	@echo
	@echo $(MSG_LINKING) $@
	@$(CC) $(CFLAGS) $(MAINOBJ) $(STARTUPOBJ) $(TARGET).a $(LDFLAGS)

# CREATE LIBRARY FROM OBJECT FILES.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(MAINOBJ)
%.a: $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(MAINOBJ)
	@echo $(MSG_CREATING_LIBRARY) $@
	@$(AR) -rcs $@ $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(MAINOBJ)

# COMPILA O PROGRAMA MAIN
$(MAINOBJ) : %.o : %.c
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(CFLAGS) $< -o $@

# ASSEMBLE: CREATE OBJECT FILES FROM ASSEMBLER SOURCE FILES. ARM-ONLY
$(STARTUPOBJ): %.o : %.S
	@echo $(MSG_ASSEMBLING_ARM) $<
	@$(AS) -c $(ASFLAGS) $< -o $@
	
# COMPILE: CRIA ARQUIVOS OBJETOS DOS ARQUIVOS FONTES C
$(COBJ) : %.o : %.c
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(CFLAGS) $< -o $@ 
	@$(AR) rc $(TARGET).a $@
	
# COMPILE: CREATE OBJECT FILES FROM C SOURCE FILES. ARM-ONLY
$(COBJARM) : %.o : %.c
	@echo $(MSG_COMPILING_ARM) $<
	@$(CC) -c $(CFLAGS) $< -o $@ 
	@$(AR) rc $(TARGET).a $@

# ASSEMBLE: CRIA ARQUIVOS OBJETOS DAS FONTES EM ASSEMBLER
$(AOBJ) : %.o : %.S
	@echo $(MSG_ASSEMBLING) $<
	@$(AS) -c $(ASFLAGS) $< -o $@
	@$(AR) rc $(TARGET).a $@

# ASSEMBLE: CREATE OBJECT FILES FROM ASSEMBLER SOURCE FILES. ARM-ONLY
$(AOBJARM) : %.o : %.S
	@echo $(MSG_ASSEMBLING_ARM) $<
	@$(AS) -c $(ASFLAGS) $< -o $@
	@$(AR) rc $(TARGET).a $@

# CRIA O ARQUIVO DE SAIDA FINAL (.HEX, .EEP) DO ARQUIVO DE SAIDA ELF
%.hex: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O ihex $< $@
	
# CREATE FINAL OUTPUT FILE (.BIN) FROM ELF OUTPUT FILE.
%.bin: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O binary $< $@

# CREATE EXTENDED LISTING FILE FROM ELF OUTPUT FILE.
%.lss: %.elf
	@echo $(MSG_EXTENDED_LISTING) $@
	@$(OBJDUMP) -h -S -C $< > $@

# CREATE A SYMBOL TABLE FROM ELF OUTPUT FILE.
%.sym: %.elf
	@echo $(MSG_SYMBOL_TABLE) $@
	@$(NM) -n $< > $@

#############################################################################################################################################
########### LISTING OF PHONY TARGETS

.PHONY : asm bin all begin finished end size ok ok_lib lib \
build build_lib elf hex eep lss sym \
clean c clean_list
