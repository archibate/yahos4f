run: kernel
	qemu-system-i386 -kernel $<

kernel.o: kernel.asm
	nasm -felf -o $@ $<

kernel: kernel.o
	ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $<
