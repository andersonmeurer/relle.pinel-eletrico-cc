#include "stdio_uc.h"
#if defined(LINUX)
#include <stdarg.h>
#include <stdio.h>
#endif

#if defined(FREE_RTOS)
#include "task.h"
xQueueHandle stdout_queueTX;
xSemaphoreHandle stdout_mutex;

xQueueHandle debug_queueTX;
xSemaphoreHandle debug_mutex;

xQueueHandle err_queueTX;
xSemaphoreHandle err_mutex;

xQueueHandle dev1_queueTX;
xSemaphoreHandle dev1_mutex;

xQueueHandle dev2_queueTX;
xSemaphoreHandle dev2_mutex;

xQueueHandle dev3_queueTX;
xSemaphoreHandle dev3_mutex;

xQueueHandle dev4_queueTX;
xSemaphoreHandle dev4_mutex;
#endif //#if defined(FREE_RTOS)

// PONTEIROS DAS FUNÇÕES DE ENTRADA E SAÍDA
static int (*stdio_getc)(u8 *ch);		// Ponteiro para a função padrão de entrada
static int (*stdio_putc)(n16 ch);		// Ponteiro para a função padrão de saída
static int (*debug_putc)(n16 ch);		// Ponteiro para a função padrão de saída de LOG
static int (*err_putc)(n16 ch);			// Ponteiro para a função padrão de saída de erro

#if !defined(LINUX)
static int (*dev1_putc)(n16 c);	// Ponteiro para a função de saida do primeiro dipositivo
static int (*dev2_putc)(n16 c);	// Ponteiro para a função de saida do segundo dipositivo
static int (*dev3_putc)(n16 c);	// Ponteiro para a função de saida do terceiro dipositivo
static int (*dev4_putc)(n16 c);	// Ponteiro para a função de saida do quarto dipositivo
#endif // #if defined(LINUX)

// ESTRUTURA DE IMPRESSÃO
typedef struct {
	pchar buf; // ponteiro do buffer para ser atribuido a formatação
	int count; //dado impressos na saida, muito util para preencher buffers
	char prefix; //tamanho minimo da impressao
	char fill; //caracter que vai preencher o espaco vazio
	char precision; //precisão durante a impressão de numeros reais
	char isSformat; // sinalizador que é sformat
	u8 device;
}tPrint, *pPrint;

static void print_putchar(pPrint output, u8 c);
static void print_fill(pPrint output, int n);
static void print_string(pPrint output, char * s);
static void print_int(pPrint output, int x);
static void print_long(pPrint output, long x);
static void print_uint(pPrint output, unsigned int x);
#if ( STDIO_USE_SUBINT == pdON )
static void print_intF(pPrint output, long x);
#endif
#if ( STDIO_USE_HEX == pdON )
static void print_hex(pPrint output, unsigned int x);
#endif
#if ( STDIO_USE_BIN == pdON )
static void print_bin(pPrint output, long x);
#endif
#if ( STDIO_USE_TIME == pdON )
static void print_time(pPrint output, long x);
#endif
#if ( STDIO_USE_FLOAT == pdON )
static void print_float(pPrint output, double x);
static void print_exp(pPrint output, double x);
#endif
static void doprint(pPrint output, cpchar fmt, va_list args);

// ##################################################################################
// REGISTRO DAS FUNÇÕES STDIO


