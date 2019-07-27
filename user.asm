; user.asm

; void __attribute__((noreturn, fastcall)) move_to_user(void *pc, void *sp, unsigned int eflags);
global move_to_user
move_to_user:
	mov	eax, 0x23
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ebx, [esp + 4]
	push	eax		; ss
	push	edx		; esp
	push	ebx		; eflags
	push	dword 0x1b	; cs
	push	ecx		; eip
	iretd
