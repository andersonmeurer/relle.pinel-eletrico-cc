/*
 *  O RTC já considera a contagem do ano bissexto e respeita o última dia de cada mes
 *  Podemos ajustar qualquer horário mesmo que seja absurdo como dia 31 do mês de fevereiro,
 *  mas o valor é corrigido pelo hardware para a próxima data válida na virada do dia
 * */

#include "rtc.h"

#if (RTC_USE_INT == pdON)
typedef void (*void_pFunc)(void);
static volatile void_pFunc rtc_IntFunctions[RTC_NUM_INTERRUPTS];
void rtc_ISR(void);
#endif

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o RTC, mas a contagem do tempo esta parada. Utilize o rtc_Start para iniciar a contagem
// Parametros:	Nenhum
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// -------------------------------------------------------------------------------------------------------------------
int rtc_Init(void) {
	// Ligar circuito do timer. Tem que ser antes da configuração,
	//	pois após ligar o periféricos, seus regs assumem valores padrões
	PCONP |= PCRTC;
	// RTC parado
	CCR = 0;

	// SELECIONA A FONTE DE CLOCK RTC
	#if defined (arm7tdmi)
	#if (RTC_USE_CLOCK_EXTERNAL == pdON) && defined(LPC2468)
	// Seleciona o oscilador externo como fonte de clock do RTC
		CCR |= CLKSRC;
    #else
    	PREINT   = (PCLK/32768)-1;
    	PREFRAC  = PCLK - ((PREINT+1)*32768);    	
    #endif
	#endif
    
    AMR = AMRMASK; // Nenhum dos integrantes do tempo é usado para comparação para o alarme
    CIIR = IMNO;	// Nenhum dos integrantes do tempo é usado para gerar a interrupção
    ILR = (RTCCIF|RTCALF); // Limpando FLAGS de interrupção do RTC e ALARM
	
   	#if (RTC_USE_INT == pdON)
   	u8 int_num;
	for(int_num = 0; int_num < RTC_NUM_INTERRUPTS; int_num++)	// Faça para todas as interrupções do RTC
		rtc_ClearIntHandler(int_num);							// Retira todas as funções anexadas em suas respecticas interrupções
  	
	#if defined (arm7tdmi)
	if (!irq_Install(VIC_RTC, rtc_ISR, RTC_PRIO_LEVEL))
   		 return errIRQ_INSTALL;
	#endif
	#if defined (cortexm3)
	#if defined (cortexm3)
	nvic_enableIRQ(VIC_RTC);
	nvic_setPriority(VIC_RTC, RTC_PRIO_LEVEL);
	#endif
	#endif
   	#endif

   	CCR |= CLKEN; // Habilita Clock RTC

   	return pdPASS;
} 

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa contagem de tempo
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_Start( void ) {    
	CCR &= ~(CLKEN | CTCRST);
    CCR |= CLKEN;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Para a contagem de tempo
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_Stop( void ) {
    CCR |= ~(CLKEN | CTCRST); 
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Zera o clock
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_ResetCTC( void ) {
    CCR |= CTCRST;
}


void rtc_Sleep(void) {
  	CCR |= CLKEN;												// Habilita Clock RTC

  	#if (RTC_USE_CLOCK_EXTERNAL == pdON) && defined(LPC2468)
  		CCR |= CLKSRC;											// Seleciona o oscilador externo como fonte de clock do RTC
  	#endif

  	PCONP &= ~PCRTC;											// Desliga o timer no controle de potencia
}

void rtc_Wake(void) {
  	CCR |= CLKEN;												// Habilita Clock RTC

	#if (RTC_USE_CLOCK_EXTERNAL == pdON) && defined(LPC2468)
  		CCR |= CLKSRC;											// Seleciona o oscilador externo como fonte de clock do RTC
  	#endif

  	PCONP |= PCRTC;												// Ligar o timer no controle de potencia
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta data e hora
// Parametros:	Time   Time structure with RTC time to be set:
//      		struct {
//        			int sec;     	Second value - [0,59] 
//       			int min;     	Minute value - [0,59]
//        			int hour;    	Hour value - [0,23]
//        			int day;    	Day of the month value - [1,31]
//        			int month;     	Month value - [1,12]
//        			int year;    	Year value - [0,4095]
//        			int w_day;    	Day of week value - [0,6]
//        			int y_day;    	Day of year value - [1,365]
//      		} tRTC;
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_SetTime( tRTC *rtc ) {
  	ENTER_CRITICAL();			// Procedimento de entrada critica quando usado sistema operacional

  	CCR &= ~CLKEN; 				// Desabilita o clock do RTC

    SEC 	= rtc->sec;
    MIN 	= rtc->min;
    HOUR 	= rtc->hour;
    DOW 	= rtc->wday;
    DOM 	= rtc->mday;
    MONTH 	= rtc->month;
    YEAR 	= rtc->year;    

  	CCR |=  CLKEN;				// Habilita o clock do RTC

 	EXIT_CRITICAL();			// Procedimento de saida critica quando usado sistema operacional
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura data e hora atual
// Parametros:	Nenhum
// Retorna:		Time structure with current time counter values:
//		      		struct {
//        				int sec;    Second value - [0,59] 
//        				int min;    Minute value - [0,59]
//        				int hour;   Hour value - [0,23]
//        				int day;    Day of the month value - [1,31]
//        				int month;  Month value - [1,12]
//        				int year;   Year value - [0,4095]
//        				int w_day;  Day of week value - [0,6]
//        				int y_day;  Day of year value - [1,365]
//      			} tRTC;
// -------------------------------------------------------------------------------------------------------------------
bool rtc_GetTime( tRTC *rtc) {
	u32 d, t;

 	ENTER_CRITICAL(); // Procedimento de entrada critica quando usado sistema operacional
	t = CTIME0;
	d = CTIME1;

	rtc->sec = t & 0x3f;
	rtc->min = (t >> 8) & 0x3f;
	rtc->hour = (t >> 16) & 0x1f;
	rtc->wday = (t >> 24) & 0x7;
	rtc->mday = d & 0x1f;
	rtc->month = (d >> 8) & 0xf;
	rtc->year = (d >> 16) & 0xfff;
	EXIT_CRITICAL(); 		// Procedimento de saida critica quando usado sistema operacional

	return pdTRUE;
}

#if (RTC_USE_INT == pdON)
// -----------------------------------------------------------------------
// Descrição: 	Anexa uma routina ao vetor de interrupções
// Parametros:	int_num. Indece do vetor de uma determinada interrupção do RTC
//				RTC_INT_TIME: Interrupção de tempo
//				RTC_INT_ALARM: Interrupção do alarme
//				int_func
//					Ponteiro da rotina que tratará a interrupção do timer. 
//						Ex	rtc_SetIntHandler(RTC_VECTOR_INT, myOverflowFunction);
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void rtc_SetIntHandler(u8 int_num, void (*int_func)(void) ) {
	if(int_num < RTC_NUM_INTERRUPTS)
		rtc_IntFunctions[int_num] = int_func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina da inetrrupçao desejada do vetor de interrupções
// Parametros:	int_num. Indece do vetor de uma determinada interrupção de timer
//				RTC_INT_TIME: Interrupção de tempo
//				RTC_INT_ALARM: Interrupção do alarme
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void rtc_ClearIntHandler(u8 int_num) {
	if(int_num < RTC_NUM_INTERRUPTS)
		rtc_IntFunctions[int_num] = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta quais os resgistradores serão usados para gerar a interrupção
// Parametros:	Habilita os parametros para geração do alarme
//					(IMNO) / ( ( IMSEC | IMMIN | IMHOUR | IMDOM | IMDOW | IMDOY | IMMON | IMYEAR ) )
//						IMNO: Interrupção desligada
//						IMSEC: Interruoção gerada a cada segundos
//						IMMIN: Interruoção gerada a cada minuto
//						IMHOUR: Interruoção gerada a cada hora
//						IMDOM: Interruoção gerada a cada dia do mês
//						IMDOW: Interruoção gerada a cada dia da semana
//						IMDOY: Interruoção gerada a cada dia do ano
//						IMMON: Interruoção gerada a cada mês
//						IMYEAR: Interruoção gerada a cada ano
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_RegInt( u8 reg_int ) {
    CIIR = reg_int;    
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta quais os resgistradores serão usados para gerar a interrupção para o alarme
// Parametros:	Mascara de alarme. Habilita os parametros para geração do alarme
//					(AMRNO) 	| ( AMRSEC | AMRMIN | AMRHOUR | AMRDOM | AMRDOW | AMRDOY | AMRMON | AMRYEAR )
//    	             	Bit 0: Second value is used
//      	           	Bit 1: Minutes value is used
//            		    Bit 2: Hour value is used
//                 		Bit 3: Day of Month value is used
//                 		Bit 4: Day of Week value is used
//                 		Bit 5: Day of year value is used
//                 		Bit 6: Month value is used
//                 		Bit 7: Year value is used
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_RegIntAlarm ( u8 reg_int_alarm ) {
    AMR = ~reg_int_alarm;    
}

//###################################################################################################################
// SERVIÇO DE INT
//###################################################################################################################
//###################################################################################################################
void rtc_ISR(void) {
    ISR_ENTRY;															// Procedimento para entrada da interrupção
	if (ILR & RTCCIF) {													// Checa se foi int do RTC
		ILR|=RTCCIF;													// Limpo flag interupção
		if(rtc_IntFunctions[RTC_INT_TIME]) {							// Checo se existe uma função anexada ao Handler de int
			rtc_IntFunctions[RTC_INT_TIME]();							// Chama a função anexada ao Handler de int
		}
	} 
	
	if (ILR & RTCALF) {													// Checa se foi interrupção do alarme
		ILR|=RTCALF;													// Limpo flag interupção
		if(rtc_IntFunctions[RTC_INT_ALARM]) {							// Checo se existe uma função anexada ao Handler de int
			rtc_IntFunctions[RTC_INT_ALARM]();							// Chama a função anexada ao Handler de int
		}
	}
		
	// Limpa interrupção
	#if defined(arm7tdmi)
	VICSoftIntClr = (1<<VIC_RTC);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_RTC);
	#endif

    ISR_EXIT;															// Procedimento para saida da interrupção    
}
#endif

