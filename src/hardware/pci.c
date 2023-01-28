#include "pci.h"
#include "port.h"

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
	
	outd(PCI_COMMAND_PORT, PCI_ADDRESS(bus, device, function, offset));
	return ind(PCI_DATA_PORT);
		
}

void pci_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
	
	outd(PCI_COMMAND_PORT, PCI_ADDRESS(bus, device, function, offset));
	outd(PCI_DATA_PORT, value);
	
}

uint8_t pci_device_is_multifunction(uint8_t bus, uint8_t device) {
	
	return (pci_read(bus, device, 0, 0xc) & (1 << 23)) ? 1 : 0;
	
}

struct pci_device pci_get_device(uint8_t bus, uint8_t device, uint8_t function) {
	
	struct pci_device ret;
	
	ret.bus = bus;
	ret.device = device;
	ret.function = function;
	
	uint32_t data = pci_read(bus, device, function, 0);
	
	ret.vendor_id = data & 0xffff;
	ret.device_id = data >> 16;
	
	data = pci_read(bus, device, function, 8);
	
	ret.class = (data & 0xff000000) >> 24;
	ret.subclass = (data & 0xff0000) >> 16;
	ret.interface = (data & 0xff00) >> 8;
	ret.revision = data & 0xff;
	
	return ret;
	
}
