#pragma once
#include "Mapper.h"
class NoMBC : public Mapper
{
	using Mapper::Mapper;
public:
	u8 read(u16 addr) const override;
	void write(u16 addr, u8 data) override;
};

