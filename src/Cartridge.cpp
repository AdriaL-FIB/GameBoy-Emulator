#include "Cartridge.h"
#include <fstream>

#include "NoMBC.h"
#include "MBC1.h"

Mapper* Cartridge::create_mapper(u8 cartridge_type)
{
	switch (cartridge_type)
	{
	case 0x00:
		return new NoMBC(rom, ram);
	case 0x01:
	case 0x02:
	case 0x03:
		return new MBC1(rom, ram);
	default:
		return nullptr;
	}
}

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
	cartridge_type =  rom.at(0x0147);
	u8 rom_size_byte = rom.at(0x0148);

	// https://gbdev.io/pandocs/The_Cartridge_Header.html#0148--rom-size
	size_t rom_size = 32 * 1024 * (1 << rom_size_byte);

	// check if the values agree
	if (rom_size != size)
	{
		return false;
	}

	u8 num_rom_banks = rom_size >> 14;

	u8 ram_size_byte = rom.at(0x0149);

	int ram_size = 0;
	int ram_banks = 0;
	switch (ram_size_byte)
	{
	case 0x00:
	case 0x01:
	default:
		ram_banks = 0;
		break;
	case 0x02:
		ram_banks = 1;
		break;
	case 0x03:
		ram_banks = 4;
		break;
	case 0x04:
		ram_banks = 16;
		break;
	case 0x05:
		ram_banks = 8;
		break;
	}

	ram_size = ram_banks * 8 * 1024;

	ram.resize(ram_size);

	mapper = create_mapper(cartridge_type);

	if (mapper == nullptr)
	{
		return false;
	}


	return true;
}

u8 Cartridge::read(u16 addr) const
{
	return mapper->read(addr);
}

void Cartridge::write(u16 addr, u8 data)
{
	mapper->write(addr, data);
}