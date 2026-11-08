CC=clang
CFLAGS=-std=c23 -O3 -g3 -Wall -Wextra --target=riscv64-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib


OBJS=build/main.o

all: kernel


kernel: $(OBJS)
	$(CC) $(CFLAGS) -o kernel.elf $(OBJS)

$(OBJS): build/%.o: ./%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf main build/

.PHONY: all clean
