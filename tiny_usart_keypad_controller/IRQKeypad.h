#pragma once

#include <stdbool.h>

#include "types.h"
#include "debounce.h"
#include "IRQDispatcher.h"

struct irqkeypad {
	void (*init)(void* button_handler);
};

extern const struct irqkeypad IRQKeypad;