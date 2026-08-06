#include <iostream>
#include "GameBoy.h"

int main() {
	GameBoy gb;

	gb.load_rom("ROMs\\cpu_instrs.gb");

	gb.game_loop();
}