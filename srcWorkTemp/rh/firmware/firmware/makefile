# MAKEFILE PARA AMBIENTE CYGWIN - Atualizado 2016-08-12
# =====================================================

#############################################################################################################################################
# Linhas de comandos do MAKE
#
# make all ou make		= Compila o software
# make dump				= Exibe informações das secções de memória
# make lib				= Cria a biblioteca estática (.a) das bibliotecas usadas pelos software
# make clean ou c 		= Limpa o software e biblioteca estática (.a) compilados
# make v 				= Exibe a versão do compilador
#############################################################################################################################################

##########################################################################################################################################
########### ESPECEFICAR O TIPO DE COMPILADOR

	####	arm-elf 				= Usar o compilador GNUARM 
	####    arm-none-eabi 			= Usar o compilador YARGATO
	####    arm-eabi 				= Usar o compilador gcc arm no linux
	####	arm-none-linux-gnueabi	= Usar o compilador DS5 GNU ARM
	COMPILER_TYPE = arm-none-eabi

############################################################################################################################################
########### AJUSTAR OS LOCAIS DAS BIBLIOTECAS UC_LIBC E USO DAS BIBLIOTECAS NATIVAS DO COMPILADOR

	##### LOCAL DAS BILIOTECAS UC_LIBC. A pasta da biblioteca uc_libc deve estar junto com o programa main 		
	#####	uc_libc: Copiar as bibliotecas uc_Libc do local de desenvolvimento, D:\_libs\uc_libc\uc_libcX.XX,  para o pasta do projeto.
				#### verifique neste makefile quais as fontes que estão sendo usado para ser compiadas para uc_libc 
	#####	uc_libc_dev: Link simbólico para a D:\_libs\uc_libc\uc_libcX.XX para desenvovimento do projeto e da uc_libc.
				#### Utilize o aplicativo MuxTools para criar o link simbólico
	PATH_UC_LIBS 			= uc_libc_dev
	
	PATH_ARM_LIBS			= $(PATH_UC_LIBS)/_arch/arm_lpc
	PATH_UIP_LIBS			= $(PATH_UC_LIBS)/uip/uip-v1.0-b
	PATH_FREERTOS_LIBS		= $(PATH_UC_LIBS)/free_rtos/v8.2.3
	PATH_EMWIN_LIBS			= $(PATH_UC_LIBS)/graphic/emWin/v5.30

	##### CONFIGURAÇÕES DAS BIBLIOTECAS
	#####  -lgcc	= Biblioteca libgcc.a e contém subrotinas internas que o GCC utiliza para certos tipos de processadores ou linguagens
	#####				Como suporte a ponto flutualte e função extendidas
	#####  -lc 		= Biblioteca newlib libc.a. Funções do tipo isdigit() sprintf()
	#####  -lm		= Biblioteca newlib matemática libm.a. Funções do tipo exp(), sin()
	LDFLAGS = -lm -lgcc -lc
	
	##### CONFIGURAÇÕES DOS LOCAIS ONDE OS ARQUIVOS EXECUTÁVEIS E OBJETOS SERÃO GRAVADOS
	DIR_OBJ = obj
	DIR_BIN = bin

