#pragma once

// defined in trap_entry.S
extern void user_vec(void);
extern void userret(void);
extern void kernel_vec(void);

void user_trap(void);
void kernel_trap(void);
void init_trap(void);
