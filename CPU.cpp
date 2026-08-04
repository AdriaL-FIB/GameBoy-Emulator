#include "CPU.h"
#include "Bus.h"

constexpr u16 INTERRUPT_VECTORS[5] = {
	0x0040, // VBlank
	0x0048, // LCD STAT
	0x0050, // Timer
	0x0058, // Serial
	0x0060  // Joypad
};

CPU::CPU() {
	//PC = 0x0100;
	//SP = 0xFFFF;
	//AF.reg = 0x0000;
	//BC.reg = 0x0000;
	//DE.reg = 0x0000;
	//HL.reg = 0x0000;
	AF.reg = 0x01B0;
	BC.reg = 0x0013;
	DE.reg = 0x00D8;
	HL.reg = 0x014D;
	SP = 0xFFFE;
	PC = 0x0100;
}

CPU::~CPU(){}

uint16_t two8b_to_16b(uint8_t h, uint16_t l) {
	return ((uint16_t)h << 8) | l;
}


void* CPU::get_reg_from_enum(reg_type reg)
{
	switch (reg)
	{
	default:
	case RT_NONE:
		return nullptr;
	case RT_A:
		return &AF.A;
	case RT_F:
		return &AF.F;
	case RT_B:
		return &BC.B;
	case RT_C:
		return &BC.C;
	case RT_D:
		return &DE.D;
	case RT_E:
		return &DE.E;
	case RT_H:
		return &HL.H;
	case RT_L:
		return &HL.L;
	case RT_AF:
		return &AF.reg;
	case RT_BC:
		return &BC.reg;
	case RT_DE:
		return &DE.reg;
	case RT_HL:
		return &HL.reg;
	case RT_SP:
		return &SP;
	case RT_PC:
		return &PC;
	}
}

u16 CPU::read_reg_from_enum(reg_type reg) const
{
	switch (reg)
	{
	default:
	case RT_NONE:
		return 0;
	case RT_A:
		return AF.A;
	//case RT_F:
	//	return AF.F;
	case RT_B:
		return BC.B;
	case RT_C:
		return BC.C;
	case RT_D:
		return DE.D;
	case RT_E:
		return DE.E;
	case RT_H:
		return HL.H;
	case RT_L:
		return HL.L;
	case RT_AF:
		return AF.reg;
	case RT_BC:
		return BC.reg;
	case RT_DE:
		return DE.reg;
	case RT_HL:
		return HL.reg;
	case RT_SP:
		return SP;
	case RT_PC:
		return PC;
	}
}

//bool CPU::is_reg_16_bit(reg_type reg) const
//{
//	return reg >= RT_AF;
//}

u8 CPU::clock()
{
	if (IME) {
		u8 IF = read(IF_ADDR);
		u8 IE = read(IE_ADDR);

		u8 pending = IE & IF & 0x1F;

		if (pending) {
			for (int i = 0; i < 5; ++i) {
				if (CHECK_BIT(pending, i)) {
					IME = false;
					BIT_SET(IF, i, false);
					write(IF_ADDR, IF);

					CALL(INTERRUPT_VECTORS[i]);
					return 20; // Add 5 M cycles
				}
			}
		}
	}

	if (IME_scheduled) {
		IME = true;
		IME_scheduled = false;
	}

	curr_data = {};

	opcode = read(PC++);

	if (opcode == 0xCB) {
		opcode = read(PC++);
		curr_instruction = cb_instruction_by_opcode(opcode);
	}
	else {
		curr_instruction = instruction_by_opcode(opcode);
	}


	fetch_data();

	bool action_taken = check_condition(curr_instruction->condition);

	execute_instr();

	post_process();

	if (action_taken) return curr_instruction->cycles_if_taken;
	return curr_instruction->cycles;
}

