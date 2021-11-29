#pragma once

#include <avr/interrupt.h>
#include "bithelper.h"
#include <stdbool.h>
#include "types.h"

#define IRQD_NUM_SUBSCRIPTIONS 12

typedef struct {
	u8 gpio_pin;
	void(*callback)(u8 GPIO_Pin);
} Subscription;

struct irqdispatcher {
	void(*init)(void);
	bool(*subscribe)(u8 GPIO_Pin, void* handler);
	bool(*unsubscribe)(u8 GPIO_Pin);
};

extern const struct irqdispatcher IRQDispatcher;