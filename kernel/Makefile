CC=clang
CFLAGS=-std=c2y -O3 -g3 -Wall -Wextra --target=riscv64-unknown-elf -fno-stack-protector -ffreestanding -nostdlib -mcmodel=medany
LINKER_FLAGS=-Tkernel.ld,-Map=kernel.map

SRCS = main.c utils.c sbi.c trap_handler.c
ASM_SRCS = trap.S boot.S
OBJS = $(SRCS:%.c=build/%.o) $(ASM_SRCS:%.S=build/%.o)

all: kernel

kernel: $(OBJS)
	$(CC) $(CFLAGS) -fuse-ld=lld -Wl,$(LINKER_FLAGS) -o kernel.elf $(OBJS)

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf kernel.elf build/

.PHONY: all clean
