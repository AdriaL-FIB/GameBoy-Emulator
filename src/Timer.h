#pragma once

#include "common.h"
#include <array>

constexpr std::array<int, 4> falling_edge_bit{ {9, 3, 5, 7} };

class Bus;

class Timer
{
public:
	Timer();

	void connect_bus(Bus* b) { bus = b; }

	u8 read(u16 addr);
	void write(u16 addr, u8 data);

	void tick(u8 tcycles);

private:
	Bus* bus;

	//u8 divider_reg;		// FF04 - DIV: Divider register
	u8 tima;			// FF05 - TIMA: Timer counter
	u8 tma;				// FF06 - TMA: Timer modulo
	u8 tac;				// FF07 - TAC: Timer control

	u16 system_counter;

	bool tima_reset = false;

private:
	void set_system_counter(u16 value);
	void set_tac(u8 value);
	void timer_tick();
};

