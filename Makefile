run: run-qemu

run-bochs: bochsrc.bxrc hda.img
	-bochs -qf $<

run-qemu: kernel hda.img
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw

run-qemu-debug: kernel hda.img
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw -S -s

run-qemu-hd: hda.img
	qemu-system-i386 -drive file=hda.img,index=0,media=disk,driver=raw

hda.img: boot/boot Image bin/init
	sh tools/mkhda.sh

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
	@gcc -nostdlib -fno-stack-protector -ggdb -gstabs+ -I. -m32 -c -o $@ $<

ifneq (,$(shell [ -f misc/misc.a ] || echo 1))
misc/misc.a: misc/vsprintf.o misc/sprintf.o misc/printk.o misc/cprintf.o misc/memcpy.o misc/memset.o misc/strcmp.o misc/strchr.o misc/strlen.o 
	@echo + [ar] $@
	@ar cqs $@ $^
endif

lib/lib.a: lib/start.o lib/syscall.o
	@echo + [ar] $@
	@ar cqs $@ $^

kernel: kernel.o main.o conio.o gdt.o idt.o ient.o pic.o pit.o keybd.o tss.o sched.o cont.o user.o syscall.o proc.o cmos.o mm/mmu.o mm/mman.o mm/mallo.o mm/vmm.o mm/pmm.o fs/ide.o fs/buffer.o fs/rwblk.o fs/super.o fs/inode.o fs/dir.o fs/path.o fs/namei.o fs/blk_drv.o fs/exec.o fs/chr_drv.o misc/misc.a
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`

bin/%: usr/%.o lib/lib.a
	@mkdir -p $(@D)
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x1000000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`
