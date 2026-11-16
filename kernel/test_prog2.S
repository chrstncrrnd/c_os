.global test_main2
.balign 4096
test_main2:
  li a7, 0
  li a0, 1
  ecall
# yield syscall
  li a7, 1
  ecall

  j test_main2
