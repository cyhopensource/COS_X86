[extern isr_handle]

isr_common_stub:
    pusha
    mov ax, ds
    push eax   ;;save the data segment des.

    mov ax, 0x10  ;; kernel  data segment des.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handle  ;; use stack to pass para.

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8 ;;clean up the push error.
    sti
    iret

[extern irq_handle]

irq_common_stub:
    pusha

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handle

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret  ;; // ret from interrupt.

;; interrupt callee.
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14

; IRQs
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15


; 0: Divide By Zero Exception
isr0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

; 1: Debug Exception
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

; 2: Non Maskable Interrupt Exception
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

; 3: Int 3 Exception
isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

; 4: INTO Exception
isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: Out of Bounds Exception
isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: Invalid Opcode Exception
isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: Coprocessor Not Available Exception
isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

; 8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push byte 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

isr10:
    cli
    push byte 0
    push byte 10
    jmp isr_common_stub

isr11:
    cli
    push byte 0
    push byte 11
    jmp isr_common_stub

isr12:
    cli
    push byte 0
    push byte 12
    jmp isr_common_stub

isr13:
    cli
    push byte 13
    jmp isr_common_stub

isr14:
    cli
    push byte 14
    jmp isr_common_stub

;; irq

irq0:
    cli
    push byte 0
    push byte 32
    jmp irq_common_stub

irq1:
    cli
    push byte 1
    push byte 33
    jmp irq_common_stub

irq2:
    cli
    push byte 2
    push byte 34
    jmp irq_common_stub

irq3:
    cli
    push byte 3
    push byte 35
    jmp irq_common_stub

irq4:
    cli
    push byte 4
    push byte 36
    jmp irq_common_stub

irq5:
    cli
    push byte 5
    push byte 37
    jmp irq_common_stub

irq6:
    cli
    push byte 6
    push byte 38
    jmp irq_common_stub

irq7:
    cli
    push byte 7
    push byte 39
    jmp irq_common_stub

irq8:
    cli
    push byte 8
    push byte 40
    jmp irq_common_stub

irq9:
    cli
    push byte 9
    push byte 41
    jmp irq_common_stub

irq10:
    cli
    push byte 10
    push byte 42
    jmp irq_common_stub

irq11:
    cli
    push byte 11
    push byte 43
    jmp irq_common_stub

irq12:
    cli
    push byte 12
    push byte 44
    jmp irq_common_stub

irq13:
    cli
    push byte 13
    push byte 45
    jmp irq_common_stub

irq14:
    cli
    push byte 14
    push byte 46
    jmp irq_common_stub

irq15:
    cli
    push byte 15
    push byte 47
    jmp irq_common_stub



