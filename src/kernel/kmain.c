#if defined(__linux__) 
#error "Do not compile with a Linux-targeting compiler" 
#endif

#if !defined(__i386__) 
#error "This kernel must be compiled for 32-bit x86" 
#endif 
/*
 * kernel.c
 */
#include <drivers/video/framebuffer.h>
void kmain(void)
{
	char *str = "my first kernel"; 
	fb_write(str, 16); 
}