void CPU::fetch_data() {

	const operand& src = curr_instruction->source;
	const operand& dst = curr_instruction->destination;

	switch (src.type)
	{
	default:
		break;
	case OT_NONE:
		break;
	case OT_R:
		curr_data.fetched_data = read_reg_from_enum(src.reg);
		break;
	case OT_D8:
		curr_data.fetched_data = read(PC++);
		break;
	case OT_D16:
		curr_data.fetched_data = read_16b(PC);
		PC += 2;
		break;
	case OT_HLD:
	case OT_HLI:
	case OT_MR:
		curr_data.fetched_data = read(read_reg_from_enum(src.reg));
		break;
	case OT_MC:
		curr_data.fetched_data = read(0xFF00 | read_reg_from_enum(src.reg));
		break;
	case OT_A8:
		curr_data.fetched_data = read(0xFF00 | read(PC++));
		break;
	case OT_A16:
		curr_data.fetched_data = read(read_16b(PC));
		PC += 2;
		break;
	case OT_SPR8:
		int8_t r8 = (int8_t)read(PC++);
		curr_data.fetched_data = add_16bs(SP, r8);
		break;
	}

	switch (dst.type)
	{
	default:
		break;
	case OT_HLD:
	case OT_HLI:
	case OT_MR:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_reg_from_enum(dst.reg);
		break;
	case OT_MC:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = 0xFF00 | read_reg_from_enum(dst.reg);
		break;
	case OT_A8:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = 0xFF00 | read(PC++);
		break;
	case OT_A16:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_16b(PC);
		PC += 2;
		break;
	}
}

void CPU::execute_instr()
{
	switch (curr_instruction->type)
	{
	case IN_NONE:
		return;
	case IN_NOP:
		return;
	case IN_LDH:
	case IN_LD:
		LD();
		return;
	case IN_INC:
		INC();
		return;
	case IN_DEC:
		DEC();
		return;
	case IN_RLCA:
		RLCA();
		return;
	case IN_ADD:
		ADD();
		return;
	case IN_ADD_SP:
		ADD_SP();
		return;
	case IN_RRCA:
		RRCA();
		return;
	case IN_STOP:
		STOP();
		return;
	case IN_RLA:
		RLA();
		return;
	case IN_JR:
		JR();
		return;
	case IN_RRA:
		RRA();
		return;
	case IN_DAA:
		DAA();
		return;
	case IN_CPL:
		CPL();
		return;
	case IN_SCF:
		SCF();
		return;
	case IN_CCF:
		CCF();
		return;
	case IN_HALT:
		HALT();
		return;
	case IN_ADC:
		ADC();
		return;
	case IN_SUB:
		SUB();
		return;
	case IN_SBC:
		SBC();
		return;
	case IN_AND:
		AND();
		return;
	case IN_XOR:
		XOR();
		return;
	case IN_OR:
		OR();
		return;
	case IN_CP:
		CP();
		return;
	case IN_POP:
		POP();
		return;
	case IN_JPHL:
	case IN_JP:
		JP();
		return;
	case IN_PUSH:
		PUSH();
		return;
	case IN_RET:
		RET();
		return;
	case IN_CALL:
		CALL();
		return;
	case IN_RETI:
		RETI();
		return;
	case IN_DI:
		DI();
		return;
	case IN_EI:
		EI();
		return;
	case IN_RST:
		RST();
		return;
	case IN_ERR:
		// ?
		return;
	case IN_RLC:
		RLC();
		return;
	case IN_RRC:
		RRC();
		return;
	case IN_RL:
		RL();
		return;
	case IN_RR:
		RR();
		return;
	case IN_SLA:
		SLA();
		return;
	case IN_SRA:
		SRA();
		return;
	case IN_SWAP:
		SWAP();
		return;
	case IN_SRL:
		SRL();
		return;
	case IN_BIT:
		BIT();
		return;
	case IN_RES:
		RES();
		return;
	case IN_SET:
		SET();
		return;
	default:
		return;
	}
}

void CPU::post_process()
{
	if (curr_instruction->source.type == OT_HLI ||
		curr_instruction->destination.type == OT_HLI) {
		HL.reg++;
	}
	else if (curr_instruction->source.type == OT_HLD ||
		curr_instruction->destination.type == OT_HLD) {
		HL.reg--;
	}
}

