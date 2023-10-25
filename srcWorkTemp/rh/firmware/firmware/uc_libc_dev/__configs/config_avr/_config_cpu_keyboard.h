#ifndef __CONFIG_CPU_KEYBOARD_H
#define __CONFIG_CPU_KEYBOARD_H

#include "_config_cpu_.h"

// ###################################################################################################################
// KEYPAD 4x4
#define KEYPAD_INT				INT1
#define KEYPAD_BUFFER_SIZE		4		// Numero de bytes para o buffer TX da Uart0

#define KEYPAD_COL_PORT_CONFIG	DDRB
#define KEYPAD_COL_PORT_IN		PINB
#define KEYPAD_COL_PORT_OUT		PORTB
#define KEYPAD_COL_PIN1			PB0
#define KEYPAD_COL_PIN2			PB1
#define KEYPAD_COL_PIN3			PB2
#define KEYPAD_COL_PIN4			PB3

#define KEYPAD_ROW_PORT_CONFIG	DDRB
#define KEYPAD_ROW_PORT_IN		PINB
#define KEYPAD_ROW_PORT_OUT		PORTB
#define KEYPAD_ROW_PIN1			PB4
#define KEYPAD_ROW_PIN2			PB5
#define KEYPAD_ROW_PIN3			PB6
#define KEYPAD_ROW_PIN4			PB7


#endif