###########################################################################################################################################
########### TIPO DE PROCESSADOR E OPÇÕES DE COMPILAÇÃO
	
	#### TIPO DO ARM
	#### 	LPC2101 | LPC2102 | LPC2103 | LPC2214 | LPC2294 | LPC2468 
	#### 	KIT_ARMVIDEO_7MHZ: Kit usando ARM lpc2468 com cristal de 7.3728MHZ
	#### 	KIT_ARMVIDEO_12MHZ: Kit usando ARM lpc2468 com cristal de 12MHZ para usar o barrameto USB
	####   	KIT_LPC1788_PR111: Kit usando ARM lpc1788 com cristal de 12MHZ para usar o barrametos USB
	####	KIT_LPC1788_PR212: Kit usando ARM lpc1788 com cristal de 12MHZ para usar o barrametos USB
	####	KIT_LPC1751_PR102: Kit usando ARM lpc1751 com cristal de 10MHZ
	####	KIT_LPC1752_PR102: Kit usando ARM lpc1752 com cristal de 10MHZ
	####	MOD_LPC1751_MD100: 	Modulo md100 usando ARM lpc1751 com cristal de 10MHZ
	####	MOD_LPC1751_MD100_RC: Modulo usando ARM lpc1751 usando o oscilador interno de 4MHz
	####	MOD_LPC1752_MD100: Modulo usando ARM lpc1752 com cristal de 10MHZ
	####	MOD_LPC1752_MD100_RC: Modulo usando ARM lpc1752 usando o oscilador interno de 4MHz
	####	WORKTEMP: WorkTemp com ARM lpc1752 usando o oscilador interno de 4MHz
	####	MULTIMETER_vXY: Multimetro com ARM lpc1752 usando o oscilador interno de 4MHz. XY versão multiplicado por 10. Ex: MULTIMETER_v10 = versão 1.0
	####	HARDWARE_RESOURCES_hrXY: Recurso de hardware usando ARM lpc1752 com cristal de 10MHZ. XY versão do apareho
	ARCH = HARDWARE_RESOURCES_hr01
	
	#### UTILIZAÇÃO DE SIMULADOR 		
	####	SIM_NONE 	= Sem uso de recursos de simulação
	####	SIM_SKYEYE 	= Usando o simulador SKYEYE
	####	SIM_PROTEUS = Usando o simulador proteus -  Alguns códigos não funciona no Proteus, então é preciso sinalizar as bibliotecas para gerenciar esses erros
														#### Ex: Uart com IRQ trava, cli não funciona com certos tipos de variaáveis
 	SIMULATOR = SIM_NONE 
	
	#### UTILIZAÇÃO DE SISTEMA OPERACIONAL
	####	RTOS_NONE 		= As bibliotecas não vão rodar em cima de um sistema operacional
	####	RTOS_UCLINUX 	= Configura as bibliotecas para criar o bootloader para o uClinux
	####	FREE_RTOS		= Configura as bibliotecas para rodar com o FREE RTOS
	RTOS = RTOS_NONE

	####  UTILIZAÇÃO DE PILHA TCP/IP	
	####  	TCP_STACK_NONE = Não usar pilha TCP/IP
	####  	TCP_STACK_UIP  = Usar pilha TCP/IP UIP
	TCP_STACK = TCP_STACK_NONE
	
	#### CREATE IMAGE 					
	####	RUN_MODEBOOT1 - O código será compilado para rodar no final da memória FLASH 
	####						e usando os vetores de interrupções no início da memória FLASH  
	####						o STACK POINT será compensado para usar as funções IAP do bootROM
	####	RUN_MODEBOOT2 - O código será compilado para rodar no final da memória FLASH 
	####						e usando os vetores de interrupções no início da memória SRAM interna
	####						o STACK POINT será compensado para usar as funções IAP do bootROM
	####						ATENÇÃO. O compilador ainda aloca os vetores de interrupções na memória FLASH
	####							No próprio bootloader transferimos o conteúdo dos vetores para a SRAM e
	####								depois chaveamos para SRAM (MEMMAP=2)
	####	RUN_MODE1 (MODE_ROM) - O código será compilado para rodar no início da memória FLASH interna do ARM
	####						e usando os vetores de interrupções também no início da memória  FLASH  
	####	RUN_MODE2 	- O código será compilado para rodar no início da memória FLASH interna do ARM
	####						e usando os vetores de interrupções no início da memória SRAM interna
	####						PORÉM, este código deve ser carregado pelo bootloader, porque é preciso fazer um tratamento 
	####							nos vetores de interrupções antes de rodar o código. 
	####							Pois, o compilador ainda aloca os vetores de interrupções na memória FLASH
	####	RUN_MODE3 (MODE _RAM)	- O código será compilado para rodar no início da  memória SRAM interna do ARM e 
	####						e usando os vetores de interrupções também no início da  memória SRAM
	RUN_MODE = RUN_MODE1
		
	#### USO DE RAM EXTERNA NA PLACA. AS CONFIGURAÇÕES ABAIXO ESTÃO mem_map.h 	
	####	NO_USE_EXT_SRAM 	= Não vai ser usado acesso as SRAMs ou dispositivos IOMAP externos
	####	USE_EXT_SRAM		= Configura o barramento do ARM e as bibliotecas para acessar as SRAMs ou dispositivos IOMAP externos
	####	NO_USE_EXT_SDRAM 	= Não vai ser usado acesso as SDRAMs externas
	####	USE_EXT_SDRAM		= Configura o barramento do ARM e as bibliotecas para acessar as SDRAMs externas
	EXT_SRAM = NO_USE_EXT_SRAM
	EXT_SDRAM = NO_USE_EXT_SDRAM

	#### FORMATO DE DEPURAÇÃO
	####	g
	####	gstabs  = ARM (extended) o arquivo COFF requer este formato
	####	gdwarf-2 = Este é o padrão para o todos os GCC para o ARM
	DEBUG = gdwarf-2
	
	#### NIVEL DE OTIMIZAÇÃO 
	####    <<< ATENÇÃO >>> Para depuração o GBD pode não funcionar corretamente com otimização 0
	#### 	0 = Desliga a otimização
	####	1 a 3: Níveis de otimização, quanto mais alto mais otimizado. Nem sempre o valor 3 é o melhor nível de otimização
	####	s = Otimização no tamanho do código
	OPT =  s
			
	#### FLAG DO COMPILADOR PARA AJUSTAR O NIVEL DE PADRÃO DO COMPILADOR C
	####   	c89   	= "ANSI" C 
	####   	gnu89 	= c89 plus GCC extensions 
	####   	c99 	= ISO C99 standard (not yet fully implemented) 
	####   	gnu99 	= c99 plus GCC extensions
	####   	c89   	= "ANSI" C | gnu89 = c89 plus GCC extensions | c99 = ISO C99 standard (not yet fully implemented) | gnu99 = c99 plus GCC extensions
	CSTANDARD = -std=gnu89

