#include "debounce.h"


ISR(TIMER0_OVF_vect)
{
	u8 i;
	u8 temp;
	u8 workDone = 0;

	//Cycle through all debounced items
	for (i=0;i<DBNC_NUM_DEBOUNCERS;i++)
	{
		//Skip items that have been idle
		if (db.dbUnits[i].counter == 0)
		continue;

		workDone = 1;

		//If debounce period has elapsed
		if (--db.dbUnits[i].counter == 0)
		{
			
			//Grab current logical bit state of the port (1 or 0)
			temp = (((*(db.dbUnits[i].port)) & _BV((db.dbUnits[i].bitmap & 0b111))) != 0);

			//If current state != last state
			//store change
			if (temp != ((db.dbUnits[i].bitmap & _BV(5)) >> 5))
			{
				//Flip last state bit
				db.dbUnits[i].bitmap ^= _BV(5);

				//If this debouncer item is synchronous
				//Then signal it
				if (!(db.dbUnits[i].bitmap & _BV(7))) {

					//Signal this debouncer item
					db.dbUnits[i].bitmap |= _BV(6);

					//Signaling options
					db.signalReady = 1;
				}

				//Otherwise it's asynchronous,
				//call immediately
				else
				//Call Handler
				(*db.dbUnits[i].handler)(i,temp);
			}
		}
	}

	//If all counters were already 0, disable the timer
	if (!workDone)
	TCCR0B = 0;

	TCNT0  = DBNC_TIMR0_BASEVAL;
}


//Call any signaled handlers (to be executed in main program loop)
void callSignaledHandlers(void)
{
	u8 i;

	if (!db.signalReady) return;
	
	for (i=0;i<DBNC_NUM_DEBOUNCERS;i++)
	{
		//Check if this item is signaled
		if (db.dbUnits[i].bitmap & _BV(6)) {

			//If so, reset its signal
			db.dbUnits[i].bitmap &= ~_BV(6);

			//Call item and pass on last known state
			(*db.dbUnits[i].handler)(i,(db.dbUnits[i].bitmap & _BV(5))>>5);
		}
	}

	//Reset signal
	db.signalReady = 0;

}

void registerDebouncer(volatile u8 *port ,u8 bit,uint8_t index, u8 Asynchronous, debounceHandler handler)
{
	//Store port pointer
	//Store handler pointer
	//Reset counter to 0
	//Store bitmap of bit offset/asynchronous state
	//Set signaled to 0
	db.dbUnits[index].port 		= port;
	db.dbUnits[index].handler 	= handler;
	db.dbUnits[index].counter  	= 0;

	db.dbUnits[index].bitmap	= _BITSHIFTBY((Asynchronous != 0),7)|
						 		  _BITSHIFTBY((((*port) & _BV(bit)) != 0),5)|
						 		  bit;
}
void signalChangedState(u8 index)
{
	db.dbUnits[index].counter = DBNC_COUNTER_TOP;	
	if (!TCCR0B)
	TCCR0B = DBNC_TIMR0_PRESCALER;
}


void initializeDebouncerTimer()
{
	//Note: doesn't start timer
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	TCNT0  = DBNC_TIMR0_BASEVAL;
	TIMSK = _BV(TOIE0);
}


