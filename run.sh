#! /bin/sh
set -xue

QEMU=qemu-system-riscv64

cd ./kernel/
make

# Start QEMU
# $QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
