MEMORY
{
    DDR (rwx) : ORIGIN = 0x80000000,  LENGTH = 0x10000000  /* 256MB */
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
    } > DDR

    /* -------------------------------------------------
       TEXT / RODATA
       ------------------------------------------------- */
    .text : ALIGN(4)
    {
        *(.text*)
        *(.rodata*)
    } > DDR

    /* -------------------------------------------------
       DATA
       ------------------------------------------------- */
    .data : ALIGN(8)
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > DDR

    /* -------------------------------------------------
       BSS
       ------------------------------------------------- */
    .bss (NOLOAD) : ALIGN(8)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > DDR

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
    } > DDR

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
    } > DDR

    /* ------------------------------------------------- */
    __end__ = .;

    ASSERT(__end__ <= ORIGIN(DDR)+LENGTH(DDR), "DDR overflow")
}
