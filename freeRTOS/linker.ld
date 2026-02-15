MEMORY
{
    SRAM (rwx) : ORIGIN = 0x402F0400, LENGTH = 63K
}

ENTRY(_start)

SECTIONS
{
    /* -------------------------------------------------
       VECTORS
       ------------------------------------------------- */
    .vectors : ALIGN(32)
    {
        KEEP(*(.vectors))
    } > SRAM

    /* -------------------------------------------------
       TEXT / RODATA
       ------------------------------------------------- */
    .text : ALIGN(4)
    {
        *(.text*)
        *(.rodata*)
    } > SRAM

    /* -------------------------------------------------
       DATA
       ------------------------------------------------- */
    .data : ALIGN(8)
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > SRAM

    /* -------------------------------------------------
       BSS
       ------------------------------------------------- */
    .bss (NOLOAD) : ALIGN(8)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > SRAM

    /* -------------------------------------------------
       HEAP (for heap_4.c / debug tools)
       ------------------------------------------------- */
    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_start = .;
        __heap_start__ = .;

        . += 8K;     /* adjust as needed */

        __heap_end = .;
        __heap_end__ = .;
    } > SRAM

    /* -------------------------------------------------
       STACKS
       ------------------------------------------------- */
    .stacks (NOLOAD) : ALIGN(8)
    {
        /* IRQ */
        __irq_stack_top__ = . + 1024;
        . += 1024;

        /* SVC (kernel + C runtime) */
        __svc_stack_top__ = . + 16384;
        . += 4096;

        /* SYS (tasks) */
        __sys_stack_top__ = . + 4096;
        . += 4096;

        /* provide classic main stack symbols for tools */
        __stack_start__ = __svc_stack_top__ - 4096;
        __stack_top__   = __svc_stack_top__;
    } > SRAM

    /* ------------------------------------------------- */
    __end__ = .;

    ASSERT(__end__ <= ORIGIN(SRAM)+LENGTH(SRAM), "SRAM overflow")
}
