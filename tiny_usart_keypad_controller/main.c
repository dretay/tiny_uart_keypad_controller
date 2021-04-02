#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/sfr_defs.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdbool.h>

#include "debounce.h"
#include "uart.h"
#include "atmega-timers.h"
#include "types.h"
#include "bithelper.h"

//function prototypes
void button_handler(uint8_t index,uint8_t state);
bool is_idle(void);
void power_down(void);

// timer ticks before a debounced value is considered legitimate
#define DEBOUNCE_TICKS 3


// PCI0 will trigger if any enabled PCINT7..0 pin toggles. (everything on port B)
volatile u8 pcint0_last_state;
ISR(PCINT0_vect){
	u8 temp = pcint0_last_state^PINB;
	pcint0_last_state = PINB;
	if ((temp & _BV(0))){
		signalChangedState(0,DEBOUNCE_TICKS); //B0
	}
	if ((temp & _BV(1))){
		signalChangedState(1,DEBOUNCE_TICKS); //B1
	}
	if ((temp & _BV(2))){
		signalChangedState(2,DEBOUNCE_TICKS); //B2
	}
	if ((temp & _BV(3))){
		signalChangedState(3,DEBOUNCE_TICKS); //B3
	}
	if ((temp & _BV(4))){
		signalChangedState(4,DEBOUNCE_TICKS); //B4
	}
}
//PCI2 will trigger, if any enabled PCINT17..11 pin toggles (everything on port D)
volatile u8 pcint2_last_state;
ISR(PCINT2_vect) {
	u8 temp = pcint2_last_state^PIND;
	pcint2_last_state = PIND;
	if ((temp & _BV(2))){
		signalChangedState(5,DEBOUNCE_TICKS);//D2
	}
	if ((temp & _BV(3))){
		signalChangedState(6,DEBOUNCE_TICKS);//D3
	}
	if ((temp & _BV(4))){
		signalChangedState(7,DEBOUNCE_TICKS);//D4
	}
	if ((temp & _BV(5))){
		signalChangedState(8,DEBOUNCE_TICKS);//D5
	}
	if ((temp & _BV(6))){
		signalChangedState(9,DEBOUNCE_TICKS);//D6
	}
}
//PCI1 will trigger if any enabled PCINT10..8 pin toggles. (everything on port A)
volatile u8 pcint1_last_state;
ISR(PCINT1_vect) {
	u8 temp = pcint1_last_state^PINA;
	pcint1_last_state = PINA;
	if ((temp & _BV(0))){
		signalChangedState(10,DEBOUNCE_TICKS);//A0
	}
	if ((temp & _BV(1))){
		signalChangedState(11,DEBOUNCE_TICKS);//A1
	}
}
bool is_idle(void){
	return (TCCR0B == 0 && ( UCSRA & (1<<UDRE)) );
}
void power_down(void){
	//http://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//go to sleep
	sleep_enable();
	sei();
	sleep_cpu();
	
	//wake up
	sleep_disable();
}

int main(void){
	//configure all pins as pull-up inputs
	DDRB = 0x00;
	PORTB = 0xff;
	DDRA = 0x00;
	PORTA = 0xff;
	DDRD = 0x00;
	PORTD = 0xff;

	//the ble irq pin d0 to be an output
	DDRD |= 1<<PORTD0;
	
	//this uses tccr0 (timer0)
	initializeDebouncerTimer();
	
	//register all pins that will be used as buttons
	u8 async_events = true;
	registerDebouncer(&PINB,PORTB0,0,async_events,&button_handler);
	registerDebouncer(&PINB,PORTB1,1,async_events,&button_handler);
	registerDebouncer(&PINB,PORTB2,2,async_events,&button_handler);
	registerDebouncer(&PINB,PORTB3,3,async_events,&button_handler);
	registerDebouncer(&PINB,PORTB4,4,async_events,&button_handler);
	
	registerDebouncer(&PIND,PIND2,5,async_events,&button_handler);
	registerDebouncer(&PIND,PIND3,6,async_events,&button_handler);
	registerDebouncer(&PIND,PIND4,7,async_events,&button_handler);
	registerDebouncer(&PIND,PIND5,8,async_events,&button_handler);
	registerDebouncer(&PIND,PIND6,9,async_events,&button_handler);
	
	registerDebouncer(&PINA,PINA0,10,async_events,&button_handler);
	registerDebouncer(&PINA,PINA1,11,async_events,&button_handler);

	//enable pin change interrupts in the general mask register
	GIMSK |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
	
	//enable pci for relevant port b (pcmsk), port a(pcmsk1), and port d (pcmsk2) pins
	PCMSK |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2)| (1 << PCINT3)| (1 << PCINT4);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9); //porta
	PCMSK2 |= (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15)| (1 << PCINT16)| (1 << PCINT17);

	//enable uart
	init_uart();
	
	//disable analog comparator
	ACSR |= (1<<ACD);
	
	//disable timer1
	PRR |= (1<<PRTIM1);
	
	//disable the usi
	PRR |= (1<<PRUSI);
	
	//disable watchdog timer
	wdt_disable();
		
	//enable interrupts
	sei();
	
	while (1){
		if(is_idle()){
			power_down();
		}
	}
}

//callback once button click has been debounced
void button_handler(uint8_t index,uint8_t state){
	static uint16_t button_state;
	if(state == 1){
		button_state &= ~(1<<index);
	}
	else{
		button_state |= 1<<index;
	}

	send_uart(highByte(button_state));
	send_uart(lowByte(button_state));
}

