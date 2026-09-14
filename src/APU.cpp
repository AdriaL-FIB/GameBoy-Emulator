#include "APU.h"
#include <cassert>



PulseChannel::PulseChannel(bool* ch_on) : ch_on(ch_on) {}


void PulseChannel::write_sweep(u8 v)
{
	sweep_pace = BITS(v, 6, 4);
	direction = (v >> 3) & 0x1;
	individual_step = v & 0x7;
}

void PulseChannel::write_duty_length(u8 v)
{
	wave_duty = BITS(v, 7, 6);
	initial_length_timer = v & 0x3F;
}

void PulseChannel::write_vol(u8 v)
{
	init_volume = BITS(v, 7, 4);
	env_dir = (v >> 3) & 0x1;
	env_pace = v & 0x7;

	if (BITS(v, 7, 3) == 0)
	{
		*ch_on = false;
	}
}

void PulseChannel::write_freq_low(u8 v)
{
	period_value = period_value & 0x700 | v;
}

void PulseChannel::write_control(u8 v)
{
	bool trigger = CHECK_BIT(v, 7);
	length_enable = CHECK_BIT(v, 6);
	period_value = period_value & 0xFF | (v & 0x7) << 8;

	if (trigger)
	{
		// Ch1 is enabled
		*ch_on = true;

		// If length timer expired it is reset.
		if (current_length_timer++ >= 64)
		{
			current_length_timer = initial_length_timer;
		}

		// The period divider is set to the contents of NR13 and NR14.
		period_div = period_value;

		// Envelope timer is reset.

		// Volume is set to contents of NR12 initial volume.
		current_volume = init_volume;

		// Sweep does several things.
	}
}

void PulseChannel::tick(u8 mcycles)
{
	period_div += mcycles;
	if (period_div >= 0x800)
	{
		period_div -= (0x800 - period_value);
		duty_pos = (duty_pos + 1) % 8;
	}
}

void PulseChannel::tick_length_timer()
{
	if (current_length_timer++ >= 64)
	{
		*ch_on = false;
	}
}

void PulseChannel::tick_sweep()
{
	if (sweep_pace > 0)
	{
		sweep_iterations++;
		if (sweep_iterations >= sweep_pace)
		{
			sweep_iterations = 0;
			int inc = period_value / (2 << individual_step);

			period_value = (direction) ? period_value - inc : period_value + inc;

		}
	}

	if (period_value > 0x7FF)
	{
		*ch_on = false;
	}
}

void PulseChannel::tick_envelope()
{
	if (env_pace == 0) return;

	envelope_iterations++;
	if (envelope_iterations >= env_pace)
	{
		envelope_iterations = 0;

		if (current_volume == 0 and not env_dir or current_volume == 15 and env_dir) return;
		current_volume += env_dir * 2 - 1; // [0, 1] --> [-1, 1]
		//std::printf("Current volume: %d\n", current_volume);
	}
}

u8 PulseChannel::sample() const
{
	return duty_cycles[wave_duty][duty_pos] * current_volume;
}


WaveChannel::WaveChannel(bool* ch_on) :
	ch_on(ch_on),
	wave_ram{}
{
}

void WaveChannel::write_control(u8 v)
{
	bool trigger = CHECK_BIT(v, 7);
	length_enable = CHECK_BIT(v, 6);
	period_value = period_value & 0xFF | (v & 0x7) << 8;

	if (trigger)
	{
		// Ch3 is enabled
		*ch_on = true;

		// If length timer expired it is reset.
		if (current_length_timer++ >= 256)
		{
			current_length_timer = initial_length_timer;
		}

		// The period divider is set to the contents of NR33 and NR34.
		period_div = period_value;

		// Volume is set to contents of NR32 initial volume.

		// Wave RAM index is reset, but its not refilled.
		wave_pos = 1;
	}
}

void WaveChannel::tick(u8 tcycles)
{
	assert(tcycles % 2 == 0);
	period_div += tcycles / 2;
	if (period_div >= 0x800)
	{
		period_div -= (0x800 - period_value);
		wave_pos = (wave_pos + 1) % 32;
	}
}

void WaveChannel::tick_length_timer()
{
	if (current_length_timer++ >= 256)
	{
		*ch_on = false;
	}
}

u8 WaveChannel::sample() const
{
	int a = wave_pos / 2;
	int b = wave_pos % 2;
	u8 byte = wave_ram[a];
	u8 nibble = (byte >> ((1-b) * 4)) & 0xF;

	switch (output_level)
	{
	default:
	case 0:
		return 0;
	case 1:
		return nibble;
	case 2:
		return nibble >> 1;
	case 3:
		return nibble >> 2;
	}
}

float APU::dacOutput(u8 value, int channel) const
{
	assert(channel < 4 and channel >= 0);
	return (1.0f - 2.0f * (value / 15.0f)) * NR52.ch_on[channel];
}

APU::APU() :
	ch1(&NR52.ch_on[0]),
	ch2(&NR52.ch_on[1]),
	ch3(&NR52.ch_on[2])
{
}

