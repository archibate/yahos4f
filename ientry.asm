global asm_on_soft_interrupt
extern on_soft_interrupt
asm_on_soft_interrupt:
	pushad
	call on_soft_interrupt
	popad
	iretd

global asm_on_keyboard
extern on_keyboard
asm_on_keyboard:
	pushad
	call on_keyboard
	popad
	iretd

global asm_on_timer
extern on_timer
asm_on_timer:
	pushad
	call on_timer
	popad
	iretd
