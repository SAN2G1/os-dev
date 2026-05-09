#ifndef __DRIVERS_KEYBOARD_H__
#define __DRIVERS_KEYBOARD_H__

#include "klib/types.h"

void keyboard_init(void);
int keyboard_getchar(void);
void keyboard_handle_irq(void);

#endif
