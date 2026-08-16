#include "Joypad.h"

Joypad::Joypad() :
	p1(0xC0),
	dpad(0xF),
	buttons(0xF)
{
}

u8 Joypad::read() const
{
	if (not CHECK_BIT(p1, 4))
	{
		return p1 | dpad & 0x0F;
	}
	if (not CHECK_BIT(p1, 5))
	{
		return p1 | buttons & 0x0F;
	}

	return p1 | 0xF;
}

void Joypad::write(u8 data)
{
	p1 = data & 0xF0;
}

void Joypad::button_down(Buttons btn)
{
	switch (btn)
	{
	case DPAD_RIGHT:
		BIT_SET(dpad, 0, 0);
		break;
	case DPAD_LEFT:
		BIT_SET(dpad, 1, 0);
		break;
	case DPAD_UP:
		BIT_SET(dpad, 2, 0);
		break;
	case DPAD_DOWN:
		BIT_SET(dpad, 3, 0);
		break;
	case BTN_A:
		BIT_SET(buttons, 0, 0);
		break;
	case BTN_B:
		BIT_SET(buttons, 1, 0);
		break;
	case BTN_SELECT:
		BIT_SET(buttons, 2, 0);
		break;
	case BTN_START:
		BIT_SET(buttons, 3, 0);
		break;
	default:
		break;
	}
}

void Joypad::button_up(Buttons btn)
{
	switch (btn)
	{
	case DPAD_RIGHT:
		BIT_SET(dpad, 0, 1);
		break;
	case DPAD_LEFT:
		BIT_SET(dpad, 1, 1);
		break;
	case DPAD_UP:
		BIT_SET(dpad, 2, 1);
		break;
	case DPAD_DOWN:
		BIT_SET(dpad, 3, 1);
		break;
	case BTN_A:
		BIT_SET(buttons, 0, 1);
		break;
	case BTN_B:
		BIT_SET(buttons, 1, 1);
		break;
	case BTN_SELECT:
		BIT_SET(buttons, 2, 1);
		break;
	case BTN_START:
		BIT_SET(buttons, 3, 1);
		break;
	default:
		break;
	}
}

