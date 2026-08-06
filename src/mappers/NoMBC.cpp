#include "NoMBC.h"

u8 NoMBC::read(u16 addr) const
{
	if (addr <= 0x7FFF)
	{
		return rom[addr];
	}

	if (BETWEEN(addr, 0xA000, 0xBFFF) and not ram.empty())
	{
		return ram[addr - 0xA000];
	}

	return 0xFF;
}

void NoMBC::write(u16 addr, u8 data)
{
	if (BETWEEN(addr, 0xA000, 0xBFFF) and not ram.empty())
	{
		ram[addr - 0xA000] = data;
	}
}
