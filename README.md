# C operating system
C as in my name not the programming language.

A RISC-V (rv64 with sv39 virtual memory) based operating system.
Currently under development, a lot (most) of the stuff still to do.

## Running
C_OS can be run using the script `./run.sh`.
You can use the `--debug` flag to expose a debug port on QEMU.

## Requirements:
- Clang (Tested with version 23.1)
- GNU Make (Tested with version 4.4)
- QEMU (Tested with 11.1)

## Specifications:
- Built for the 64 bit version of the RISC-V architecture.
- Uses sv39 for paging/virtual memory.
- Xv6 inspired trampoline page and trap frame saving for supervisor to user mode switch.
- OpenSBI base for simple text I/O.

## TODOs:
- Mega/Giga-page mappings.
- Trap handler from both supervisor and user mode.
- Clean up and homogenize int 64 types.
- User page table permissions.


## Rough roadmap:
- (IN PROGRESS) Virtual memory and trap handling.
- Loading of ELF files.
- Allow for multiple processes and basic process scheduling (with `fork()` and other syscalls).
- File system.
- Deallocation of pages.
- Allow for multiple CPU cores (with locking).
- Software threads and hardware thread mapping.
- Page faults and lazy memory allocation (maybe COW systems).
- Interrupts and device drivers.

# Tool notes:
## 1. QEMU
- To exit qemu input: `Ctrl + A` followed by `x`.
- To enter monitor mode input `Ctrl + A` followed by `c`.

## 2. GDB
- The file `.gdbinit` contains the startup commands. You may need to update
gdb's permissions in order for this to be executed automatically.
- `i r` to dump registers.
- `si` to step one instruction.
- `hbreak` command to break at given address not symbol.

## 3. LLVM
- `llvm-addr2line --obj=build/kernel.elf <address>` can be used to find what
source line was mapped to which address.
- `llvm-objdump -d build/kernel.elf` can be used to inspect the generated assembly.

# References:
- [Xv6](https://pdos.csail.mit.edu/6.828/2025/xv6/book-riscv-rev5.pdf)
- [OS in 1K Lines](https://operating-system-in-1000-lines.vercel.app/)
- [SBI boot process](https://popovicu.com/posts/risc-v-sbi-and-full-boot-process/)
- [RISC-V SBI specification](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [RISC-V Ratified Specifications Library](https://docs.riscv.org/reference/home/index.html)
