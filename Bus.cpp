#include "Bus.h"

Bus::Bus() {
	for (uint8_t& i : ram) i = 0x00;

	cpu.connectBus(this);
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 and addr <= 0xFFFF)
		ram[addr] = data;
}

uint8_t Bus::read(uint16_t addr) {
	if (addr >= 0x0000 and addr <= 0xFFFF)
		return ram[addr];
	return 0x00;
}