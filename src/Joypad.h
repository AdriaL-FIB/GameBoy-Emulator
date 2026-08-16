#pragma once

#include "common.h"

enum Buttons
{
	DPAD_RIGHT,
	DPAD_LEFT,
	DPAD_UP,
	DPAD_DOWN,
	BTN_A,
	BTN_B,
	BTN_SELECT,
	BTN_START
};

class Joypad
{
private:
	// FF00 - P1/JOYP: Joypad
	u8 p1;

	u8 dpad;
	u8 buttons;

public:
	Joypad();

	u8 read() const;
	void write(u8 data);

	void button_down(Buttons btn);
	void button_up(Buttons btn);
};

