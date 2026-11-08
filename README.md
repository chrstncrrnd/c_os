# COS (C OS)

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
