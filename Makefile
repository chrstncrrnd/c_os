CC=clang
CFLAGS=-std=c2y -O1 -g -Wall -Wextra --target=riscv64-unknown-elf -fno-stack-protector -ffreestanding -nostdlib -mcmodel=medany
LINKER_FLAGS=-Tkernel/kernel.ld,-Map=build/kernel.map

KERNEL_SRCS = main.c utils.c sbi.c trap.c paging.c proc.c
KERNEL_ASM_SRCS = trap_entry.S boot.S switch.S test_prog.S trampoline.S

# prepend kernel/ (this is so we can define KERNEL_SRCS and KERNEL_ASM_SRCS cleanly)
SRCS = $(KERNEL_SRCS:%=kernel/%)
ASM_SRCS = $(KERNEL_ASM_SRCS:%=kernel/%)

OBJS = $(SRCS:%.c=build/%.o) $(ASM_SRCS:%.S=build/%.o)

all: kernel

kernel: $(OBJS)
	@mkdir -p build/
	$(CC) $(CFLAGS) -fuse-ld=lld -Wl,$(LINKER_FLAGS) -o build/kernel.elf $(OBJS)

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/

.PHONY: all clean
