#include "CPU.h"

CPU::CPU() {
	PC = 0x0100;
	SP = 0xFFFF;
	AF.reg = 0x0000;
	BC.reg = 0x0000;
	DE.reg = 0x0000;
	HL.reg = 0x0000;
}

CPU::~CPU(){}

uint16_t two8b_to_16b(uint8_t h, uint16_t l) {
	return ((uint16_t)h << 8) | l;
}

void CPU::clock()
{
	opcode = read(PC++);

	curr_instruction = instruction_by_opcode(opcode);

	fetch_data();

	//switch (opcode) {
	//default:
	//	return;
	//case 0x00:
	//	break;
	//case 0x01:
	//	uint8_t low = read(PC++);
	//	uint8_t high = read(PC++);
	//	uint16_t nn = unsignedint16(low, high);
	//	BC.reg = nn;
	//	break;
	//case 0x02:
	//	write(BC.reg, AF.A); break;
	//case 0x03:
	//	BC.reg++; break;
	//case 0x04:
	//	INC(&BC.B); break;
	//case 0x05:
	//	DEC(&BC.B); break;
	//case 0x06:
	//	BC.B = read(PC++); break;
	//case 0x07:

	//}
}

void CPU::fetch_data() {
	switch (curr_instruction->mode) {
	default:
		break;
	}
}

