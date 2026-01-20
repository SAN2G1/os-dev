/*
 * include/drivers/video/framebuffer.h 
 */

#ifndef DRIVERS_VIDEO_FRAMEBUFFER_H 
#define DRIVERS_VIDEO_FRAMEBUFFER_H

#include "drivers/video/fb_colors.h"
#include "klib/types.h" 
void fb_write_cell(uint32_t i, char c, fb_colors_t fg, fb_colors_t bg);

void fb_write(char *buf, uint32_t len);

void fb_clear();

void fb_move_cursor(uint16_t pos);


#endif

