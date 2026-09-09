#include "Bus.h"
#include "CPU.h"

#include <iostream>

// https://gbdev.io/pandocs/Power_Up_Sequence.html
Bus::Bus(CPU* cpu, Cartridge* cart, PPU* ppu, Timer* timer, Joypad* joypad) :
	wram{},
	hram{},
	IE(0x00),
	IF(0xE1)
{
	this->cpu = cpu;
	this->cartridge = cart;
	this->ppu = ppu;
	this->timer = timer;
	this->joypad = joypad;
}

Bus::~Bus() {}

u8 sb, sc;

void Bus::handle_IO_write(u16 addr, u8 data)
{
	if (addr == 0xFF00)
	{
		// Joypad Input
		joypad->write(data);
		return;
	}

	// Serial transfer
	// DEBUG
	if (addr == 0xFF01)
	{
		sb = data;
		return;
	}

	if (addr == 0xFF02)
	{
		sc = data;
		//std::cout << sb;
		return;
	}


	if (BETWEEN(addr, 0xFF04, 0xFF07))
	{
		timer->write(addr, data);
		return;
	}

	if (addr == 0xFF0F)
	{
		write_IF(data);
		return;
	}

	if (BETWEEN(addr, 0xFF10, 0xFF3F))
	{
		// Audio registers
		return;
	}

	if (BETWEEN(addr, 0xFF40, 0xFF4B))
	{
		// LCD (PPU)
		ppu->write(addr, data);
		return;
	}
}

void Bus::write(u16 addr, u8 data) {

	if (BETWEEN(addr, 0xFF80, 0xFFFE)) // HRAM (Internal RAM)
	{
		hram[addr - 0xFF80] = data;
		return;
	}

	if (ppu->dma_active()) return;

	if (addr <= 0x7FFF) // Cartridge
	{
		cartridge->write(addr, data);
		return;
	}

	if (addr <= 0x9FFF) // PPU VRAM
	{
		ppu->write(addr, data);
		return;
	}

	if (addr <= 0xBFFF) // Cartridge RAM, 8kB switchable RAM bank
	{
		cartridge->write(addr, data);
		return;
	}

	if (addr <= 0xDFFF) // WRAM (Internal RAM) (Work RAM)
	{
		wram[addr - 0xC000] = data;
		return;
	}

	if (addr <= 0xFDFF) // WRAM mirror E000-FDFF --> C000-DDFF
	{
		wram[addr - 0xE000] = data;
		return;
	}

	if (addr <= 0xFE9F) // Sprite Attrib Memory (OAM)
	{
		ppu->write(addr, data);
		return;
	}

	if (addr <= 0xFEFF) // Unusable
	{
		return;
	}

	if (addr <= 0xFF7F) // I/O Ports
	{
		handle_IO_write(addr, data);
		return;
	}

	// Interrupt enable register IE
	write_IE(data);
}

u8 Bus::handle_IO_read(u16 addr) {
	if (addr == 0xFF00)
		// Joypad Input
		return joypad->read();

	if (BETWEEN(addr, 0xFF01, 0xFF02))
		// Serial transfer
		return 0xFF;

	if (BETWEEN(addr, 0xFF04, 0xFF07))
		return timer->read(addr);

	if (addr == 0xFF0F)
		return IF;

	if (BETWEEN(addr, 0xFF10, 0xFF3F))
		// Audio registers
		return 0xFF;

	if (BETWEEN(addr, 0xFF40, 0xFF4B))
		// LCD (PPU)
		return ppu->read(addr);

	return 0xFF;
}

u8 Bus::read(u16 addr) {

	if (BETWEEN(addr, 0xFF80, 0xFFFE)) // HRAM (Internal RAM)
		return hram[addr - 0xFF80];

	if (ppu->dma_active()) return 0xFF;

	if (addr <= 0x7FFF) // Cartridge
		return cartridge->read(addr);

	if (addr <= 0x9FFF) // PPU VRAM
		return ppu->read(addr);

	if (addr <= 0xBFFF) // Cartridge RAM, 8kB switchable RAM bank 
		return cartridge->read(addr);

	if (addr <= 0xDFFF) // WRAM (Internal RAM) (Work RAM)
		return wram[addr - 0xC000];

	if (addr <= 0xFDFF) // WRAM mirror E000-FDFF --> C000-DDFF
		return wram[addr - 0xE000];

	if (addr <= 0xFE9F) // Sprite Attrib Memory (OAM)
		return ppu->read(addr);

	if (addr <= 0xFEFF) // Unusable
		return 0xFF;

	if (addr <= 0xFF7F) // I/O Ports
		return handle_IO_read(addr);

	// Interrupt enable register IE
	return IE;
}

void Bus::write_IE(u8 value)
{
	IE = value & 0x1F | 0xE0;
}

void Bus::write_IF(u8 value)
{
	IF = value & 0x1F | 0xE0;
}

u8 Bus::read_IE()
{
	return IE;
}

u8 Bus::read_IF()
{
	return IF;
}

void Bus::set_IF(unsigned int bit, bool on)
{
	BIT_SET(IF, bit, on);
}

u8 Bus::dma_read(u16 addr)
{
	if (addr <= 0x7FFF) // Cartridge
		return cartridge->read(addr);

	if (addr <= 0x9FFF) // PPU VRAM
		return ppu->read(addr);

	if (addr <= 0xBFFF) // Cartridge RAM, 8kB switchable RAM bank 
		return cartridge->read(addr);

	if (addr <= 0xDFFF) // WRAM (Internal RAM) (Work RAM)
		return wram[addr - 0xC000];

	return 0xFF;
}