void CPU::write(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

void CPU::write(uint16_t addr, uint16_t data)
{
	u8 h = data >> 8;
	u8 l = data & 0xFF;
	bus->write(addr, l);
	bus->write(addr+1, h);
}

uint8_t CPU::read(uint16_t addr) {
	return bus->read(addr);
}

uint16_t CPU::read_16b(uint16_t addr)
{
	u8 l = bus->read(addr);
	u8 h = bus->read(addr + 1);
	return two8b_to_16b(h, l);
}

//uint8_t* CPU::get_addr(uint16_t addr)
//{
//	return nullptr;
//}

bool CPU::check_condition(cond_type cc) const
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
	default:
	case CT_NONE:
		return true;
	}
}

void CPU::LD()
{
	u16 value = curr_data.fetched_data;
	if (curr_data.dest_is_mem) {
		if (curr_instruction->width == WIDTH_16) {
			write(curr_data.mem_dest, value);
		}
		else {
			write(curr_data.mem_dest, (u8)value);
		}
	}
	else {
		void* dest = get_reg_from_enum(curr_instruction->destination.reg);
		if (curr_instruction->width == WIDTH_16) {
			*((u16*)dest) = value;
		}
		else {
			*((u8*)dest) = value;
		}
	}
}

void CPU::PUSH()
{
	u16 value = curr_data.fetched_data;
	SP -= 2;
	write(SP, value);
}

void CPU::POP()
{
	reg_type rt = curr_instruction->destination.reg;
	u16* dest = (u16*)get_reg_from_enum(rt);
	*dest = read_16b(SP);
	if (rt == RT_AF) {
		AF.reg &= 0xFFF0;
	}
	SP += 2;
}

uint8_t CPU::add_8b(uint8_t dest, uint8_t value, uint8_t carry /*= 0*/)
{
	u16 result = dest + value + carry;
	AF.F.Z = (result & 0xFF) == 0;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x10; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x100;
	return result;
}

uint16_t CPU::add_16bs(uint16_t dest, int8_t value)
{
	u32 result = dest + value;
	AF.F.Z = 0;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x10; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x100;

	return result;
}

uint8_t CPU::sub_8b(uint8_t dest, uint8_t value, uint8_t borrow /*= 0*/)
{
	u16 result = dest - value - borrow;
	AF.F.Z = (result & 0xFF) == 0;
	AF.F.N = 1;
	AF.F.H = (dest ^ value ^ result) & 0x10;
	AF.F.C = (dest ^ value ^ result) & 0x100;
	return result;
}

void CPU::ADD()
{
	if (curr_instruction->width == WIDTH_16) {
		HL.reg = add_16b(HL.reg, curr_data.fetched_data);
	}
	else {
		AF.A = add_8b(AF.A, curr_data.fetched_data);
	}
}

void CPU::ADC()
{
	AF.A = add_8b(AF.A, curr_data.fetched_data, AF.F.C);
}

void CPU::SUB()
{
	AF.A = sub_8b(AF.A, curr_data.fetched_data);
}

void CPU::SBC()
{
	AF.A = sub_8b(AF.A, curr_data.fetched_data, AF.F.C);
}

void CPU::AND()
{
	AF.A &= curr_data.fetched_data;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 1;
	AF.F.C = 0;
}

