#pragma once

#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"
#include "Timer.h"
#include "APU.h"


#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

class GameBoy
{
private:
	CPU cpu;
	Bus bus;
	PPU ppu;
	Cartridge cartridge;
	Timer timer;
	Joypad joypad;
	APU apu;

	bool cartridge_loaded;

	u32 freq;

public:
	GameBoy();

	bool load_rom(const std::string& path);
	bool game_loaded() const { return cartridge_loaded; }

	void game_loop();

	unsigned int tick();

	const std::array<u8, WINDOW_WIDTH * WINDOW_HEIGHT>& get_framebuffer() const { return ppu.get_framebuffer(); }

	const StereoSample get_audio() const { return apu.get_audio(); }

	u32 get_freq() const { return freq; }

	void button_down(Buttons btn);
	void button_up(Buttons btn);
};

