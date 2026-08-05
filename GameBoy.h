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

public:
	GameBoy();

	void load_rom(const std::string& path);

	void game_loop();
};

