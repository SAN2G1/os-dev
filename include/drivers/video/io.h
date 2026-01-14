#ifndef DRIVERS_VIDEO_IO_H
#define DRIVERS_VIDEO_IO_H
/* outb:
 * 데이터를 주어진 I/O 포트로 전송한다. 
 * io.s에 정의되어 있음. 
 * 
 * @param port 데이터를 보낼 I/O 포트
 * @param data 포트로 보낼 데이터 
 */

void outb(unsigned short port, unsigned char data ); 


#endif 
