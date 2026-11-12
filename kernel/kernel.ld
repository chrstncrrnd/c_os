ENTRY(boot)

SECTIONS {
    . = 0x80200000;
    __kernel_base = .;

    .text   : ALIGN(4) {
      KEEP(*(.text.boot));
      *(.text .text.*);
      . = ALIGN(0x1000);
      __trampoline = .;
      *(trampoline_section);
      . = ALIGN(0x1000);
      ASSERT(. - __trampoline == 0x1000, "error: trampoline is larger than one page");
    }

    .rodata : ALIGN(4) { *(.rodata .rodata.*); }

    .data   : ALIGN(4) { *(.data .data.*); }

    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    . = ALIGN(16);
    . += 128 * 1024;
    __stack_top = .; /* 128 KB*/

    . = ALIGN(0x1000);
    __free_ram = .;
    . += 64 * 1024 * 1024; /* 64MB */
    __free_ram_end = .;
}
