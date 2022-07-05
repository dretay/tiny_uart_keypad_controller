#include "uart.h"
#include "types.h"
#include "IRQKeypad.h"
#include "LowPower.h"
#include "bithelper.h"
#include <avr/portpins.h>#include <avr/io.h>#include <avr/sfr_defs.h>

//function prototypes
void button_handler(uint8_t index,uint8_t state);

struct GPIO_Pin{
	u8 pin;
	volatile u8* ddr;
	volatile u8* port;
	volatile u8* input;
};

int main(void){
	//LowPower.all_pins_to_pullup();	
	
	init_uart();
	
	//IRQKeypad.init(&button_handler);
	//
	//LowPower.disable_perhiperals();
	//LowPower.loop();	
	
	/*
	2,4,7,9 outputs
	1,3,5,6,8 inputs
	
	1,2 set
	2,6 review
	4,6 light
	1,9 hr+
	3,9 min+
	3,4 disp
	2,3 onoff
	4,5 teach
	8,9 hour-
	1,7 min-
	*/
	
	//DDRx = data direction register, 0 for input, 1 for output
	//PORTx = if pin is output then controls high and low, if input then controls pullup
	
	////set pb2 as an output
	//DDRB |= _BV(PB2);
	//
	////set pb2 low
	//PORTB |= ~_BV(PB2);


	//set pd5 as an input
	DDRD |= ~_BV(PD5);	
	//set pullup for pd5
	PORTD |= _BV(PD5);

	//set pd4 as an input
	DDRD |= ~_BV(PD4);
	//set pullup for pd4
	PORTD |= _BV(PD4);

	
	//set rows as outputs with an initial high value
	struct GPIO_Pin rows[4] = {
		{PB0,&DDRB,&PORTB,&PINB},
		{PB1,&DDRB,&PORTB,&PINB},
		{PB2,&DDRB,&PORTB,&PINB},
		{PB3,&DDRB,&PORTB,&PINB}
	};
	for(u8 i=0;i<4;i++){
		struct GPIO_Pin row = rows[i];
		bitSet(*row.ddr,row.pin);
		bitSet(*row.port,row.pin);
	}
	//set rows as inputs with high impedance
	struct GPIO_Pin cols[5] = {
		{PD6,&DDRD,&PORTD,&PIND},
		{PB4,&DDRB,&PORTB,&PINB},
		{PB5,&DDRB,&PORTB,&PINB},
		{PB6,&DDRB,&PORTB,&PINB},
		{PB7,&DDRB,&PORTB,&PINB}
	};
	for(u8 i=0;i<4;i++){
		struct GPIO_Pin col = cols[i];
		bitClear(*col.ddr,col.pin);
		bitSet(*col.port,col.pin);
	}
	
	
	
	while(1){
		for(u8 row_idx=0;row_idx<4;row_idx++){
			struct GPIO_Pin row = rows[row_idx];
			bitClear(*row.port, row.pin);
			for(u8 col_idx=0;col_idx<5;col_idx++){
				struct GPIO_Pin col = cols[col_idx];
				if(bitRead(*col.input,col.pin) == 0){
					send_uart(row_idx+(col_idx*5));			
					while(bitRead(*col.input,col.pin) == 0){}
				}
			}
			bitSet(*row.port, row.pin);
		}
		if(bitRead(PIND,5) == 0){
			send_uart(20);
			while(bitRead(PIND,5) == 0){}
		}
		if(bitRead(PIND,4) == 0){
			send_uart(21);
			while(bitRead(PIND,4) == 0){}
		}
		//send_uart(1);
		////get PB4
		//if (PINB & _BV(PB4)){
			//bitClear(PORTB, PB2);
		//}
		//else{
			//bitSet(PORTB, PB2);
		//}
	}
}

//callback once button click has been debounced
void button_handler(u8 index,u8 state){
	static u16 button_state;
	if(state == 1){
		button_state &= ~(1<<index);
	}
	else{
		button_state |= 1<<index;
	}

	send_uart(highByte(button_state));
	send_uart(lowByte(button_state));
}

