#pragma once

#include "common.h"

class Timer
{
public:
	Timer();

	u8 read(u16 addr);
	void write(u16 addr, u8 data);

	void tick(u8 tcycles);

private:
	u8 divider_reg;		// FF04 - DIV: Divider register
	u8 tima;			// FF05 - TIMA: Timer counter
	u8 tma;				// FF06 - TMA: Timer modulo
	u8 tac;				// FF07 - TAC: Timer control
};

