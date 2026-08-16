#include "GameBoy.h"
#include <cassert>

GameBoy::GameBoy() :
	bus(&cpu, &cartridge, &ppu, &timer, &joypad),
	cartridge_loaded(false),
	freq(4194304)
{
	cpu.connect_bus(&bus);
	timer.connect_bus(&bus);
	ppu.connect_bus(&bus);
}

bool GameBoy::load_rom(const std::string& path)
{
	cartridge_loaded = cartridge.load(path);
	return cartridge_loaded;
}

void GameBoy::game_loop()
{
	cartridge_loaded = true;
	u64 total_cycles = 0;
	u64 cycle_limit = 999999999;

	while (cartridge_loaded and total_cycles <= cycle_limit)
	{
		u8 cycles = cpu.step_instruction();
		total_cycles += cycles;

		timer.tick(cycles);
		ppu.tick(cycles);
	}
}

unsigned int GameBoy::tick()
{
	assert(cartridge_loaded);
	//if (not cartridge_loaded) return 4000000;

	u8 cycles = cpu.step_instruction();
	timer.tick(cycles);
	ppu.tick(cycles);

	return cycles;
}

void GameBoy::button_down(Buttons btn)
{
	joypad.button_down(btn);
}

void GameBoy::button_up(Buttons btn)
{
	joypad.button_up(btn);
}
