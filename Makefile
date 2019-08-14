run: kernel
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw

run-hd: boot/boot Image
	sh tools/mkhda.sh
	qemu-system-i386 -drive file=hda.img,index=0,media=disk,driver=raw

boot/%: boot/%.asm
	@echo - [as] $<
	@nasm -fbin -o $@ $<

Image: boot/head kernel
	cat $^ > $@

%.o: %.asm
	@echo - [as] $<
	@nasm -felf -o $@ $<

%.o: %.c
	@echo - [cc] $<
	@cc -nostdlib -fno-stack-protector -I. -m32 -c -o $@ $<

kernel: kernel.o main.o conio.o gdt.o idt.o ient.o pic.o pit.o keybd.o tss.o mm/mmu.o mm/mman.o mm/mallo.o sched.o cont.o user.o syscall.o proc.o cmos.o misc/vsprintf.o misc/sprintf.o misc/printk.o misc/cprintf.o misc/memcpy.o misc/strcmp.o misc/strchr.o misc/strlen.o blk_drv/ide.o fs/buffer.o fs/rwblk.o fs/super.o fs/inode.o fs/dir.o fs/path.o fs/namei.o fs/blkrw.o
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`
