#pragma once

// defined in trap_entry.S
extern void trap_entry(void);

void trap_handler(void);
void init_trap(void);
