#include "utils.h"

#if (UTIL_USE_DUMP == pdON)
#include "stdio_uc.h"
#endif

#if (UTIL_USE_XITOA == pdON)
#include <string.h>
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emprime uma linhas no formato HEX e ASCII 
// Parametros: 	*buff: Ponteiro, endereço, do buffer a ser impresso
//				offset: endereço do buffer do inicio da impressão
//				cnt: Quantidade de bytes a ser exibido na linha
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
#if (UTIL_USE_DUMP == pdON)
#ifdef CPU_AVR
void put_dump (const u8 *buff, u16 showAddrOffSet, int cnt) {
#else
void put_dump (const u8 *buff, u32 showAddrOffSet, int cnt) {
#endif
	u8 n;

	// IMPRIME O ENDEREÇO INICIAL (OFFSET)
	#ifdef CPU_AVR
	lprintf("%04X: ", showAddrOffSet);
	#else
	lprintf("%08X: ", showAddrOffSet);
	#endif
	
	// IMPRIME HEXDECIMAL
	for(n = 0; n < cnt; n++)
		lprintf(" %02X", buff[n]);
		
	// IMPRIME SEPARADOR
	lprintf(" ; ");
	
	// IMPRIME CHAR
	for(n = 0; n < cnt; n++) {
		//if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
		if ( buff[n] < 0x20 )
			lprintf(".");
		else
			lprintf("%c", buff[n]);
	}
	
	lprintf(CMD_TERMINATOR);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emprime uma linhas no formato HEX e ASCII 
// Parametros: 	*buff: Ponteiro, endereço, do buffer a ser impresso
//				offset: endereço do buffer do inicio da impressão
//				cnt: Quantidade de bytes a ser exibido na linha
//				mask: Quantidade de bytes que deve ser exibido na linha. 
//					Isto é útil para quanod a cnt for menor que mask, imprimir caracter em branco para fazer
//					alinhamento entre as colunas da impressão anterior
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
#ifdef CPU_AVR
void put_dump16 (const u8 *buff, u16 showAddrOffSet, int cnt, u8 mask ) {
#else
void put_dump16 (const u8 *buff, u32 showAddrOffSet, int cnt, u8 mask ) {
#endif
	u8 n;

	// IMPRIME O ENDEREÇO INICIAL (OFFSET)
	#ifdef CPU_AVR
	lprintf("%04x ", showAddrOffSet);
	#else
	lprintf("%08x ", showAddrOffSet);
	#endif
	
	// IMPRIME HEXDECIMAL
	for(n=0; n<cnt; n++) lprintf(" %02x", buff[n]);
		
	// CASO A QUANTIDADE DE BYTES DA LINHA NÃO SEJA O TAMANHO DA MASK
	// Isto preenche a coluna hex com espaços para que a coluna dos caracteres se alinhe com as impressões anteriores
	for(n=cnt; n<mask; n++)	lprintf("   ");
	
	// IMPRIME SEPARADOR
	lprintf(" : ");
	
	// IMPRIME CHAR
	for(n = 0; n < cnt; n++) {
		//if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
		if ( buff[n] < 0x20 )	lprintf(".");
		else					lprintf("%c", buff[n]);
	}
	
	lprintf(CMD_TERMINATOR);
}

void buffer_dump(u8 *buff, int cnt, u8 mask) {
	u8 *ptr;
	
	for (ptr=buff; cnt >= 16; ptr += 16, cnt -= 16)
		#ifdef CPU_AVR
		put_dump16((u8*)ptr, (u16)ptr, 16, mask);
		#else
		put_dump16((u8*)ptr, (u32)ptr, 16, mask);
		#endif
					
	if (cnt) 
		#ifdef CPU_AVR
		put_dump16((u8*)ptr, (u16)ptr, cnt, mask);
		#else
		put_dump16((u8*)ptr, (u32)ptr, cnt, mask);
		#endif
}
#endif

#if (UTIL_USE_XATOI == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Converte uma string no formato hex, bin ou octal para um número inteiro
//				Exemplos: "0x10" ret 16. 	"-100" ret -100. 	"0b1100" ret 12
// Parametros: 	Endereço da string para ser convertida e o endereço da var inteira par retorno da conversão
// Retorna: 	pdTRUE se houve conversão ou pdFALSE caso não foi possivel converter
//------------------------------------------------------------------------------------------------------------------
int xatoi (char **str, long *res) {
	u32 val;
	u8 c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return pdTRUE;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return pdFALSE;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return pdFALSE;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return pdFALSE;
		}
		if (c >= radix) return pdFALSE;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return pdTRUE;
}
#endif


#if (UTIL_USE_XITOA == pdON)

//void xitoa (long val, int radix, int len)
void xitoa (char* str, long val) {
	u8 c;
	long v;
	char* p1 = str;

	v = (val<0)?-val:val;

	// converter para ascii
	do {
		c = (u8)(v % 10);
		if (c >= 10) c += 7;
		c += '0';
		*p1++ = c;
		v /= 10;
	} while (v);

	if (val<0) *p1++ = '-';
	*p1 = 0;

	// inverter string
	char* p2 = &str[strlen(str)-1];
	p1 = str;
	while(p1<p2) {
		c= *p2;
		*p2-- = *p1;
		*p1++ = c;
	}
}
#endif

#if (UTIL_USE_DATETIME == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com hora e minutos da contagem do minuto total
// Parametros: 	min: Minutos corrido
//				h: ponteiro para retorno das horas
//				m: ponteiro para retorno dos minutos
// Retorna: 	Horas e minutos
//------------------------------------------------------------------------------------------------------------------
void minutesToHours(uint min, uint* h, uint* m) {
	*h = (uint)(min/60);
	*m = min - (*h*60);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna em minutos total em uma determinada hora e minuto
// Parametros: 	h: Horas para calculo
//				m: Minutos para calculo
//				min: Ponteiro da quantidade de minutos total
// Retorna: 	Minutos corridos
//------------------------------------------------------------------------------------------------------------------
void hoursToMinutes(uint h, uint m, uint* min) {
	*min = h*60+m;
}
#endif


#if (UTIL_USE_POT == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Eleva a base a potencia desejada
// Parametros: 	base e a potencia a ser elevada
// Retorna: 	Resultado da potencia
//------------------------------------------------------------------------------------------------------------------
u32 pot(uint base, uint exp)	{
	u32 aux;
	u16 t;
		
	if (exp == 0) return 1;
	
	aux = base;
	for (t = 1; t < exp; t++)
		aux *= base;
	
	return aux;
}
#endif

#if (UTIL_USE_GET_NUMBER == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	
// Parametros: 	Nenhum
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
u32 get_Number(u8 *enter_data, u8 enter_data_index) {
	u8 yy;
	u32 number;

	number=0;
	for (yy = 0; yy < enter_data_index; yy++)
		number += (enter_data[yy]-KEY_0)*pot(10,(enter_data_index-1-yy));
	
	return number;
}
#endif


#if (UTIL_USE_ASCII2BYTE == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Converte 2 bytes hex ASCII em 1 byte
// Parametros: 	Ponteiro do primeiro char ASCII
// Retorna: 	Byte
//------------------------------------------------------------------------------------------------------------------
u8 ascii2byte (u8 *val) {
	u8 temp = *val;
	
	if(temp > 0x60) temp -= 39;  		// convert chars a-f
	
	temp -= 48;  						// convert chars 0-9
	temp *= 16;
	temp += *(val+1);
	
	if(*(val+1) > 0x60) temp -= 39;  	// convert chars a-f
	
	temp -= 48;  						// convert chars 0-9	

	return temp;
}
#endif

#if (UTIL_USE_HEX2BYTE == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o valor inteiro de um caractere em hexadecimal
// Parametros: 	c: O valor do caractere a ser convertido
// Retorna: 	Retorna com o valor decimal do caractere hexadecimal, caso a converão foi feita com sucesso
//				Retorna com o valor -1 caso o caractere hexadecimal é inválido
//------------------------------------------------------------------------------------------------------------------
s16 hex2byte(char c) {
	if ((c>='0')&&(c<='9')) return (c-'0');
	if ((c>='A')&&(c<='F')) return (c-'A')+10;
	if ((c>='a')&&(c<='f')) return (c-'a')+10;
	return -1;
}
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a porcentagem do valor
// Parametros: 	Primeiro parametro é valor a ser calculado a porcentagem
//  			Segundo parametro é valor da escala máxima que pertence o valor do primeiro parametro
//				Teceiro parametro é a escala máxima em porcentagem para que a função retorne TRUE
//				Quarto parametro o valor calculado da porcentagem
// Retorna: 	pdTRUE se a escala em porcentagem foi alcançada e foi alcançada pela primeira vez. 
//					Senão, por exemplo, o número 800 na escala 10 repetira entre 800 a 809
//------------------------------------------------------------------------------------------------------------------
#if (UTIL_USE_PERCENT_INT == pdON)

u8 pt_aux = 255; // var global
//u8 percent (u32 value, u32 value_max) {
	//return (u8)((100*value)/value_max);
//}
int percent (u32 value, u32 value_max, u8 scale, u8 *p) {
	u8 pt = (u8)((100*value)/value_max);
	*p = pt;
		
	if ( (pt % scale ==  0) && (pt_aux != pt) ) {
		pt_aux = pt;
		return pdTRUE;
	}
	
	return pdFALSE;}

#endif




#if (UTIL_USE_ACCESS_BUF_LE == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função é para ler um valor u32 de um buffer u8, em uma plataforma com processador little endian
// Parametros:	Ponteiro do buffer u8
// Retorna:		Valor u32
//------------------------------------------------------------------------------------------------------------------
u32 read_LE32u (volatile u8 *pmem) {
    u32 val;

    ((u8 *)&val)[0] = pmem[0];
    ((u8 *)&val)[1] = pmem[1];
    ((u8 *)&val)[2] = pmem[2];
    ((u8 *)&val)[3] = pmem[3];

    return (val);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função escreve um valor u32 em um buffer u8, em uma plataforma com processador little endian
// Parametros: 	pmem:	Pointeiro do buffer u8
//				val: 	valor u32 para ser escrito no buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void write_LE32u (volatile u8 *pmem, u32 val) {
    pmem[0] = ((u8 *)&val)[0];
    pmem[1] = ((u8 *)&val)[1];
    pmem[2] = ((u8 *)&val)[2];
    pmem[3] = ((u8 *)&val)[3];
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função é para ler um valor u16 de um buffer u8, em uma plataforma com processador little endian
// Parametros:	Ponteiro do buffer u8
// Retorna:		Valor u16
//------------------------------------------------------------------------------------------------------------------
u16 read_LE16u (volatile u8 *pmem) {
    u16 val;

    ((u8 *)&val)[0] = pmem[0];
    ((u8 *)&val)[1] = pmem[1];

    return (val);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função escreve um valor u16 em um buffer u8, em uma plataforma com processador little endian
// Parametros: 	pmem:   Pointeiro do buffer u8
//				val: 	valor u16 para ser escrito no buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void write_LE16u (volatile u8 *pmem, u16   val) {
    pmem[0] = ((u8 *)&val)[0];
    pmem[1] = ((u8 *)&val)[1];
}
#endif


#if (UTIL_USE_ACCESS_BUF_BE == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função é para ler um valor u32 de um buffer u8, em uma plataforma com processador big endian
// Parametros:	Ponteiro do buffer u8
// Retorna:		Valor u32
//------------------------------------------------------------------------------------------------------------------
u32 read_BE32u (volatile u8 *pmem) {
    u32 val;

    ((u8 *)&val)[0] = pmem[3];
    ((u8 *)&val)[1] = pmem[2];
    ((u8 *)&val)[2] = pmem[1];
    ((u8 *)&val)[3] = pmem[0];

    return (val);
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função escreve um valor u32 em um buffer u8, em uma plataforma com processador big endian
// Parametros: 	pmem:	Pointeiro do buffer u8
//				val: 	valor u32 para ser escrito no buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void write_BE32u (volatile u8 *pmem, u32 val) {
    pmem[0] = ((u8 *)&val)[3];
    pmem[1] = ((u8 *)&val)[2];
    pmem[2] = ((u8 *)&val)[1];
    pmem[3] = ((u8 *)&val)[0];

}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função é para ler um valor u16 de um buffer u8, em uma plataforma com processador big endian
// Parametros:	Ponteiro do buffer u8
// Retorna:		Valor u16
//------------------------------------------------------------------------------------------------------------------
u16 read_BE16u (volatile u8 *pmem) {
    u16  val;

    ((u8 *)&val)[0] = pmem[1];
    ((u8 *)&val)[1] = pmem[0];

    return (val);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Esta função escreve um valor u16 em um buffer u8, em uma plataforma com processador big endian
// Parametros: 	pmem:   Pointeiro do buffer u8
//				val: 	valor u16 para ser escrito no buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void write_BE16u (volatile u8 *pmem, u16 val) {
    pmem[0] = ((u8 *)&val)[1];
    pmem[1] = ((u8 *)&val)[0];
}
#endif

//#include <ctype.h>
//double xatof(char *s) // arrumar a conversão do número negativo, pois passa para positivo
//{
//	double a = 0.0;
//	int e = 0;
//	int c;
//	while ((c = *s++) != '\0' && isdigit(c)) {
//		a = a*10.0 + (c - '0');
//	}
//	if (c == '.') {
//		while ((c = *s++) != '\0' && isdigit(c)) {
//			a = a*10.0 + (c - '0');
//			e = e-1;
//		}
//	}
//	if (c == 'e' || c == 'E') {
//		int sign = 1;
//		int i = 0;
//		c = *s++;
//		if (c == '+')
//			c = *s++;
//		else if (c == '-') {
//			c = *s++;
//			sign = -1;
//		}
//		while (isdigit(c)) {
//			i = i*10 + (c - '0');
//			c = *s++;
//		}
//		e += i*sign;
//	}
//	while (e > 0) {
//		a *= 10.0;
//		e--;
//	}
//	while (e < 0) {
//		a *= 0.1;
//		e++;
//	}
//	return a;
//}