###########################################################################################################################################
########### DEFININDO AUTOMATICAMENTE O TIPO DE CPU (NÃO MEXER AQUI)
	
	DEVICES_LPC1788_XTAL = "KIT_LPC1788_PR111" "KIT_LPC1788_PR212" "LPC1788"
	DEVICES_LPC1751_XTAL = "MOD_LPC1751_MD100" "KIT_LPC1751_PR102" "LPC1751"
	DEVICES_LPC1751_RC = "MOD_LPC1751_MD100_RC"
	DEVICES_LPC1752_XTAL = "MOD_LPC1752_MD100" "KIT_LPC1752_PR102" "LPC1752" "HARDWARE_RESOURCES_hr01"
	DEVICES_LPC1752_RC = "MOD_LPC1752_MD100_RC" "WORKTEMP" "MULTIMETER_v10" "MULTIMETER_v11"
	DEVICES_LPC2468_XTAL = "KIT_ARMVIDEO_7MHZ" "KIT_ARMVIDEO_12MHZ"
		
	ifeq ($(findstring $(ARCH), $(DEVICES_LPC1788_XTAL)), $(ARCH))
		SUBMDL = LPC1788
		CORE = cortexm3
		OSC = OSC_XTAL
	else ifeq ($(findstring $(ARCH), $(DEVICES_LPC1751_XTAL)), $(ARCH))
		SUBMDL = LPC1751
		CORE = cortexm3
		OSC = OSC_XTAL
	else ifeq ($(findstring $(ARCH), $(DEVICES_LPC1751_RC)), $(ARCH))	
		SUBMDL = LPC1751
		CORE = cortexm3
		OSC = OSC_RC
	else ifeq ($(findstring $(ARCH), $(DEVICES_LPC1752_XTAL)), $(ARCH))
		SUBMDL = LPC1752
		CORE = cortexm3
		OSC = OSC_XTAL
	else ifeq ($(findstring $(ARCH), $(DEVICES_LPC1752_RC)), $(ARCH))
		SUBMDL = LPC1752
		CORE = cortexm3
		OSC = OSC_RC
	else ifeq ($(findstring $(ARCH), $(DEVICES_LPC2468_XTAL)), $(ARCH))
		SUBMDL = LPC2468
		CORE = arm7tdmi
		OSC = OSC_XTAL
	else 
		SUBMDL = $(ARCH)
		CORE = arm7tdmi
		OSC = OSC_XTAL
	endif

