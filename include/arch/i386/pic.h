#ifndef _ARCH_I386_PIC_H_
#define _ARCH_I386_PIC_H_

#include "klib/types.h"
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI   0x20 /* End-of-interrupt command code */

void pic_sendEOI(uint8_t irq); 

/* PIC 컨트롤러를 재초기화하여 기본값이 아닌 
 * 지정된 인터럽트 백터 오프셋을 사용하도록 설정한다. */ 

#define ICW1_ICW4	0x01		/* ICW4가 뒤에 이어서 전달된다는 것을 의미 */
#define ICW1_SINGLE	0x02		/* 단일 모드 (cascade 모드가 아님) */
#define ICW1_INTERVAL4	0x04		/* 인터럽트 벡터 간격 4 (기본은 8) */
#define ICW1_LEVEL	0x08		/* 레벨 트리거 모드 (기본은 엣지 트리거) */
#define ICW1_INIT	0x10		/* 초기화 시작 - 반드시 필요 */

#define ICW4_8086	0x01		/* 8086/88 모드 (MCS-80/85 모드가 아님) */
#define ICW4_AUTO	0x02		/* 자동 EOI 모드 */
#define ICW4_BUF_SLAVE	0x08		/* 버퍼 모드 / 슬레이브 */
#define ICW4_BUF_MASTER	0x0C		/* 버퍼 모드 / 마스터 */
#define ICW4_SFNM	0x10		/* 특수 완전 중첩 모드 */

#define CASCADE_IRQ 2

void pic_remap(int offset1, int offset2);

/// IRQ 마스킹

void irq_set_mask(uint8_t IRQline);
void irq_clear_mask(uint8_t IRQline);
#define PIC_READ_IRR                0x0a    /* OCW3: 다음 CMD 포트 읽기에서 IRR 값을 읽도록 설정 */
#define PIC_READ_ISR                0x0b    /* OCW3: 다음 CMD 포트 읽기에서 ISR 값을 읽도록 설정 */

uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
#endif
