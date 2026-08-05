#include "Cartridge.h"

bool Cartridge::load(const std::string& path)
{
	std::ifstream gb(path, std::ios::binary | std::ios::ate);
	if (not gb.is_open()) return false;

	size_t size = gb.tellg();

	rom.resize(size);

	gb.seekg(0, std::ios::beg);
	gb.read(reinterpret_cast<char*>(rom.data()), size);
	gb.close();

	// Check header
	cartridge_type = read(0x0147);
	u8 rom_size_byte = read(0x0148);
	u8 ram_size_byte = read(0x0149);

	int ram_size = 0;
	switch (ram_size_byte)
	{
	case 0x00:
	case 0x01:
	default:
		ram_size = 0;
		break;
	case 0x02:
		ram_size = 8 * 1024;
		break;
	case 0x03:
		ram_size = 4 * 8 * 1024;
		break;
	case 0x04:
		ram_size = 16 * 8 * 1024;
		break;
	case 0x05:
		ram_size = 8 * 8 * 1024;
		break;
	}

	ram.resize(ram_size);

	return true;
}

u8 Cartridge::read(u16 addr) const
{
	if (addr <= 0x3FFF)
	{
		return rom[addr];
	}

	if (addr <= 0x7FFF)
	{
		return rom[selected_rom_bank * ROM_BANK_SIZE + (addr - 0x4000)];
	}

	if (BETWEEN(addr, 0xA000, 0xBFFF))
	{
		return ram[selected_ram_bank * RAM_BANK_SIZE + (addr - 0xA000)];
	}

	return 0xFF;
}

void Cartridge::write(u16 addr, u8 data)
{
	if (addr <= 0x1FFF) // RAM Enable
	{
		is_ram_enabled = (data == 0xA);
		return;
	}

	if (addr <= 0x3FFF)
	{

		return;
	}

	if (BETWEEN(addr, 0xA000, 0xBFFF))
	{
		ram[selected_ram_bank * RAM_BANK_SIZE + (addr - 0xA000)] = data;
		return;
	}
}