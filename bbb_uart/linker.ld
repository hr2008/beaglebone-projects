ENTRY(_start)

MEMORY
{
    OCM (rwx) : ORIGIN = 0x402F0400, LENGTH = 64K
}

/* Tunables */
__stack_size__ = 8K;
__heap_size__  = 16K;

SECTIONS
{
    . = ORIGIN(OCM);

    /* ---------------- Vector table ---------------- */
    .vectors : {
        . = ALIGN(32);
        __vectors_start__ = .;
        KEEP(*(.vectors))
        __vectors_end__ = .;
    } > OCM

    /* ---------------- Code & RO data ---------------- */
    .text : {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
    } > OCM

    /* ---------------- Initialized data ---------------- */
    .data : {
        . = ALIGN(4);
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > OCM

    /* ---------------- BSS ---------------- */
    .bss (NOLOAD) : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > OCM

    /* ---------- Heap ---------- */
    . = ALIGN(8);
    __heap_start__ = .;
    . = . + __heap_size__;
    __heap_end__ = .;

    /* ---------- Stack ---------- */
    __stack_top__    = ORIGIN(OCM) + LENGTH(OCM);
    __stack_bottom__ = __stack_top__ - __stack_size__;
}