###########################################################################################################################################
#### TAMANHO DAS STACKS
####	Não precisa definir o tamanho do stack do usuário/sistema, é calculada automaticamente no startup
	
	ifeq ($(RTOS), FREE_RTOS) ## ATENÇÃO: Para o cortex-M3 ignorar estes STACK_SIZEs
		UND_STACK_SIZE = 0x4					# Tamanho da STACK para modo indefinido. Caso não usar colocar 0
		SVC_STACK_SIZE = 0x1800					# Tamanho da STACK para modo supervisor. Caso não usar colocar 0
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
########### ARQUIVOS FONTES EM C A SEREM COMPILADOS
########### 	As dependencias são geradas automaticamente
########### 	SRC são as fontes da aplicação
########### 	SRCLIB são as fontes das bibliotecas uc_libc
########### 	SRCARM são as fontes das bibliotecas que lidam diretamente com o processador ARM
	
	TARGET  = main
	
	SRC = $(TARGET).c
	SRC += app.c
				
	# -------------------------------------------------------------------------------
	# FONTES OBRIGATÓRIAS
	SRCARM  = $(PATH_ARM_LIBS)/common/arm.c
	ifneq ($(CORE),  arm7tdmi) 
	SRCARM += $(PATH_ARM_LIBS)/common/startup/startup.c  		# familia cortex
	endif	
	SRCARM += $(PATH_ARM_LIBS)/common/vic_$(CORE).c

	# -------------------------------------------------------------------------------
	# DISPOSITIVOS ARM
	
	SRCARM += $(PATH_ARM_LIBS)/timer/systick.c
	SRCARM += $(PATH_ARM_LIBS)/timer/delay.c

	# -------------------------------------------------------------------------------
	# MISC & STDIO
	SRCLIB += $(PATH_UC_LIBS)/stdio/stdio_uc.c
	SRCLIB += $(PATH_UC_LIBS)/misc/buffer.c
	SRCLIB += $(PATH_UC_LIBS)/misc/utils.c

	# -------------------------------------------------------------------------------
	# MODBUS
	SRC += modbus.c
	SRCLIB += $(PATH_UC_LIBS)/crc/crc.c
	SRCLIB += $(PATH_UC_LIBS)/protocol/modbus/modbus_master.c	# requer now do systime.c ou do rtc, requer crc.c
	SRCLIB += $(PATH_UC_LIBS)/protocol/modbus/modbus_slave.c	# requer now do systime.c ou do rtc, requer crc.c
	SRCARM += $(PATH_ARM_LIBS)/uart/uart1_irq.c				# requer buffer
	SRCARM += $(PATH_ARM_LIBS)/uart/uart3_irq.c				# requer buffer
	
	# -------------------------------------------------------------------------------
	# PROMPT
	SRC += prompt.c
	SRCLIB += $(PATH_UC_LIBS)/cli/cli.c							# requer stdio_uc.c
	SRCARM += $(PATH_ARM_LIBS)/uart/uart0_irq.c

			
###########################################################################################################################################
########### ARQUIVOS ASM A SEREM COMPILADOS
########### 	ASCR são as fontes das demais bibliotecas
########### 	ASRCARM são as fontes das bibliotecas que lidam diretamente com o processador ARM

	ASRC = 
	
	ifeq ($(CORE),  arm7tdmi)
	ASRCARM = $(PATH_ARM_LIBS)/common/startup/vectors_$(CORE).S 	# OBRIGATÓRIO PARA O ARM7TDMI
	ASRCARM += $(PATH_ARM_LIBS)/common/startup/startup.S						# OBRIGATÓRIO
	endif
	
############################################################################################################################################
########### CONFIGURAÇÕES PARA A APLICAÇÃO E DA UC_LIBC
	
	#### ASFLAGS
	ADEFS =  -D $(RUN_MODE)
	ADEFS += -D $(SIMULATOR)
	ADEFS += -D $(RTOS)
	ADEFS += -D $(ARCH)
	ADEFS += -D $(SUBMDL)
	ADEFS += -D $(CORE)
	ADEFS += -D $(OSC)
	ADEFS += -D CPU_ARM_LPC
	ADEFS += -D $(EXT_SRAM)
	ADEFS += -D $(EXT_SDRAM)
	ADEFS += -D ASM_CODE
	ADEFS += -D UND_STACK_SIZE=$(UND_STACK_SIZE)
	ADEFS += -D ABT_STACK_SIZE=$(ABT_STACK_SIZE)
	ADEFS += -D FIQ_STACK_SIZE=$(FIQ_STACK_SIZE)
	ADEFS += -D IRQ_STACK_SIZE=$(IRQ_STACK_SIZE)
	ADEFS += -D SVC_STACK_SIZE=$(SVC_STACK_SIZE)
	
	ifeq ($(COMPILER_TYPE), arm-none-eabi)
	ADEFS += -D COMPILER_TYPE_YAGARTO
	endif


	#### CFLAGS
	CDEFS =  -D $(RUN_MODE)
	CDEFS += -D $(SIMULATOR)
	CDEFS += -D $(RTOS)
	CDEFS += -D CPU_ARM_LPC
	CDEFS += -D $(ARCH)
	CDEFS += -D $(SUBMDL)
	CDEFS += -D $(CORE)
	CDEFS += -D $(OSC)
	CDEFS += -D $(EXT_SRAM)
	CDEFS += -D $(EXT_SDRAM)
	CDEFS += -DC_CODE
	
	ifeq ($(COMPILER_TYPE), arm-none-eabi)
	CDEFS += -D COMPILER_TYPE_YAGARTO
	endif
	
############################################################################################################################################
########### OPÇÕES DO ASSEMBLER
###########  	-Wa,...:   tell GCC to pass this to the assembler.
###########  	-ahlms:    create listing
###########  	-gstabs:   have the assembler create line number information; note that
###########             for use in COFF files, additional information about filenames
###########             and function names needs to be present in the assembler source
###########             files -- see arm-libc docs [FIXME: not yet described there]
###########  	-$(DEBUG): have the assembler create line number information

	ifeq ($(CORE), cortexm3)	
	ASFLAGS = -mcpu=cortex-m3
	ASFLAGS += -mthumb
	else
	ASFLAGS = -mcpu=arm7tdmi
	endif

	ASFLAGS += $(ADEFS)
	ASFLAGS += -$(DEBUG)
	ASFLAGS += -I.
	ASFLAGS += -Wa,-adhlns=$(<:%.S=$(DIR_OBJ)/%.lst)
	ASFLAGS += -I $(PATH_ARM_LIBS)
	ASFLAGS += -I $(PATH_UC_LIBS)/_libdef
	      	    
