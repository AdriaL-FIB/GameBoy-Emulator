#pragma once

#include "common.h"
#include <array>

struct LCDC {
	u8 value;

	bool lcd_enable() const { return CHECK_BIT(value, 7); }
	void set_lcd_enable(bool enabled) { BIT_SET(value, 7, enabled); }

	bool window_tile_map() const { return CHECK_BIT(value, 6); }
	void set_window_tile_map(bool enabled) { BIT_SET(value, 6, enabled); }

	bool window_enable() const { return CHECK_BIT(value, 5); }
	void set_window_enable(bool enabled) { BIT_SET(value, 5, enabled); }

	bool bg_window_addr_mode() const { return CHECK_BIT(value, 4); }
	void set_bg_window_addr_mode(bool enabled) { BIT_SET(value, 4, enabled); }

	bool bg_tile_map() const { return CHECK_BIT(value, 3); }
	void set_bg_tile_map(bool enabled) { BIT_SET(value, 3, enabled); }

	bool obj_size() const { return CHECK_BIT(value, 2); }
	void set_obj_size(bool enabled) { BIT_SET(value, 2, enabled); }

	bool obj_enable() const { return CHECK_BIT(value, 1); }
	void set_obj_enable(bool enabled) { BIT_SET(value, 1, enabled); }

	bool bg_window_enable() const { return CHECK_BIT(value, 0); }
	void set_bg_window_enable(bool enabled) { BIT_SET(value, 0, enabled); }

	//u16 bg_tile_map_addr() { return  }
};

struct LCDStatus {
	u8 value;

	bool LYC_int_select() const { return CHECK_BIT(value, 6); }
	void set_LYC_int_select(bool enabled) { BIT_SET(value, 6, enabled); }

	bool mode_2() const { return CHECK_BIT(value, 5); }
	void set_mode_2(bool enabled) { BIT_SET(value, 5, enabled); }

	bool mode_1() const { return CHECK_BIT(value, 4); }
	void set_mode_1(bool enabled) { BIT_SET(value, 4, enabled); }

	bool mode_0() const { return CHECK_BIT(value, 3); }
	void set_mode_0(bool enabled) { BIT_SET(value, 3, enabled); }

	bool LYC_eq_LY() const { return CHECK_BIT(value, 2); }
	void set_LYC_eq_LY(bool enabled) { BIT_SET(value, 2, enabled); }

	u8 ppu_mode() const { return value & 0x3; }
	void set_ppu_mode(u8 v) { value = value & (u8)~0x3 | v & 0x3; }
};

struct Object {
	u8 y;
	u8 x;
	u8 tile_idx;
	u8 flags;
};

constexpr int SCANLINE_DOTS = 456;
constexpr int OAM_BASE_ADDR = 0xFE00;

class PPU
{
private:
	std::array<u8, 6 * 1024> tile_data; // VRAM $8000-$97FF. 8x8 tiles
	std::array<u8, 2 * 1024> tile_maps; // 2 tile maps of 32x32 tiles

	std::array<u8, 40 * 4> oam; // Object Attribute Memory (OAM) $FE00-FE9F


	// Registers --
	u8 oam_dma; // FF46
	u8 wy, wx; // FF4A–FF4B WY, WX: Window Y position, X position plus 7
	LCDC lcdc; // FF40 - LCDC: LCD control

	u8 ly; // FF44 - LY: LCD Y coordinate [read-only]
	u8 lyc; // FF45 - LYC: LY compare
	LCDStatus stat; // FF41 - STAT: LCD status

	u8 scy, scx; // FF42-FF43 - SCY, SCX: Background viewport Y position, X position
	// bottom := (SCY + 143) % 256 and right := (SCX + 159) % 256

	u8 bgp; // FF47 - BGP (Non-CGB Mode only): BG palette data

	u8 obp0, obp1; // FF48-FF49 - OBP0, OBP1 (Non-CGB Mode only): OBJ palette 0, 1 data

	// ---
	int scanline_progress = 0;
	bool oam_scan_performed = false;

	int selected_objects_count = 0;
	std::array<u8, 10> selected_objects;

private:
	void oam_scan();

	void advance_line();

	// Horizontal blank
	u8 mode0(u8 dots);
	// Vertical blank
	u8 mode1(u8 dots);
	// OAM Scan
	u8 mode2(u8 dots);
	// Drawing Pixels
	u8 mode3(u8 dots);
public:
	PPU();

	u8 read(u16 addr);
	void write(u16 addr, u8 data);

	void tick(u8 tcycles);

	bool dma_active() { return false; }
};

