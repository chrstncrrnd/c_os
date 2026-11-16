.global test_main
.balign 4096
test_main:
  li a7, 0
  li a0, 0
  ecall

# yield syscall
  li a7, 1
  ecall
  j test_main
