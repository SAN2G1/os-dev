global outb
global inb
; outb - I/O 포트로 1바이트를 전송
; 스택 구조 : 
; [esp + 8] 전송할 데이터
; [esp + 4] I/O 포트 번호
; [esp    ] 반환 주소

outb: 
    mov al, [esp + 8] ; 전송할 데이터를 al 레지스터에 저장
    mov dx, [esp + 4] ; 포트 주소를 dx 레지스터에 저장
    out dx, al        ; 포트로 데이터 전송
    ret               ; 호출한 함수로 복귀

; inb - 지정된 I/O 포트에서 1바이트를 읽어 반환
; stack: [esp+4] I/O 포트 주소
;        [esp  ] 반환 주소
inb:
    mov dx, [esp + 4]
    in al, dx
    movzx eax, al ; 반환 값 정리
    ret

