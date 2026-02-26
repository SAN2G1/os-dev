#ifndef DRIVERS_SERIAL_SERIAL_H 
#define DRIVERS_SERIAL_SERIAL_H

#include "klib/types.h" 
void serial_init(void); 

void serial_write(const char *buf, uint32_t len); 

void serial_putc(char c) ; 
void serial_puts(const char *str); 

void serial_printf(const char* str, ...);
#endif
