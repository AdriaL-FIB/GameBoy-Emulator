#pragma once

#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"
#include "Timer.h"

class GameBoy
{
private:
	CPU cpu;
	Bus bus;
	PPU ppu;
	Cartridge cartridge;
	Timer timer;

	bool running;

	u32 freq;

public:
	GameBoy();

	void load_rom(const std::string& path);

	void game_loop();

	unsigned int tick();

	const std::array<u8, 144 * 160>& get_framebuffer() const { return ppu.get_framebuffer(); }

	u32 get_freq() { return freq; }
};

