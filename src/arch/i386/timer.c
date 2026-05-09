#include "arch/i386/timer.h"
#include "arch/i386/io.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQUENCY 1193182

static volatile uint32_t ticks = 0;


void timer_init(uint32_t frequency)
{
    uint32_t divisor = PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
// 타이머 인터럽트가 발생할 때마다 호출되는 함수
// ticks 변수를 증가시켜 시스템이 부팅된 이후로 경과된 틱 수를 추적
void timer_tick(void)
{
    ticks++;
}
// 현재까지 경과된 틱 수를 반환하는 함수
uint32_t timer_get_ticks(void)
{
    return ticks;
}
