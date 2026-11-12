#include <defines.h>

.section trampoline_sec
.balign 4
.global user_vec
user_vec:
# a0 contains *(struct trap_frame)
  csrw sscratch, a0

  li a0, TRAP_FRAME
  sd ra, 40(a0)
  sd sp, 48(a0)
  sd gp, 56(a0)
  sd tp, 64(a0)
  sd t0, 72(a0)
  sd t1, 80(a0)
  sd t2, 88(a0)
  sd s0, 96(a0)
  sd s1, 104(a0)
  sd a1, 120(a0)
  sd a2, 128(a0)
  sd a3, 136(a0)
  sd a4, 144(a0)
  sd a5, 152(a0)
  sd a6, 160(a0)
  sd a7, 168(a0)
  sd s2, 176(a0)
  sd s3, 184(a0)
  sd s4, 192(a0)
  sd s5, 200(a0)
  sd s6, 208(a0)
  sd s7, 216(a0)
  sd s8, 224(a0)
  sd s9, 232(a0)
  sd s10, 240(a0)
  sd s11, 248(a0)
  sd t3, 256(a0)
  sd t4, 264(a0)
  sd t5, 272(a0)
  sd t6, 280(a0)

  csrr t0, sscratch
  sd t0, 112(a0)

# kernel stack pointer
  ld sp, 8(a0)
# make tp hold the current hartid, from p->trapframe->kernel_hartid
# ld tp, 32(a0)


# kernel page table root address
  ld t1, 0(a0)

  sfence.vma zero, zero

# finally, switch to the kernel page table
  csrw satp, t1
  sfence.vma zero, zero

  call user_trap


.balign 4
.global userret
userret:
  # usertrap() returns here, with user satp in a0.
  # return from kernel to user.

  # switch to the user page table.
  sfence.vma zero, zero
  csrw satp, a0
  sfence.vma zero, zero

  li a0, TRAPFRAME

  # restore all but a0 from TRAPFRAME
  ld ra, 40(a0)
  ld sp, 48(a0)
  ld gp, 56(a0)
  ld tp, 64(a0)
  ld t0, 72(a0)
  ld t1, 80(a0)
  ld t2, 88(a0)
  ld s0, 96(a0)
  ld s1, 104(a0)
  ld a1, 120(a0)
  ld a2, 128(a0)
  ld a3, 136(a0)
  ld a4, 144(a0)
  ld a5, 152(a0)
  ld a6, 160(a0)
  ld a7, 168(a0)
  ld s2, 176(a0)
  ld s3, 184(a0)
  ld s4, 192(a0)
  ld s5, 200(a0)
  ld s6, 208(a0)
  ld s7, 216(a0)
  ld s8, 224(a0)
  ld s9, 232(a0)
  ld s10, 240(a0)
  ld s11, 248(a0)
  ld t3, 256(a0)
  ld t4, 264(a0)
  ld t5, 272(a0)
  ld t6, 280(a0)

# restore user a0
  ld a0, 112(a0)
  
  # return to user mode and user pc.
  # prepare_return() sets up sstatus and sepc.
  sret



.section .text
.global kernel_vec
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
