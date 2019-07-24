run: kernel
	qemu-system-i386 -kernel $<

%.o: %.asm
	nasm -felf -o $@ $<

%.o: %.c
	cc -nostdlib -m32 -c -o $@ $<

kernel: kernel.o main.o console.o gdt.o idt.o ientry.o
	ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^
