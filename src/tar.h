#pragma once
#include "common.h"

struct tar_header {
    
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    
}__attribute__((packed));

void* tar_open_file(void* tar, char* path);
uint64_t tar_get_file_size(void* tar, char* path);
