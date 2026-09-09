#include "PPU.h"
#include <cassert>
#include <iostream>
#include "Bus.h"
#include <algorithm>

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


void PPU::dma_transfer(int mcycles)
{
	for (int i = 0; i < mcycles; ++i)
	{
		oam[dma_transfer_progress] = bus->dma_read(dma_transfer_start_addr + dma_transfer_progress);
		dma_transfer_progress++;

		if (dma_transfer_progress >= 160)
		{
			dma_transfer_progress = -1;
			return;
		}
	}
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

	std::stable_sort(visible_objects.begin(), visible_objects.begin() + selected_objects_count, [this](u8 a, u8 b) {
		int obj_addr_a = a * 4;
		int obj_addr_b = b * 4;

		u8 xa = oam[obj_addr_a + 1];
		u8 xb = oam[obj_addr_b + 1];

		return xa < xb;
		});

	next_object = 0;
}

void PPU::advance_line()
{
	assert(scanline_progress >= SCANLINE_DOTS);

	scanline_progress -= SCANLINE_DOTS;
	set_ly((ly + 1) % 154);
	selected_objects_count = 0;
	oam_scan_performed = false;
	window_area = false;
	another_obj_need_fetch = false;

}

void PPU::set_mode(u8 mode)
{
	stat.set_ppu_mode(mode);

	bool new_stat_interrupt_line =
		mode == 0 and stat.mode_0() or
		mode == 1 and stat.mode_1() or
		mode == 2 and stat.mode_2();

	if (not stat_interrupt_line and new_stat_interrupt_line)
	{
		bus->set_IF(1, true);
	}
	stat_interrupt_line = new_stat_interrupt_line;

	if (mode == 1)
	{
		bus->set_IF(0, true);
	}
}

void PPU::check_STAT_int()
{
	u8 mode = stat.ppu_mode();

	bool new_stat_interrupt_line =
		mode == 0 and stat.mode_0() or
		mode == 1 and stat.mode_1() or
		mode == 2 and stat.mode_2() or
		ly == lyc and stat.LYC_int_select();

	if (not stat_interrupt_line and new_stat_interrupt_line)
	{
		bus->set_IF(1, true);
	}
	stat_interrupt_line = new_stat_interrupt_line;
}

void PPU::set_ly(u8 value)
{
	ly = value;
	check_STAT_int();
	stat.set_LYC_eq_LY(ly == lyc);
}

void PPU::set_lyc(u8 value)
{
	lyc = value;
	check_STAT_int();
	stat.set_LYC_eq_LY(ly == lyc);
}

void PPU::turn_off()
{
	//set_ly
}

u8 PPU::get_palette_color(u8 palette, u8 col_id)
{
	u8 mask = 0b11 << col_id * 2;
	return (palette & mask) >> col_id * 2;
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

	advance_line();

	if (ly > 143)
	{
		set_mode(1);
		//std::cout << "Frame completed" << std::endl;
	}
	else
		set_mode(2);


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

	set_mode(2);

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
	set_mode(3);

	return not_used;
}

u8 PPU::mode3(u8 dots)
{
	int screen_y = ly;
	bool _window = lcdc.window_enable() and wx - 7 <= screen_x and wy <= screen_y;

	// Only when entering mode 3: empty queues, compute pixels to discard
	if (scanline_progress == 80)
	{
		while (not bg_fifo.empty()) bg_fifo.pop();
		while (not obj_fifo.empty()) obj_fifo.pop_front();
		pf.reset();

		// fetcher fetches the entire tile, some pixels need to be discarded
		// Only discard when the line does not start with window
		pixels_to_discard = (_window) ? 0 : scx % 8;
	}

	for (int t = dots - 1; t >= 0; --t)
	{
		_window = lcdc.window_enable() and wx - 7 <= screen_x and wy <= screen_y;
		if (not window_area and _window)
		{
			pf.reset();
			while (not bg_fifo.empty()) bg_fifo.pop();
			window_area = true;
		}

		if (not pf.fetching_obj() and selected_objects_count > 0 and lcdc.obj_enable() and next_object < selected_objects_count)
		{
			another_obj_need_fetch = false;

			int obj_idx = visible_objects[next_object] * 4;
			Object& obj = reinterpret_cast<Object&>(oam[obj_idx]);
			if (obj.x - 8 <= screen_x)
			{
				pf.reset(true);

				if (next_object + 1 < selected_objects_count)
				{
					Object& next_obj = reinterpret_cast<Object&>(oam[visible_objects[next_object + 1] * 4]);
					if (next_obj.x - 8 <= screen_x)
					{
						another_obj_need_fetch = true;
					}
				}
			}
		}

		// Pixel fetcher tick
		pf.tick(1);

		// FIFO push to LCD
		if (bg_fifo.size() >= 8 and not pf.fetching_obj() and not another_obj_need_fetch)
		{
			FIFOPixel p_bg = bg_fifo.front();
			bg_fifo.pop();

			if (pixels_to_discard-- <= 0)
			{
				u8 color = get_palette_color(bgp, p_bg.color);

				if (not obj_fifo.empty())
				{
					FIFOPixel p_obj = obj_fifo.front();
					obj_fifo.pop_front();

					if (not (p_obj.color == 0 or p_obj.bg_prority and p_bg.color != 0))
						color = get_palette_color((p_obj.palette) ? obp1 : obp0, p_obj.color);
				}
	
				framebuffer[screen_y * 160 + screen_x] = color;
				screen_x++;
			}

			if (screen_x >= 160)
			{
				screen_x = 0;
				set_mode(0);
				return t;
			}
		}

		scanline_progress++;
	}
	return 0;
}

