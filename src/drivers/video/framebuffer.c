#include <drivers/video/fb_colors.h>
#include <drivers/video/framebuffer.h>

#define FB_START 0xb8000

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

}

