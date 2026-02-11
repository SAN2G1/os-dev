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
   

}
