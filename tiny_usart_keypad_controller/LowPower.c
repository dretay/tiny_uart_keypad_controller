#include "LowPower.h"

static void all_pins_to_pullup(void){
	//configure all pins as pull-up inputs
	DDRB = 0x00;
	PORTB = 0xff;
	DDRA = 0x00;
	PORTA = 0xff;
	DDRD = 0x00;
	PORTD = 0xff;
}
static void disable_perhiperals(void){
	//disable analog comparator
	ACSR |= (1<<ACD);
	
	//disable timer1
	PRR |= (1<<PRTIM1);
	
	//disable the usi
	PRR |= (1<<PRUSI);
	
	//disable watchdog timer
	wdt_disable();
}
static void power_down(void){
	//http://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//go to sleep
	sleep_enable();
	sei();
	sleep_cpu();
		
	//wake up
	sleep_disable();	
}
static bool is_idle(void){
	return (TCCR0B == 0 && ( UCSRA & (1<<UDRE)) );
}

static void loop(){
	while (1){
		if(is_idle()){
			//power_down();
		}
	}
}

const struct lowpower LowPower = {
	.loop = loop,
	.power_down = power_down,
	.all_pins_to_pullup = all_pins_to_pullup,
	.disable_perhiperals = disable_perhiperals
};