#if defined(FREE_RTOS)
#if !defined(LINUX)
// Registra a função de saída do determinado dipositivo, registra o semáforo e a fila de impressão
void rprintf_RegisterDev(const u8 device, int(*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex ) {
   	// regitrar a função de saida do dispositivo
   	switch(device) {
	case STDIO_DEV1: 	dev1_putc = putc; dev1_queueTX = queue; dev1_mutex = mutex; break;
	case STDIO_DEV2: 	dev2_putc = putc; dev2_queueTX = queue; dev2_mutex = mutex; break;
	case STDIO_DEV3: 	dev3_putc = putc; dev3_queueTX = queue; dev3_mutex = mutex; break;
	case STDIO_DEV4: 	dev4_putc = putc; dev4_queueTX = queue; dev4_mutex = mutex; break;
  	}
}
#endif // #if !defined(LINUX)

// Registra a função de saída padrão
void lputchar_RegisterDev(int (*func_putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex ) {
	stdio_putc = func_putc;
	stdout_queueTX = queue;
	stdout_mutex = mutex;
}

// Registra a função de saída padrão de LOG
void ldebug_RegisterDev(int (*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex ) {
	debug_putc = putc;
	debug_queueTX = queue;
	debug_mutex = mutex;
}

// Registra a função de saída padrão de erro
void lerr_RegisterDev(int (*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex ) {
	err_putc = putc;
	err_queueTX = queue;
	err_mutex = mutex;
}

#else // #if defined(FREE_RTOS)

#if !defined(LINUX)
// Registra a função de saída do determinado dipositivo
void rprintf_RegisterDev(const u8 device, int(*putc)(n16) ) {
   	// regitrar a função de saida do dispositivo
   	switch(device) {
		case STDIO_DEV1: 	dev1_putc = putc; break;
   		case STDIO_DEV2: 	dev2_putc = putc; break;
   		case STDIO_DEV3: 	dev3_putc = putc; break;
   		case STDIO_DEV4: 	dev4_putc = putc; break;
  	}
}
#endif // #if !defined(LINUX)

// Registra a função de saída padrão
void lputchar_RegisterDev(int (*func_putc)(n16)) {
	stdio_putc = func_putc;
}

// Registra a função de saída padrão de LOG
void ldebug_RegisterDev(int (*putc)(n16)) {
	debug_putc = putc;
}

// Registra a função de saída padrão de erro
void lerr_RegisterDev(int (*putc)(n16)) {
	err_putc = putc;
}
#endif // #if defined(FREE_RTOS)


// Registra a função de entrada padrão
void lgetchar_RegisterDev(int (*func_getc)(u8*)) {
	stdio_getc = func_getc;
}

// #######################################################################################################################
// IMPRESSÃO DE CARACTERES
// #######################################################################################################################

//  Esta função direciona a impressão do caractere para o dipositivo de saida ou armazena em buffer de formatação de string
static void print_putchar(pPrint output, u8 c) {
	if (output->buf!=NULL) { 				// Checa se há buffer para fazer formatação de string
		output->buf[output->count++] = c;
		return;		// se é somente para fazer formatação de string não vamos fazer impressão em dispositivo
	}

//	#if defined(FREE_RTOS)
//	captura a fila de saida
//		if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) { // Checa se o scheduler do kernel está rodando
//			xQueueHandle queue = NULL;
//
//   			switch(output->device) {
//   				case STDIO_LPRINTF:		queue = stdio_queueTX; break;
//				case STDIO_DEBUG:		queue = debug_queueTX; break;
//				case STDIO_ERROR:		queue = err_queueTX; break;
//   				case STDIO_DEV_UART0: 	queue = uart0_queueTX; break;
//   				case STDIO_DEV_UART1: 	queue = uart1_queueTX; break;
//   				case STDIO_DEV_UART2: 	queue = uart2_queueTX; break;
//   				case STDIO_DEV_UART3: 	queue = uart3_queueTX; break;
//   				case STDIO_DEV_LCD: 	queue = lcd_queueTX;   break;
//  			}
//  			xQueueSend(queue, &c, portMAX_DELAY);
//
//  			return;
//		}
//	#endif

	#if defined(LINUX)
	stdio_putc(c);
	#else
	static int (*putcharfunc)(n16 c);
	
	switch(output->device) {
		case STDIO_LPRINTF:	putcharfunc = stdio_putc; break;
		case STDIO_DEBUG:	putcharfunc = debug_putc; break;
		case STDIO_ERROR:	putcharfunc = err_putc; break;
		case STDIO_DEV1: 	putcharfunc = dev1_putc; break;
   		case STDIO_DEV2: 	putcharfunc = dev2_putc; break;
   		case STDIO_DEV3: 	putcharfunc = dev3_putc; break;
   		case STDIO_DEV4: 	putcharfunc = dev4_putc; break;
  	}

	// senão joga na porta de saida
	// Espera até que consiga incluir o caractere no buffer de TX
	while (putcharfunc(c) < 0) continue;
	#endif // #if defined(LINUX)
}

// #######################################################################################################################
// FUNÇÕES DE FORMATAÇÃO
// #######################################################################################################################

//completa a impressao com os caracteres desejados
//n=quantos caracteres ja serao impressos, o que faltar
//a rotina preenche
static void print_fill(pPrint output, int n) {
	n = output->prefix - n;
	while (n-->0) print_putchar(output, output->fill);
}

static void print_string(pPrint output, char * s) {
	int d = 0;
	if (s==NULL) return;
	while (s[d])
		print_putchar(output, s[d++]);
	print_fill(output, d);
}

static void print_int(pPrint output, int x) {
	int d,sig;
	string digits;

	if (x<0) { sig = 1; x = -x; } else sig = 0;
	d = 0;
	do	{
		digits[d++] = '0' + x%10;
		x = x/10;
	}while (x);
	if (sig) digits[d++] = '-';

	print_fill(output, d);
	while (d--) print_putchar(output, digits[d]);
}

static void print_long(pPrint output, long x) {
	int d; //digitos
	string digits;

	if (x<0) { print_putchar(output,'-'); x = -x; }
	d = 0;
	do	{
		digits[d++] = '0' + x%10;
		x = x/10;
	}while (x);

	print_fill(output, d);
	while (d--) print_putchar(output, digits[d]);
}

static void print_uint(pPrint output, unsigned int x) {
	unsigned int d; //digitos
	string digits;

	d = 0;
	do	{
		digits[d++] = '0' + x%10;
		x = x/10;
	}while (x);

	print_fill(output, d);
	while (d--) print_putchar(output, digits[d]);
}

#if ( STDIO_USE_SUBINT == pdON )
static void print_intF(pPrint output, long x) {
	int b = 0;
	while (x>10240) { x = x/1024; b++; }
	print_int(output, x);
	if (b==1) print_putchar(output, 'k');
	if (b==2) print_putchar(output, 'M');
	if (b==3) print_putchar(output, 'G');
}
#endif

#if ( STDIO_USE_HEX == pdON )
static void print_hex(pPrint output, unsigned int x) {
	const char HexTable[16] = {
		'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
	};
	char digits[16];

	int d = 0;
	do	{
		digits[d++] = HexTable[x&0xF];
		x = x>>4;
	}while (x);

	//print_putchar(output, '0');
	//print_putchar(output, 'x');
	print_fill(output, d);
	while (d--) print_putchar(output, digits[d]);
}
#endif

#if ( STDIO_USE_BIN == pdON )
static void print_bin(pPrint output, long x) {
	string digits;
	int d = 0;
	output->fill = '0';
	do {
		digits[d++] = '0'+(x&1);
		x = x>>1;
	}while (x);
	//print_putchar(output, '0');
	//print_putchar(output, 'b');
	print_fill(output, d);
	while (d--) print_putchar(output, digits[d]);
}
#endif

#if ( STDIO_USE_TIME == pdON )
//tempo em segundos
static void print_time(pPrint output, long x) {
	int h,m,s;

	s = x%60;	x = x/60;
	m = x%60;	x = x/60;
	h = x;

	//formato = "00:00'00"
	output->prefix = 2;
	output->fill = '0';
	print_int(output, h);	print_putchar(output, ':');
	print_int(output, m);	print_putchar(output, 39);
	print_int(output, s);
}
#endif

#if ( STDIO_USE_FLOAT == pdON )
static void print_float(pPrint output, double x) {
	if (x<0) { print_putchar(output, '-'); x = -x; }
	//parte inteira
	print_long(output, (long)x);
	if (output->precision<=0) return;
	//parte fracionaria
	print_putchar(output, '.');
	output->prefix = 0;
	x = x - (int)(x);
	while (output->precision-->0) {
		x = x*10.0;
		if (x<1.0) print_putchar(output, '0');
	}
	if (x>1) print_long(output, (long)x);
}

static void print_exp(pPrint output, double x) {
	int exp = 0;
	if (x<0) { print_putchar(output, '-'); x = -x; }
	if (x!=0) {
		while (x<1) { x = x*10; exp--; }
		while (x>10){ x = x/10; exp++; }
	}
	print_float(output, x);
	print_putchar(output, 'E');
	print_int(output, exp);
}
#endif

static void doprint(pPrint output, cpchar fmt, va_list args) {
	char c, doformat = 0;

	output->count = 0;

	//repete o processo ate encontrar o terminador nulo
	do {
		c = *fmt++;

		if (doformat) {
			doformat = 0;

			if ( ((c=='l')||(c=='L')) &&(*fmt=='d')) {
				print_long(output, va_arg(args,long));
				fmt++;
				continue;
			}

			switch (c) {
			case '%' : print_putchar(output, '%'); break;
			case 'C' :
			case 'c' : print_putchar(output, va_arg(args,int)); break;
			case 'D' :
			case 'd' : print_int(output, va_arg(args,int)); break;
			case 'U' :
			case 'u' : print_uint(output, va_arg(args,int)); break;
			#if ( STDIO_USE_HEX == pdON )
			case 'X' :
			case 'x' : print_hex(output, va_arg(args,int));	break;
			#endif
			case 'S' :
			case 's' : print_string(output, (char*)va_arg(args,int)); break;
			#if ( STDIO_USE_SUBINT == pdON )
			case 'k' : print_intF(output, va_arg(args,long));	break;
			#endif
			#if ( STDIO_USE_TIME == pdON )
			case 't' : print_time(output, va_arg(args,long)); break;
			#endif
			#if ( STDIO_USE_BIN == pdON )
			case 'b' : print_bin(output, va_arg(args,long)); break;
			#endif
			#if ( STDIO_USE_FLOAT == pdON )
			case 'f' : print_float(output, va_arg(args, double)); break;
			case 'e' : print_exp(output, va_arg(args, double)); break;
			#endif
			default :
				// preencher com '0'?
				if ( (c=='0')&&(output->prefix==0) ) {
					doformat = 1; output->fill = '0';
					continue;
				}

				// vai indicar a precisao
				if (c=='.') {
					doformat = 1;
					output->precision = (*fmt++)-'0';
					continue;
				}

				//esta indicando o prefixo...
				if ( (c>='0')&&(c<='9') ) {
					doformat = 1;
					output->prefix = output->prefix*10 + (c - '0');
					continue;
				}
				print_putchar(output, '?');
				break;
			}
			continue;
		}

		if (c=='%')	{
			doformat = 1;
			output->prefix = 0;
			output->precision = 5;
			output->fill = ' ';
			continue;
		}

		// Não imprime o caractere nulo (foi incluso essa cláusula em 27/06/2014)
		// o caractere nulo somente será impresso caso for chamado sformat (foi incluso essa cláusula em 14/07/2014)
		//	Pois é preciso adicionar o finalizador de string
		if ( (c > 0) || ((c==0) && (output->isSformat)))
			print_putchar(output, c);
	} while (c);
}


// #######################################################################################################################
// FUNÇÕES DE ENTRADA E DE SAÍDAS DE IMPRESSÃO NOS DISPOSITIVOS
// #######################################################################################################################
int lgetchar(u8 *ch) {
	return stdio_getc(ch);
}

int lputchar(n16 ch) {
	return stdio_putc(ch);
}

// Exemplos:
//	lprintf(PROGAM_NAME);
//	lprintf("TESTE FORMATAÇÃO DE lprintf"CMD_TERMINATOR);
//	lprintf("Impressão direta"CMD_TERMINATOR);
//	lprintf("Inteiro: %d %d %04d"CMD_TERMINATOR, -2, -4, 5);
//	lprintf("Natural: %02u %03u %04u"CMD_TERMINATOR, 2, 4, 5);

//	float a=2.78;
//	lprintf("Float: %f"CMD_TERMINATOR, a);
//	lprintf("Float: %f %f"CMD_TERMINATOR, a/4, (float)5);
//	lprintf("Float def casas fracionarias precisão: %.2f %.2f"CMD_TERMINATOR, a/4, (float)5);
//		OBS: máximo precisão é de 9. Ex: %f.9
//	lprintf("Float: %f %e %e"CMD_TERMINATOR, 2e10, -1e4, 5);
//	lprintf("Float com precisão: %.2f %.5e %.2e"CMD_TERMINATOR, 2e10, -1e4, 5);
//		OBS: máximo precisão é de 9. Ex: %f.9

//	lprintf("tempo: %t"CMD_TERMINATOR, 123456L);
//	lprintf("Inteiro com subdivições de 1024: %k %k %k %k"CMD_TERMINATOR, 100, 1024, 1024*1024, 1024*1024*1024);
//	lprintf("HEX: 0x%x 0x%2x"CMD_TERMINATOR, 0x123, 0x456);
//	lprintf("BIN: 0b%b 0b%2b"CMD_TERMINATOR, 0x123, 0x456);
//	lprintf("String: %s"CMD_TERMINATOR, "imprimindo");
#if defined(LINUX)
int rprintf(const char* fmt, ...) {
#else
int rprintf(const u8 device, const cpchar fmt, ...) {
#endif
	int ret = pdPASS;
	tPrint output;


	#if defined(FREE_RTOS)
	// Travar o recuro de saida para imprimir toda a mensagem sem que haja troca de contexto.
	//	Ou seja, nenhuma outra tarefa pode enviar uma mensagem no mesmo dispositivo de saída,
	//	através da troca de contexto do kernel, sem que mensagem da tarefa anterior seja totalmente enviada.
  	xSemaphoreHandle mutex = NULL;

  	switch(device) {
	case STDIO_LPRINTF:	mutex = stdout_mutex; break;
	case STDIO_DEBUG:	mutex = debug_mutex; break;
	case STDIO_ERROR:	mutex = err_mutex; break;
	case STDIO_DEV1: 	mutex = dev1_mutex; break;
	case STDIO_DEV2: 	mutex = dev2_mutex; break;
	case STDIO_DEV3: 	mutex = dev3_mutex; break;
	case STDIO_DEV4: 	mutex = dev4_mutex; break;
 	}

	// Travar o recurso de saída para transmissão completa da mensagem, esperar MAX_DELAY caso o recurso está sendo usado
	ret = xSemaphoreTake(mutex, portMAX_DELAY);
	#endif

	#if !defined(LINUX)
	output.device = device;			// Sinaliza qual dipositivo de saída deve ser impresso
	#endif
	output.buf = NULL;				// Sinaliza que não vamos usar buffer para formatação de string
	output.isSformat = 0;			// Sinaliza que não é um sformat

	va_list args;					// Cria var para captura de argumentos (fmt) no stack da chamada de função
	va_start(args, fmt);			// Captura os argumentos após fmt
	doprint(&output, fmt, args);	// Faz a formatação da string com ftm e args e manda imprimir
	va_end(args);					// Desmonta args

	#if defined(FREE_RTOS)
	// Liberar o recuro de saida
	xSemaphoreGive(mutex);
	#endif

  	return ret;
}


// #######################################################################################################################
// FORMATAÇÃO DE STRINGS
// #######################################################################################################################

// Exemplos:
//	string s;

//	sformat(s, "Inteiro: %d %d %04d", -2, -4, 5);
//	sformat(s, "Natural: %02u %03u %04u", 2, 4, 5);

//	float a=2.78;
//	sformat(s, "Float: %f"CMD_TERMINATOR, a);
//	sformat(s, "Float: %f %f"CMD_TERMINATOR, a/4, (float)5);
//	sformat(s, "Float def casas fracionarias precisão: %.2f %.2f"CMD_TERMINATOR, a/4, (float)5);
//	sformat(s, "Float: %f %e %e", 2e10, -1e4, 5);
//	sformat(s, "Float com precisão: %.2f %.5e %.2e", 2e10, -1e4, 5);
//		OBS: máximo precisão é de 9. Ex: %f.9

//	sformat(s, "tempo: %t", 123456L);
//	sformat(s, "Inteiro com subdivições de 1024: %k %k %k %k", 100, 1024, 1024*1024, 1024*1024*1024);
//	sformat(s, "HEX: 0x%x 0x%2x", 0x123, 0x456);
//	sformat(s, "BIN: 0b%b 0b%2b", 0x123, 0x456);
//	sformat(s, "String: %s", "imprimindo");

// sformat(s,"%02d:%02d", rtc->hour, rtc->min); //convertendo hora
void sformat(pchar s, cpchar fmt, ...) {
	tPrint output;

	output.device = 0;
	output.buf = s;					// Sinaliza qual buffer para armazenar a formatação de string. Também sinaliza que é uma formatação de string e não uma impressão em dipositivo
	output.isSformat = 1;			// Sinaliza que é um sformat

	va_list args;					// Cria var para captura de argumentos (fmt) no stack da chamada de função
	va_start(args, fmt);			// Captura os argumentos após fmt
	doprint(&output, fmt, args);	// Faz a formatação da string com ftm e args e manda imprimir
	va_end(args);					// Desmonta args
}

int sFormatArgs(pchar s, cpchar fmt, va_list args) {
	tPrint output;

	output.device = 0;
	output.buf = s;					// Sinaliza qual buffer para armazenar a formatação de string. Também sinaliza que é uma formatação de string e não uma impressão em dipositivo
	output.isSformat = 0;			// Sinaliza que não é um sformat
	doprint(&output, fmt, args);

	return output.count;
}

#if (STDIO_USE_TIME == pdON)
pchar date2str(u8 wday, u8 mday, u8 month, u16 year) {
	#if (STDIO_USE_WEEKDAY_PORTUGUESE == pdON)
	const char diasem[7][4] = { "DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB" };
	#else
	const char diasem[7][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
	#endif

	static string result;
	sformat(result, "%s - %02d/%02d/%02d", diasem[wday%7], mday, month, year);
	return result;
}

pchar time2str(u8 hour, u8 min, u8 sec) {
	static string result;
	sformat(result,"%02d:%02d'%02d", hour, min, sec);
	return result;
}

#endif
