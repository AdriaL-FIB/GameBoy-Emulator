#pragma once

#include "common.h"
#include <vector>
#include <ctime>

constexpr int ROM_BANK_SIZE = (16 * 1024);
constexpr int RAM_BANK_SIZE = (8 * 1024);

class Mapper
{
protected:
	const std::vector<u8>& rom;
	std::vector<u8>& ram;
public:
	Mapper(const std::vector<u8>& rom, std::vector<u8>& ram) :
		rom(rom),
		ram(ram)
	{ 
	}

	virtual ~Mapper() {}

	virtual std::vector<u8> get_extra_save_bytes() { return {}; }
	virtual void load_extra_save_bytes(const std::vector<u8>& data) {}

	virtual u8 read(u16 addr) const = 0;
	virtual void write(u16 addr, u8 data) = 0;
};
