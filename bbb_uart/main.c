#include <stdint.h>
#include "uart.h"
#include "printf.h"

volatile int dummy_bss;
char s[] = "Hello world\n";

int main(void)
{
    uart_init();
    int x = 0;
    while (1)
    {
        printf(s);
        uart_putc('\n');
        uart_putc('\r');
        printf("counter - %d\n\r",x);
        x++;
        for (volatile int i = 0; i < 1000000; i++);
    }
}
