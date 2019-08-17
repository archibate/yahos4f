	org 0x7c00

bits 16
global _start
_start:
	jmp entry
	times 3-($-$$) nop

entry:
	xor ax, ax
	mov ds, ax
	mov es, ax

	mov si, load_msg
	call print

	mov ah, 0x41
	mov bx, 0x55aa
	mov dl, 0x80
	int 0x13
	cmp bx, 0xaa55
	jnz error

	call read_block

	mov eax, [0x8408]
	shl eax, 1
	mov [packet.blocknr], eax
	mov [packet.bufferseg], word 0x880
	mov [packet.count], word 8
	call read_block

	mov eax, [0x88a8]
	shl eax, 1
	mov [packet.blocknr], eax
	mov [packet.bufferseg], word 0x980
	mov [packet.count], word 2
	call read_block

	mov si, 0x9808
	jmp .go
.next_p	pop si
.next	mov cx, [si - 4]
	test cx, cx
	jle error
	add si, cx
	cmp si, 0xa008
	jae error
.go	mov cl, [si - 2]
	cmp cl, [file_name_len]
	jnz .next
	xor ch, ch
	push si
	mov di, file_name
.rep	cmpsb
	jnz .next_p
	loop .rep
	pop si

	mov ax, [si - 8]
	shl ax, 7
	mov si, 0x87a8
	add si, ax
	mov eax, [si - 36]
	jz error
	add eax, 1023
	shr eax, 10
	cmp ax, 12
	jna .cont
	sub ax, 12
	mov [rest_count], ax
	mov ax, 12
.cont	mov [counter], ax

	mov [packet.bufferseg], word 0x900
.load	lodsd
	shl eax, 1
	mov [packet.blocknr], eax
	mov bp, [packet.bufferseg]
	call read_block
	add [packet.bufferseg], word 0x40
	dec word [counter]
	jnz .load

	mov ax, [rest_count]
	test ax, ax
	jz .ok
	cmp ax, 1024
	ja error
	mov [counter], ax
	mov eax, [si]
	shl eax, 1
	mov [packet.blocknr], eax
	mov si, 0x780
	xchg [packet.bufferseg], si
	mov [rest_count], dword 0
	call read_block
	mov [packet.bufferseg], si
	mov si, 0x7800
	jmp .load

.ok	mov si, ok_msg
	call print

	jmp 0x9000

read_block:
	push si
	mov si, dot_msg
	call print
	mov ah, 0x42
	mov dl, 0x80
	mov si, packet
	int 0x13
	jc error
	pop si
	ret

error:
	mov si, error_msg
	call print
halt:
	hlt
	jmp halt

print:
	mov ah, 0x0e
	mov bx, 0x0007
.repeat	lodsb
	test al, al
	jz .done
	int 0x10
	jmp .repeat
.done	ret

packet:
.packet_size	db	16
.reserved_1	db	0
.count		dw	4
.bufferoff	dw	0
.bufferseg	dw	0x800
.blocknr	dq	2

load_msg	db	"Loading System...", 0
error_msg	db	"ERROR", 13, 10, 0
ok_msg		db	"OK", 13, 10, 0
dot_msg		db	".", 0

counter		dw	0
rest_count	dw	0

file_name	db	"Image"
file_name_len	db	$ - file_name

	times 510-($-$$) db 0
	dw 0xaa55
