; user.asm

; void __attribute__((noreturn)) move_to_user(
;		unsigned long pc, unsigned long sp, unsigned int eflags);
global move_to_user
extern __int_leave
move_to_user:
	cli
	mov eax, 0x23
	mov ebx, dword [esp + 4]  ; @pc
	mov ecx, dword [esp + 8]  ; @sp
	mov edx, dword [esp + 12] ; @eflags
	push eax	; ss
	push ecx	; esp
	push edx	; eflags
	push dword 0x1b	; cs
	push ebx	; eip
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	call __int_leave
	xor eax, eax
	mov ecx, eax
	mov edx, eax
	mov ebx, eax
	mov esi, eax
	mov edi, eax
	mov ebp, eax
	iretd
