#include "PPU.h"
#include <cassert>
#include <iostream>

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
	wx(0x00),
	pf(*this)
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
			visible_objects[selected_objects_count++] = i;

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
	window_area = false;
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
	{
		stat.set_ppu_mode(1);
		//std::cout << "Frame completed" << std::endl;
	}
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
	if (scanline_progress == 80)
	{
		while (not bg_fifo.empty()) bg_fifo.pop();
		while (not obj_fifo.empty()) obj_fifo.pop();
		pf.reset();

		pixels_to_discard = scx % 8;
	}

	for (int t = dots - 1; t >= 0; --t)
	{
		int screen_y = ly;
		bool _window = lcdc.window_enable() and wx - 7 <= screen_x and wy <= screen_y;
		if (not window_area and _window)
		{
			pf.reset();
			while (not bg_fifo.empty()) bg_fifo.pop();
			window_area = true;
		}


		// Pixel fetcher tick
		pf.tick(1);


		// FIFO push to LCD
		if (bg_fifo.size() >= 8)
		{
			FIFOPixel p = bg_fifo.front();
			bg_fifo.pop();

			if (pixels_to_discard-- <= 0)
			{
				framebuffer[screen_y * 160 + screen_x] = p.color;
				screen_x++;
			}


			if (screen_x >= 160)
			{
				screen_x = 0;
				stat.set_ppu_mode(0);
				return t;
			}
		}

		scanline_progress++;
	}
	return 0;
}

void PPU::tick(u8 tcycles)
{
	// 1 dot = 1 tcycle

	if (not lcdc.lcd_enable()) return;

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
	u8 ppu_mode = stat.ppu_mode() * lcdc.lcd_enable();

	if (ppu_mode <= 2) // If in mode 3, VRAM is locked
	{
		if (addr <= 0x97FF)
			return tile_data[addr - 0x8000];

		if (addr <= 0x9FFF)
			return tile_maps[addr - 0x9800];

		if (addr <= 0xFE9F and ppu_mode <= 1) // If in mode 3 or 2, Sprite Attrib Memory (OAM) is locked
			return oam[addr - 0xFE00];
	}

	switch (addr)
	{
	case 0xFF40:
		return lcdc.value;
	case 0xFF41:
		return stat.value;
	case 0xFF42:
		return scy;
	case 0xFF43:
		return scx;
	case 0xFF44:
		return ly;
	case 0xFF45:
		return lyc;
	case 0xFF46:
		return oam_dma;
	case 0xFF47:
		return bgp;
	case 0xFF48:
		return obp0;
	case 0xFF49:
		return obp1;
	case 0xFF4A:
		return wy;
	case 0xFF4B:
		return wx;
	default:
		return 0xFF;
	}
}

void PPU::write(u16 addr, u8 data)
{
	u8 ppu_mode = stat.ppu_mode() * lcdc.lcd_enable();
	if (ppu_mode <= 2) // If in mode 3, VRAM is locked
	{
		if (addr <= 0x97FF)
		{
			tile_data[addr - 0x8000] = data;
			return;
		}
			

		if (addr <= 0x9FFF)
		{
			tile_maps[addr - 0x9800] = data;
			return;
		}


		if (addr <= 0xFE9F and ppu_mode <= 1) // If in mode 3 or 2, Sprite Attrib Memory (OAM) is locked
		{
			oam[addr - 0xFE00] = data;
		}
	}



	switch (addr)
	{
	case 0xFF40:
		lcdc.value = data;
		break;
	case 0xFF41:
		stat.set(data);
		break;
	case 0xFF42:
		scy = data;
		break;
	case 0xFF43:
		scx = data;
		break;
	case 0xFF45:
		lyc = data;
		break;
	case 0xFF46:
		oam_dma = data;
		break;
	case 0xFF47:
		bgp = data;
		break;
	case 0xFF48:
		obp0 = data;
		break;
	case 0xFF49:
		obp1 = data;
		break;
	case 0xFF4A:
		wy = data;
		break;
	case 0xFF4B:
		wx = data;
		break;
	}
}

void PPU::PixelFetcher::tick(u8 dots)
{
	current_dots++;

	switch (state)
	{
	case GET_TILE:
	{
		// Get tile - 2 dots

		if (current_dots == 2)
		{
			int wx = ppu.wx - 7;
			int wy = ppu.wy;
			int screen_x = ppu.screen_x;
			int screen_y = ppu.ly;

			bool window_tile = ppu.lcdc.window_enable() and wx <= screen_x and wy <= screen_y;

			u16 base_address = TILE_MAP_1_BASE_ADDR;
			if (ppu.lcdc.bg_tile_map() and not window_tile or ppu.lcdc.window_tile_map() and window_tile)
				base_address = TILE_MAP_2_BASE_ADDR;

			int tile_x, tile_y;
			if (window_tile)
			{
				tile_x = window_tile_x;
				tile_y = (screen_y - wy) / 8;
				row_tile = (screen_y - wy) % 8;
				window_tile_x++;
			}
			else
			{
				int pixel_y = (ppu.scy + screen_y) & 0xFF;
				tile_x = (ppu.scx / 8 + x) & 0x1F;
				tile_y = pixel_y / 8;
				row_tile = pixel_y % 8;
			}

			tile_id = ppu.tile_maps[base_address + tile_y * 32 + tile_x - TILE_MAP_1_BASE_ADDR];

			x++;
			
			state = FetcherState::GET_TILE_DATA_LOW;
			current_dots = 0;
		}
		break;
	}

	case GET_TILE_DATA_LOW:
	{
		// Get tile data low - 2 dots

		if (current_dots == 2)
		{
			if (ppu.lcdc.bg_window_addr_mode())
			{
				tile_addr = TILE_DATA_BASE_ADDR + tile_id * TILE_SIZE + row_tile * TILE_ROW_SIZE;
			}
			else
			{
				tile_addr = TILE_DATA_BASE_ADDR2 + (int8_t)tile_id * TILE_SIZE + row_tile * TILE_ROW_SIZE;
			}

			tile_data_low = ppu.tile_data[tile_addr - TILE_DATA_BASE_ADDR];

			state = FetcherState::GET_TILE_DATA_HIGH;
			current_dots = 0;
		}
		break;
	}
	case GET_TILE_DATA_HIGH:
	{
		// Get tile data high - 2 dots
		if (current_dots == 2)
		{
			tile_data_high = ppu.tile_data[tile_addr + 1 - TILE_DATA_BASE_ADDR];

			state = FetcherState::PUSH;
			current_dots = 0;

			try_push();
		}
		break;
	}
	case PUSH:
		// Push - 1+ dot
		try_push();
		break;
	}
}

void PPU::PixelFetcher::reset()
{
	window_tile_x = 0;
	x = 0;
	state = FetcherState::GET_TILE;
	current_dots = 0;
}

void PPU::PixelFetcher::try_push()
{
	if (ppu.bg_fifo.size() > 8) return;
	
	for (int i = 7; i >= 0; --i)
	{
		FIFOPixel p{};
		p.color = CHECK_BIT(tile_data_high, i) << 1 | CHECK_BIT(tile_data_low, i);
		p.bg_prority = false;
		p.palette = 0;
		ppu.bg_fifo.push(p);
	}

	state = FetcherState::GET_TILE;
	current_dots = 0;		
}
