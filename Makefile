run: kernel
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw

%.o: %.asm
	@echo - [as] $<
	@nasm -felf -o $@ $<

%.o: %.c
	@echo - [cc] $<
	@cc -nostdlib -fno-stack-protector -I. -m32 -c -o $@ $<

kernel: kernel.o main.o console.o gdt.o idt.o ient.o pic.o pit.o keybd.o tss.o mm/mmu.o mm/mman.o mm/mallo.o sched.o cont.o user.o syscall.o proc.o cmos.o misc/vsprintf.o misc/sprintf.o misc/printk.o misc/cprintf.o misc/memcpy.o fs/buffer.o fs/rwblk.o fs/super.o fs/inode.o blk_drv/ide.o
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`
