; boot0.asm -- first section of bootloader ;
start:
	cld
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov bp, 0x7C00
	mov sp, bp
	jmp 0000:load_stage2

load_stage2:
	mov di, STAGE2_BASE
	mov ax, STAGE2_SECTOR
	mov cl, STAGE2_SECTORS
	call read_sector
	
load_superblock:
	mov di, SUPERBLOCK_BASE
	mov ax, SUPERBLOCK_SECTOR
	mov cl, SUPERBLOCK_SECTORS
	call read_sector

load_group_desc:
	mov di, GROUP_DESC_BASE
	mov ax, GROUP_DESC_BLOCK
	mov cl, GROUP_DESC_COUNT
	call read_blocks

load_inodes:
	mov ax, di
	add ax, 8
	mov di, INODE_TABLE_BASE
	mov cx, INODE_BLOCKS
load_inodes_loop:
	push ax
	push cx
	mov cl, 1
	call read_blocks
	pop cx
	pop ax
	inc ax
	add di, BLOCK_SIZE
	loop load_inodes_loop

load_root_dir_entry:
	mov ax, ROOT_DENTRY_NODE
	call get_inode_addr
	push di + I_SIZE

; boot signature ;
times 510-($-$$) db 0
dw 0xaa55