void CPU::write(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

void CPU::write(uint16_t addr, uint16_t data)
{
	u8 h = data & 0xFF00;
	u8 l = data & 0x00FF;
	bus->write(addr, l);
	bus->write(addr+1, h);
}

uint8_t CPU::read(uint16_t addr) {
	return bus->read(addr);
}

uint16_t CPU::read_16b(uint16_t addr)
{
	return two8b_to_16b(bus->read(addr+1), bus->read(addr));
}

bool CPU::check_condition(cond_type cc)
{
	switch (cc)
	{
	case CT_NZ:
		//Jump if Z flag is reset
		return (!AF.F.Z);
	case CT_Z:
		//Jump if Z flag is set.
		return (AF.F.Z);
	case CT_NC:
		//Jump if C flag is reset.
		return (!AF.F.C);
	case CT_C:
		//Jump if C flag is set.
		return (AF.F.C);
	}
	return true;
}

void CPU::LD(uint8_t* dest, uint8_t value)
{
	*dest = value;
}

void CPU::LD(uint16_t* dest, uint16_t value)
{
	*dest = value;
}

void CPU::PUSH(uint16_t value)
{
	SP -= 2;
	write(SP, value);
}

void CPU::POP(uint16_t* dest)
{
	*dest = read_16b(SP);
	SP += 2;
}

uint8_t CPU::add_8b(uint8_t dest, uint8_t value)
{
	u16 result = dest + value;
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x10; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x100;
	return result;
}

uint8_t CPU::sub_8b(uint8_t dest, uint8_t value)
{
	u16 result = dest - value;
	AF.F.Z = result == 0;
	AF.F.N = 1;
	AF.F.H = ((AF.A & 0x0F) - (value & 0x0F) < 0);
	AF.F.C = AF.A < value;
}

void CPU::ADD(uint8_t value)
{
	AF.A = add_8b(AF.A, value);
}

void CPU::ADC(uint8_t value)
{
	AF.A = add_8b(AF.A, value + AF.F.C);
}

void CPU::SUB(uint8_t value)
{
	AF.A = sub_8b(AF.A, value);
}

void CPU::SBC(uint8_t value)
{
	AF.A = sub_8b(AF.A, value + AF.F.C);
}

void CPU::AND(uint8_t value)
{
	AF.A &= value;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 1;
	AF.F.C = 0;
}

void CPU::OR(uint8_t value)
{
	AF.A |= value;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::XOR(uint8_t value)
{
	AF.A ^= value;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::CP(uint8_t value)
{
	u16 result = AF.A - value - AF.F.C;
	AF.F.Z = result == 0;
	AF.F.N = 1;
	AF.F.H = ((AF.A & 0x0F) - (value & 0x0F) < 0);
	AF.F.C = AF.A < value;
}

void CPU::INC(uint8_t* n)
{
	*n = add_8b(*n, 1);
	// Carry flag not affected!
	AF.F.C = 0;
}

void CPU::DEC(uint8_t* n)
{
	*n = sub_8b(*n, 1);
	// Carry flag not affected!
	AF.F.C = 0;
}

void CPU::ADD(uint16_t value)
{
	u32 result = HL.reg + value;
	AF.F.N = 0;
	AF.F.H = (HL.reg ^ value ^ result) & 0x1000; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (HL.reg ^ value ^ result) & 0x10000;

	HL.reg = result;
}

void CPU::ADD_SP(int8_t signed_value)
{
	int32_t result = SP + signed_value;
	AF.F.Z = 0;
	AF.F.N = 0;
	AF.F.H = (HL.reg ^ signed_value ^ result) & 0x10; // Esto esta bien?
	AF.F.C = (HL.reg ^ signed_value ^ result) & 0x100;

	HL.reg = result;
}

void CPU::INC(uint16_t* nn)
{
	*nn += 1;
}

void CPU::DEC(uint16_t* nn)
{
	*nn -= 1;
}

void CPU::SWAP(uint8_t* n)
{
	u8 h = *n & 0xF0;
	u8 l = *n & 0x0F;

	*n = l << 4 | h >> 4;

	AF.F.Z = *n == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::DAA()
{
	NO_IMPL;
}

void CPU::CPL()
{
	AF.A = ~AF.A;

	AF.F.N = 1;
	AF.F.H = 1;
}

void CPU::CCF()
{
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = !AF.F.C;
}

void CPU::SCF()
{
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 1;
}

void CPU::NOP()
{
}

void CPU::HALT()
{
	NO_IMPL;
}

void CPU::STOP()
{
	NO_IMPL;
}

void CPU::DI()
{
	NO_IMPL;
}

void CPU::EI()
{
	NO_IMPL;
}

uint8_t CPU::rotate_left(uint8_t n)
{
	u8 result = (n << 1) | (n & 0x80);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x80; // Contains old bit 7 data.
	return result;
}

uint8_t CPU::rotate_left_carry(uint8_t n)
{
	u8 result = (n << 1) | (AF.F.C);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x80; // Contains old bit 7 data.
	return result;
}

uint8_t CPU::rotate_right(uint8_t n)
{
	u8 result = (n >> 1) | ((n & 0x01) << 7);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x1; // Contains old bit 0 data.
	return result;
}

uint8_t CPU::rotate_right_carry(uint8_t n)
{
	u8 result = (n >> 1) | (AF.F.C << 7);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x1; // Contains old bit 0 data.
	return result;
}

void CPU::RLCA()
{
	AF.A = rotate_left(AF.A);
	AF.F.Z = 0;
}

void CPU::RLA()
{
	AF.A = rotate_left_carry(AF.A);
	AF.F.Z = 0;
}

void CPU::RRCA()
{
	AF.A = rotate_right(AF.A);
	AF.F.Z = 0;
}

void CPU::RRA()
{
	AF.A = rotate_right_carry(AF.A);
	AF.F.Z = 0;
}

void CPU::RLC(uint8_t* n)
{
	*n = rotate_left(*n);
}

void CPU::RL(uint8_t* n)
{
	*n = rotate_left_carry(*n);
}

void CPU::RRC(uint8_t* n)
{
	*n = rotate_right(*n);
}

void CPU::RR(uint8_t* n)
{
	*n = rotate_right_carry(*n);
}

void CPU::SLA(uint8_t* n)
{
	u8 result = *n << 1;
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x80; // Contains old bit 7 data.
	
	*n = result;
}

void CPU::SRA(uint8_t* n)
{
	u8 result = (*n >> 1) | (*n & 0x80);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x1; // Contains old bit 0 data.
	*n = result;
}

void CPU::SRL(uint8_t* n)
{
	u8 result = (*n >> 1);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x1; // Contains old bit 0 data.
	*n = result;
}

void CPU::BIT(uint8_t bit, uint8_t* r)
{
	AF.F.Z = !CHECK_BIT(*r, bit);
	AF.F.N = 0;
	AF.F.H = 1;
}

void CPU::SET(uint8_t bit, uint8_t* r)
{
	*r |= (1 << bit);
}

void CPU::RES(uint8_t bit, uint8_t* r)
{
	*r &= ~(1 << bit);
}

void CPU::JP(uint16_t addr)
{
	PC = addr;
}

void CPU::JP(cond_type cc, uint16_t addr)
{
	if (check_condition(cc))
		PC = addr;
}

void CPU::JR(uint8_t addr)
{
	PC += addr;
}

void CPU::JR(cond_type cc, uint8_t addr)
{
	if (check_condition(cc))
		PC += addr;
}

void CPU::CALL(uint16_t nn)
{
	SP -= 2;
	write(SP, PC); // nota, quizas quitar el +1
	PC = nn;
}

void CPU::CALL(cond_type cc, uint16_t nn)
{
	if (check_condition(cc)) {
		CALL(nn);
	}
}

void CPU::RST(uint8_t n)
{
	SP -= 2;
	write(SP, PC);
	PC = two8b_to_16b(0x00, n);
}

void CPU::RET()
{
	PC = read_16b(SP);
	SP += 2;
}

void CPU::RET(cond_type cc)
{
	if (check_condition(cc))
		RET();
}

void CPU::RETI()
{
	RET();
	EI();
}