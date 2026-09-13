#pragma once

#include "common.h"
#include <array>

struct StereoSample
{
	float left;
	float right;
};


struct AudioMasterControl // NR52
{
	bool on; // R/W
	std::array<bool,4> ch_on; // Read-only

	u8 read() const { return on << 7 | ch_on[3] << 3 | ch_on[2] << 2 | ch_on[1] << 1 | ch_on[0]; }
	void write(u8 v) 
	{ 
		on = v & 0x80;
		// clears all APU registers and makes them read-only until turned back on, except NR521. Turning the APU off, however, does not affect Wave RAM, which can always be read/written, nor the DIV-APU counter.
	}
};

struct SoundPanning // NR51
{
	std::array<bool, 4> ch_left;
	std::array<bool, 4> ch_right;

	u8 read() const { return ch_left[3] << 7 | ch_left[2] << 6 | ch_left[1] << 5 | ch_left[0] << 4 | ch_right[3] << 3 | ch_right[2] << 2 | ch_right[1] << 1 | ch_right[0]; }
	void write(u8 v)
	{
		ch_left[3] = v & 0x80;
		ch_left[2] = v & 0x40;
		ch_left[1] = v & 0x20;
		ch_left[0] = v & 0x10;
		ch_right[3] = v & 0x08;
		ch_right[2] = v & 0x04;
		ch_right[1] = v & 0x02;
		ch_right[0] = v & 0x01;
	}
};

struct MasterVolume // NR50
{
	u8 left_volume;
	u8 right_volume;

	u8 read() const { return left_volume << 4 | right_volume; }
	void write(u8 v)
	{
		left_volume = (v & 0x70) >> 4;
		right_volume = v & 0x07;
	}
};

constexpr bool duty_cycles[4][8] =
{
	{0, 0, 0, 0, 0, 0, 0, 1}, // 12.5 %
	{1, 0, 0, 0, 0, 0, 0, 1}, // 25 %
	{1, 0, 0, 0, 0, 1, 1, 1}, // 50 %
	{0, 1, 1, 1, 1, 1, 1, 0}  // 75 %
};

struct PulseChannel
{
	// Internal

	// 11-bit
	u16 period_div = 0;
	// 3-bit
	u8 duty_pos = 0;
	// 2-bit
	u8 current_volume = 0;
	bool* ch_on = nullptr;

	PulseChannel(bool* ch_on);

	// sweep - NR10
	u8 sweep_pace = 0;
	bool direction = false;
	u8 individual_step = 0;

	u8 read_sweep() const { return sweep_pace << 4 | direction << 3 | individual_step; }
	void write_sweep(u8 v);

	// length and duty - NR11
	u8 wave_duty = 0; // R/W
	u8 initial_length_timer = 0x3F; // Read-only
	u8 read_duty() const { return wave_duty << 6; }
	void write_duty_length(u8 v);

	// Volume and envelope - NR12
	// Writes to this register while the channel is on require retriggering it afterwards. If the write turns the channel off, retriggering is not necessary (it would do nothing).
	u8 init_volume = 0;
	bool env_dir = false;
	u8 env_pace = 0;
	u8 read_vol() const { return init_volume << 4 | env_dir << 3 | env_pace; }
	void write_vol(u8 v);

	// Freq low - NR13 (Read-only)
	u16 period_value = 0x7FF;
	void write_freq_low(u8 v);

	// Control & Freq high - NR14
	bool length_enable = false;
	u8 read_control() const { return length_enable << 6; }
	void write_control(u8 v);

	void tick(u8 mcycles);
	u8 sample() const;
};

struct WaveChannel
{
	bool* ch_on = nullptr;
	WaveChannel(bool* ch_on);

	// Internal
	// 11-bit
	u16 period_div = 0;

	// 0 --> 31
	u8 wave_pos = 1;

	std::array<u8, 16> wave_ram;

	// DAC enable - NR30
	bool dac_on = false;
	u8 read_dac() const { return dac_on << 7; }
	void write_dac(u8 v)
	{ 
		dac_on = v & 0x80;
		*ch_on = dac_on;
	}

	// Length timer - NR31
	u8 length_timer;
	void write_length_timer(u8 v) { length_timer = v; }

	// Output level - NR32
	u8 output_level;
	u8 read_output_level() const { return output_level << 5; }
	void write_output_level(u8 v) { output_level = BITS(v, 6, 5); }

	// Freq low - NR33 (Read-only)
	u16 period_value = 0x7FF;
	void write_freq_low(u8 v) { period_value = period_value & 0x700 | v; }

	// Control & Freq high - NR34
	bool length_enable = false;
	u8 read_control() const { return length_enable << 6; }
	void write_control(u8 v);

	void tick(u8 mcycles);
	u8 sample() const;

};

class APU
{
private:
	PulseChannel ch1;
	PulseChannel ch2;
	WaveChannel ch3;

	//u8 NR41;
	//u8 NR42;
	//u8 NR43;
	//u8 NR44;

	MasterVolume NR50;
	SoundPanning NR51;
	AudioMasterControl NR52;
private:
	// [0,15] --> [1, -1]
	float dacOutput(u8 value, int channel) const; 
public:
	APU();

	void tick(u8 tcycles);
	const StereoSample get_audio() const;
	
	u8 read(u16 addr);
	void write(u16 addr, u8 data);
};

