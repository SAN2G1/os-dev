/*인터럽트 디스크립터 테이블*/
#ifndef _ARCH_I386_IDT_H_
#define _ARCH_I386_IDT_H_

#include "klib/types.h"
#include "arch/i386/interrupts.h"

//IDT 엔트리 개수(보통 256 or 마지막 엔트리 인덱스 +1)
#define IDT_MAX_DESCRIPTORS 256 

 
typedef struct {
    uint16_t isr_low ; // ISR(interrupt Service Routine)의 하위 16비트 주소
    uint16_t kernel_cs ; // ISR 호출 전에 CPU가 CS에 로드할 GDT 세그먼트 셀렉터 
    uint8_t reserved; // 0으로 설정
    uint8_t attributes; // 타입 및 속성
    uint16_t isr_high; // ISR 주소의 상위 16비트 
} __attribute__((packed)) idt_entry_t; 


// IDTR 구조체
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_init(void);



#endif