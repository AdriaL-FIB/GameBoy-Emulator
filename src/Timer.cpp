#include "Timer.h"
#include <cassert>
#include "Bus.h"

// https://gbdev.io/pandocs/Power_Up_Sequence.html
Timer::Timer() :
	system_counter(0xAB << 8),
	tima(0x00),
	tma(0x00),
	tac(0xF8)
{

}


// https://gbdev.io/pandocs/Timer_and_Divider_Registers.html
// https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html

void Timer::set_system_counter(u16 value)
{
	u8 bit_to_check = falling_edge_bit[tac & 0x3];

	bool old_tick = CHECK_BIT(system_counter, bit_to_check);
	bool old_div_apu_bit = CHECK_BIT(system_counter, 12);

	system_counter = value;

	bool new_tick = CHECK_BIT(system_counter, bit_to_check);
	bool new_div_apu_bit = CHECK_BIT(system_counter, 12);

	bool clock_enabled = CHECK_BIT(tac, 2);

	if (clock_enabled and old_tick and not new_tick)
		timer_tick();

	if (old_div_apu_bit and not new_div_apu_bit)
	{
		// DIV-APU event
	}
}


void Timer::set_tac(u8 value)
{
	bool old_selected_bit = CHECK_BIT(system_counter, falling_edge_bit[tac & 0x3]);
	bool new_selected_bit = CHECK_BIT(system_counter, falling_edge_bit[value & 0x3]);

	bool old_clock_enabled = CHECK_BIT(tac, 2);
	bool new_clock_enabled = CHECK_BIT(value, 2);

	tac = value & 0x7 | 0xF8;

	// DMG
	if ((old_selected_bit and old_clock_enabled) and not (new_selected_bit and new_clock_enabled))
		timer_tick();
}

void Timer::timer_tick()
{
	if (tima == 0xFF)
	{
		tima_reset = true;
	}

	tima++;
}

u8 Timer::read(u16 addr)
{
	switch (addr)
	{
	case 0xFF04:
		return system_counter >> 8;
	case 0xFF05:
		return tima;
	case 0xFF06:
		return tma;
	case 0xFF07:
		return tac;
	default:
		return 0xFF;
	}
}

void Timer::write(u16 addr, u8 data)
{
	switch (addr)
	{
	case 0xFF04:
		set_system_counter(0);
		break;
	case 0xFF05:
		if (tima_reset) tima_reset = false; // Cancel reset
		tima = data;
		break;
	case 0xFF06:
		tma = data;
		break;
	case 0xFF07:
		set_tac(data);
		break;
	}
}

void Timer::tick(u8 tcycles)
{
	assert(tcycles % 4 == 0);

	int mcycles = tcycles / 4;
	
	for (int k = 0; k < mcycles; ++k)
	{
		if (tima_reset)
		{
			tima = tma;
			u8 IF = (bus->read_IF());
			BIT_SET(IF, 2, true)
			bus->write_IF(IF);
			tima_reset = false;
		}

		set_system_counter(system_counter + 4);
	}
}