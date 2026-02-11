; src/arch/i386/gdt_flush.s
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ; data seg reload (0x10 = GDT_KERNEL_DS)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; code seg reload (far jump)
    jmp 0x08:flush_done
flush_done:
    ret
