# handoff to kernel main
.section .text.boot
.balign 4
.global boot
boot:
  lla sp, __stack_top
  j kernel_main
