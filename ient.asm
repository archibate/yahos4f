; ientry.asm
extern __int_leave

global asm_on_keyboard
extern on_keyboard
asm_on_keyboard:
	pushad
	call on_keyboard
	call __int_leave
	popad
	iretd

global asm_on_timer
extern on_timer
asm_on_timer:
	pushad
	call on_timer
	call __int_leave
	popad
	iretd

global asm_on_page_fault
extern on_page_fault
asm_on_page_fault:
	pushad
	call on_page_fault
	call __int_leave
	popad
	add esp, 4
	iretd

global asm_on_syscall
extern on_syscall
asm_on_syscall:
	pushad
	call on_syscall
	call __int_leave
	popad
	iretd
