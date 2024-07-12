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


void* CPU::get_reg_from_enum(reg_type reg)
{
	switch (reg)
	{
	case RT_NONE:
		return nullptr;
	case RT_A:
		return &AF.A;
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
	case RT_NONE:
		return 0;
	case RT_A:
		return AF.A;
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

bool CPU::is_reg_16_bit(reg_type reg) const
{
	return reg >= RT_AF;
}

void CPU::fetch_data() {
	switch (curr_instruction->mode)
	{
	case AM_IMP:
		break;

	case AM_R_A16:
		u16 addr = read_16b(PC); PC += 2;
		curr_data.fetched_data = read(addr);
		break;

	case AM_D16:
	case AM_R_D16:
		curr_data.fetched_data = read_16b(PC); PC += 2;
		break;

	case AM_MR_R:
		if (is_reg_16_bit(curr_instruction->reg_2)) {
			curr_data.mem_dest = read_reg_from_enum(curr_instruction->reg_1);
		}
		else {
			curr_data.mem_dest = 0xFF00 | read(curr_instruction->reg_1);
		}
		curr_data.dest_is_mem = true;
	case AM_R_R:
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_2);
		break;

	case AM_R:
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_1);
		break;

	case AM_R_D8:
		curr_data.fetched_data = read(PC++);
		break;

	case AM_R_MR:
		if (is_reg_16_bit(curr_instruction->reg_2)) {
			curr_data.fetched_data = read_16b(curr_instruction->reg_2);
		}
		else {
			curr_data.fetched_data = 0xFF00 | read(curr_instruction->reg_2);
		}
		break;

	case AM_R_HLI:
		u16 addr = read_reg_from_enum(curr_instruction->reg_2);
		curr_data.fetched_data = read_16b(addr);
		HL.reg++;
		break;

	case AM_R_HLD:
		u16 addr = read_reg_from_enum(curr_instruction->reg_2);
		curr_data.fetched_data = read_16b(addr);
		HL.reg--;
		break;

	case AM_HLI_R:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_reg_from_enum(curr_instruction->reg_1);
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_2);
		HL.reg++;
		break;

	case AM_HLD_R:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_reg_from_enum(curr_instruction->reg_1);
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_2);
		HL.reg--;
		break;

	case AM_R_A8:
		curr_data.fetched_data = 0xFF00 | read(PC++);
		break;

	case AM_A8_R:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = 0xFF00 | read(PC++);
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_2);
		break;

	case AM_HL_SPR:
		curr_data.fetched_data = read_16b(SP + read(PC++));
		break;

	case AM_D8:
		curr_data.fetched_data = read(PC++);
		break;

	case AM_D16_R:
	case AM_A16_R:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_16b(PC); PC += 2;
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_2);
		break;

	case AM_MR_D8:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_reg_from_enum(curr_instruction->reg_1);
		curr_data.fetched_data = read(PC++);
		break;

	case AM_MR:
		curr_data.dest_is_mem = true;
		curr_data.mem_dest = read_reg_from_enum(curr_instruction->reg_1);
		curr_data.fetched_data = read_reg_from_enum(curr_instruction->reg_1);
		break;
	}
}

