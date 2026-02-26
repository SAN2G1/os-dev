#ifndef KLIB_TYPES_H
#define KLIB_TYPES_H

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef unsigned char uint8_t; 
typedef unsigned short uint16_t; 
typedef unsigned int uint32_t; 
typedef unsigned long uint64_t; 

//포인터를 안전하게 담을 수 있는 unsigned 정수형 , 아키텍처마다 달라질 수 있음. 
typedef unsigned int uintptr_t;


// bool , true, false를 정의 

typedef unsigned char bool ; 

#define true 1
#define false 0

#endif