############################################################################################################################################
########### OPÇÕES DO COMPILADOR C
###########	-g*:          generate debugging information
########### -O*:          optimization level
########### -f...:        tuning, see GCC manual and arm-libc documentation
########### -Wall...:     warning level
########### -Wa,...:      tell GCC to pass this to the assembler.
########### -adhlns...: create assembler listing
###########	suffix: traz o sufixo do nome do arquivo. Ex main.c traz c
###########	subst XX, YY, KK: Substitui as ocorrencia XX, pela acorrencia YY, no arquivo KK

	ifeq ($(CORE), cortexm3)	
	CFLAGS = -mcpu=cortex-m3
	CFLAGS += -mthumb
	else
	CFLAGS = -mcpu=arm7tdmi
	endif

	CFLAGS += -$(DEBUG)
	CFLAGS += -O$(OPT)
	CFLAGS += $(CDEFS)
	CFLAGS += -Wa,-adhlns=$(<:%.c=$(DIR_OBJ)/%.lst)
	CFLAGS += $(CSTANDARD)
		
	CFLAGS += -I.
	CFLAGS += -I $(PATH_ARM_LIBS)
	CFLAGS += -I $(PATH_ARM_LIBS)/common
	CFLAGS += -I $(PATH_ARM_LIBS)/uart
	CFLAGS += -I $(PATH_ARM_LIBS)/analogic
	CFLAGS += -I $(PATH_ARM_LIBS)/display
	CFLAGS += -I $(PATH_ARM_LIBS)/driver
	CFLAGS += -I $(PATH_ARM_LIBS)/eint
	CFLAGS += -I $(PATH_ARM_LIBS)/timer
	CFLAGS += -I $(PATH_ARM_LIBS)/spi
	CFLAGS += -I $(PATH_ARM_LIBS)/i2c
	CFLAGS += -I $(PATH_ARM_LIBS)/usb
	CFLAGS += -I $(PATH_ARM_LIBS)/flash
	CFLAGS += -I $(PATH_ARM_LIBS)/eeprom
	CFLAGS += -I $(PATH_ARM_LIBS)/remote_control_ir
	CFLAGS += -I $(PATH_UC_LIBS)/_libdef
	CFLAGS += -I $(PATH_UC_LIBS)/misc
	CFLAGS += -I $(PATH_UC_LIBS)/ihm
	CFLAGS += -I $(PATH_UC_LIBS)/fs
	CFLAGS += -I $(PATH_UC_LIBS)/cli
	CFLAGS += -I $(PATH_UC_LIBS)/gmenu
	CFLAGS += -I $(PATH_UC_LIBS)/crc
	CFLAGS += -I $(PATH_UC_LIBS)/sensor
	CFLAGS += -I $(PATH_UC_LIBS)/motors
	CFLAGS += -I $(PATH_UC_LIBS)/motors/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/sensor/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/stdio
	CFLAGS += -I $(PATH_UC_LIBS)/video/
	CFLAGS += -I $(PATH_UC_LIBS)/midia
	CFLAGS += -I $(PATH_UC_LIBS)/midia/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/device
	CFLAGS += -I $(PATH_UC_LIBS)/sound/beep
	CFLAGS += -I $(PATH_UC_LIBS)/sound/beep/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/device/altera
	CFLAGS += -I $(PATH_UC_LIBS)/device/altera/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/device/altera/app
	CFLAGS += -I $(PATH_UC_LIBS)/device/pll_cdce
	CFLAGS += -I $(PATH_UC_LIBS)/driver
	CFLAGS += -I $(PATH_UC_LIBS)/memory/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/codec/mp3/
	CFLAGS += -I $(PATH_UC_LIBS)/codec/mp3/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/display
	CFLAGS += -I $(PATH_UC_LIBS)/display/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/nic
	CFLAGS += -I $(PATH_UC_LIBS)/nic/portable/arm_lpc
	CFLAGS += -I $(PATH_UC_LIBS)/protocol
	CFLAGS += -I $(PATH_UC_LIBS)/protocol/modbus
	CFLAGS += -I $(PATH_UC_LIBS)/keyboard
	CFLAGS += -I $(PATH_UC_LIBS)/graphic/emWin
	CFLAGS += -I $(PATH_EMWIN_LIBS)
	CFLAGS += -I $(PATH_UIP_LIBS)
	CFLAGS += -I $(PATH_UIP_LIBS)/uip
	CFLAGS += -I $(PATH_UIP_LIBS)/portable/arm_lpc
	CFLAGS += -I $(PATH_UIP_LIBS)/webserver
	CFLAGS += -I $(PATH_FREERTOS_LIBS)/include
	CFLAGS += -I $(PATH_FREERTOS_LIBS)/portable/arm_lpc
	CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))					# especifica as includes dos diretórios extras
	
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
	CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
	#CFLAGS +=-Waggregate-return

	ifeq ($(TCP_STACK), TCP_STACK_NONE)
	 	CFLAGS += -Wcast-align
	 	CFLAGS += -Wcast-qual 
	else
		CFLAGS += -fno-strict-aliasing
	endif
		
	ifneq ($(RTOS), FREE_RTOS)
		CFLAGS += -Wnested-externs -Wredundant-decls
	endif		
	

