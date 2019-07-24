; kernel.asm

MB_MAGIC	equ	0x1BADB002
MB_FLAGS	equ	0
	dd	MB_MAGIC
	dd	MB_FLAGS
	dd	-(MB_MAGIC + MB_FLAGS)

global _start
_start:
	mov [0xb8000], word 0xf00 + 'O'
	mov [0xb8002], word 0xf00 + 'K'
	cli
	hlt
