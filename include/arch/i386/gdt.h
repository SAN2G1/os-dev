#ifndef _ARCH_I386_GDT_H_
#define _ARCH_I386_GDT_H_

#include "klib/types.h"

// 아래 매크로들은 디스크립터의 특정 비트 필드를 설정하기 위한 플래그들이다.

// G 비트: Granularity
// 0이면 limit이 바이트 단위(최대 1MB 근처), 1이면 limit이 4KB 단위로 확장(최대 4GB 근처)
#define SEG_GRAN(x)      ((x) << 0x0F)

// D/B 비트: 0이면 16비트 세그먼트, 1이면 32비트 세그먼트
#define SEG_SIZE(x)      ((x) << 0x0E)

// L 비트: Long mode(64비트 코드 세그먼트) 여부
// 32비트 운영체제(Protected Mode)에서는 반드시 0이어야 함
#define SEG_LONG(x)      ((x) << 0x0D)

// AVL 비트: OS가 임의로 쓰는 비트(하드웨어는 의미 부여 안 함)
#define SEG_SAVL(x)      ((x) << 0x0C)


// P 비트: Present (메모리에 존재하는 세그먼트인지)
#define SEG_PRES(x)      ((x) << 0x07)

// DPL(권한 레벨): 0~3
#define SEG_PRIV(x)     (((x) & 0x03) << 0x05)

// S 비트: 0이면 시스템 세그먼트(TSS/LDT 등), 1이면 코드/데이터 세그먼트
#define SEG_DESCTYPE(x)  ((x) << 0x04)


// 타입 필드(4비트): 코드/데이터 세그먼트의 세부 속성
#define SEG_DATA_RD        0x00 // 데이터: 읽기 전용
#define SEG_DATA_RDA       0x01 // 데이터: 읽기 전용(Accessed)
#define SEG_DATA_RDWR      0x02 // 데이터: 읽기/쓰기
#define SEG_DATA_RDWRA     0x03 // 데이터: 읽기/쓰기(Accessed)
#define SEG_DATA_RDEXPD    0x04 // 데이터: 읽기 전용, expand-down
#define SEG_DATA_RDEXPDA   0x05 // 데이터: 읽기 전용, expand-down(Accessed)
#define SEG_DATA_RDWREXPD  0x06 // 데이터: 읽기/쓰기, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // 데이터: 읽기/쓰기, expand-down(Accessed)

#define SEG_CODE_EX        0x08 // 코드: 실행 전용
#define SEG_CODE_EXA       0x09 // 코드: 실행 전용(Accessed)
#define SEG_CODE_EXRD      0x0A // 코드: 실행/읽기
#define SEG_CODE_EXRDA     0x0B // 코드: 실행/읽기(Accessed)
#define SEG_CODE_EXC       0x0C // 코드: 실행 전용, conforming
#define SEG_CODE_EXCA      0x0D // 코드: 실행 전용, conforming(Accessed)
#define SEG_CODE_EXRDC     0x0E // 코드: 실행/읽기, conforming
#define SEG_CODE_EXRDCA    0x0F // 코드: 실행/읽기, conforming(Accessed)

// ----------------------
// 32비트 OS용(Protected Mode)에서 흔히 쓰는 플랫 세그먼트
// base=0, limit=0xFFFFF, G=1(4KB 단위) => 사실상 4GB-1 범위
// L=0(64비트 아님), D/B=1(32비트 세그먼트)
// ----------------------

#define GDT_CODE_PL0 ( SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                       SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                       SEG_PRIV(0)     | SEG_CODE_EXRD )

#define GDT_DATA_PL0 ( SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                       SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                       SEG_PRIV(0)     | SEG_DATA_RDWR )

#define GDT_CODE_PL3 ( SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                       SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                       SEG_PRIV(3)     | SEG_CODE_EXRD )

#define GDT_DATA_PL3 ( SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                       SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                       SEG_PRIV(3)     | SEG_DATA_RDWR )


// 32비트 세그먼트 디스크립터(8바이트)를 구성해 64비트 정수로 출력한다.
// base: 32비트 베이스 주소
// limit: 20비트 limit (G=1이면 4KB 단위로 해석되어 확장됨)
// flag: Access Byte + Flags(G/D/B/L/AVL) 조합(위 매크로로 생성)
/*8bytes gdt entry*/

typedef struct __attribute__((packed))
{
    uint16_t limit_low; 
    uint16_t base_low; 
    uint8_t base_mid; 
    uint8_t access; 
    uint8_t granularity; 
    uint8_t base_high; 
} gdt_entry_t;

/*
 * GDTR 포멧 : limit(16) , base(32)
 * 중요 : lgdt 명령으로 gdt_ptr 이 호출 되어야함.
*/
typedef struct __attribute__((packed))
{
    uint16_t limit ; 
    uint32_t base ; 
} gdt_ptr_t; 

void gdt_install(void) ; 

#endif
