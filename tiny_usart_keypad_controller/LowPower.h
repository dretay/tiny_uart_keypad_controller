#pragma once

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

struct lowpower {
	void (*all_pins_to_pullup)(void);
	void (*disable_perhiperals)(void);
	void (*power_down)(void);
	void (*loop)(void);
};

extern const struct lowpower LowPower;
