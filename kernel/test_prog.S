.global test_main
.balign 4096
test_main:
  li s0, 0
loop:
  li a7, 0
  li a0, 0
  ecall
# yield syscall
  li a7, 1
  ecall
  addi s0, s0, 1
  li t0, 10
  bne s0, t0, loop
