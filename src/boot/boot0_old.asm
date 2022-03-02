; boot0.asm -- first section of bootloader ;
	bits 16
	org 0x7c00 ; program start in memory ;

	jmp start
	nop

; start of bootloader ;
start:
	mov byte[bootdev], dl
	
	mov si, msg
	call print
	
	; load the superblock ;
	mov dh, 0
	mov dl, byte[bootdev]
	mov al, 2
	mov cl, 3
	mov bx, superblock
	call read_sector
	
	; obtain block size ;
	;mov ax, 1024
	;mov cl, byte[size_of_blocks + 3]
	;shl ax, cl
	;mov word[size_of_blocks + 2], ax
	
	; load the block descriptor table ;
	;mov dh, 0
	;mov dl, byte[bootdev]
	;mov al, 2
	;mov cl, 5
	;mov bx, bdl
	;call read_sector
	mov dx, 2
	call read_block
	
	; load first block group ;
	mov dx, word[number_of_blocks]
	shl dx, 8
	call read_block
	
	jmp $
	
	msg db "booting...", 10, 13, 0
	errmsg db "disk read error", 0

read_sector:
	; ah: instruction ;
	; dh: head ;
	; dl: drive ;
	; al: sectors ;
	; cl: start sector ;
	; bx: buffer ;
	mov ah, 02h
	push bx
	mov bx, 0
	mov es, bx
	pop bx
	int 13h ; call interrupt ;
	
	jc print_err ; print an error ;
	
	ret

read_block:
	; dx: block number ;
	; cl: number of sectors ;
	; calculate block number ;
	push ax
	push cx
	mov cx, 2
	mul cx
	pop cx
	add dx, 1
	mov ax, dx
	; convert logical to physical ;
	call l2hts
	; read sectors ;
	mov al, 2
	mov dl, byte[bootdev]
	call read_sector
	; return ax ;
	pop ax
	
	; return ;
	ret

print_err:
	; error ;
	mov si, errmsg
	call print
	jmp $

print:
	pusha
	mov ah, 0eh
	cld
.repeat:
	lodsb
	cmp al, 0
	je .done
	int 10h
	jmp .repeat
.done:
	popa
	ret
	
; convert logical sectors to hts stuff ;
l2hts:
	
	; division ;
	mov dl, 63 ; number of sectors in a track ;
	add ax, 1  ; starts at one, not zero ;
	div dl ; get number of sectors and track ;
	mov dh, al ; dh = head = track ;
	mov cl, ah ; cl = sector number ;
	
	ret

; values ;
	bootdev db 0x00

; boot signature ;
times 510-($-$$) db 0
dw 0xaa55
superblock: times 1024 db 0 ; buffer for superblock ;
number_of_inodes equ superblock + 0
number_of_blocks equ superblock + 4
number_of_inodes_bg equ superblock + 40
number_of_blocks_bg equ superblock + 32
size_of_blocks equ superblock + 24
bdl: times 1024 db 0
bdl_addr_bub equ bdl + 0 ; block address of blk usage bitmap ;
bdl_addr_iub equ bdl + 4 ; block address of inode usage bitmap ;
bdl_start_inode_table equ bdl + 8 ; dword ;
bdl_n_directories_grp equ bdl + 16 ; word ;
