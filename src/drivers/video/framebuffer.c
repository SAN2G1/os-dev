#include "drivers/video/fb_colors.h"
#include "drivers/video/framebuffer.h"
#include "arch/i386/io.h"

//커서의 위치를 저장하는 전역 변수 
static uint16_t cursor_pos = 0;

//Tab 폭
#define TAB_WIDTH 8

// 프레임 버퍼의 시작 지점
#define FB_START 0xb8000
#define FB_WIDTH 80
#define FB_HEIGHT 25

/* I/O 포트 */

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* 포트 명령 */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15



/*fb_write_cell : 
 * 프레임 버퍼의 i 위치에 주어진 전경색과 배경색으로 문자를 쓴다. 
 *
 * @param i 프레임버퍼 내 위치 
 * @param c 출력할 문자
 * @param fg 전경색
 * @param bg 배경색
 */

void fb_write_cell(uint32_t i, char c, fb_colors_t fg, fb_colors_t bg)
{
	char *vidptr = (char*) FB_START;

	vidptr[i] = c; 
	vidptr[i+1] = (0x10 * fg) + bg;
}

/*fb_putc
 * 프레임 버퍼의 cursor_pos 위치에 주어진 문자를 쓴다. 
 *
 * @param c 출력할 문자
 */
void fb_putc(char c)
{
	//LF, Line Feed
	if (c == '\n')
	{
		cursor_pos += FB_WIDTH - (cursor_pos % FB_WIDTH);
	}
	//CR, Carriage Return
	else if (c == '\r')
	{
		cursor_pos -= cursor_pos % FB_WIDTH;
	}
	//TAB
	else if (c == '\t')
	{
		cursor_pos += TAB_WIDTH - ((cursor_pos % FB_WIDTH) % TAB_WIDTH);
	}
	else if (c == '\b')
	{
		if (cursor_pos > 0)
		{
			cursor_pos--;
			fb_write_cell(cursor_pos*2, ' ', FB_BLACK, FB_LIGHTGREY);
		}
	}
	else
	{
		fb_write_cell(cursor_pos*2, c, FB_BLACK, FB_LIGHTGREY);
		cursor_pos++;
	}
	if (cursor_pos >= FB_WIDTH * FB_HEIGHT)
	{
		//스크롤링의 단순화 추후 구현
		fb_clear(); 
		
	}
	fb_move_cursor(cursor_pos);
}


/*fb_write : 
 * 프레임 버퍼에 fb_write_cell을 이용하여 내용을 쓴다. 
 *
 * @param buf 쓸 문자열이 저장된 버퍼 
 * @param len 쓸 문자열 길이
 */

void fb_write(char *buf, uint32_t len)
{
	uint32_t buf_point = 0 ; 

	while(buf[buf_point] != '\0' && buf_point < len)
	{
		fb_putc(buf[buf_point]);
		buf_point++; 
	}
}

/*
 * fb_clear : 
 * 화면을 공백 문자열로 초기화 한다.
 */
void fb_clear()
{
	for (uint32_t fb_point = 0; fb_point < 80 * 25 * 2; fb_point += 2) 
	{
		fb_write_cell(fb_point, ' ', FB_BLACK, FB_LIGHTGREY);
	}
    fb_move_cursor(0);
}


/*
 * fb_move_cursor:
 * 프레임 버퍼의 커서를 지정한 위치로 이동시킨다. 
 *
 * @param pos 커서의 새 위치 
 */

void fb_move_cursor(uint16_t pos)
{
	if (pos >= FB_WIDTH * FB_HEIGHT)
		return; 
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND); 
    outb(FB_DATA_PORT, ((pos >>8) & 0x00FF)); 
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND); 
    outb(FB_DATA_PORT, pos & 0x00FF); 
	cursor_pos = pos; 
}


