##########################################################################################################################################
########### ESPECEFICA QUAL PROMPT O SISTEMA SERÁ COMPILADO  >> CYGWIN | WINDOWS
	
	PROMTP_TYPE = CYGWIN

###########################################################################################################################################
########### TIPO DE PROCESSADOR E DE COMPILAÇÃO
	
	#### TIPO DO ARM  					>> LPC2294 | LPC2468
	SUBMDL = LPC2294

############################################################################################################################################
########### DRIERTÓRIOS E CONFIGURAÇÕES DAS BIBLIOTECAS

	ifeq ($(PROMTP_TYPE), WINDOWS)
		PATH_UC_LIBS 	= D:/_libs/uC_libc1.1
	else
		PATH_UC_LIBS 	= /cygdrive/d/_libs/uC_libc1.1
	endif

	PATH_ARM_LIBS		= $(PATH_UC_LIBS)/_arch/arm_lpc
	
###########################################################################################################################################
########### ARQUIVOS C A SEREM COMPILADOS

	TARGET  = syscalls
	SRC = $(TARGET).c
		

############################################################################################################################################
########### FORMATO DE DEPURAÇÃO
########### 	FORMATOS NATIVOS DO ARMGCC -g SÃO dwarf-2 [PADRÃO] or stabs. ARM (extended) COFF REQUER stabs, plus an arm-objcopy run.

#	DEBUG = stabs
	DEBUG = dwarf-2

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
	CDEFS += -D CPU_ARM_LPC
	CDEFS += -D $(SUBMDL)

      	    
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
	
	CFLAGS += -I $(PATH_ARM_LIBS)
	CFLAGS += -I $(PATH_UC_LIBS)/_libdef
	
	CFLAGS += -Wall
	CFLAGS += -Wpointer-arith -Wbad-function-cast -Wshadow 
	CFLAGS += -Wimplicit
	CFLAGS += -Wsign-compare -Wextra
	CFLAGS += -Wswitch -Wreturn-type -Wunused
	CFLAGS += -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations

	CFLAGS += -Wcast-align 
	CFLAGS += -fno-strict-aliasing

###########################################################################################################################################
########### FLAGS DO LINKADOR
###########		-nostartfiles:	Diz ao GCC para não usar o startup padrão
###########  		-Wl,...:		Diz ao GCC para passar os argumento abaixo ao linker
###########    		-Map:      	Cria o mapa da memória
###########    		--cref:    	Adiciona a referencia cruzada ao mapa de memória

	LDFLAGS += -nostartfiles -Wl,-Map=$(TARGET).map,--cref,-nostdlib
	
###########################################################################################################################################
########### DEFINIÇÃO DE TODOS OS ARQUIVOS OBJETOS E LISTAS

	# DEFINIÇÃO DE TODOS ARQUIVOS OBJETOS
	COBJ      = $(SRC:.c=.o) 

############################################################################################################################################
############# ALVO PADRÃO PARA COMPILAÇÃO

LIBNAME=$(TARGET)_lib.a
lib: begin build_lib end ok_lib
build_lib: $(LIBNAME)

############################################################################################################################################
########## APAGA ARQUIVOS COMPILADOS

c: clean
clean: begin clean_list finished end 

clean_list:
	@echo
	@echo $(MSG_CLEANING)
	@$(REMOVE) $(COBJ)
	@$(REMOVE) $(SRC:.c=.s)
	@$(REMOVE) $(SRC:.c=.d)
	@$(REMOVE) $(TARGET).map
	@$(REMOVE) $(TARGET).elf
	@$(REMOVE) $(TARGET).obj
	@$(REMOVE) $(TARGET).hex
	@$(REMOVE) $(TARGET).bin
	@$(REMOVE) $(TARGET).a90
	@$(REMOVE) $(TARGET).sym
	@$(REMOVE) $(TARGET).lnk
	@$(REMOVE) $(TARGET).lss
	@$(REMOVEDIR) .dep
	@$(REMOVE) $(LIBNAME)

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
########## REGRAS IMPLICITAS

# CREATE LIBRARY FROM OBJECT FILES.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(COBJ)
%.a: $(COBJ)
	@echo $(MSG_CREATING_LIBRARY) $@
	@$(AR) -rc $@ $(COBJ)

# COMPILE: CRIA ARQUIVOS OBJETOS DOS ARQUIVOS FONTES C
$(COBJ) : %.o : %.c
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(CFLAGS) $< -o $@ 

#############################################################################################################################################
########### LISTING OF PHONY TARGETS

.PHONY : asm bin all begin finished end size ok ok_lib lib \
build build_lib elf hex eep lss sym \
clean clean_list program
