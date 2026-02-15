#ifndef PRINTF_H
#define PRINTF_H   
/**
 * Simple printf implementation for AM335x bare-metal applications.
 * Supports %d, %u, %x, %s, and %% format specifiers.
 * Uses UART0 for output.
 */
#include <stdarg.h>
#include <stdint.h>
#include "debug_uart.h"

void print_hex(uint32_t val);

void print_dec(int val);

void printf(const char *fmt, ...);

#endif /* PRINTF_H */