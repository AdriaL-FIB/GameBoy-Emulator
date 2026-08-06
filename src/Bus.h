#pragma once
#include <array>
#include <cstdint>
#include "CPU.h"
#include "Cartridge.h"
#include "PPU.h"
#include "Timer.h"

class Bus
{
public:
	Bus(CPU* cpu, Cartridge* cart, PPU* ppu, Timer* timer);
	~Bus();

private: // Devices on Bus
	CPU* cpu;
	Cartridge* cartridge;
	PPU* ppu;
	Timer* timer;

	// Work RAM 8 KiB
	std::array<u8, 8 * 1024> wram;

	// High RAM 127 Bytes
	std::array<u8, 127> hram;

	// Interrupts
	u8 IE; // FFFF
	u8 IF; // FF0F

public: // Bus read & write
	void write(u16 addr, u8 data);
	u8 read(u16 addr);
	u8 handle_IO_read(u16 addr);
	void handle_IO_write(u16 addr, u8 data);
};

