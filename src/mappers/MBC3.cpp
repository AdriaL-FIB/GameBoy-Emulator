#include "MBC3.h"
#include <cassert>

MBC3::MBC3(const std::vector<u8>& rom, std::vector<u8>& ram, bool timer) :
	Mapper(rom, ram),
	timer(timer),
	time_RTC_updated(std::time(nullptr))
{
}


void MBC3::update_rtc()
{
	if (not timer) return;

	long long now = std::time(nullptr);

	u32 elapsed_s = now - time_RTC_updated;
	time_RTC_updated = now;

	if (CHECK_BIT(RTC_dh, 6))
		return;

	u32 total_s = RTC_s + elapsed_s;
	RTC_s = total_s % 60;

	u32 total_m = RTC_m + (total_s / 60);
	RTC_m = total_m % 60;

	u32 total_h = RTC_h + (total_m / 60);
	RTC_h = total_h % 24;

	u32 elapsed_d = (total_h / 24);


	if (RTC_dl + elapsed_d >= 256)
	{
		if (RTC_dh & 0x01)
		{
			BIT_SET(RTC_dh, 7, true);
			BIT_SET(RTC_dh, 0, false);
		}
		else
		{
			BIT_SET(RTC_dh, 0, true);
		}
	}
	RTC_dl += elapsed_d;
}

std::vector<u8> MBC3::get_extra_save_bytes()
{
	if (not timer) return {};

	std::vector<u8> res = { RTC_s, RTC_m, RTC_h, RTC_dl, RTC_dh };
	
	static_assert(sizeof(time_RTC_updated) == 8, "time_RTC_updated must be 64 bits");
	for (int i = 0; i < 8; ++i)
		res.push_back(static_cast<u8>(time_RTC_updated >> (i * 8)));

	return res;
}

void MBC3::load_extra_save_bytes(const std::vector<u8>& data)
{
	if (not timer) return;

	static_assert(sizeof(time_RTC_updated) == 8, "time_RTC_updated must be 64 bits");

	assert(data.size() == 13);
	RTC_s = data[0];
	RTC_m = data[1];
	RTC_h = data[2];
	RTC_dl = data[3];
	RTC_dh = data[4];

	uint64_t timestamp = 0;
	for (int i = 0; i < 8; ++i)
		timestamp |= static_cast<uint64_t>(data[5+i]) << (i * 8);
	time_RTC_updated = static_cast<long long>(timestamp);
}

u8 MBC3::read(u16 addr) const
{
	if (addr <= 0x3FFF) // 0000-3FFF - ROM Bank 00
	{
		return rom[addr];
	}
	if (addr <= 0x7FFF) // 4000-7FFF - ROM Bank 01-7F 
	{
		return rom[addr - 0x4000 + ROM_BANK_SIZE * selected_rom_bank];
	}
	
	if (BETWEEN(addr, 0xA000, 0xBFFF) and ram_and_timer_enabled)
	{
		if (selected_ram_bank <= 0x7 and not ram.empty())
			return ram[addr - 0xA000 + RAM_BANK_SIZE * selected_ram_bank];
		
		switch (selected_ram_bank)
		{
		case 0x08:
			return RTC_s;
		case 0x09:
			return RTC_m;
		case 0x0A:
			return RTC_h;
		case 0x0B:
			return RTC_dl;
		case 0x0C:
			return RTC_dh;
		}
	}

	return 0xFF;
}

void MBC3::write(u16 addr, u8 data)
{
	if (addr <= 0x1FFF) // 0000-1FFF - RAM and Timer Enable
	{
		ram_and_timer_enabled = data == 0x0A;
	}
	else if (addr <= 0x3FFF) // 2000-3FFF - ROM Bank Number
	{
		selected_rom_bank = data & 0x7F;
		if (selected_rom_bank == 0)
			selected_rom_bank = 1;
	}
	else if (addr <= 0x5FFF) // 4000-5FFF - RAM Bank Number - or - RTC Register Select
	{
		selected_ram_bank = data;
	}
	else if (addr <= 0x7FFF) // 6000-7FFF - Latch Clock Data
	{
		if (last_latch == 0 and data == 1)
		{
			update_rtc();
		}
		last_latch = data;
	}
	else if (BETWEEN(addr, 0xA000, 0xBFFF) and ram_and_timer_enabled)
	{
		if (selected_ram_bank <= 0x7 and not ram.empty())
		{
			ram[addr - 0xA000 + RAM_BANK_SIZE * selected_ram_bank] = data;
			return;
		}

		switch (selected_ram_bank)
		{
		case 0x08:
			RTC_s = data;
			break;
		case 0x09:
			RTC_m = data;
			break;
		case 0x0A:
			RTC_h = data;
			break;
		case 0x0B:
			RTC_dl = data;
			break;
		case 0x0C:
			RTC_dh = data;
			break;
		}
	}
}
