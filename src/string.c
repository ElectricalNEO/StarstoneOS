#include "string.h"

int strcmp(char* str1, char* str2) {
    
    while(1) {
        
        if(*str1 > *str2) return 1;
        if(*str1 < *str2) return -1;
        if(!*str1) return 0;
        str1++;
        str2++;
        
    }
    
}

char* itoa(uint64_t value, char* str, int base) {
    
    if(!value) {
        
        str[0] = '0';
        str[1] = 0;
        return str;
        
    }
    
    uint64_t v = value;
    uint8_t numbers = 0;
    while(v) {
        numbers++;
        v /= base;
    }
    
    str[numbers] = 0;
    
    uint8_t n;
    for(uint8_t i = numbers; i > 0; i--) {
        n = value % base;
        str[i - 1] = n < 0xa ? n + '0' : n + 'a' - 0xa;
        value /= base;
    }
    
    return str;
    
}