###########################################################################################################################################
########### FLAGS DO LINKADOR
###########		-T				: 
###########		-Ttext			: Passo o endereço inicial para a secção .text. Isto vai subescrever o enderço inicial no link script
###########		-nostartfiles	: Não usar o startup padr?o
###########		-nostdlib		: Não usar as bibliotecas padrões. Somente incluir aquelas especificadas pelo -l (ver LDFLAGS acima)
###########		-nodefaultlibs	: Não usar as bibliotecas padrões. Somente incluir aquelas especificadas pelo -l (ver LDFLAGS acima)
###########  	-Wl,...			: Diz ao linker do GCC para passar para o linker os argumento abaixo:
###########    		-Map		: Cria o mapa da memória
###########    		--cref		: Adiciona a referencia cruzada ao mapa de memória

	# BUSCA O LINKER SCRIPT DEPENDENDO DO TIPO DE SELEÇÃO DA MEMÓRIA
	LDSCRIPT = $(PATH_ARM_LIBS)/common/ld/$(SUBMDL)-$(RUN_MODE).ld

	LDFLAGS += -T$(LDSCRIPT)
	ifeq ($(CORE), cortexm3)	
	LDFLAGS += -mthumb
	endif

	ifeq ($(COMPILER_TYPE), arm-none-linux-gnueabi)
		# para essa versão do compilador é preciso usar as libs padrões, senão é preciso adicionar funções dummy na app
		#	Ex: funções void __aeabi_unwind_cpp_pr0(void){} e  //void __aeabi_unwind_cpp_pr1(void){};
		LDFLAGS += -nostartfiles -Wl,-Map=$(DIR_BIN)/$(TARGET).map,--cref
	else
		LDFLAGS += -nostartfiles -nostdlib -nodefaultlibs -Wl,-Map=$(DIR_BIN)/$(TARGET).map,--cref
	endif
	
	
	# A operação de eliminação de código e dados não utilizado do executável final é de responsabilidade do linker.
	# Para isto o linker deve trabalhar com objetos compilados com as seguintes opções “-ffunction-sections -fdata-sections”. 
	# Essas opções vai colocar respectivamente cada função e dados em seções separadas no arquivo objeto compilado.
	# Uma vez que o objeto ou biblioteca compilada são criadas com essas opções, o linker tem condições de realizar a 
	#   eliminação do código morto. Para instruir o linker a fazer isto devemos adicionar a opção “-Wl,--gc-sections” a ele. 
	# Isto irá executar a coleta de lixo de código e dados nunca referenciado.

	CFLAGS += -ffunction-sections -fdata-sections
	#CFLAGS += -ffunction-sections
	LDFLAGS += -Wl,-gc-sections
		
###########################################################################################################################################
########### FLAGS PARA CRIAÇÃO DE ARQUIVOS DE DEPENDENCIAS

	### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
	GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

###########################################################################################################################################
########### DEFINIÇÃO DE TODOS OS ARQUIVOS OBJETOS E LISTAS

	# DEFINIÇÃO DE TODOS ARQUIVOS OBJETOS
	COBJx      = $(SRC:.c=.o) 
	AOBJx      = $(ASRC:.S=.o)
	COBJLIBx   = $(SRCLIB:.c=.o)
	COBJARMx   = $(SRCARM:.c=.o)
	AOBJARMx   = $(ASRCARM:.S=.o)
	COBJ      	= $(addprefix $(DIR_OBJ)/, $(COBJx))
	AOBJ      	= $(addprefix $(DIR_OBJ)/, $(AOBJx))
	COBJLIB    	= $(addprefix $(DIR_OBJ)/, $(COBJLIBx))
	COBJARM   	= $(addprefix $(DIR_OBJ)/, $(COBJARMx))
	AOBJARM   	= $(addprefix $(DIR_OBJ)/, $(AOBJARMx))
	
