/**
 * AM335x Interrupt Controller (INTC) Driver
 * 
 * The AM335x has 128 interrupt sources managed by the INTC.
 * This driver provides basic initialization and interrupt management.
 */

#ifndef INTC_H
#define INTC_H

#include <stdint.h>

/* INTC Base Address */
#define INTC_BASE               0x48200000

/* INTC Register Offsets */
#define INTC_SYSCONFIG          (INTC_BASE + 0x010)
#define INTC_SYSSTATUS          (INTC_BASE + 0x014)
#define INTC_SIR_IRQ            (INTC_BASE + 0x040)
#define INTC_SIR_FIQ            (INTC_BASE + 0x044)
#define INTC_CONTROL            (INTC_BASE + 0x048)
#define INTC_PROTECTION         (INTC_BASE + 0x04C)
#define INTC_IDLE               (INTC_BASE + 0x050)
#define INTC_IRQ_PRIORITY       (INTC_BASE + 0x060)
#define INTC_FIQ_PRIORITY       (INTC_BASE + 0x064)
#define INTC_THRESHOLD          (INTC_BASE + 0x068)

/* Interrupt Line Registers (ILR) - 128 registers, one per interrupt */
#define INTC_ILR_BASE           (INTC_BASE + 0x100)
#define INTC_ILR(n)             (INTC_ILR_BASE + ((n) * 4))

/* Interrupt Mask Registers (MIR) - 4 registers covering 128 interrupts */
#define INTC_MIR_BASE           (INTC_BASE + 0x084)
#define INTC_MIR(n)             (INTC_MIR_BASE + ((n) * 0x20))

#define INTC_MIR_CLEAR_BASE     (INTC_BASE + 0x088)
#define INTC_MIR_CLEAR(n)       (INTC_MIR_CLEAR_BASE + ((n) * 0x20))

#define INTC_MIR_SET_BASE       (INTC_BASE + 0x08C)
#define INTC_MIR_SET(n)         (INTC_MIR_SET_BASE + ((n) * 0x20))

/* Interrupt Source Registers */
#define INTC_ISR_SET_BASE       (INTC_BASE + 0x090)
#define INTC_ISR_CLEAR_BASE     (INTC_BASE + 0x094)

