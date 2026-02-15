.syntax unified
.cpu cortex-a8
.global _start

/* --------------------------------------------------
 * Vector table
 * -------------------------------------------------- */
.section .vectors, "ax"
.align 5
_vectors_start:
_start:
    b reset              /* Reset */
    b .                  /* Undefined */
    b .                  /* SVC */
    b .                  /* Prefetch abort */
    b .                  /* Data abort */
    b .                  /* Reserved */
    b .                  /* IRQ */
    b .                  /* FIQ */

/* --------------------------------------------------
 * Reset handler
 * -------------------------------------------------- */
.section .text
reset:
    /* Enter SVC mode, disable IRQ & FIQ */
        /* Disable MMU, I-cache, D-cache */
    mrc p15, 0, r0, c1, c0, 0   /* SCTLR */
    bic r0, r0, #(1 << 0)      /* MMU off */
    bic r0, r0, #(1 << 2)      /* D-cache off */
    bic r0, r0, #(1 << 12)     /* I-cache off */
    mcr p15, 0, r0, c1, c0, 0
    isb
    dsb
    
        /* Invalidate I-cache */
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    dsb
    isb
    
    cpsid if
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x13     /* SVC mode */
    msr cpsr_c, r0

    /* Disable watchdog (AM335x requires polling) */
    ldr r0, =0x44E35048   /* WDT_WSPR */
    ldr r1, =0xAAAA
    str r1, [r0]
wdt_wait1:
    ldr r2, =0x44E35034   /* WDT_WWPS */
    ldr r3, [r2]
    tst r3, #0x10
    bne wdt_wait1

    ldr r1, =0x5555
    str r1, [r0]
wdt_wait2:
    ldr r3, [r2]
    tst r3, #0x10
    bne wdt_wait2

    /* Set stack pointer */
    ldr sp, =__stack_top__

    /* Zero BSS */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0
bss_loop:
    cmp r0, r1
    strlo r2, [r0], #4
    blo bss_loop

    /* Set vector base register */
    ldr r0, =_vectors_start
    mcr p15, 0, r0, c12, c0, 0   /* VBAR */
    isb

    /* Jump to C */
    bl main

hang:
    b hang
