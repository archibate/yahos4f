; usr/test.asm

global usr_test_start
usr_test_start:
	mov word [0xb8000], 0xc00 + 'O'
	mov word [0xb8002], 0xc00 + 'K'
	int 0x80
	jmp usr_test_start
