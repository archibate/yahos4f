#include "pic.h"
#include "io.h"

#define IO_PIC0  0x20
#define IO_PIC1  0xa0

// https://github.com/archibate/jos/blob/master/lab4/partC/kern/picirq.c
// https://github.com/archibate/OSASK/blob/master/src/haribote/int.c
void init_pic(void)
{
	outb(IO_PIC0+1, 0xfb);
	outb(IO_PIC1+1, 0xff);

	outb(IO_PIC0+0, 0x11);
	outb(IO_PIC0+1, INT_IRQ0);
	outb(IO_PIC0+1, 1<<2);
	outb(IO_PIC0+1, 0x03);

	outb(IO_PIC1+0, 0x11);
	outb(IO_PIC1+1, INT_IRQ0 + 8);
	outb(IO_PIC1+1,    2);
	outb(IO_PIC1+1, 0x01);

	outb(IO_PIC0+1, 0xfb); // PIC0: all masked except slave PIC1
	outb(IO_PIC1+1, 0xff); // PIC1: all masked
}

void irq_setenable(int irq, int enable)
{
	unsigned char port = IO_PIC0+1;
	if (irq > 8) {
		port = IO_PIC1+1;
		irq -= 8;
	}
	unsigned char mask = inb(port);
	if (enable)
		mask &= 0xff ^ (1 << irq);
	else
		mask |= 1 << irq;
	outb(port, mask);
}

void irq_done(int irq)
{
	outb(IO_PIC0, 0x20);
	if (irq > 8)
		outb(IO_PIC1, 0x20);
}