############################################################################################################################################
########## ALVOS PARA COMPILAÇÃO
all: begin build_app size end finished

build_app: elf hex bin lss sym
hex: $(DIR_BIN)/$(TARGET).hex
bin: $(DIR_BIN)/$(TARGET).bin
elf: $(DIR_BIN)/$(TARGET).elf
lss: $(DIR_BIN)/$(TARGET).lss 
sym: $(DIR_BIN)/$(TARGET).sym

# Árvore de dependências
#	ri = Regras ímplicitas
#	Sentido de cima para baixo e da direita para esquerda
#
#	all
#	 |
#	 ---------------------------------------------------------------------------------------------
#	 |					 |					 |					 |					 |           |
#	bin					lss					sym					hex					exe		--> elf
#	 |					 |					 |					 |					 |    	|	 |
#	main.bin			main.lss 			main.sys			main.hex			 |		|   main.elf
#	 |					 |					 |					 |					 |    	|	 |
#	(ri: %.sym: %.elf) 	(ri: %.lss: %.elf)	(ri: %.sys: %.elf) 	(ri: %.hex: %.elf)	 |	 	|	(ri: %.elf)
#	 |					 |					 |					 |					 |    	|	 |
#	 ----------------------------------------------------------------------------------------	AOBJ - compila todas as fontes AS através da ri OBJ: %.o : %.S
# 	                    Resolve as dependemcias com elf e retorna aqui							COBJ - compila todas as fontes Cravés da ri OBJ: %.o : %.c
#	 |					 |					 |					 |					 |    		linka os objetos e cria arquivo elf
#	cria arquivo bin	cria arquivo lss	cria arquivo sys	cria arquivo hex	cria arquivo executável

LIBNAME=uc_libc_v2.7.a
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
clean: begin clean_list end finished  

clean_list:
	@echo $(MSG_CLEANING)

	@$(REMOVE) $(DIR_BIN)/$(TARGET).map
	@$(REMOVE) $(DIR_BIN)/$(TARGET).elf
	@$(REMOVE) $(DIR_BIN)/$(TARGET).obj
	@$(REMOVE) $(DIR_BIN)/$(TARGET).hex
	@$(REMOVE) $(DIR_BIN)/$(TARGET).bin
	@$(REMOVE) $(DIR_BIN)/$(TARGET).a90
	@$(REMOVE) $(DIR_BIN)/$(TARGET).sym
	@$(REMOVE) $(DIR_BIN)/$(TARGET).lnk
	@$(REMOVE) $(DIR_BIN)/$(TARGET).lss
	@$(REMOVEDIR) $(DIR_OBJ)
	@$(REMOVEDIR) .dep
	@$(REMOVE) $(LIBNAME)

############################################################################################################################################
########### MOSTRA OS TAMANHOS DO ARQUIVO
HEXSIZE = $(SIZE) --target=ihex $(DIR_BIN)/$(TARGET).hex
ELFSIZE = $(SIZE) -d -B $(DIR_BIN)/$(TARGET).elf #-d = radix em decimal, -B formato berkeley, -A formato sysv
size:
	@if test -f $(DIR_BIN)/$(TARGET).elf; then echo; echo $(MSG_SIZE); $(ELFSIZE); 2>/dev/null; echo; fi


############################################################################################################################################
############ EXIBE MENSAGENS DE INCIO E FIM DA COMPILAÇÃO

begin:
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)

ok_lib:
	@echo $(MSG_ERRORS_NONE)


#############################################################################################################################################
########### DEFINIÇÃO DAS MENSAGENS

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
AS			= $(COMPILER_TYPE)-gcc
AR			= $(COMPILER_TYPE)-ar
CC 			= $(COMPILER_TYPE)-gcc
OBJCOPY 	= $(COMPILER_TYPE)-objcopy
OBJDUMP 	= $(COMPILER_TYPE)-objdump
SIZE 		= $(COMPILER_TYPE)-size
NM 			= $(COMPILER_TYPE)-nm
RN			= mv
REMOVE 		= rm -f
REMOVEDIR 	= rm -rf
COPY 		= cp
#MAKEDEPEND	= $(COMPILER_TYPE)-gcc -M -o $*.d $<
MAKEDEPEND	= makedepend

############################################################################################################################################
########## REGRAS IMPLICITAS
# AS REGRAS DE INTERFERÊNCIA
#	.c.o : 		Ela significa fazer um arquivo.o a partir de um arquivo.c .
#	%.o: %.c 	A mesma coisa

