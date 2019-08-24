#!/usr/bin/gdb -x
# vim: ft=gdb ts=4 sts=4 tw=4

file kernel
target remote localhost:1234

define bl
	set architecture i8086
	break *0x7c00
	continue
end

define tm
	break main
	continue
end

define q
	kill
	quit
end
