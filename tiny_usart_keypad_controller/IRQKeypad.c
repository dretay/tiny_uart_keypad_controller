#include "IRQKeypad.h"

static void init(void* button_handler){
	//this uses tccr0 (timer0)
	initializeDebouncerTimer();
	
	//register all pins that will be used as buttons
	u8 async_events = true;
	registerDebouncer(&PINB,PORTB0,0,async_events,button_handler);
	registerDebouncer(&PINB,PORTB1,1,async_events,button_handler);
	registerDebouncer(&PINB,PORTB2,2,async_events,button_handler);
	registerDebouncer(&PINB,PORTB3,3,async_events,button_handler);
	registerDebouncer(&PINB,PORTB4,4,async_events,button_handler);
		
	registerDebouncer(&PIND,PIND2,5,async_events,button_handler);
	registerDebouncer(&PIND,PIND3,6,async_events,button_handler);
	registerDebouncer(&PIND,PIND4,7,async_events,button_handler);
	registerDebouncer(&PIND,PIND5,8,async_events,button_handler);
	registerDebouncer(&PIND,PIND6,9,async_events,button_handler);
		
	registerDebouncer(&PINA,PINA0,10,async_events,button_handler);
	registerDebouncer(&PINA,PINA1,11,async_events,button_handler);
	
	IRQDispatcher.init();
	IRQDispatcher.subscribe(0, &signalChangedState);
	IRQDispatcher.subscribe(1, &signalChangedState);
	IRQDispatcher.subscribe(2, &signalChangedState);
	IRQDispatcher.subscribe(3, &signalChangedState);
	IRQDispatcher.subscribe(4, &signalChangedState);
	IRQDispatcher.subscribe(5, &signalChangedState);
	IRQDispatcher.subscribe(6, &signalChangedState);
	IRQDispatcher.subscribe(7, &signalChangedState);
	IRQDispatcher.subscribe(8, &signalChangedState);
	IRQDispatcher.subscribe(9, &signalChangedState);
	IRQDispatcher.subscribe(10, &signalChangedState);
	IRQDispatcher.subscribe(11, &signalChangedState);
}
const struct irqkeypad IRQKeypad = {
	.init = init,
};