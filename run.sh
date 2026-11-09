#! /bin/sh
set -xue

QEMU=qemu-system-riscv64
QEMU_FLAGS="-machine virt -bios default -nographic -serial mon:stdio --no-reboot"

make
# Start QEMU
$QEMU $QEMU_FLAGS -kernel ./build/kernel.elf
