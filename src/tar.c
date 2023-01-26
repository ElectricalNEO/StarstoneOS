#include "tar.h"
#include "string.h"

uint64_t tar_get_size(struct tar_header* header) {
    
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;
    
    for (j = 11; j > 0; j--, count *= 8)
        size += ((header->size[j - 1] - '0') * count);
    
    return size;
    
}

void* tar_open_file(void* tar, char* path) {
    
    while(1) {
        
        if(!*(char*)tar) return 0;
        
        if(strcmp((char*)tar, path)) {
            
            tar += 512 + ((tar_get_size((struct tar_header*)tar) + 511) / 512) * 512;
            
        } else return tar + 512;
        
    }
    
}

uint64_t tar_get_file_size(void* tar, char* path) {
	
	while(1) {
        
        if(!*(char*)tar) return 0;
        
        if(strcmp((char*)tar, path)) {
            
            tar += 512 + ((tar_get_size((struct tar_header*)tar) + 511) / 512) * 512;
            
        } else return tar_get_size((struct tar_header*)tar);
        
    }
	
}
