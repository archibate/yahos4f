; usr/test.asm

global usr_test_start
usr_test_start:
	mov eax, 1
	int 0x80
	mov eax, 2
	int 0x80
	jmp usr_test_start
