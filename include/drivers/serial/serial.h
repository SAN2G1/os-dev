#ifndef DRIVERS_SERIAL_SERIAL_H 
#define DRIVERS_SERIAL_SERIAL_H

#include "klib/types.h" 
int serial_init(void); 

void serial_write(const char *buf, uint32_t len); 

void serial_putc(char c) ; 

#endif
