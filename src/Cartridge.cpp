#include "Cartridge.h"
#include <fstream>
#include <cassert>
#include <iostream>
#include <filesystem>

#include "NoMBC.h"
#include "MBC1.h"
#include "MBC3.h"

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
	case 0x0F:
	case 0x10:
		return new MBC3(rom, ram, true);
	case 0x11:
	case 0x12:
	case 0x13:
		return new MBC3(rom, ram, false);
	default:
		return nullptr;
	}
}

bool Cartridge::has_battery(u8 cartridge_type)
{
	switch (cartridge_type)
	{
	case 0x03:
	case 0x06:
	case 0x09:
	case 0x0D:
	case 0x0F:
	case 0x10:
	case 0x13:
	case 0x1B:
	case 0x1E:
	case 0x22:
	case 0xFF:
		return true;
	default:
		return false;
	}
}

bool Cartridge::load(const std::string& path)
{
	std::ifstream gb(path, std::ios::binary | std::ios::ate);
	if (not gb.is_open()) return false;

	std::filesystem::path p(path);
	path_no_ext = p.replace_extension("").string();
	filename = p.filename().string();

	size_t size = gb.tellg();

	rom.resize(size);

	gb.seekg(0, std::ios::beg);
	gb.read(reinterpret_cast<char*>(rom.data()), size);
	gb.close();

	// Check header
	battery = false;
	cartridge_type =  rom.at(0x0147);
	u8 rom_size_byte = rom.at(0x0148);

	// https://gbdev.io/pandocs/The_Cartridge_Header.html#0148--rom-size
	size_t rom_size = 32 * 1024 * (1 << rom_size_byte);

	// check if the values agree
	if (rom_size != size)
	{
		return false;
	}

	u8 num_rom_banks = u8(rom_size >> 14);

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

	battery = has_battery(cartridge_type);

	if (battery)
	{
		load_save(path_no_ext + ".sav");
	}

	return true;
}

bool Cartridge::load_save(const std::string& path)
{
	assert(battery);
	if (not std::filesystem::exists(path)) return false;

	std::ifstream savefile(path, std::ios::binary | std::ios::ate);
	if (not savefile.is_open()) return false;

	size_t size = savefile.tellg();

	savefile.seekg(0, std::ios::beg);
	savefile.read(reinterpret_cast<char*>(ram.data()), ram.size());

	size_t remaining = size - ram.size();
	if (remaining > 0)
	{
		std::vector<u8> extra_data(remaining);
		savefile.read(reinterpret_cast<char*>(extra_data.data()), remaining);

		mapper->load_extra_save_bytes(extra_data);
	}

	savefile.close();

	std::cout << "Battery loaded" << std::endl;

	return true;
}

bool Cartridge::save_ram()
{
	if (not battery)
		return false;

	std::ofstream sf(path_no_ext + ".sav", std::ios::out | std::ios::trunc | std::ios::binary);
	if (not sf.is_open()) return false;

	sf.write(reinterpret_cast<const char*>(ram.data()), ram.size());

	std::vector<u8> extra_data = mapper->get_extra_save_bytes();
	sf.write(reinterpret_cast<const char*>(extra_data.data()), extra_data.size());

	sf.close();

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
