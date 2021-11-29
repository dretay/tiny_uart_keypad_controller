#include "uart.h"
#include "types.h"
#include "IRQKeypad.h"
#include "LowPower.h"

//function prototypes
void button_handler(uint8_t index,uint8_t state);

int main(void){
	LowPower.all_pins_to_pullup();	
	
	init_uart();
	
	IRQKeypad.init(&button_handler);
	
	LowPower.disable_perhiperals();
	LowPower.loop();	

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

