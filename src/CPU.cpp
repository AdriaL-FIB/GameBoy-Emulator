#include "CPU.h"
#include "Bus.h"

constexpr u16 INTERRUPT_VECTORS[5] = {
	0x0040, // VBlank
	0x0048, // LCD STAT
	0x0050, // Timer
	0x0058, // Serial
	0x0060  // Joypad
};

CPU::CPU() :
	curr_instruction{}
{
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

u8 CPU::step_instruction()
{
	// Interruptions
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


	opcode = read(PC++);

	if (opcode == 0xCB) {
		opcode = read(PC++);
		curr_instruction = cb_instruction_by_opcode(opcode);
	}
	else {
		curr_instruction = instruction_by_opcode(opcode);
	}

	bool action_taken = check_condition(curr_instruction->condition);

	execute_instr();

	post_process();

	if (action_taken) return curr_instruction->cycles_if_taken;
	return curr_instruction->cycles;
}

void CPU::execute_instr()
{
	switch (curr_instruction->type)
	{
	case IN_NONE:
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

u8 CPU::read_reg8(reg_type reg) const
{
	switch (reg)
	{
	case RT_A:
		return AF.A;
	case RT_F:
		return AF.F.reg;
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
	default:
		return 0x00;
	}
}

void CPU::write_reg8(reg_type reg, u8 data)
{
	switch (reg)
	{
	case RT_A:
		AF.A = data;
		break;
	case RT_F:
		AF.F.reg = data & 0xF0;
		break;
	case RT_B:
		BC.B = data;
		break;
	case RT_C:
		BC.C = data;
		break;
	case RT_D:
		DE.D = data;
		break;
	case RT_E:
		DE.E = data;
		break;
	case RT_H:
		HL.H = data;
		break;
	case RT_L:
		HL.L = data;
		break;
	}
}

u16 CPU::read_reg16(reg_type reg) const
{
	switch (reg)
	{
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
	default:
		return 0x00;
	}
}

void CPU::write_reg16(reg_type reg, u16 data)
{
	switch (reg)
	{
	case RT_AF:
		AF.reg = data & 0xFFF0;
		break;
	case RT_BC:
		BC.reg = data;
		break;
	case RT_DE:
		DE.reg = data;
		break;
	case RT_HL:
		HL.reg = data;
		break;
	case RT_SP:
		SP = data;
		break;
	case RT_PC:
		PC = data;
		break;
	}
}

u8 CPU::read_operand8(const operand& op)
{
	switch (op.type)
	{
	case OT_R:
		return read_reg8(op.reg);
	case OT_D8:
		return read(PC++);
	case OT_HLD:
	case OT_HLI:
	case OT_MR:
		return read(read_reg16(op.reg));
	case OT_MC:
		return read(0xFF00 | read_reg8(op.reg));
	case OT_A8:
		return read(0xFF00 | read(PC++));
	case OT_A16:
	{
		const u16 addr = read_16b(PC);
		PC += 2;
		return read(addr);
	}
	default:
	case OT_NONE:
		return 0x00;
	}
}

u16 CPU::read_operand16(const operand& op)
{
	switch (op.type)
	{
	case OT_R:
		return read_reg16(op.reg);
	case OT_D16:
	{
		const u16 value = read_16b(PC);
		PC += 2;
		return value;
	}
	case OT_SPR8:
	{
		int8_t r8 = (int8_t)read(PC++);
		return add_16bs(SP, r8);
	}
	default:
		return 0x0000;
	}
}

void CPU::write_operand8(const operand& op, u8 data)
{
	switch (op.type)
	{
	case OT_R:
		write_reg8(op.reg, data);
		break;
	case OT_HLD:
	case OT_HLI:
	case OT_MR:
		write(read_reg16(op.reg), data);
		break;
	case OT_MC:
		write(0xFF00 | read_reg8(op.reg), data);
		break;
	case OT_A8:
		write(0xFF00 | read(PC++), data);
		break;
	case OT_A16:
	{
		const u16 addr = read_16b(PC);
		PC += 2;
		write(addr, data);
		break;
	}
	}
}

void CPU::write_operand16(const operand& op, u16 data)
{
	switch (op.type)
	{
	case OT_R:
		write_reg16(op.reg, data);
		break;
	case OT_A16:
	{
		const u16 addr = read_16b(PC);
		PC += 2;
		write(addr, data);
		break;
	}
	}
}

void CPU::write(u16 addr, u8 data) {
	bus->write(addr, data);
}

void CPU::write(u16 addr, u16 data)
{
	u8 h = data >> 8;
	u8 l = data & 0xFF;
	bus->write(addr, l);
	bus->write(addr+1, h);
}

u8 CPU::read(u16 addr) {
	return bus->read(addr);
}

u16 CPU::read_16b(u16 addr)
{
	u8 l = bus->read(addr);
	u8 h = bus->read(addr + 1);
	return two8b_to_16b(h, l);
}


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
	if (curr_instruction->width == WIDTH_16)
	{
		u16 value = read_operand16(curr_instruction->source);
		write_operand16(curr_instruction->destination, value);
	}
	else
	{
		u8 value = read_operand8(curr_instruction->source);
		write_operand8(curr_instruction->destination, value);
	}
}

void CPU::PUSH()
{
	u16 value = read_operand16(curr_instruction->source);
	SP -= 2;
	write(SP, value);
}

void CPU::POP()
{
	u16 value = read_16b(SP);
	SP += 2;
	write_operand16(curr_instruction->destination, value);
}

uint8_t CPU::add_8b(uint8_t dest, uint8_t value, uint8_t carry /*= 0*/)
{
	u16 result = dest + value + carry;
	AF.F.Z = (result & 0xFF) == 0;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x10; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x100;
	return (u8)result;
}

u16 CPU::add_16b(u16 dest, u16 value)
{
	u32 result = dest + value;
	AF.F.N = 0;
	AF.F.H = (dest ^ value ^ result) & 0x1000; // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
	AF.F.C = (dest ^ value ^ result) & 0x10000;

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
	return (u8)result;
}

void CPU::ADD()
{
	if (curr_instruction->width == WIDTH_16) {
		HL.reg = add_16b(HL.reg, read_operand16(curr_instruction->source));
	}
	else {
		AF.A = add_8b(AF.A, read_operand8(curr_instruction->source));
	}
}

void CPU::ADC()
{
	AF.A = add_8b(AF.A, read_operand8(curr_instruction->source), AF.F.C);
}

void CPU::SUB()
{
	AF.A = sub_8b(AF.A, read_operand8(curr_instruction->source));
}

void CPU::SBC()
{
	AF.A = sub_8b(AF.A, read_operand8(curr_instruction->source), AF.F.C);
}

void CPU::AND()
{
	AF.A &= read_operand8(curr_instruction->source);
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 1;
	AF.F.C = 0;
}

void CPU::OR()
{
	AF.A |= read_operand8(curr_instruction->source);
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::XOR()
{
	AF.A ^= read_operand8(curr_instruction->source);
	AF.F.Z = AF.A == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

void CPU::CP()
{
	u16 value = read_operand8(curr_instruction->source);
	sub_8b(AF.A, value);
}

void CPU::INC()
{
	const operand& op_dest = curr_instruction->destination;
	if (curr_instruction->width == WIDTH_16) {
		write_operand16(op_dest, read_operand16(op_dest) + 1);
	}
	else {
		bool old_carry = AF.F.C;
		write_operand8(op_dest, add_8b(read_operand8(op_dest), 1));
		// Carry flag not affected!
		AF.F.C = old_carry;
	}
}

void CPU::DEC()
{
	const operand& op_dest = curr_instruction->destination;
	if (curr_instruction->width == WIDTH_16) {
		write_operand16(op_dest, read_operand16(op_dest) - 1);
	}
	else {
		bool old_carry = AF.F.C;
		write_operand8(op_dest, sub_8b(read_operand8(op_dest), 1));
		// Carry flag not affected!
		AF.F.C = old_carry;
	}
}

void CPU::ADD_SP()
{
	SP = add_16bs(SP, (int8_t)read_operand8(curr_instruction->source));
}

void CPU::SWAP()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	u8 h = n & 0xF0;
	u8 l = n & 0x0F;

	write_operand8(op_dest, l << 4 | h >> 4);

	AF.F.Z = n == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = 0;
}

// https://blog.ollien.com/posts/gb-daa/
void CPU::DAA()
{
	u8 offset = 0x00;
	if ((not AF.F.N and (AF.A & 0xF) > 0x09) or AF.F.H)
	{
		offset |= 0x06;
	}
	if ((not AF.F.N and AF.A > 0x99) or AF.F.C)
	{
		offset |= 0x60;
		AF.F.C = 1;
	}

	if (AF.F.N) AF.A -= offset;
	else AF.A += offset;

	AF.F.Z = AF.A == 0;
	AF.F.H = 0;
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

void CPU::NOP(){}

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

u8 CPU::rotate_left(u8 n)
{
	u8 result = (n << 1) | (n >> 7) & 0x01;
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x80; // Contains old bit 7 data.
	return result;
}

u8 CPU::rotate_left_carry(u8 n)
{
	u8 result = (n << 1) | u8(AF.F.C);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x80; // Contains old bit 7 data.
	return result;
}

u8 CPU::rotate_right(u8 n)
{
	u8 result = (n >> 1) | ((n & 0x01) << 7);
	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x1; // Contains old bit 0 data.
	return result;
}

u8 CPU::rotate_right_carry(u8 n)
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
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	n = rotate_left(n);
	write_operand8(op_dest, n);
}

void CPU::RL()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	n = rotate_left_carry(n);
	write_operand8(op_dest, n);
}

void CPU::RRC()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	n = rotate_right(n);
	write_operand8(op_dest, n);
}

void CPU::RR()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	n = rotate_right_carry(n);
	write_operand8(op_dest, n);
}

