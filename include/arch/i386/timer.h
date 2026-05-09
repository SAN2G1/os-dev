#ifndef __ARCH_I386_TIMER_H__
#define __ARCH_I386_TIMER_H__

#include "klib/types.h"


void timer_init(uint32_t frequency); 

void timer_tick(void); 

uint32_t timer_get_ticks(void); 


#endif 