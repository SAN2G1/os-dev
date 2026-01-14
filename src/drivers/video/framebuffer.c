#include <drivers/video/fb_colors.h>
#include <drivers/video/framebuffer.h>
#include <drivers/video/io.h>

// 프레임 버퍼의 시작 지점
#define FB_START 0xb8000

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

void fb_write_cell(unsigned int i, char c, fb_colors_t fg, fb_colors_t bg)
{
	char *vidptr = (char*) FB_START;

	vidptr[i] = c; 
	vidptr[i+1] = (0x10 * fg) + bg;
}

/*fb_write : 
 * 프레임 버퍼에 fb_write_cell을 이용하여 내용을 쓴다. 
 *
 * @param buf 쓸 문자열이 저장된 버퍼 
 * @param len 쓸 문자열 길이
 */

void fb_write(char *buf, unsigned int len)
{
	unsigned int buf_point = 0 ; 
	unsigned int fb_point = 0 ; 
	
	fb_clear(); 

	while(buf[buf_point] != '\0' && buf_point < len)
	{
		fb_write_cell(fb_point, buf[buf_point], FB_BLACK, FB_LIGHTGREY);
		fb_point = fb_point + 2;
		buf_point++; 
	}
    // 쓰고 난 뒤, 커서 한번에 이동. 
	fb_move_cursor(fb_point / 2 ) ; 	
}

/*
 * fb_clear : 
 * 화면을 공백 문자열로 초기화 한다.
 */
void fb_clear()
{
	for (unsigned int fb_point = 0; fb_point < 80 * 25 * 2; fb_point += 2) 
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

void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND); 
    outb(FB_DATA_PORT, ((pos >>8) & 0x00FF)); 
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND); 
    outb(FB_DATA_PORT, pos & 0x00FF); 
}


