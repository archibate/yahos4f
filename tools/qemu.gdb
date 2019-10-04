#!/usr/bin/gdb -x
# vim: ft=gdb ts=4 sts=4 tw=4

define cn
		file kernel
		target remote localhost:1234
end
cn

define boo
	set architecture i8086
	break *0x7c00
	continue
end

define bc
	break $arg0
	continue
end

define bcc
	break $arg0
	continue
	continue
end

define q
	kill
	quit
end

define pe
	x/4wx (int *)current->file - 1
end

define u
	x/10i $pc
end

bc idt.c:37
