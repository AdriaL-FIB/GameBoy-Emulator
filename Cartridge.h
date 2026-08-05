#pragma once

#include "common.h"
#include <vector>
#include <string>
#include <fstream>

constexpr int ROM_BANK_SIZE = (16 * 1024);
constexpr int RAM_BANK_SIZE = (8 * 1024);

class Cartridge
{
private:
	std::vector<u8> rom;
	std::vector<u8> ram;

	// ((address - external_ram_start_address) + (active_ram_bank * ram_bank_size)) % max_external_ram_size

	u16 selected_rom_bank = 1;
	u8 selected_ram_bank = 0;

	u8 cartridge_type;
	//u8 rom_size;
	//u8 ram_size;

	//MBC1
	bool is_ram_enabled = false;

public:
	bool load(const std::string& path);

	u8 read(u16 addr) const;
	void write(u16 addr, u8 data);
};

