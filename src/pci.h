#pragma once
#include "common.h"

#define PCI_ADDRESS(bus, device, function, offset) ((1 << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xfc))

#define PCI_DATA_PORT 0xcfc
#define PCI_COMMAND_PORT 0xcf8

struct pci_device {
	
	uint8_t bus;
	uint8_t device;
	uint8_t function;
	
	uint16_t vendor_id;
	uint16_t device_id;
	
	uint8_t class;
	uint8_t subclass;
	uint8_t interface;
	uint8_t revision;
	
};

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

uint8_t pci_device_is_multifunction(uint8_t bus, uint8_t device);
struct pci_device pci_get_device(uint8_t bus, uint8_t device, uint8_t function);