void APU::div_apu_event()
{
	div_apu++;
	
	// Envelope sweep (volume)
	if (div_apu % 8 == 0)
	{
		ch1.tick_envelope();
		ch2.tick_envelope();
	}

	// Sound length
	if (div_apu % 2 == 0)
	{
		ch1.tick_length_timer();
		ch2.tick_length_timer();
		ch3.tick_length_timer();
	}

	// CH1 freq sweep
	if (div_apu % 4 == 0)
	{
		ch1.tick_sweep();
	}
}

void APU::tick(u8 tcycles)
{
	assert(tcycles % 4 == 0);
	int mcycles = tcycles / 4;
	ch1.tick(mcycles);
	ch2.tick(mcycles);
	ch3.tick(tcycles);
}

const StereoSample APU::get_audio() const
{
	StereoSample ss;
	float mix_l = 0;
	float mix_r = 0;

	int ch1_sample = ch1.sample();
	int ch2_sample = ch2.sample();
	int ch3_sample = ch3.sample();
	int ch4_sample = 0;

	float ch1_dac = dacOutput(ch1_sample, 0) * NR52.ch_on[0];
	float ch2_dac = dacOutput(ch2_sample, 1) * NR52.ch_on[1];
	float ch3_dac = dacOutput(ch3_sample, 2) * NR52.ch_on[2];
	float ch4_dac = dacOutput(ch4_sample, 3) * NR52.ch_on[3];



	mix_l += NR51.ch_left[0] * ch1_dac;
	mix_l += NR51.ch_left[1] * ch2_dac;
	mix_l += NR51.ch_left[2] * ch3_dac;
	mix_l += NR51.ch_left[3] * ch4_dac;

	mix_r += NR51.ch_right[0] * ch1_dac;
	mix_r += NR51.ch_right[1] * ch2_dac;
	mix_r += NR51.ch_right[2] * ch3_dac;
	mix_r += NR51.ch_right[3] * ch4_dac;

	float master_vol_left = (NR50.left_volume + 1.0f) / 8.0f;
	ss.left = mix_l * 0.25f * master_vol_left;

	float master_vol_right = (NR50.right_volume + 1.0f) / 8.0f;
	ss.right = mix_r * 0.25f * master_vol_right;

	return ss;
}

u8 APU::read(u16 addr)
{
	switch (addr)
	{
	case 0xFF10:
		return ch1.read_sweep();
	case 0xFF11:
		return ch1.read_duty();
	case 0xFF12:
		return ch1.read_vol();
	case 0xFF14:
		return ch1.read_control();
	case 0xFF16:
		return ch2.read_duty();
	case 0xFF17:
		return ch2.read_vol();
	case 0xFF19:
		return ch2.read_control();
	case 0xFF24:
		return NR50.read();
	case 0xFF25:
		return NR51.read();
	case 0xFF26:
		return NR52.read();
	case 0xFF1A:
		return ch3.read_dac();
	case 0xFF1C:
		return ch3.read_output_level();
	case 0xFF1E:
		return ch3.read_control();
	case 0xFF30:
	case 0xFF31:
	case 0xFF32:
	case 0xFF33:
	case 0xFF34:
	case 0xFF35:
	case 0xFF36:
	case 0xFF37:
	case 0xFF38:
	case 0xFF39:
	case 0xFF3A:
	case 0xFF3B:
	case 0xFF3C:
	case 0xFF3D:
	case 0xFF3E:
	case 0xFF3F:
		return ch3.wave_ram[addr - 0xFF30];
	default:
		return 0xFF;
	}
}

void APU::write(u16 addr, u8 data)
{
	switch (addr)
	{
	case 0xFF10:
		ch1.write_sweep(data);
		break;
	case 0xFF11:
		ch1.write_duty_length(data);
		break;
	case 0xFF12:
		ch1.write_vol(data);
		break;
	case 0xFF13:
		ch1.write_freq_low(data);
		break;
	case 0xFF14:
		ch1.write_control(data);
		break;
	case 0xFF16:
		ch2.write_duty_length(data);
		break;
	case 0xFF17:
		ch2.write_vol(data);
		break;
	case 0xFF18:
		ch2.write_freq_low(data);
		break;
	case 0xFF19:
		ch2.write_control(data);
		break;
	case 0xFF24:
		NR50.write(data);
		break;
	case 0xFF25:
		NR51.write(data);
		break;
	case 0xFF26:
		NR52.write(data);
		break;
	case 0xFF1A:
		ch3.write_dac(data);
		break;
	case 0xFF1B:
		ch3.write_length_timer(data);
		break;
	case 0xFF1C:
		ch3.write_output_level(data);
		break;
	case 0xFF1D:
		ch3.write_freq_low(data);
		break;
	case 0xFF1E:
		ch3.write_control(data);
		break;
	case 0xFF30:
	case 0xFF31:
	case 0xFF32:
	case 0xFF33:
	case 0xFF34:
	case 0xFF35:
	case 0xFF36:
	case 0xFF37:
	case 0xFF38:
	case 0xFF39:
	case 0xFF3A:
	case 0xFF3B:
	case 0xFF3C:
	case 0xFF3D:
	case 0xFF3E:
	case 0xFF3F:
		ch3.wave_ram[addr - 0xFF30] = data;
		break;
	}
}
