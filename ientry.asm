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
