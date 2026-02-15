.syntax unified
.cpu cortex-a8
.global _start
.global irq_handler_entry
.global svc_handler_entry
.global FreeRTOS_IRQ_Handler
.global FreeRTOS_SVC_Handler

/* Stack sizes for different modes - reduced to fit in SRAM */
.equ IRQ_STACK_SIZE,    0x200   /* 512 bytes for IRQ stack */
.equ FIQ_STACK_SIZE,    0x100   /* 256 bytes for FIQ stack */
.equ SVC_STACK_SIZE,    0x200   /* 512 bytes for SVC stack */
.equ ABT_STACK_SIZE,    0x100   /* 256 bytes for abort stack */
.equ UND_STACK_SIZE,    0x100   /* 256 bytes for undefined stack */

/* CPU mode definitions */
.equ MODE_USR,          0x10
.equ MODE_FIQ,          0x11
.equ MODE_IRQ,          0x12
.equ MODE_SVC,          0x13
.equ MODE_ABT,          0x17
.equ MODE_UND,          0x1B
.equ MODE_SYS,          0x1F

.equ I_BIT,             0x80    /* IRQ disable bit */
.equ F_BIT,             0x40    /* FIQ disable bit */

/* --------------------------------------------------
 * Vector table
 * -------------------------------------------------- */
.section .vectors, "ax"
.align 5
_vectors_start:
_start:
    b reset              /* Reset */
    b undefined_handler  /* Undefined instruction */
    b FreeRTOS_SVC_Handler  /* Supervisor call (SVC) */
    b prefetch_handler   /* Prefetch abort */
    b data_abort_handler /* Data abort */
    b .                  /* Reserved */
    b FreeRTOS_IRQ_Handler  /* IRQ */
    b fiq_handler        /* FIQ */

/* --------------------------------------------------
 * Reset handler
 * -------------------------------------------------- */
.section .text
reset:
    /* Disable MMU, I-cache, D-cache */
    mrc p15, 0, r0, c1, c0, 0   /* Read SCTLR */
    bic r0, r0, #(1 << 0)       /* MMU off */
    bic r0, r0, #(1 << 2)       /* D-cache off */
    bic r0, r0, #(1 << 12)      /* I-cache off */
    mcr p15, 0, r0, c1, c0, 0   /* Write SCTLR */
    isb
    dsb
    
    /* Invalidate I-cache */
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    dsb
    isb
    
    /* Start in SVC mode with IRQ and FIQ disabled */
    cpsid if, #MODE_SVC

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

    /* Initialize stacks for all modes */
    /* Stack grows downward, so we set SP to top of each stack area */

        /* SYS mode stack (main application stack) */
    cps #MODE_SYS
    MSR     cpsr_c, #MODE_SYS | F_BIT              
    # Change the mode

    ldr sp, =__sys_stack_top__


        /* IRQ mode stack (main application stack) */
    cps #MODE_IRQ
    MSR     cpsr_c, #MODE_IRQ | F_BIT  | I_BIT          
    # Change the mode

    ldr sp, =__irq_stack_top__
    
    /* SVC mode stack (main application stack) */
    cps #MODE_SVC
    MSR     cpsr_c, #MODE_SVC | F_BIT              
    # Change the mode

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

   # /* Enable VFP (Floating Point) if needed by FreeRTOS */
  #  mrc p15, 0, r0, c1, c0, 2    /* Read CPACR */
  #  orr r0, r0, #0x00F00000      /* Enable CP10 and CP11 coprocessors */
  #  mcr p15, 0, r0, c1, c0, 2    /* Write CPACR */
  #  isb
  #  mov r0, #0x40000000          /* Enable VFP itself */
  #  vmsr fpexc, r0               /* FPEXC.EN = 1 */

    /* Jump to C code - IRQs still disabled, main() will enable them */
    bl main

hang:
    b hang


/* --------------------------------------------------
 * Default exception handlers (infinite loops)
 * -------------------------------------------------- */
.align 4
undefined_handler:
    b user_led_toggle_0
    b .

.align 4
prefetch_handler:
    b user_led_toggle_1
    b .

.align 4
data_abort_handler:
    b user_led_toggle_2
    b .

.align 4
fiq_handler:
    b user_led_toggle_3
    b .

