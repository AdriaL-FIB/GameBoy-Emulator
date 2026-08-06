#pragma once

#include "common.h"
#include <vector>
#include <string>
#include "Mapper.h"

class Cartridge
{
private:
	std::vector<u8> rom;
	std::vector<u8> ram;

	Mapper* mapper;

	// ((address - external_ram_start_address) + (active_ram_bank * ram_bank_size)) % max_external_ram_size

	u8 cartridge_type;
	//u8 rom_size;
	//u8 ram_size;

	//int rom_size;

private:
	Mapper* create_mapper(u8 cartridge_type);

public:
	bool load(const std::string& path);

	u8 read(u16 addr) const;
	void write(u16 addr, u8 data);
};

