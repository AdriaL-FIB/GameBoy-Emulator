#pragma once
#include "common.h"
#include "Mapper.h"

class MBC3 : public Mapper
{
private:
	u8 selected_rom_bank = 0;
	u8 selected_ram_bank = 0;
	bool ram_and_timer_enabled = false;
	u8 last_latch = 0xFF;

	u8 RTC_s = 0;
	u8 RTC_m = 0;
	u8 RTC_h = 0;
	u8 RTC_dl = 0;
	u8 RTC_dh = 0;

	bool timer;

	long long time_RTC_updated;

private:
	void update_rtc();

public:
	MBC3(const std::vector<u8>& rom, std::vector<u8>& ram, bool timer);


	std::vector<u8> get_extra_save_bytes() override;
	void load_extra_save_bytes(const std::vector<u8>& data) override;

	u8 read(u16 addr) const override;
	void write(u16 addr, u8 data) override;
};

