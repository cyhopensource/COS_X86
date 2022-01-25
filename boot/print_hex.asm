print_16bhex_func:    ; bx para
    push ax
    push bx
    push cx
    push dx
    mov ah, 0x0e
    mov cl, 12
    mov al, '0'
    int 0x10
    mov al, 'x'
    int 0x10

print_16hex_func_start:
    mov dx, bx
    shr dx, cl
    sub cl, 4

    and dl, 0x0F
    cmp dl, 10
    jl print_16hex_jl10

    add dl, 55
    mov al, dl
    int 0x10
    cmp cl, -4
    je print_16hex_end
    jmp print_16hex_func_start

print_16hex_jl10:
    add dl, 48
    mov al, dl
    int 0x10
    cmp cl, -4
    je print_16hex_end
    jmp print_16hex_func_start

print_16hex_end:
    mov al, ' '
    int 0x10
    pop dx
    pop cx
    pop bx
    pop ax
    ret