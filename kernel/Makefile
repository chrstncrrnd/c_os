CC=clang
CFLAGS=-std=c2y -O3 -g3 -Wall -Wextra --target=riscv64-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib -mcmodel=medany
LINKER_FLAGS=-Tkernel.ld,-Map=kernel.map


OBJS=build/main.o

all: kernel


kernel: $(OBJS)
	$(CC) $(CFLAGS) -Wl,$(LINKER_FLAGS) -o kernel.elf $(OBJS)

$(OBJS): build/%.o: ./%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf kernel.elf build/

.PHONY: all clean
