#include "arch/i386/io.h"
#include "drivers/serial/serial.h"
#include "drivers/video/framebuffer.h"
#include "klib/types.h"
// PC keyboard interface constants

#define KBSTATP         0x64    // kbd controller status port(I)
#define KBS_DIB         0x01    // kbd data in buffer
#define KBDATAP         0x60    // kbd data port(I)

#define NO              0

#define SHIFT           (1<<0)
#define CTL             (1<<1)
#define ALT             (1<<2)

#define CAPSLOCK        (1<<3)
#define NUMLOCK         (1<<4)
#define SCROLLLOCK      (1<<5)

#define E0ESC           (1<<6)

// Special keycodes
#define KEY_HOME        0xE0
#define KEY_END         0xE1
#define KEY_UP          0xE2
#define KEY_DN          0xE3
#define KEY_LF          0xE4
#define KEY_RT          0xE5
#define KEY_PGUP        0xE6
#define KEY_PGDN        0xE7
#define KEY_INS         0xE8
#define KEY_DEL         0xE9

// C('A') == Control-A
#define C(x) (x - '@')

static const uint8_t shiftcode[256] =
{
  [0x1D] CTL,
  [0x2A] SHIFT,
  [0x36] SHIFT,
  [0x38] ALT,
  [0x9D] CTL,
  [0xB8] ALT
};

static const uint8_t togglecode[256] =
{
  [0x3A] CAPSLOCK,
  [0x45] NUMLOCK,
  [0x46] SCROLLLOCK
};

static const uint8_t normalmap[256] =
{
  NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static const uint8_t shiftmap[256] =
{
  NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
  'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
  '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static const uint8_t ctlmap[256] =
{
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
  C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
  C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
  NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
  C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
  [0x9C] '\r',      // KP_Enter
  [0xB5] C('/'),    // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

// 키보드 드라이버 헤더 파일에서 선언된 함수들의 구현

/*
   키보드 드라이버가 사용할 내부 상태를 초기화하는 함수. 
   키보드 상태가 커널 부팅 시점에서 깨끗하게 시작할지... 아니면 뭐 어떻게 할지
   입력 버퍼 초기화 등등의 작업을 추가할 수 있다. 
*/
void keyboard_init(void);

static uint8_t shift_state = 0; // Shift, Ctrl, Alt 상태를 추적하는 변수

static const uint8_t * keymap[4] = {
  normalmap, shiftmap, ctlmap, ctlmap
};

int keyboard_getchar(void){
    if ((inb(KBSTATP) & KBS_DIB) == 0) {
        return 0; // No data available
    }

    uint8_t scancode = inb(KBDATAP);

    //E0 확장 scancode 처리
    if (scancode == 0xE0) {
        shift_state |= E0ESC;
        return 0;
    }

    if (scancode & 0x80) {
        // Key release event 
        if ((shift_state & E0ESC) == 0) {
            scancode &= 0x7F;
        }
        shift_state &= ~(shiftcode[scancode & 0x7F]); // Update shift state on key release
        shift_state &= ~E0ESC;
        return 0; 
    }
    else {
        // 이전에 E0 확장 scancode가 들어왔는지 확인하고, 그렇다면 scancode에 0x80을 더하여 구분
        if (shift_state & E0ESC) {
            scancode |= 0x80;
            shift_state &= ~E0ESC;
        }

        shift_state |= shiftcode[scancode];
        shift_state ^= togglecode[scancode];

        int c = keymap[shift_state & (CTL | SHIFT)][scancode];

        if (shift_state & CAPSLOCK) {
            if ('a' <= c && c <= 'z') {
                c += 'A' - 'a';
            } else if ('A' <= c && c <= 'Z') {
                c += 'a' - 'A';
            }
        }
        return c; // Return the corresponding character
    }
}
void keyboard_handle_irq(void){ 

    int c = keyboard_getchar();
    if (c == 0 ) return ;
    
    fb_putkbd(c); // 키보드 입력을 화면에 출력
}
