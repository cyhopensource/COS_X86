print_msg_func:
    pusha

print_start:
    mov al, [bx] 
    cmp al, 0 
    je print_msg_end

    mov ah, 0x0e
    int 0x10 

    add bx, 1
    jmp print_start

print_msg_end:
    ;;mov bx, dx
    ;;call print_16bhex_func
    popa
    ret


print_nl:
    pusha
    
    mov ah, 0x0e
    mov al, 0x0a ; newline char
    int 0x10
    mov al, 0x0d ; carriage return
    int 0x10
    
    popa
    ret

print_flush_screen:
    pusha

    mov ah,0x06
    mov al,0

    mov ch,0  ;(0,0)
    mov cl,0
    mov dh,24  ;(24,79)
    mov dl,79
    mov bh,0x07 ;whith black
    int 0x10

    popa
    ret
