; kernel.asm

MB_MAGIC	equ	0x1BADB002
MB_FLAGS	equ	0
	dd	MB_MAGIC
	dd	MB_FLAGS
	dd	-(MB_MAGIC + MB_FLAGS)

global _start
extern main
extern setup_pagging
_start:
	call setup_pagging
	call main
	cli
	hlt
