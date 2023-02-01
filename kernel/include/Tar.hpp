#pragma once
#include <common.hpp>

class Tar {
	
	class TarHeader {
		
		char filename[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char checksum[8];
		char typeflag;
		
	public:
		uint64_t getSize();
		
	}__attribute__((packed));
	
	void* start;
	
public:
	Tar(void* address);
	void* findFile(char* path);
	uint64_t getFileSize(char* path);
	
};
