.section .text
.balign 4
.global user_vec
user_vec:
# make room to save registers.
  csrrw sp, sscratch, sp # sp = kernel stack top, sscratch = user sp
  addi sp, sp, -256

  sd ra, 0(sp)
# sd sp, 8(sp)
  sd gp, 16(sp)
# sd tp, 24(sp)
  sd t0, 32(sp)
  sd t1, 40(sp)
  sd t2, 48(sp)
  sd a0, 72(sp)
  sd a1, 80(sp)
  sd a2, 88(sp)
  sd a3, 96(sp)
  sd a4, 104(sp)
  sd a5, 112(sp)
  sd a6, 120(sp)
  sd a7, 128(sp)
  sd t3, 216(sp)
  sd t4, 224(sp)
  sd t5, 232(sp)
  sd t6, 240(sp)
# No need to save sx registers since they are callee saved

  csrr t0, sscratch # ...aand the stack pointer is on the stack
  sd t0, 8(sp)

  csrr t0, sepc # save the user pc
  sd t0, 24(sp) # where tp used to be

  la t0, kernel_vec # since we're in kernel mode, set the kernel trap handler
  csrw stvec, t0
  call user_trap



.global userret
.balign 4
userret:
  la t0, user_vec
  csrw stvec, t0 # going back to user mode, set the correct trap handler

  li t0, 1 << 8
  csrc sstatus, t0 # sret returns to user mode

  li t0, 1 << 5
  csrs sstatus, t0

  ld t0, 24(sp)
  csrw sepc, t0

  ld ra, 0(sp)
# ld sp, 8(sp)
  ld gp, 16(sp)
# not tp (contains hartid), in case we moved CPUs
  ld t0, 32(sp)
  ld t1, 40(sp)
  ld t2, 48(sp)
  ld a0, 72(sp)
  ld a1, 80(sp)
  ld a2, 88(sp)
  ld a3, 96(sp)
  ld a4, 104(sp)
  ld a5, 112(sp)
  ld a6, 120(sp)
  ld a7, 128(sp)
  ld t3, 216(sp)
  ld t4, 224(sp)
  ld t5, 232(sp)
  ld t6, 240(sp)

  addi sp, sp, 256
# put the kernel sp back in sscratch
  csrw sscratch, sp

  ld sp, 8(sp)
  sret
.balign 4
kernel_vec:
# make room to save registers.
  addi sp, sp, -256

  sd ra, 0(sp)
# sd sp, 8(sp)
  sd gp, 16(sp)
# sd tp, 24(sp)
  sd t0, 32(sp)
  sd t1, 40(sp)
  sd t2, 48(sp)
  sd a0, 72(sp)
  sd a1, 80(sp)
  sd a2, 88(sp)
  sd a3, 96(sp)
  sd a4, 104(sp)
  sd a5, 112(sp)
  sd a6, 120(sp)
  sd a7, 128(sp)
  sd t3, 216(sp)
  sd t4, 224(sp)
  sd t5, 232(sp)
  sd t6, 240(sp)

  call kernel_trap

  ld ra, 0(sp)
# ld sp, 8(sp)
  ld gp, 16(sp)
# not tp (contains hartid), in case we moved CPUs
  ld t0, 32(sp)
  ld t1, 40(sp)
  ld t2, 48(sp)
  ld a0, 72(sp)
  ld a1, 80(sp)
  ld a2, 88(sp)
  ld a3, 96(sp)
  ld a4, 104(sp)
  ld a5, 112(sp)
  ld a6, 120(sp)
  ld a7, 128(sp)
  ld t3, 216(sp)
  ld t4, 224(sp)
  ld t5, 232(sp)
  ld t6, 240(sp)

  addi sp, sp, 256
  sret
