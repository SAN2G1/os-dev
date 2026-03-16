#include "arch/i386/pic.h"
#include "arch/i386/io.h"



void pic_sendEOI(uint8_t irq)
{
    if(irq >= 8) 
        outb(PIC2_COMMAND, PIC_EOI); 

    outb(PIC1_COMMAND, PIC_EOI);
}

/* 
 * 매개변수: 
 *      offset1 - 마스터 PIC의 인터럽트 벡터 오프셋
 *                마스터 PIC의 인터럽트는 offset1 ... offset1+7 범위를 사용
 */

void pic_remap(int offset1, int offset2) 
{
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // 초기화 시퀀스 시작 (cascade 모드)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	outb(PIC1_DATA, offset1);                 // ICW2: 마스터 PIC 인터럽트 벡터 오프셋 설정
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: 슬레이브 PIC 인터럽트 벡터 오프셋 설정
	io_wait();

	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: 마스터 PIC에게 IRQ2에 슬레이브 PIC가 연결되어 있음을 알림
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: 슬레이브 PIC에게 자신의 cascade ID가 2임을 알림 (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);               // ICW4: PIC를 8086 모드로 설정 (8080 모드가 아님)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// 두 PIC 모두의 인터럽트 마스크를 해제 (모든 IRQ 허용)
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void irq_set_mask(uint8_t IRQline)
{
    uint16_t port ; 
    uint8_t value; 

    if(IRQline < 8) 
    {
        port = PIC1_DATA; 
    }
    else
    {
        port = PIC2_DATA; 
        IRQline -= 8 ; 
    }

    value = inb(port) | (1 << IRQline); 
    outb(port, value); 
}


void irq_clear_mask(uint8_t IRQline)
{
    uint16_t port; 
    uint8_t value; 

    if(IRQline < 8) 
    {
        port = PIC1_DATA; 
    }
    else 
    {
        port = PIC2_DATA; 
        IRQline -= 8 ; 
    }

    value = inb(port) & ~(1 << IRQline); 
    outb(port, value); 
}

/* 보조 함수 */
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 명령을 PIC의 CMD 포트에 보내 레지스터 값을 읽을 준비를 한다.
     * PIC2는 체인 구조로 연결되어 있으며 IRQ 8~15를 담당한다.
     * PIC1은 IRQ 0~7을 담당하며, IRQ2는 PIC2와의 연결(cascade) 라인이다. */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* 두 PIC를 합친 IRQ 요청 레지스터(IRR) 값을 반환 */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

/* 두 PIC를 합친 인터럽트 서비스 레지스터(ISR) 값을 반환 */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