# VARIÁVEIS INTERNAS
# $@ Nome da regra. 
# $< Nome da primeira dependência 
# $^ Lista de dependências
# $? Lista de dependências mais recentes que a regra.
# $* Nome do arquivo sem sufixo

# MORE COMPLICATED DEPENDENCY COMPUTATION, SO ALL PREREQS LISTED
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons

# LINK: CRIA O EXECUTAVEL ELF DOS ARQUIVOS OBJETOS
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(COBJLIB)
%.elf:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(COBJLIB)
	@echo
	@echo $(MSG_LINKING) $@
	@-mkdir -p $(dir $@)				# Cria a pasta para os arquivos executáveis
	@$(CC) $(CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(COBJLIB) $(LDFLAGS) --output $@

# CREATE LIBRARY FROM OBJECT FILES.
.SECONDARY : uc_libc.a
.PRECIOUS : $(COBJARM) $(COBJLIB)
%.a: $(COBJARM) $(COBJLIB)
	@echo $(MSG_CREATING_LIBRARY) $@
	@$(AR) -rcs $@ $(COBJARM) $(COBJLIB)

# COMPILE: CRIA ARQUIVOS OBJETOS DOS ARQUIVOS FONTES C da aplicação
$(COBJ) : $(DIR_OBJ)/%.o : %.c
	@echo $(MSG_COMPILING) $<
	@-mkdir -p $(dir $@)				# Cria a pasta para o arquivos objetos (.o)
	@$(CC) -c  $(CFLAGS) $(GENDEPFLAGS) $< -o $@
#	@$(CC) -MM $(CFLAGS) $*.c > $*.d 	# Gera a lista de dependencias (.d)
	
# COMPILE: CRIA ARQUIVOS OBJETOS DOS ARQUIVOS FONTES C uc_Libc
$(COBJLIB) : $(DIR_OBJ)/%.o : %.c
	@echo $(MSG_COMPILING) $<
	@-mkdir -p $(dir $@)				# Cria a pasta para o arquivos objetos (.o)
	@$(CC) -c  $(CFLAGS) $(GENDEPFLAGS) $< -o $@
#	@$(CC) -MM $(CFLAGS) $*.c > $*.d 	# Gera a lista de dependencias (.d)

# COMPILE: CREATE OBJECT FILES FROM C SOURCE FILES. ARM-ONLY
$(COBJARM) : $(DIR_OBJ)/%.o : %.c
	@echo $(MSG_COMPILING_ARM) $<
	@-mkdir -p $(dir $@)				# Cria a pasta para o arquivos objetos (.o)
	@$(CC) -c $(CFLAGS) $(GENDEPFLAGS) $< -o $@ 
#	@$(CC) -MM $(CFLAGS) $*.c > $*.d 	# Gera a lista de dependencias (.d)
	
# ASSEMBLE: CRIA ARQUIVOS OBJETOS DAS FONTES EM ASSEMBLE
$(AOBJ) : $(DIR_OBJ)/%.o : %.S
	@echo $(MSG_ASSEMBLING) $<
	@-mkdir -p $(dir $@)				# Cria a pasta para o arquivos objetos (.o)
	@$(AS) -c $(ASFLAGS) $(GENDEPFLAGS) $< -o $@
#	@$(CC) -MM $(ASFLAGS) $*.c > $*.d  	# Gera a lista de dependencias (.d)

# ASSEMBLE: CREATE OBJECT FILES FROM ASSEMBLE SOURCE FILES. ARM-ONLY
$(AOBJARM) : $(DIR_OBJ)/%.o : %.S
	@echo $(MSG_ASSEMBLING_ARM) $<
	@-mkdir -p $(dir $@)				# Cria a pasta para o arquivos objetos (.o)
	@$(AS) -c $(ASFLAGS) $< -o $@
#	@$(CC) -MM $(ASFLAGS) $*.c > $*.d  	# Gera a lista de dependencias (.d)

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
########### INCLUE OS ARQUIVOS DE DEPENDENCIAS
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

#############################################################################################################################################
########### LISTING OF PHONY TARGETS
#	PHONY: Esta regra permite de evitar conflitos.
#		Por exemplo "clean:" e uma regra sem nem uma dependência não temos nada na pasta que se chame clean.
#		Agora vamos colocar na pasta um arquivo chamado clean. 
#		Se você tentar apagar os "arquivos.o" escrevendo "make clean" não vai acontecer nada porque make diz que clean não foi modificado.
#		Para evitar esse problema usamos a regra .PHONY
.PHONY : asm bin all begin finished end size ok_lib lib \
build_app build_lib elf hex eep lss sym \
clean clean_list
