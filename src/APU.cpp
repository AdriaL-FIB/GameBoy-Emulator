#include "APU.h"
#include <cassert>



PulseChannel::PulseChannel(bool* ch_on) : ch_on(ch_on)
{

}


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
}

void PulseChannel::write_freq_low(u8 v)
{
	period_value = period_value & 0x700 | v;
}

void PulseChannel::write_control(u8 v)
{
	bool trigger = CHECK_BIT(v, 7);
	length_enable = CHECK_BIT(v, 6);
	period_value = period_value & 0xFF | v & 0x7;

	if (trigger)
	{
		// Ch1 is enabled
		*ch_on = true;

		// If length timer expired it is reset.

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

u8 PulseChannel::sample() const
{
	return duty_cycles[wave_duty][duty_pos] * current_volume;
}

APU::APU() :
	ch1(&NR52.ch_on[0]),
	ch2(&NR52.ch_on[1])
{
}

void APU::tick(u8 tcycles)
{
	assert(tcycles % 4 == 0);
	int mcycles = tcycles / 4;
	ch1.tick(mcycles);
	ch2.tick(mcycles);
}

const StereoSample APU::get_audio() const
{
	StereoSample ss;
	float mix_l = 0;
	float mix_r = 0;

	int ch1_sample = ch1.sample();
	int ch2_sample = ch2.sample();
	int ch3_sample = 0;
	int ch4_sample = 0;


	mix_l += NR51.ch_left[0] * dacOutput(ch1_sample);
	mix_l += NR51.ch_left[1] * dacOutput(ch2_sample);
	//mix_l += NR51.ch_left[3] * dacOutput(ch3_sample);
	//mix_l += NR51.ch_left[4] * dacOutput(ch4_sample);

	mix_r += NR51.ch_right[0] * dacOutput(ch1_sample);
	mix_r += NR51.ch_right[1] * dacOutput(ch2_sample);
	//mix_r += NR51.ch_right[2] * dacOutput(ch3_sample);
	//mix_r += NR51.ch_right[3] * dacOutput(ch4_sample);

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
	}
}
