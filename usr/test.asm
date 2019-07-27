; usr/test.asm

global usr_test_start
usr_test_start:
	mov eax, 1
	jc .m2
	mov ecx, 0xc
	mov edx, message1
	jmp .mx
.m2:
	mov ecx, 0xa
	mov edx, message2
.mx:
	int 0x80
	mov eax, 2
	int 0x80
	jmp usr_test_start

message1 db "O", 0
message2 db "K", 0
