; user.asm

; void __attribute__((noreturn, fastcall)) move_to_user(void *sp, unsigned int eflags);
global move_to_user
extern __int_leave
move_to_user:
	cli
	mov eax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	push eax	; ss
	push ecx	; esp
	push edx	; eflags
	push dword 0x1b	; cs
	push dword .ret	; eip
	call __int_leave
	xor eax, eax
	mov ecx, eax
	mov edx, eax
	mov ebx, eax
	mov esi, eax
	mov edi, eax
	mov ebp, eax
	iretd
.ret:
	ret
