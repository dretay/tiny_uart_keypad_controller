/* -----------------------------------------------------------------------
 * uart.c
 * baudrate 9600
 * adapted to ATtiny2313
 */


#include "uart.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0 
#endif

//#define BAUD 19200
#define BAUD 4800
//#define BAUD 9600
// 4.096MHz
//  4800: 52.3333333
//  9600: 25.6666667
// 14400: 16.7777778
// 19600: 12.06
// 28800: 7.8889
// 38400: 5.6667

#define MYUBBR ((F_CPU / (BAUD * 16L)) - 1)

/*
 * init_uart
 */
void init_uart(void) {
  // set baud rate
  UBRRH = (uint8_t)(MYUBBR >> 8); 
  UBRRL = (uint8_t)(MYUBBR);
  // enable transmit
  UCSRB = (1 << TXEN);
  // set frame format
  UCSRC = (1 << USBS) | (3 << UCSZ0);	// asynchron 8n1
}



/*
 * send_uart
 * Sends a single char to UART without ISR
 */
void send_uart(u8 c) {
  // wait for empty data register
  while (!(UCSRA & (1<<UDRE)));
  // set data into data register
  UDR = c;
}

