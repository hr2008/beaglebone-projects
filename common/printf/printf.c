#include <stdarg.h>
#include <stdint.h>
#include "printf.h"

void print_hex(uint32_t val)
{
    char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4)
        uart_putc(hex[(val >> i) & 0xF]);
}

void print_dec(int val)
{
    char buf[12];
    int i = 0;

    if (val == 0) {
        uart_putc('0');
        return;
    }

    if (val < 0) {
        uart_putc('-');
        val = -val;
    }

    while (val) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i--)
        uart_putc(buf[i]);
}

static void print_udec(unsigned long v)
{
    char buf[16];
    int i = 0;

    if (v == 0) {
        uart_putc('0');
        return;
    }

    while (v) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }

    while (i--)
        uart_putc(buf[i]);
}

static void print_ptr(void *p)
{
    uart_puts("0x");
    print_hex((uint32_t)p);
}


static void print_ldec(long v)
{
    if (v < 0) {
        uart_putc('-');
        v = -v;
    }
    print_udec((unsigned long)v);
}
void printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {

        if (*fmt != '%') {
            uart_putc(*fmt++);
            continue;
        }

        fmt++;

        int long_flag = 0;

        /* handle length modifier */
        if (*fmt == 'l') {
            long_flag = 1;
            fmt++;
        }

        switch (*fmt++) {

        case 's':
            uart_puts(va_arg(ap, char *));
            break;

        case 'c':
            uart_putc((char)va_arg(ap, int));
            break;

        case 'x':
            print_hex(va_arg(ap, uint32_t));
            break;

        case 'd':
            if (long_flag)
                print_ldec(va_arg(ap, long));
            else
                print_ldec(va_arg(ap, int));
            break;

        case 'u':
            if (long_flag)
                print_udec(va_arg(ap, unsigned long));
            else
                print_udec(va_arg(ap, unsigned int));
            break;

        case 'p':
            print_ptr(va_arg(ap, void *));
            break;

        default:
            uart_putc('?');
            break;
        }
    }

    va_end(ap);
}
