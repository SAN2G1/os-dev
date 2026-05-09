
#include "arch/i386/interrupts.h"
#include "arch/i386/pic.h"
#include "arch/i386/io.h"
#include "drivers/serial/serial.h"
#include "arch/i386/timer.h"
#include "drivers/keyboard/keyboard.h"

void irq_handler(registers_t *r) 
{
    uint8_t irq = (uint8_t)(r->int_no - 32); 
    switch(irq) 
    {
        case 0: /*timer tick */ 
            timer_tick();
            break; 
        case 1: /*keyboard read */ 
            keyboard_handle_irq();
            break; 

        default: break ; 
    }

    pic_sendEOI(irq); 
}
  