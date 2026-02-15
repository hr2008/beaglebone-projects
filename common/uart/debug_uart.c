/**
 * BBB UART Driver Implementation
 */


#include "debug_uart.h"

/* ---------- MMIO helpers ---------- */
static inline void mmio_write(uint32_t reg, uint32_t val)
{
    *(volatile uint32_t *)reg = val;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t *)reg;
}

static inline void mmio_write8(uint32_t reg, uint8_t val)
{
    *(volatile uint8_t *)reg = val;
}

/* ---------- UART + clocks ---------- */
#define UART0_BASE     0x44E09000

#define UART_THR        0x00
#define UART_LSR        0x14
#define UART_LCR        0x0C
#define UART_DLL        0x00
#define UART_DLH        0x04
#define UART_FCR        0x08
#define UART_MDR1       0x20
#define UART_SYSC       0x54
#define UART_SYSS       0x58
#define UART_IER        0x04
#define UART_IIR        0x08

#define IER_THRE   (1 << 1)
#define IIR_THRE   0x02
#define LSR_THRE       (1 << 5)

/* ---------- Clock control ---------- */
#define CM_WKUP_BASE               0x44E00400
#define CM_WKUP_UART0_CLKCTRL      0xB4

/* ---------- Pin mux ---------- */
#define CONTROL_MODULE_BASE        0x44E10000
#define CONF_UART0_RXD             0x970
#define CONF_UART0_TXD             0x974

#define UART_TX_BUF_SIZE 1024

static volatile uint8_t tx_buf[UART_TX_BUF_SIZE];
static volatile uint32_t tx_head = 0;
static volatile uint32_t tx_tail = 0;

static inline int tx_empty(void)
{
    return tx_head == tx_tail;
}

static inline int tx_full(void)
{
    return ((tx_head + 1) % UART_TX_BUF_SIZE) == tx_tail;
}

void uart_init(void)
{
    /* Enable UART0 clock */
    mmio_write(CM_WKUP_BASE + CM_WKUP_UART0_CLKCTRL, 0x2);
    while ((mmio_read(CM_WKUP_BASE + CM_WKUP_UART0_CLKCTRL) & 0x3) != 0x2);

    /* Pinmux UART0 */
    mmio_write(CONTROL_MODULE_BASE + CONF_UART0_RXD, 0x30);
    mmio_write(CONTROL_MODULE_BASE + CONF_UART0_TXD, 0x00);

    /* Soft reset */
    mmio_write(UART0_BASE + UART_SYSC, 0x2);
    while (!(mmio_read(UART0_BASE + UART_SYSS) & 0x1));

    /* Disable UART */
    mmio_write(UART0_BASE + UART_MDR1, 0x7);

    /* DLAB + 8N1 */
    mmio_write(UART0_BASE + UART_LCR, 0x80);

    /* 115200 baud @ 48 MHz */
    mmio_write(UART0_BASE + UART_DLL, 26);
    mmio_write(UART0_BASE + UART_DLH, 0);

    mmio_write(UART0_BASE + UART_LCR, 0x03);

    /* Enable FIFO */
    mmio_write(UART0_BASE + UART_FCR, 0x07);

    /* Enable UART */
    mmio_write(UART0_BASE + UART_MDR1, 0x0);

    /* Enable THRE interrupt */
    //mmio_write(UART0_BASE + UART_IER, IER_THRE);
}

void uart_putc(char c)
{
    #ifdef UART_INTERUPT
        uart_putc_it(c);
    #else
        while(!(mmio_read(UART0_BASE + UART_LSR) & LSR_THRE));
        mmio_write8(UART0_BASE + UART_THR, c);
    #endif  
}

void uart_puts(const char *s)
{
    while (*s) {
    
        #ifdef UART_INTERUPT
        if (*s == '\n') uart_putc_it('\r');
            uart_putc_it(*s++);
        #else
        
            if (*s == '\n') uart_putc('\r');
            uart_putc(*s++);
        #endif
    }
}

void uart_puts_D(const char *s)
{
    while (*s) {
            if (*s == '\n') uart_putc('\r');
            uart_putc(*s++);
    
    }
}

void uart_putc_it(char c)
{
    uint32_t next;

    /* wait if full */
    while (tx_full());

    next = (tx_head + 1) % UART_TX_BUF_SIZE;

    tx_buf[tx_head] = c;
    tx_head = next;
    /* kick TX interrupt */
    mmio_write(UART0_BASE + UART_IER, IER_THRE);
}

void uart_tx_isr(void)
{
    while (!tx_empty() &&
           (mmio_read(UART0_BASE + UART_LSR) & LSR_THRE))
    {
        mmio_write8(UART0_BASE + UART_THR, tx_buf[tx_tail]);
        tx_tail = (tx_tail + 1) % UART_TX_BUF_SIZE;
    }

    /* nothing left -> disable interrupt */
    if (tx_empty()) {
        mmio_write(UART0_BASE + UART_IER, 0x0);
    }
}
