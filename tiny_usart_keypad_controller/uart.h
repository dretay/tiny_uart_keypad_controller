#pragma once

#include <stdio.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "types.h"

#define UART_NO_DATA 0x0100

// Initialize UART to 9600 baud with 8N1.
void init_uart(void);

// Send and receive functions, that run without ISRs
void send_uart(uint8_t c);
