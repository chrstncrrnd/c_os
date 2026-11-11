# c operating system
C as in my name not the programming language.

A(nother) RISC-V based operating system (only kernel as of now, but operating system sounds cool).



## TODOs:
- Mega/Giga-page mappings.
- Trap handler from both supervisor and user mode.
- Clean up and homogenize int 64 types.

## Useful QEMU notes:
- to exit qemu input: `Ctrl + A` followed by `x`, 
- and to enter monitor mode input `Ctrl + A` followed by `c`.

## GDB notes:
- Use `-s -S` flag with QEMU to expose a gdb debug port.
- Use `gdb-multiarch`:
    - `set architecture riscv:rv64`
    - `set disassemble-next-line on`
    - `target remote :1234`
- `i r` to dump registers.


## References:
- [Xv6](https://pdos.csail.mit.edu/6.828/2025/xv6/book-riscv-rev5.pdf)
- [OS in 1K Lines](https://operating-system-in-1000-lines.vercel.app/)
- [SBI boot process](https://popovicu.com/posts/risc-v-sbi-and-full-boot-process/)
- [RISC-V SBI specification](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [RISC-V Documentation](https://docs.riscv.org/reference/home/index.html)
- [RISC-V Ratified Specifications Library](https://docs.riscv.org/reference/home/index.html)
