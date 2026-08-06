#pragma once

#include "common.h"

class PPU
{
public:
	u8 read(u16 addr);
	void write(u16 addr, u8 data);
};