void PPU::tick(u8 tcycles)
{
	if (dma_transfer_progress >= 0)
	{
		dma_transfer(tcycles / 4);
	}
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
		check_STAT_int();
		break;
	case 0xFF42:
		scy = data;
		break;
	case 0xFF43:
		scx = data;
		break;
	case 0xFF45:
		set_lyc(data);
		break;
	case 0xFF46:
		oam_dma = data;
		dma_transfer_progress = 0;
		dma_transfer_start_addr = data << 8;
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
			if (_fetching_obj)
			{
				int screen_x = ppu.screen_x;
				int screen_y = ppu.ly;

				bool big_obj = ppu.lcdc.obj_size();

				int obj_idx = ppu.visible_objects[ppu.next_object] * 4;
				current_obj = reinterpret_cast<Object*>(&ppu.oam[obj_idx]);

				tile_id = current_obj->tile_idx;
				if (big_obj)
					tile_id &= 0xFE;


				row_tile = screen_y - (current_obj->y - 16);
				if (current_obj->flip_y())
				{
					// row_tile [0, obj_size - 1]
					int obj_size = 8 + 8 * big_obj;
					row_tile = obj_size - row_tile - 1;
				}
				//assert(screen_x == current_obj->x - 8);
			}
			else
			{
				int wx = ppu.wx - 7;
				int wy = ppu.wy;
				int screen_y = ppu.ly;

				bool is_window_tile = ppu.window_area;

				u16 base_address = TILE_MAP_1_BASE_ADDR;
				if (ppu.lcdc.bg_tile_map() and not is_window_tile or ppu.lcdc.window_tile_map() and is_window_tile)
					base_address = TILE_MAP_2_BASE_ADDR;

				int tile_x, tile_y;
				if (is_window_tile)
				{
					tile_x = window_tile_x;
					tile_y = (screen_y - wy) / 8;
					row_tile = (screen_y - wy) % 8;
				}
				else
				{
					int pixel_y = (ppu.scy + screen_y) & 0xFF;
					tile_x = (ppu.scx / 8 + x) & 0x1F;
					tile_y = pixel_y / 8;
					row_tile = pixel_y % 8;
				}

				tile_id = ppu.tile_maps[base_address + tile_y * 32 + tile_x - TILE_MAP_1_BASE_ADDR];
			}
			
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
			if (ppu.lcdc.bg_window_addr_mode() or _fetching_obj)
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

void PPU::PixelFetcher::reset(bool obj /*= false*/)
{
	if (not obj)
	{
		window_tile_x = 0;
		x = 0;
	}
	state = FetcherState::GET_TILE;
	current_dots = 0;
	_fetching_obj = obj;
}

void PPU::PixelFetcher::try_push()
{
	if (_fetching_obj)
	{
		const bool flip_x = current_obj->flip_x();
		const bool priority = current_obj->priority();
		const bool palette = current_obj->palette();

		int skipped_px = 0;
		if (current_obj->x < 8)
		{
			skipped_px = 8 - current_obj->x;
		}

		for (int i = skipped_px; i < 8; ++i)
		{
			int b = flip_x ? i : 7 - i;

			FIFOPixel p{};
			p.color = CHECK_BIT(tile_data_high, b) << 1 | CHECK_BIT(tile_data_low, b);
			p.bg_prority = current_obj->priority();
			p.palette = current_obj->palette();

			if (i < ppu.obj_fifo.size() and ppu.obj_fifo[i].color == 0x00 /* Transparent */)
			{
				ppu.obj_fifo[i] = p;
			}
			else if (i >= ppu.obj_fifo.size())
				ppu.obj_fifo.push_back(p);
		}

		_fetching_obj = false;
		ppu.next_object++;
	}
	else
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
		x++;
		if (ppu.window_area)
			window_tile_x++;
	}

	state = FetcherState::GET_TILE;
	current_dots = 0;		
}
