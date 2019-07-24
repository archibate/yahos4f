run: kernel
	qemu-system-i386 -kernel $<

kernel.o: kernel.asm
	nasm -felf -o $@ $<

main.o: main.c
	cc -nostdlib -m32 -c -o $@ $<

kernel: kernel.o main.o
	ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^
