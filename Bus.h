#pragma once
#include <array>
#include <cstdint>
#include "CPU.h"

class Bus
{
public:
	Bus();
	~Bus();

public: // Devices on Bus
	CPU cpu;

	// Fake RAM
	std::array<uint8_t, 64 * 1024> ram;

public: // Bus read & write
	void write(uint16_t addr, uint8_t data);
	uint8_t read(uint16_t addr);
};

