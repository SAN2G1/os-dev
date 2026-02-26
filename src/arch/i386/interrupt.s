extern isr_handler



%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0    ; 에러코드로 0을 푸시
    push dword %1   ; 인터럽트 번호 푸시
    jmp common_isr ; 공통 핸들러로 점프
%endmacro


%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    dword %1                    ; 인터럽트 번호를 푸시
    jmp     common_isr   ; 공통 핸들러로 점프
%endmacro

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler    8
no_error_code_interrupt_handler 9
error_code_interrupt_handler    10
error_code_interrupt_handler    11
error_code_interrupt_handler    12
error_code_interrupt_handler    13
error_code_interrupt_handler    14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler    17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
error_code_interrupt_handler    30
no_error_code_interrupt_handler 31


common_isr: 
    pusha ; Push All General-Purpose Registers 

    xor eax, eax
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call isr_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    iret


global isr_stub_table
isr_stub_table:
%assign i 0 
%rep 32
    dd interrupt_handler_%+i ; 
%assign i i+1
%endrep