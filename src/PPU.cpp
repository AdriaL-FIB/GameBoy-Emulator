#include "PPU.h"
#include <cassert>

PPU::PPU() :
	lcdc({ 0x91 }),
	stat({ 0x86 }),
	ly(0x00),
	scx(0x00),
	scy(0x00),
	lyc(0x00),
	oam_dma(0xFF),
	bgp(0xFC),
	obp0(0x00),
	obp1(0x00),
	wy(0x00),
	wx(0x00)
{
}


void PPU::oam_scan()
{
	bool big_obj = lcdc.obj_size();
	int obj_size = 8 + 8 * big_obj;
	int _ly = ly + 16; // LY starts at the top of the screen, Y object coords starts 16 pixels before.

	for (int i = 0; i < 40; ++i)
	{
		int obj_addr = i * 4;
		u8 y = oam[obj_addr];

		int miny = y;
		int maxy = y + obj_size - 1;

		if (miny <= _ly and _ly <= maxy)
		{
			selected_objects[selected_objects_count++] = i;

			if (selected_objects_count >= 10)
				break;
		}

	}
}

void PPU::advance_line()
{
	assert(scanline_progress >= SCANLINE_DOTS);

	scanline_progress -= SCANLINE_DOTS;
	ly = (ly + 1) % 154;
	selected_objects_count = 0;
	oam_scan_performed = false;
}

u8 PPU::mode0(u8 dots)
{
	int dots_remaining = SCANLINE_DOTS - scanline_progress;

	if (dots < dots_remaining)
	{
		scanline_progress += dots;
		return 0;
	}

	int not_used = dots - dots_remaining;
	scanline_progress = SCANLINE_DOTS;

	if (ly >= 143)
		stat.set_ppu_mode(1);
	else
		stat.set_ppu_mode(2);

	advance_line();

	return not_used;
}

u8 PPU::mode1(u8 dots)
{
	int dots_remaining = SCANLINE_DOTS - scanline_progress;

	if (dots < dots_remaining or ly < 153)
	{
		scanline_progress += dots;

		if (dots >= dots_remaining)
			advance_line();

		return 0;
	}

	int not_used = dots - dots_remaining;
	scanline_progress = SCANLINE_DOTS;

	stat.set_ppu_mode(2);

	advance_line();

	return not_used;
}

u8 PPU::mode2(u8 dots)
{
	if (not oam_scan_performed)
	{
		oam_scan();
		oam_scan_performed = true;
	}

	int dots_remaining = 80 - scanline_progress;

	if (dots < dots_remaining)
	{
		scanline_progress += dots;
		return 0;
	}

	int not_used = dots - dots_remaining;
	scanline_progress = 80;
	stat.set_ppu_mode(3);

	return not_used;
}

u8 PPU::mode3(u8 dots)
{

}

void PPU::tick(u8 tcycles)
{
	// 1 dot = 1 tcycle

	u8 mode = stat.ppu_mode();

	int remaining_dots = tcycles;

	while (remaining_dots > 0)
	{
		// Perform mode action
		switch (mode)
		{
		case 0:
			remaining_dots = mode0(remaining_dots);
			break;
		case 1:
			remaining_dots = mode1(remaining_dots);
			break;
		case 2: // OAM scan
			remaining_dots = mode2(remaining_dots);
			break;
		case 3: // Drawing pixels
			remaining_dots = mode3(remaining_dots);
			break;
		}

		mode = stat.ppu_mode();
	}
}

u8 PPU::read(u16 addr)
{
	return 0xFF;
}

void PPU::write(u16 addr, u8 data)
{

}