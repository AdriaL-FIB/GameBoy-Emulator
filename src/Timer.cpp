#include "Timer.h"

// https://gbdev.io/pandocs/Power_Up_Sequence.html
Timer::Timer() :
	divider_reg(0xAB),
	tima(0x00),
	tma(0x00),
	tac(0xF8)
{

}

u8 Timer::read(u16 addr)
{
	switch (addr)
	{
	case 0xFF04:
		return divider_reg;
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

}

void Timer::tick(u8 tcycles)
{
}
