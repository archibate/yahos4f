bits 16

	org 0x9000
_start:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00

	push word 0
	call pc_thunk
	add bp, kernel - $
	lea sp, [bp - 2]
	cmp [bp], dword 0x464c457f
	jnz error
	
	cli
	cld
	
	call kbc_wait
	mov al, 0xd1
	out 0x64, al
	call kbc_wait
	mov al, 0xdf
	out 0x60, al
	call kbc_wait
	
	lgdt [gdtr]
	lidt [idtr]
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax
	
	jmp dword 0x8 : prot_start

pc_thunk:
	pop bp
	push bp
	retf

error:
	int 0x19
	cli
	hlt

kbc_wait:
	in al, 0x64
	test al, 0x02
	jnz kbc_wait
	ret

bits 32
prot_start:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	call bootmain
halt:
	hlt
	jmp halt

bootmain: ; (ebp = elf)
	mov eax, [ebp + 28] ; e_phoff
	movzx edx, word [ebp + 42] ; e_phentsize
	lea ebx, [ebp + eax] ; ph
.repeat: ; do {
	dec word [ebp + 44] ; if (!--e_phnum)
	jz .ok
	call load_prog ; load_prog(ph)
	add ebx, edx ; ph += e_phentsize
	jmp .repeat ; } while (1)
.ok 	jmp dword [ebp + 24] ; (*e_entry)()

load_prog: ; (ebp = elf, ebx = ph)
	mov esi, [ebx + 4] ; p_offset
	cmp esi, 0
	jz .return
	add esi, ebp ; src = elf + p_offset
	mov ecx, [ebx + 16] ; size = p_filesz
	mov edi, [ebx + 12] ; dst = p_paddr
	rep movsb ; memcpy(src, dst, size)
	; src += size; dst += size
	mov ecx, [ebx + 20] ; p_memsz
	sub ecx, [ebx + 16] ; size = p_memsz - p_filesz
	xor al, al
	rep stosb ; memset(dst, 0, size)
.return	ret

	align 4
gdt:
	dd	0x00000000, 0x00000000
	dd	0x0000ffff, 0x00cf9a00
	dd	0x0000ffff, 0x00cf9200
	
gdtr:
	dw	$-gdt-1
	dd	gdt
idtr:
	dw	0
	dd	0

	dd	0, 0, 0, 0
	times 1024-($-$$) db 0
kernel:
