#pragma once

#define INT_IRQ0	0x20

void init_pic(void);
void irq_setenable(int irq, int enable);
void irq_done(int irq);
