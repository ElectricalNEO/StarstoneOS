#include <Tar.hpp>
#include <string.hpp>

uint64_t Tar::TarHeader::getSize() {
	
	uint64_t size = 0;
    unsigned int count = 1;
    
    for (uint8_t j = 11; j > 0; j--, count *= 8)
        size += ((this->size[j - 1] - '0') * count);
    
    return size;
	
}

Tar::Tar(void* address) {
	
	this->start = address;
	
}

void* Tar::findFile(char* path) {
	
	void* p = this->start;
	
	while(1) {
        
        if(!*(char*)p) return 0;
        
        if(strcmp((char*)p, path)) {
            
            p = (void*)((uint64_t)p + 512 + ((((TarHeader*)p)->getSize() + 511) / 512) * 512);
            
        } else return (void*)((uint64_t)p + 512);
        
    }
	
}

uint64_t Tar::getFileSize(char* path) {
	
	void* p = this->start;
	
	while(1) {
        
        if(!*(char*)p) return 0;
        
        if(strcmp((char*)p, path)) {
            
            p = (void*)((uint64_t)p + 512 + ((((TarHeader*)p)->getSize() + 511) / 512) * 512);
            
        } else return ((TarHeader*)p)->getSize();
        
    }
	
}
