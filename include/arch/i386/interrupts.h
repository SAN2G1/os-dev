#ifndef DRIVERS_INTERRUPTS_H
#define DRIVERS_INTERRUPTS_H

#include "klib/types.h"

typedef struct registers {
    uint32_t ds;          // common_isr에서 push한 ds 저장값

    // pusha 결과 (메모리에서 보이는 순서)
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;         // pusha가 저장한 "진입 시점 esp"
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // stub이 push한 값들
    uint32_t int_no;
    uint32_t error_code; // cpu or stub

    // CPU가 자동으로 push한 값들
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed)) registers_t;

void isr_handler(registers_t *r);

#endif