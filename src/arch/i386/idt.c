/* 인터럽트 디스크립터 테이블*/

#include "arch/i386/idt.h"
#include "klib/types.h"
#include "arch/i386/interrupts.h"
#include "drivers/serial/serial.h" //디버깅용

// 인터럽트 디스크립터 테이블 생성 -> 256
__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS] ; // IDT 엔트리 배열 생성 ; 성능을 위한 16바이트 경계에 정렬


static idtr_t idtr; 
extern void* isr_stub_table[32];
static bool vectors[IDT_MAX_DESCRIPTORS];

//irq 엔트리 등록
extern void * irq_stub_table[16]; 

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags)
{
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

static bool vectors[IDT_MAX_DESCRIPTORS];

void idt_init(void) {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
    for (uint8_t i = 0; i < 16; i++) 
    {
        idt_set_descriptor(32+i, irq_stub_table[i], 0x8E); 
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    // __asm__ volatile ("sti"); // set the interrupt flag -> kernel/kmai.c 에서 진행
}

//general exception handler
void isr_handler(registers_t *r) {
    serial_printf("[ISR] int=%d err=0x%x eip=0x%x\n",
                  r->int_no, r->error_code, r->eip);

    if (r->int_no == 14) { // page fault 같은 치명 예외만 일단 정지
        for (;;) { __asm__ volatile("cli; hlt"); }
    }
}
