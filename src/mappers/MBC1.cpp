#include "MBC1.h"

MBC1::MBC1(const std::vector<u8>& rom, std::vector<u8>& ram) : Mapper(rom, ram)
{
	num_rom_banks = rom.size() / (16 * 1024);
	select_rom_bank_mask = num_rom_banks - 1;

	num_ram_banks = ram.size() / (8 * 1024);

	uses_ram_banks = num_rom_banks <= 32 and num_ram_banks > 1;
	uses_secondary_reg = num_rom_banks > 32 or num_ram_banks > 1;
}

// https://gbdev.io/pandocs/MBC1.html

u8 MBC1::read(u16 addr) const
{
	if (addr <= 0x3FFF)
	{
		if (mode == 0)
			return rom[addr];

		int rom_addr = addr | int(selected_rom_bank_high) << 19; // hh * 32 * 16*1024 = hh*2^(5+4+10) = hh*2^19
		if (rom_addr < rom.size())
			return rom[rom_addr];
	}

	if (addr <= 0x7FFF)
	{
		int rom_addr = (addr - 0x4000) | int(selected_rom_bank_low) << 14 | int(selected_rom_bank_high) << 19;
		if (rom_addr < rom.size())
			return rom[rom_addr];
	}

	if (is_ram_enabled and not ram.empty() and BETWEEN(addr, 0xA000, 0xBFFF))
	{
		if (mode == 0)
			return ram[addr - 0xA000];

		int ram_addr = int(selected_ram_bank) << 13 | (addr - 0xA000);
		if (ram_addr < ram.size())
			return ram[ram_addr];
	}

	return 0xFF;
}

void MBC1::write(u16 addr, u8 data)
{
	if (addr <= 0x1FFF) // RAM Enable
	{
		is_ram_enabled = ((data & 0xF) == 0xA);
		return;
	}

	if (addr <= 0x3FFF) // ROM bank number
	{
		data &= 0x1F; // Only 5 lower bits
		if (data == 0x00)
		{
			selected_rom_bank_low = 1;
			return;
		}

		selected_rom_bank_low = data & select_rom_bank_mask;
		return;
	}

	if (addr <= 0x5FFF) // RAM bank or Upper bits of ROM bank number
	{
		if (not uses_secondary_reg)
			return;

		data &= 0x03; // Only 2 lower bits
		if (uses_ram_banks)
			selected_ram_bank = data;
		else
			selected_rom_bank_high = data & (select_rom_bank_mask >> 5);
		return;
	}

	if (addr <= 0x7FFF)
	{
		mode = data & 0x01;
		return;
	}

	if (is_ram_enabled and not ram.empty() and BETWEEN(addr, 0xA000, 0xBFFF))
	{
		if (mode == 0)
			ram[addr - 0xA000] = data;
		else
		{
			int ram_addr = int(selected_ram_bank) << 13 | (addr - 0xA000);
			if (ram_addr < ram.size())
				ram[ram_addr] = data;
		}
		return;
	}
}