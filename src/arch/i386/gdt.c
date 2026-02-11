#include "klib/types.h"
#include "arch/i386/gdt.h"
#include "debug.h"


#define GDTLOG_INFO(str) serial_log(INFO, "GDT", str)
#define GDTLOG_DEBUG(str) serial_log(DEBUG, "GDT", str)
#define GDTLOG_ERROR(str) serial_log(ERROR, "GDT", str)

#define GDT_ENTRY_COUNT 6

static gdt_entry_t gdt[GDT_ENTRY_COUNT];
static gdt_ptr_t gdt_ptr; 
static int gdt_index = 0; 

extern void gdt_flush(uint32_t gdt_ptr_addr); 

static void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    // 1. 인덱스 범위 검사
    if (gdt_index >= GDT_ENTRY_COUNT) {
        GDTLOG_ERROR("GDT Index Overflow!");
        return;
    }

    // 2. Limit 범위 검사 (20비트)
    if (limit > 0xFFFFF) {
        GDTLOG_ERROR("GDT Limit too large (max 0xFFFFF)");
        return;
    }

    gdt_entry_t *e = &gdt[gdt_index++];

    // 1. limit 설정 (하위 16비트)
    e->limit_low = (uint16_t)(limit & 0xFFFF);

    // 2. base 설정 (24비트 + 8비트 분할)
    e->base_low  = (uint16_t)(base & 0xFFFF);
    e->base_mid  = (uint8_t)((base >> 16) & 0xFF);
    e->base_high = (uint8_t)((base >> 24) & 0xFF);

    // 3. access 설정 (flag의 하위 8비트)
    e->access = (uint8_t)(flag & 0xFF);

    // 4. flags 및 limit 상위 4비트 설정
    // flag >> 8을 통해 매크로의 G, D/B 비트 등을 granularity의 상위 4비트 위치로 보냅니다.
    e->granularity = (uint8_t)(((limit >> 16) & 0x0F) | ((flag >> 8) & 0xF0));


    GDTLOG_INFO("GDT Descriptor created");
}

void gdt_install(void)
{
    gdt_index = 0;

    // 1. NULL Descriptor
    create_descriptor(0, 0, 0);

    // 2. Kernel Code/Data (Flat Model: 0~4GB)
    create_descriptor(0, 0xFFFFF, (uint16_t)GDT_CODE_PL0);
    create_descriptor(0, 0xFFFFF, (uint16_t)GDT_DATA_PL0);

    // 3. User Code/Data (Flat Model: 0~4GB)
    create_descriptor(0, 0xFFFFF, (uint16_t)GDT_CODE_PL3);
    create_descriptor(0, 0xFFFFF, (uint16_t)GDT_DATA_PL3);

    // 4. GDT Pointer 설정
    gdt_ptr.limit = (uint16_t)(sizeof(gdt) - 1);
    gdt_ptr.base  = (uint32_t)&gdt;

    // 5. 어셈블리 함수 호출하여 CPU에 로드
    gdt_flush((uint32_t)&gdt_ptr);
    GDTLOG_INFO("GDT loaded");

}