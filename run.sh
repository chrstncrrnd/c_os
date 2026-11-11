#! /bin/sh
QEMU=qemu-system-riscv64
QEMU_FLAGS="-machine virt -bios default -nographic -serial mon:stdio --no-reboot"

if [ "$1" = "--debug" ];then
  echo "Using QEMU debug mode..."
  QEMU_FLAGS+=" -s -S"
fi

set -xue

make

# Start QEMU
$QEMU $QEMU_FLAGS -kernel ./build/kernel.elf
