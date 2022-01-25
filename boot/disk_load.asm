disk_load:
    push dx

    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

    int 0x13

    jc disk_error

    pop dx
    cmp dh, al
    jne disk_error
    ret

disk_error:
    mov bx, ax
    call print_16bhex_func

    mov bx, Disk_error_msg
    call print_msg_func
    call print_nl
    jmp $