/* Common AM335x Interrupt Numbers */
#define IRQ_EMUINT              0
#define IRQ_COMMTX              1
#define IRQ_COMMRX              2
#define IRQ_BENCH               3
#define IRQ_ELM_IRQ             4
#define IRQ_NMI                 7
#define IRQ_L3DEBUG             9
#define IRQ_L3APPINT            10
#define IRQ_PRCMINT             11
#define IRQ_EDMACOMPINT         12
#define IRQ_EDMAMPERR           13
#define IRQ_EDMAERRINT          14
#define IRQ_ADC_TSC_GENINT      16
#define IRQ_USBSSINT            17
#define IRQ_USB0                18
#define IRQ_USB1                19
#define IRQ_PRUSS1_EVTOUT0      20
#define IRQ_PRUSS1_EVTOUT1      21
#define IRQ_PRUSS1_EVTOUT2      22
#define IRQ_PRUSS1_EVTOUT3      23
#define IRQ_PRUSS1_EVTOUT4      24
#define IRQ_PRUSS1_EVTOUT5      25
#define IRQ_PRUSS1_EVTOUT6      26
#define IRQ_PRUSS1_EVTOUT7      27
#define IRQ_MMCSD1INT           28
#define IRQ_MMCSD2INT           29
#define IRQ_I2C2INT             30
#define IRQ_ECAP0INT            31
#define IRQ_GPIOINT2A           32
#define IRQ_GPIOINT2B           33
#define IRQ_USBWAKEUP           34
#define IRQ_LCDCINT             36
#define IRQ_GFXINT              37
#define IRQ_ePWM2INT            39
#define IRQ_3PGSWRXTHR0         40
#define IRQ_3PGSWRXINT0         41
#define IRQ_3PGSWTXINT0         42
#define IRQ_3PGSWMISC0          43
#define IRQ_UART3INT            44
#define IRQ_UART4INT            45
#define IRQ_UART5INT            46
#define IRQ_ECAP1INT            47
#define IRQ_DCAN0_INT0          52
#define IRQ_DCAN0_INT1          53
#define IRQ_DCAN0_PARITY        54
#define IRQ_DCAN1_INT0          55
#define IRQ_DCAN1_INT1          56
#define IRQ_DCAN1_PARITY        57
#define IRQ_ePWM0_TZINT         58
#define IRQ_ePWM1_TZINT         59
#define IRQ_ePWM2_TZINT         60
#define IRQ_ECAP2INT            61
#define IRQ_GPIOINT3A           62
#define IRQ_GPIOINT3B           63
#define IRQ_MMCSD0INT           64
#define IRQ_SPI0INT             65
#define IRQ_TINT0               66  /* DMTimer0 */
#define IRQ_TINT1_1MS           67  /* DMTimer1 1ms */
#define IRQ_TINT2               68  /* DMTimer2 */
#define IRQ_TINT3               69  /* DMTimer3 */
#define IRQ_I2C0INT             70
#define IRQ_I2C1INT             71
#define IRQ_UART0INT            72  /*UART 0*/
#define IRQ_UART1INT            73
#define IRQ_UART2INT            74
#define IRQ_RTCINT              75
#define IRQ_RTCALARMINT         76
#define IRQ_MBINT0              77
#define IRQ_M3_TXEV             78
#define IRQ_eQEP0INT            79
#define IRQ_MCATXINT0           80
#define IRQ_MCARXINT0           81
#define IRQ_MCATXINT1           82
#define IRQ_MCARXINT1           83
#define IRQ_ePWM0INT            86
#define IRQ_ePWM1INT            87
#define IRQ_eQEP1INT            88
#define IRQ_eQEP2INT            89
#define IRQ_DMA_INTR_PIN2       90
#define IRQ_WDT1INT             91
#define IRQ_TINT4               92  /* DMTimer4 */
#define IRQ_TINT5               93  /* DMTimer5 */
#define IRQ_TINT6               94  /* DMTimer6 */
#define IRQ_TINT7               95  /* DMTimer7 */
#define IRQ_GPIOINT0A           96
#define IRQ_GPIOINT0B           97
#define IRQ_GPIOINT1A           98
#define IRQ_GPIOINT1B           99
#define IRQ_GPMCINT             100
#define IRQ_DDRERR0             101
#define IRQ_TCERRINT0           112
#define IRQ_TCERRINT1           113
#define IRQ_TCERRINT2           114
#define IRQ_ADC_TSC_PENINT      115
#define IRQ_SMRFLX_Sabertooth   120
#define IRQ_SMRFLX_Core         121
#define IRQ_DMA_INTR_PIN0       123
#define IRQ_DMA_INTR_PIN1       124
#define IRQ_SPI1INT             125

/* Register access macros */
#define REG(addr)               (*(volatile uint32_t *)(addr))

/* Function prototypes */
void intc_init(void);
void intc_enable_interrupt(uint32_t irq_num);
void intc_disable_interrupt(uint32_t irq_num);
void intc_set_priority(uint32_t irq_num, uint32_t priority);
uint32_t intc_get_active_irq(void);
void intc_ack_interrupt(void);


/* ============================================================================
 * Interrupt Handling
 * ========================================================================== */

/**
 * Enable IRQ at CPU level
 */
inline void enable_irq(void) {
  __asm__ volatile("cpsie i\n" ::: "memory");
}

/**
 * Disable IRQ at CPU level
 */
inline void disable_irq(void) {
  __asm__ volatile("cpsid i\n" ::: "memory");
}


#endif /* INTC_H */
