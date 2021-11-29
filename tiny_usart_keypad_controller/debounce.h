#pragma once
//based on https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "types.h"

//Optimized for 20 millisecond period on a 1Mhz clock
#define DBNC_TIMR0_PRESCALER 	_BV(CS02)
#define DBNC_TIMR0_BASEVAL	 	178

//Defines the size of the debouncer handler arrays (i.e. 4 for a maximum of four)
#define DBNC_NUM_DEBOUNCERS		12

//Defines the number of timer ticks before a value is considered legitimate
//This will define a maximum debounce time of approx 100 milliseconds @ 5
//The minimum debounce time will be approx 80 milliseconds
#define DBNC_COUNTER_TOP 3


#define _BITSHIFTBY(bit,num) ((bit)<<(num))


typedef void(*debounceHandler)(uint8_t,uint8_t);

typedef struct
{
	//A pointer to a volatile port (I/O or otherwise)
	volatile u8 *port;

	//A pointer to a debounceHandler function
	debounceHandler handler;

	//This is the decremental counter which determines
	//if the button has been debounced
	u8 counter;

	/*

		Bits 0-3: bit index to check against (0-7)
		Bits 4-5: unused
		Bit    5: Last known sate
		Bit    6: Signaled
		Bit    7: Asynchronous

	*/
	u8 bitmap;
} DBNC_ITEM;

typedef struct
{

	//An array of debouncer units
	DBNC_ITEM dbUnits[DBNC_NUM_DEBOUNCERS];

	//This is set to 1 when any signal in the dbncSignaled array has been set
	u8 signalReady;

} DBNC_GLOBAL;

volatile DBNC_GLOBAL db;


/*
	Forward Declarations
*/


//ISR for timer0 overflow interrupt
ISR(TIMER0_OVF_vect);
void callSignaledHandlers(void);
void registerDebouncer(volatile u8 *port,u8 bit,u8 index,u8 Asynchronous,debounceHandler handler);
void signalChangedState(u8 index);
void initializeDebouncerTimer();


