/*
 * TODO: disable undneeded stuff in the PRR (USI, possibly combine usart & debounce timers and disable timer1?)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/sleep.h>


#include "debounce.h"
//#include "ring_buffer.h"
#include "uart.h"

//button stuff
#define debounce 3

volatile uint8_t last_state_b;
volatile uint8_t last_state_d;

volatile uint16_t button_state;
void button_click(uint8_t index,uint8_t state);

//uart
#define BAUD 9400
#define MYUBBR ((F_CPU / (BAUD * 16L)) - 1)

//TODO - when in powerdown these will wake me back up
 ISR(PCINT0_vect){
 }
 ISR(PCINT2_vect) {
 }
 ISR(TIMER1_COMPA_vect){
	uint8_t b_temp,d_temp;
	
	b_temp = last_state_b^PINB;
	last_state_b = PINB;
	
	d_temp = last_state_d^PIND;
	last_state_d = PIND;

	if ((b_temp & _BV(0)))
		signalChangedState(0,debounce);
	if ((b_temp & _BV(1)))
		signalChangedState(1,debounce);
	if ((b_temp & _BV(2)))
		signalChangedState(2,debounce);
	if ((b_temp & _BV(3)))
		signalChangedState(3,debounce);	
	if ((b_temp & _BV(4)))
		signalChangedState(4,debounce);
		
	if ((d_temp & _BV(2)))
		signalChangedState(5,debounce);
	if ((d_temp & _BV(3)))
		signalChangedState(6,debounce);
	if ((d_temp & _BV(4)))
		signalChangedState(7,debounce);
	if ((d_temp & _BV(5)))
		signalChangedState(8,debounce);
	if ((d_temp & _BV(6)))
		signalChangedState(9,debounce);

 }

int main(void){
	//configure all pins as pull-up inputs
	DDRB = 0x00;
	PORTB = 0xff;
	DDRA = 0x00;
	PORTA = 0xff;
	DDRD = 0x00;
	PORTD = 0xff;
	
	//this uses tccr0
	initializeDebouncerTimer();
	
	registerDebouncer(&PINB,PORTB0,0,1,&button_click);
	registerDebouncer(&PINB,PORTB1,1,1,&button_click);
	registerDebouncer(&PINB,PORTB2,2,1,&button_click);
	registerDebouncer(&PINB,PORTB3,3,1,&button_click);
	registerDebouncer(&PINB,PORTB4,4,1,&button_click);	
	registerDebouncer(&PIND,PIND2,5,1,&button_click);
	registerDebouncer(&PIND,PIND3,6,1,&button_click);
	registerDebouncer(&PIND,PIND4,7,1,&button_click);
	registerDebouncer(&PIND,PIND5,8,1,&button_click);
	registerDebouncer(&PIND,PIND6,9,1,&button_click);

	//enable pin change interrupts in the general mask register
	GIMSK|= (1 << PCIE0) | (1 << PCIE2);

	//enable pci for relevant port b (pcmsk) and port d (pcmsk2) pins
	PCMSK |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2)| (1 << PCINT3)| (1 << PCINT4);
	PCMSK2 |= (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4)| (1 << PCINT5)| (1 << PCINT6);

	//setup the initial state to track button presses
	last_state_b = PINB;
	last_state_d = PIND;
	button_state = 0;

	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS10);
	
	// initialize counter
	TCNT1 = 0;
	
	// initialize compare value
	OCR1A = 2500;//5020;
	
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);

	//enable uart	
	init_uart();
	
	//disable analog comparator
	ACSR |= (1<<ACD);
	
	//disable the usi
	PRR |= (1<<PRUSI);
	
	//enable interrupts
	sei();
    
    while (1){
		//todo: if some timeout has been hit then we should shutdown rather than idle here
		//let the pci interrupt wake us back up
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
    }
}

void button_click(uint8_t index,uint8_t state){	
	button_state ^= _BV(index);	

	//add((button_state) & 0XFF);
	uart_putc((button_state >> 8) & 0XFF);
	uart_putc((button_state ) & 0XFF);
		
	// enable uart data interrupt (send data)
	UCSRB |= (1<<UDRIE);	
}