void CPU::fetch_instr()
{
	switch (curr_instruction->type)
	{
	case IN_NONE:
		return;
	case IN_NOP:
		return;
	case IN_LD:
		u16 value = curr_data.fetched_data;
		if (curr_data.dest_is_mem) {
			if (is_reg_16_bit(curr_instruction->reg_2)) {
				write(curr_data.mem_dest, value);
			}
			else {
				write(curr_data.mem_dest, (u8)value);
			}
		}
		else {
			void* dest = get_reg_from_enum(curr_instruction->reg_1);
			if (is_reg_16_bit(curr_instruction->reg_1)) {
				*((u16*)dest) = value;
			}
			else {
				*((u8*)dest) = value;
			}
		}
		return;
	case IN_INC:
		if (curr_data.dest_is_mem) {
			u8 val = add_8b(read(curr_data.mem_dest), 1);
			// Carry flag not affected!
			AF.F.C = 0;
			write(curr_data.mem_dest, val);
			return;
		}
		
		if (is_reg_16_bit(curr_instruction->reg_1)) {
			INC((u16*)get_reg_from_enum(curr_instruction->reg_1));
		}
		else {
			INC((u8*)get_reg_from_enum(curr_instruction->reg_1));
		}
		return;
	case IN_DEC:
		if (curr_data.dest_is_mem) {
			u8 val = sub_8b(read(curr_data.mem_dest), 1);
			// Carry flag not affected!
			AF.F.C = 0;
			write(curr_data.mem_dest, val);
			return;
		}

		if (is_reg_16_bit(curr_instruction->reg_1)) {
			DEC((u16*)get_reg_from_enum(curr_instruction->reg_1));
		}
		else {
			DEC((u8*)get_reg_from_enum(curr_instruction->reg_1));
		}
		return;
	case IN_RLCA:
		RLCA();
		return;
	case IN_ADD:
		if (is_reg_16_bit(curr_instruction->reg_1)) {
			ADD(read_reg_from_enum(curr_instruction->reg_2));
		}
		else {
			ADD((u8)read_reg_from_enum(curr_instruction->reg_2));
		}
		return;
	case IN_ADD_SP:
		ADD_SP((int8_t)curr_data.fetched_data);
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
		//if (curr_instruction->cond == CT_NONE) {
		//	JR(curr_data.fetched_data);
		//	return;
		//}
		JR(curr_instruction->cond, curr_data.fetched_data);
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
		ADC(curr_data.fetched_data);
		return;
	case IN_SUB:
		SUB(curr_data.fetched_data);
		return;
	case IN_SBC:
		SBC(curr_data.fetched_data);
		return;
	case IN_AND:
		AND(curr_data.fetched_data);
		return;
	case IN_XOR:
		XOR(curr_data.fetched_data);
		return;
	case IN_OR:
		OR(curr_data.fetched_data);
		return;
	case IN_CP:
		CP(curr_data.fetched_data);
		return;
	case IN_POP:
		POP((u16*)get_reg_from_enum(curr_instruction->reg_1));
		return;
	case IN_JP:
		//if (curr_instruction->cond == CT_NONE) {
		//	JP(curr_data.fetched_data);
		//	return;
		//}
		JP(curr_instruction->cond, curr_data.fetched_data);
		return;
	case IN_PUSH:
		PUSH(curr_data.fetched_data);
		return;
	case IN_RET:
		RET();
		return;
	case IN_CB:
		// 
		return;
	case IN_CALL:
		CALL(curr_instruction->cond, curr_data.fetched_data);
		return;
	case IN_RETI:
		RETI();
		return;
	case IN_LDH:
		if (curr_data.dest_is_mem) {
			write(curr_data.mem_dest, curr_data.fetched_data);
		}
		else {
			void* dest = get_reg_from_enum(curr_instruction->reg_1);
			*((u8*)dest) = curr_data.fetched_data;
		}
		return;
	case IN_JPHL:
		NO_IMPL;
		return;
	case IN_DI:
		DI();
		return;
	case IN_EI:
		EI();
		return;
	case IN_RST:
		RST(curr_instruction->param);
		return;
	case IN_ERR:
		// ?
		return;
	case IN_RLC:
		return;
	case IN_RRC:
		return;
	case IN_RL:
		return;
	case IN_RR:
		return;
	case IN_SLA:
		return;
	case IN_SRA:
		return;
	case IN_SWAP:
		return;
	case IN_SRL:
		return;
	case IN_BIT:
		return;
	case IN_RES:
		return;
	case IN_SET:
		return;
	default:
		return;
	}
}

void CPU::clock()
{
	opcode = read(PC++);

	curr_instruction = instruction_by_opcode(opcode);

	fetch_data();

	fetch_instr();

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
	case CT_NONE:
		return true;
	}
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
	bool aux = AF.F.C;
	*n = add_8b(*n, 1);
	// Carry flag not affected!
	AF.F.C = aux;
}

void CPU::DEC(uint8_t* n)
{
	bool aux = AF.F.C;
	*n = sub_8b(*n, 1);
	// Carry flag not affected!
	AF.F.C = aux;

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