void CPU::SLA()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	u8 result = n << 1;
	write_operand8(op_dest, result);

	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x80; // Contains old bit 7 data.

}

void CPU::SRA()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	u8 result = (n >> 1) | (n & 0x80);
	write_operand8(op_dest, result);

	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x1; // Contains old bit 0 data.
}

void CPU::SRL()
{
	const operand& op_dest = curr_instruction->destination;
	u8 n = read_operand8(op_dest);
	u8 result = (n >> 1);
	write_operand8(op_dest, result);

	AF.F.Z = result == 0;
	AF.F.N = 0;
	AF.F.H = 0;
	AF.F.C = n & 0x1; // Contains old bit 0 data.
	n = result;
}

void CPU::BIT()
{
	const operand& op_dest = curr_instruction->destination;
	u8 r = read_operand8(op_dest);
	u8 bit = curr_instruction->param;
	AF.F.Z = not CHECK_BIT(r, bit);
	AF.F.N = 0;
	AF.F.H = 1;
}

void CPU::SET()
{
	const operand& op_dest = curr_instruction->destination;
	u8 r = read_operand8(op_dest);
	u8 bit = curr_instruction->param;
	r |= (1 << bit);
	write_operand8(op_dest, r);
}

void CPU::RES()
{
	const operand& op_dest = curr_instruction->destination;
	u8 r = read_operand8(op_dest);
	u8 bit = curr_instruction->param;
	r &= ~(1 << bit);
	write_operand8(op_dest, r);
}


void CPU::JP()
{
	cond_type cc = curr_instruction->condition;
	u16 addr = read_operand16(curr_instruction->source);
	if (check_condition(cc))
		PC = addr;
}

void CPU::JR()
{
	cond_type cc = curr_instruction->condition;
	int8_t offset = (int8_t)read_operand8(curr_instruction->source);
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
	u16 nn = read_operand16(curr_instruction->source);
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