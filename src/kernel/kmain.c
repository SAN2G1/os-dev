#if defined(__linux__) 
#error "Do not compile with a Linux-targeting compiler" 
#endif

#if !defined(__i386__) 
#error "This kernel must be compiled for 32-bit x86" 
#endif 
/*
 * kernel.c
 */
#include "drivers/video/framebuffer.h"
#include "drivers/serial/serial.h"
#include "debug.h"
#include "klib/kstring.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/pic.h"
#include "arch/i386/timer.h"
#define PRINT_DEBUG(msg) fb_write(msg, kstrlen(msg))

/*LOG 출력 관련 메크로 정의*/

#define KLOG_INFO(tag, str) serial_log(INFO, tag, str)
#define KLOG_DEBUG(tag, str) serial_log(DEBUG, tag, str)
#define KLOG_ERROR(tag, str) serial_log(ERROR, tag, str)

void kmain(void)
{
    //debug log를 위한 초기화. 
    //최초 로그 레벨은 INFO로 설정.
    serial_set_log_level(INFO); 
    serial_init(); 

    KLOG_INFO("Kernel", "Kernel is starting"); 
    KLOG_DEBUG("Kernel", "Kernel is starting"); 
    KLOG_ERROR("Kernel", "Kernel is starting"); 
	char *str = "my first kernel\n"; 
    fb_clear(); 
	fb_write(str, kstrlen(str)); 
    gdt_install(); 
    PRINT_DEBUG("GDT installation successful!\n");
    serial_printf("[KMAIN] gdt_install done\n");
    
    idt_init(); 
    serial_printf("[KMAIN] idt_init done\n");
    //PIC 초기화
    pic_remap(0x20, 0x28); 
    serial_printf("[KMAIN] pic_remap done\n");
    
    // 모든 IRQ를 마스킹하여 인터럽트가 발생하지 않도록 설정
    for (int i = 0; i  < 16; i++)
    {
        irq_set_mask(i); 
    }

    // IRQ0 setup: 타이머 인터럽트 설정
    timer_init(100); // 100Hz로 타이머 초기화
    PRINT_DEBUG("timer_init done\n");
    serial_printf("[KMAIN] timer_init done\n");


    // 필요한 IRQ만 언마스킹하여 인터럽트가 발생하도록 설정
    irq_clear_mask(0); // timer
    irq_clear_mask(1); // keyboard
    //irq_clear_mask(2); // cascade 이후 IRQ 8~15 쓸 때나 언마스킹
    serial_printf("[KMAIN] irq mask configured\n");
    __asm__ volatile ("sti"); // set the interrupt
    serial_printf("[KMAIN] sti enabled\n");
    PRINT_DEBUG("PIC ON"); 


    for (;;) {
        __asm__ volatile ("hlt");
    }
}
