#include "GameBoy.h"

GameBoy::GameBoy() :
	bus(&cpu, &cartridge, &ppu, &timer, &joypad),
	running(false),
	freq(4194394)
{
	cpu.connect_bus(&bus);
	timer.connect_bus(&bus);
	ppu.connect_bus(&bus);
}

void GameBoy::load_rom(const std::string& path)
{
	cartridge.load(path);
}

void GameBoy::game_loop()
{
	running = true;
	u64 total_cycles = 0;
	u64 cycle_limit = 999999999;

	while (running and total_cycles <= cycle_limit)
	{
		u8 cycles = cpu.step_instruction();
		total_cycles += cycles;

		timer.tick(cycles);
		ppu.tick(cycles);
	}
}

unsigned int GameBoy::tick()
{
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
