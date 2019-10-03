#!/usr/bin/gdb -x
# vim: ft=gdb ts=4 sts=4 tw=4

define cn
		file kernel
		target remote localhost:1234
end

define boo
	set architecture i8086
	break *0x7c00
	continue
end

define bc
	break $arg0
	continue
end

define q
	kill
	quit
end

cn
