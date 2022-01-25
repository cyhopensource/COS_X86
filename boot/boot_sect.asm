
[org 0x7c00]
KERNEL_OFFSET equ 0x1000  ;; 

mov [Boot_driver], dl

;;mov bp, 0x9000   ;; stack
mov bp, 0x9000
mov sp, bp

mov bx, Msg_real_mode
call print_msg_func
call print_nl

call load_kernel

mov bx, Msg_Finshed_load_kernel
call print_msg_func
call print_nl

call switch_to_pm

jmp $
;;mov al, ' '
;;int 0x10

;;mov bx, Good_bye_msg
;;call print_msg_func

;;mov bx, 0x8B01
;;call print_16bhex_func

;;-----------------------

;; call function section
%include "print_msg.asm"
%include "print_hex.asm"
%include "disk_load.asm"
%include "gdt_init.asm"
%include "print_msg_32.asm"
%include "switch_to_pm.asm"

[bits 16]

load_kernel:

    mov bx, Msg_load_kernel
    call print_msg_func
    call print_nl

    mov bx, KERNEL_OFFSET
    mov dh, 31
    mov dl, [Boot_driver]
    call disk_load

    ret


[bits 32]
BEGIN_PM:
    mov ebx, Msg_prot_mode
    call print_msg_pm

    call KERNEL_OFFSET

    jmp $

Boot_driver:
    db 0

;; data section
Disk_error_msg:
    db 'Disk_error!',0
Debug_msg:
    db 'Here!',0
Msg_real_mode:
    db "Started in 16-bit Real Mode", 0
Msg_prot_mode:
    db "Landed in 32-bit Protected Mode", 0
Msg_load_kernel:
    db "Loading kernel into memory", 0
Msg_Finshed_load_kernel:
    db "Loading kernel Finished", 0

;; padding section
times 510-($-$$) db 0
dw 0xaa55

;; bootstrape section
;;times 256 dw 0xdada
;;times 256 dw 0xface
;;times 256 dw 0xabab
