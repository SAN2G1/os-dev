#include "klib/kstring.h" 
#include "klib/types.h"
uint32_t kstrlen(const char* str)
{
    uint32_t len = 0; 
    while(str[len] != '\0')
    {
        len++; 
    }

    return len; 
}
