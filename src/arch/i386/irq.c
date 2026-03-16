
#include "arch/i386/interrupts.h"
#include "arch/i386/pic.h"
#include "arch/i386/io.h"
#include "drivers/serial/serial.h"
void irq_handler(registers_t *r) 
{
    uint8_t irq = (uint8_t)(r->int_no - 32); 
    switch(irq) 
    {
        case 0: /*timer tick */ break; 
        case 1: /*keyboard read */ 
                if(inb(0x64) & 0x01)
                {
                    uint8_t sc = inb(0x60) ; 
                    serial_printf("[IRQ1] keyboard scancode = 0x%x\n", sc) ; 
                }
                break; 

        default: break ; 
    }

    pic_sendEOI(irq); 
}
