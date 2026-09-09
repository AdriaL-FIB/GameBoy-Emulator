#pragma once

#include "common.h"

class Bus;

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

	Bus* bus;

public:
	Joypad();

	void connect_bus(Bus* b) { bus = b; }


	u8 read() const;
	void write(u8 data);

	void button_down(Buttons btn);
	void button_up(Buttons btn);
};

