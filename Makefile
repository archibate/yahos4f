run: kernel
	qemu-system-i386 -kernel $< -hda hda.img

%.o: %.asm
	nasm -felf -o $@ $<

%.o: %.c
	cc -nostdlib -I. -m32 -c -o $@ $<

kernel: kernel.o main.o console.o gdt.o idt.o ient.o pic.o pit.o keybd.o tss.o mmu.o mman.o mallo.o sched.o cont.o user.o fail.o ide.o syscall.o proc.o cmos.o vsprintf.o format.o fs/buffer.o fs/rwblk.o fs/ide.o fs/super.o
	ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`

fsctl: fsctl.c
	cc -o $@ $<
