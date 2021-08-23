section .multiboot
header_start:
        ; magic number ;
header_magic dd 0xe85250d6
        ; architecture ;
header_arch dd 0
        ; length ;
header_length dd header_end - header_start
        ; checksum ;
header_checksum dd 0x100000000 - (0xe85250d6 + (header_end - header_start))
        ; end tag ;
header_end_tag:
        dw 0
        dw 0
        dd 8
header_end:
