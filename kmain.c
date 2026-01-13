#if defined(__linux__) 
#error "Do not compile with a Linux-targeting compiler" 
#endif

#if !defined(__i386__) 
#error "This kernel must be compiled for 32-bit x86" 
#endif 
/*
 * kernel.c
 */

void kmain(void)
{
	const char *str = "my first kernel"; 
	char *vidptr = (char*)0xb8000; // video mem의 시작 지점. 
	unsigned int i = 0 ;
	unsigned int j = 0; 
	/* 화면 초기화 루프 
	 * 25행 80열 ; 각각 2B의 크기를 가진다 */
	while(j < 80 * 25 * 2)
	{
		vidptr[j] = ' '; //빈 문자열
		vidptr[j+1] = 0x07 ; // light grey on black screen
		j = j + 2; 
	}

	j = 0 ; 

	/*문자열을 video memory에 적는 루프*/
	while(str[i] != '\0') 
	{
		vidptr[j] = str[i]; 
		vidptr[j+1] = 0x07; 
		i++; 
		j = j+2; 
	}
	return; 
}
