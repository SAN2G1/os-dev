#include "arch/i386/io.h" 
#include "debug.h"
#include "klib/kstring.h"
#include "klib/types.h"

/* DEFAULT LOG LEVEL */
static log_level serial_log_level = INFO; 

/* I/O 포트 */

/* 모든 시리얼 포트(COM1, COM2, COM3, COM4)는
 * 동일한 포트 순서를 가지며, 시작 주소만 다르다.
 */

#define SERIAL_COM1_BASE                0x3F8      /* COM1 기준 포트 */

#define SERIAL_DATA_PORT(base)             (base)
#define SERIAL_INTERRUPT_ENABLE_PORT(base) (base + 1) 
#define SERIAL_FIFO_COMMAND_PORT(base)     (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)     (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base)    (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)      (base + 5)

/* I/O 포트 명령 */

/* SERIAL_LINE_ENABLE_DLAB:
 * 데이터 포트로 상위 8비트를 먼저 받고,
 * 이후 하위 8비트를 받을 것임을 시리얼 포트에 알림
 */
#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
 *  데이터 전송 속도를 설정한다.
 *  시리얼 포트의 기본 속도는 115200 bits/s이며,
 *  divisor 값으로 나눈 값이 실제 전송 속도가 된다.
 *
 *  @param com      설정할 COM 포트
 *  @param divisor  디바이저 값
 */
void serial_configure_baud_rate(uint16_t com, uint16_t divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com),
         SERIAL_LINE_ENABLE_DLAB);
    //MSB -> PORT +1 
    outb(SERIAL_DATA_PORT(com)+1,
         (divisor >> 8) & 0x00FF);
    //LSB -> PORT + 0
    outb(SERIAL_DATA_PORT(com),
         divisor & 0x00FF);
}


/* serial_configure_line: 
 * 지정된 시리얼 포트의 라인을 설정한다. 
 * 데이터 길이 8비트, 패리티 없음,
 * 스톱 비트 1개, 브레이크 제어 비활성화
 *
 *
 * @param com 설정할 시리얼 포트
 */

void serial_configure_line(uint16_t com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}


/* serial_is_transmit_fifo_empty: 
 * 주어진 com 포트의 송신 FIFO가 비어 있는지 확인한다. 
 *
 * @param com COM 포트 
 * @return 0 FIFO가 비어 있지 않음
 *         1 FIFO가 비어 있음
 */

int serial_is_transmit_fifo_empty(uint32_t com)
{
    /* 0x20 = 0010 0000 */
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20; 
}


/* serial_init : 
 * serial port로 통신 전 초기화 작업을 진행한다. 
 * 설정 데이터를 합의(baud rate, parity bit, stop bits, 등)
 *
 */

void serial_init()
{
    unsigned short com = SERIAL_COM1_BASE; 

    //1) 인터럽트 끄기
    outb(SERIAL_INTERRUPT_ENABLE_PORT(com), 0x00); 
    //2) baud rate 세팅 (divisor 3 ) 
    serial_configure_baud_rate(com, 0x03);
    //3) 8N1 
    serial_configure_line(com); 
    //4) Enable FIFO, clear RX/TX, 14-byte threshold
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7); 
    //5) Modem control: RTS/DTR on 
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03); 
}

void serial_wait_tx_ready(uint32_t com)
{
    while(serial_is_transmit_fifo_empty(com) == 0)
    {
        //busy wait 
    }
}


/* serial_putc
 * 문자를 serial port를 이용하여 보내는 함수
 *
 * @param c 보낼 문자
 */

void serial_putc(char c)
{
    uint32_t com = SERIAL_COM1_BASE;
    
    if (c == '\n')
    {
        serial_wait_tx_ready(com);
        outb(SERIAL_DATA_PORT(com) , '\r'); 
        serial_wait_tx_ready(com);
        outb(SERIAL_DATA_PORT(com) , '\n'); 
        return;
    }

    else
    {
        serial_wait_tx_ready(com);
        outb(SERIAL_DATA_PORT(com), c);
    }

}


/* serial_write
 * 문자열을 FIFO 큐에 쓰는 함수
 *
 * @param buf 쓰고자하는 문자열이 저장된 주소
 * @param len 쓰고자하는 문자열의 길이
 */
void serial_write(const char *buf, uint32_t len)
{
    for(uint32_t i = 0 ; i < len; i ++ ) 
    {
        serial_putc(buf[i]); 
    }

}

/* serial_set_log_level
 * 시리얼 로그의 로그 레벨을 설정하는 함수
 *
 * @param lvl 설정할 로그 레벨
 */
void serial_set_log_level(log_level lvl)
{
    if ((unsigned)lvl >= LOG_LEVEL_COUNT) lvl = ERROR ; 
    serial_log_level = lvl; 
}

/* serial_log
 * 디버깅용 로그를 남기는 함수 
 * 
 * @param level 로그 레벨
 * @param tag 로그 태그
 * @param text 로그 메시지
 */
void serial_log(log_level level, const char* tag, const char* text)
{   
    // 범위 체크
    if ((unsigned)level >= LOG_LEVEL_COUNT) level = ERROR ;
    if (tag == NULL) tag = ""; 
    if (text == NULL) text = ""; 

    // error level filter   
    if (level != ERROR && level < serial_log_level) return; 

    serial_write(log_level_str[level], kstrlen(log_level_str[level])); 
    serial_putc(' '); 
    serial_write(tag, kstrlen(tag)); 
    serial_putc(' '); 
    serial_putc(':'); 
    serial_putc(' '); 
    serial_write(text, kstrlen(text)); 
    serial_putc('\n'); 
}


