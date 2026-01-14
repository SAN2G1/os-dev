/*
 * include/drivers/video/framebuffer.h 
 */

#ifndef DRIVERS_VIDEO_FRAMEBUFFER_H 
#define DRIVERS_VIDEO_FRAMEBUFFER_H

#include <drivers/video/fb_colors.h>

void fb_write_cell(unsigned int i, char c, fb_colors_t fg, fb_colors_t bg);

void fb_write(char *buf, unsigned int len);

void fb_clear();

void fb_move_cursor(unsigned short pos);


#endif

