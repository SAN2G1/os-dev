global loader ; ELF 에서의 entry symbool
extern kmain ; c 파일에서 정의되어 있음.

;multiboot 헤더용 상수 정의
MAGIC_NUMBER equ 0x1BADB002
FLAGS 	     equ 0x0
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)
; magic_number + checksum + flags의 합은 0이여야 함.

; text(code) 섹션의 시작
section .text   
align 4
	dd MAGIC_NUMBER
	dd FLAGS 
	dd CHECKSUM
; loader label (linker의 스크립트에서 entry point로 정의됨)
loader: 
	;C 함수는 무조건 스택을 사용함. (함수 호출, 지역 변수, 반환 주소 등)
	;x86 스택은 아래 방향으로 성장한다. (처음엔 가장 높은 주소 -> PUSH 할 수록 주소가 감소함)
	mov esp, stack_top ; stack pointer 세팅
	; 이 부분에서 메인 C 함수를 호출한다. 
	call kmain

.hang :
	cli 
	hlt
	jmp .hang

section .bss 
align 16
stack_bottom: 
	resb 4096 ; 4KB for stack
stack_top: 

