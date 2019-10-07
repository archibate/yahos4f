run: run-qemu

debug: run-qemu-debug

run-bochs: bochsrc.bxrc hda.img
	-bochs -qf $<

run-qemu: kernel hda.img
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw

run-qemu-debug: kernel hda.img
	qemu-system-i386 -kernel $< -drive file=hda.img,index=0,media=disk,driver=raw -S -s

run-qemu-hd: hda.img
	qemu-system-i386 -drive file=hda.img,index=0,media=disk,driver=raw

hda.img: boot/boot Image bin/init bin/cat bin/env bin/sh bin/ls bin/hello
	sh tools/mkhda.sh

boot/%: boot/%.asm
	@echo - [as] $<
	@nasm -fbin -o $@ $<

Image: boot/head kernel
	cat $^ > $@

%.o: %.asm
	@echo - [as] $<
	@nasm -felf -o $@ $<

usr/%.o: usr/%.c
	@echo - [cc] $<
	@gcc -nostdlib -fno-stack-protector -ggdb -gstabs+ -I. -Iinclude -D_USER -m32 -c -o $@ $<

%.o: %.c
	@echo - [cc] $<
	@gcc -nostdlib -fno-stack-protector -ggdb -gstabs+ -I. -Iinclude -D_KERNEL -m32 -c -o $@ $<

ifneq (,$(shell [ -f lib/lib.a ] || echo 1))
lib/lib.a: lib/printf.o lib/memcpy.o lib/memset.o lib/strcmp.o lib/strchr.o lib/strlen.o lib/strcpy.o lib/strcat.o lib/strdup.o lib/atoi.o lib/strerror.o lib/memcmp.o
	@echo + [ar] $@
	@ar cqs $@ $^
endif

usr/lib/lib.a: usr/lib/start.o usr/lib/syscall.o usr/lib/exec.o usr/lib/env.o usr/lib/malloc.o usr/lib/stdio.o usr/lib/fprintf.o usr/lib/perror.o usr/lib/errno.o usr/lib/system.o
	@echo + [ar] $@
	@ar cqs $@ $^

kernel: kernel.o main.o conio.o gdt.o idt.o ient.o pic.o pit.o keybd.o tss.o sched.o cont.o user.o syscall.o proc.o fork.o wait.o cmos.o errno.o mm/mmu.o mm/mman.o mm/vmm.o mm/pmm.o fs/ide.o fs/buffer.o fs/rwblk.o fs/super.o fs/inode.o fs/dir.o fs/path.o fs/namei.o fs/blk_drv.o fs/exec.o fs/chr_drv.o fs/vfs.o fs/file.o misc/cprintf.o misc/printk.o lib/lib.a lib/malloc.o
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x100000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`

bin/%: usr/%.o usr/lib/lib.a lib/lib.a
	@mkdir -p $(@D)
	@echo + [ld] $@
	@ld -m elf_i386 -e _start -Ttext 0x1000000 -o $@ $^ `gcc -m32 -print-libgcc-file-name`
