#pragma once
#include "Mapper.h"
class MBC1 : public Mapper
{
	using Mapper::Mapper;
private:
	u8 selected_rom_bank_low = 1;	// lllll
	u8 selected_rom_bank_high = 0;	// hh
	u8 mode = 0;

	u8 selected_ram_bank = 0;
	u8 select_rom_bank_mask;
	bool is_ram_enabled = false;

	bool uses_ram_banks = true;
	bool uses_secondary_reg = true;

	u8 num_rom_banks;
	u8 num_ram_banks;
public:
	MBC1(const std::vector<u8>& rom, std::vector<u8>& ram);
	~MBC1() {}

	u8 read(u16 addr) const override;
	void write(u16 addr, u8 data) override;
};