void CPU::OR()
{
	AF.A |= curr_data.fetched_data;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::XOR()
{
	AF.A ^= curr_data.fetched_data;
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::CP()
{
	u16 result = AF.A - curr_data.fetched_data;
	AF.F.Z = result == 0;
	AF.F.N = 1;
	AF.F.H = ((AF.A & 0x0F) - (curr_data.fetched_data & 0x0F) < 0);
	AF.F.C = AF.A < curr_data.fetched_data;
}

void CPU::INC()
{
	if (curr_instruction->width == WIDTH_16) {
		u16* nn = (u16*)get_reg_from_enum(curr_instruction->destination.reg);
		(*nn)++;
	}
	else {
		bool old_carry = AF.F.C;
		if (curr_data.dest_is_mem) {
			
			u8 n = read(curr_data.mem_dest);
			u8 res = add_8b(n, 1);
			write(curr_data.mem_dest, res);
		}
		else {
			u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
			*n = add_8b(*n, 1);
		}
		// Carry flag not affected!
		AF.F.C = old_carry;
	}
}

void CPU::DEC()
{
	if (curr_instruction->width == WIDTH_16) {
		u16* nn = (u16*)get_reg_from_enum(curr_instruction->destination.reg);
		(*nn)--;
	}
	else {
		bool old_carry = AF.F.C;
		if (curr_data.dest_is_mem) {

			u8 n = read(curr_data.mem_dest);
			u8 res = sub_8b(n, 1);
			write(curr_data.mem_dest, res);
		}
		else {
			u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
			*n = sub_8b(*n, 1);
		}
		// Carry flag not affected!
		AF.F.C = old_carry;
	}
}

u16 CPU::add_16b(u16 dest, u16 value)
{
	u32 result = dest + value;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x1000; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x10000;

	return result;
}

void CPU::ADD_SP()
{
	SP = add_16bs(SP, (int8_t)curr_data.fetched_data);
}

void CPU::SWAP()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
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
	IME_scheduled = false;
	IME = false;
}

void CPU::EI()
{
	IME_scheduled = true;
}

uint8_t CPU::rotate_left(uint8_t n)
{
	u8 result = (n << 1) | (n >> 7) & 0x01;
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

void CPU::RLC()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	*n = rotate_left(*n);
}

void CPU::RL()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	*n = rotate_left_carry(*n);
}

void CPU::RRC()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	*n = rotate_right(*n);
}

void CPU::RR()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	*n = rotate_right_carry(*n);
}

void CPU::SLA()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 result = *n << 1;
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x80; // Contains old bit 7 data.
	
	*n = result;
}

void CPU::SRA()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 result = (*n >> 1) | (*n & 0x80);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x1; // Contains old bit 0 data.
	*n = result;
}

void CPU::SRL()
{
	u8* n = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 result = (*n >> 1);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = *n & 0x1; // Contains old bit 0 data.
	*n = result;
}

void CPU::BIT()
{
	u8* r = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 bit = curr_instruction->param;
	AF.F.Z = !CHECK_BIT(*r, bit);
	AF.F.N = 0;
	AF.F.H = 1;
}

void CPU::SET()
{
	u8* r = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 bit = curr_instruction->param;
	*r |= (1 << bit);
}

void CPU::RES()
{
	u8* r = (u8*)get_reg_from_enum(curr_instruction->destination.reg);
	u8 bit = curr_instruction->param;
	*r &= ~(1 << bit);
}


void CPU::JP()
{
	cond_type cc = curr_instruction->condition;
	u16 addr = curr_data.fetched_data;
	if (check_condition(cc))
		PC = addr;
}

void CPU::JR()
{
	cond_type cc = curr_instruction->condition;
	int16_t offset = (int8_t)curr_data.fetched_data;
	if (check_condition(cc))
		PC += offset;
}

void CPU::CALL(u16 address)
{
	SP -= 2;
	write(SP, PC);
	PC = address;
}

void CPU::CALL()
{
	cond_type cc = curr_instruction->condition;
	u16 nn = curr_data.fetched_data;
	if (check_condition(cc)) { 
		CALL(nn);
	}
}

void CPU::RST()
{
	u16 n = curr_instruction->param;
	SP -= 2;
	write(SP, PC);
	PC = two8b_to_16b(0x00, n);
}

void CPU::RET()
{
	cond_type cc = curr_instruction->condition;
	if (check_condition(cc)) {
		PC = read_16b(SP);
		SP += 2;
	}
}

void CPU::RETI()
{
	RET();
	IME = true;
}