#pragma once
#include <cstdint>
#include "Bus.h"
#include <string>
#include <functional>
#include "instructions.h"

// http://www.codeslinger.co.uk/pages/projects/gameboy/files/GB.pdf

class CPU
{
public:
	CPU();
	~CPU();

	void connectBus(Bus* b) { bus = b; }

public:

	// Flag Register
	union FlagRegister {
		struct {
			bool Z : 1; // This bit is set when the result of a math operation is zero or two values match when using the CP instruction.
			bool N : 1; // This bit is set if a subtraction was performed in the last math instruction.
			bool H : 1; // This bit is set if a carry occurred from the lower nibble in the last math operation.
			bool C : 1; // This bit is set if a carry occurred from the last math operation or if register A is the smaller value when executing the CP instruction
			uint8_t unused : 4;

		};
		uint8_t reg;
	};

	// Registers
	union AF {
		struct {
			uint8_t A;
			FlagRegister F;
		};
		uint16_t reg;
	};
	union BC {
		struct {
			uint8_t B;
			uint8_t C;
		};
		uint16_t reg;
	};
	union DE {
		struct {
			uint8_t D;
			uint8_t E;
		};
		uint16_t reg;
	};
	union HL {
		struct {
			uint8_t H;
			uint8_t L;
		};
		uint16_t reg;
	};

	uint16_t SP, PC; // Stack and Program Counter
	AF AF;
	BC BC;
	DE DE;
	HL HL;

	// Opcodes
	/*
	void LD_INM(uint8_t* r1, uint8_t immediate);
	void LD_R1R2(uint8_t* r1, uint8_t* r2);
	void LD_A(uint8_t data);
	void LD_n_with_A(uint8_t* n); // n = A,B,C,D,E,H,L,(BC),(DE),(HL),  [(nn) = two byte immediate value. (LS byte first.)]
	void LD_AC(); // Put value at address $FF00 + register C into A. Same as : LD A, ($FF00 + C)
	void LD_CA(); // Put A into address $FF00 + register C.
	void LDD_A_HL(); // Put value at address HL into A. Decrement HL.	Same as: LD A,(HL) - DEC HL
	void LDD_HL_A(); // Put A into memory address HL. Decrement HL.		Same as: LD (HL),A - DEC HL
	void LDI_A_HL(); // Put value at address HL into A. Increment HL.	Same as: LD A,(HL) - INC HL
	void LDI_HL_A(); // Put A into memory address HL. Increment HL.		Same as: LD (HL),A - INC HL
	*/

	// 8-bit load
	void LD(uint8_t* dest, uint8_t value);

	// 16-bit load
	void LD(uint16_t* dest, uint16_t value);
	void PUSH(uint16_t value);
	void POP(uint16_t* dest);

	// 8-bit ALU
	uint8_t add_8b(uint8_t dest, uint8_t value);
	uint8_t sub_8b(uint8_t dest, uint8_t value);

	// Add value to A
	void ADD(uint8_t value); 
	// Add value + Carry flag to A
	void ADC(uint8_t value); 
	// Subtract value from A
	void SUB(uint8_t value); 
	// Subtract value + Carry flag from A.
	void SBC(uint8_t value); 
	// Logically AND value with A, result in A.
	void AND(uint8_t value); 
	// Logical OR value with register A, result in A.
	void OR(uint8_t value);
	// Logical exclusive OR value with register A, result in A.
	void XOR(uint8_t value);
	// Compare A with value. This is basically an A - value subtraction instruction but the results are thrown away.
	void CP(uint8_t value); 
	// Increment register n.
	void INC(uint8_t* n); 
	// Decrement register n.
	void DEC(uint8_t* n); 

	// 16-bit Arithmetic
	
	// Add value to HL.
	void ADD(uint16_t value); 
	// Add one byte signed immediate value to SP.
	void ADD_SP(int8_t signed_value); 
	// Increment register nn.
	void INC(uint16_t* nn); 
	// Decrement register nn.
	void DEC(uint16_t* nn); 

	// Miscellaneous
	
	// Swap upper & lower nibles of n.
	void SWAP(uint8_t* n); 
	// Decimal adjust register A. This instruction adjusts register A so that the correct representation of Binary Coded Decimal(BCD) is obtained.
	void DAA(); 
	// Complement A register. (Flip all bits.)
	void CPL(); 
	// Complement carry flag. If C flag is set, then reset it. If C flag is reset, then set it.
	void CCF(); 
	// Set Carry flag.
	void SCF(); 
	// No operation.
	void NOP();
	// Power down CPU until an interrupt occurs. Use this when ever possible to reduce energy consumption.
	void HALT(); 
	// Halt CPU & LCD display until button pressed.
	void STOP(); 
	// This instruction disables interrupts but not immediately. Interrupts are disabled after instruction after DI is executed.
	void DI(); 
	// Enable interrupts. This intruction enables interrupts but not immediately.Interrupts are enabled after instruction after EI is executed.
	void EI(); 

	// Rotates & Shifts

	// general rotation left function
	uint8_t rotate_left(uint8_t n);
	// general rotation left carry function
	uint8_t rotate_left_carry(uint8_t n);
	// general rotation right function
	uint8_t rotate_right(uint8_t n);
	// general rotation right carry function
	uint8_t rotate_right_carry(uint8_t n);

	// Rotate A left. Old bit 7 to Carry flag.
	void RLCA(); 
	// Rotate A left through Carry flag.
	void RLA();
	// Rotate A right. Old bit 0 to Carry flag.
	void RRCA(); 
	// Rotate A right through Carry flag.
	void RRA(); 
	// Rotate n left. Old bit 7 to Carry flag.
	void RLC(uint8_t* n);
	// Rotate n left through Carry flag.
	void RL(uint8_t* n); 
	// Rotate n right. Old bit 0 to Carry flag.
	void RRC(uint8_t* n); 
	// Rotate n right through Carry flag.
	void RR(uint8_t* n); 
	// Shift n left into Carry. LSB of n set to 0.
	void SLA(uint8_t* n); 
	// Shift n right into Carry. MSB doesn't change.
	void SRA(uint8_t* n); 
	// Shift n right into Carry. MSB set to 0.
	void SRL(uint8_t* n); 

	// Bit Opcodes
	void BIT(uint8_t bit, uint8_t* r); // Test bit b in register r.
	void SET(uint8_t bit, uint8_t* r); // Set bit b in register r.
	void RES(uint8_t bit, uint8_t* r); // Reset bit b in register r.

	// Jumps

	// Jump to address nn. two byte immediate value. (LS byte first.)
	void JP(uint16_t addr); 
	// Jump to address n if cc condition is true.
	void JP(cond_type cc, uint16_t addr); 
	// Add n to current address and jump to it.
	void JR(uint8_t addr); 
	// If following condition is true then add n to current address and jump to it
	void JR(cond_type cc, uint8_t addr);

	// Calls

	// Push address of next instruction onto stack and then jump to address nn. two byte immediate value. (LS byte first.)
	void CALL(uint16_t nn); 
	// Call address nn if following condition is true. two byte immediate value. (LS byte first.)
	void CALL(cond_type cc, uint16_t nn);

	// Restarts

	// Push present address onto stack. Jump to address $0000 + n. n = $00,$08,$10,$18,$20,$28,$30,$38
	void RST(uint8_t n); 

	// Returns

	// Pop two bytes from stack & jump to that address.
	void RET(); 
	// Return if cc condition is true.
	void RET(cond_type cc); 
	// Pop two bytes from stack & jump to that address then enable interrupts.
	void RETI(); 




	void clock();
	void fetch_instr();
	void fetch_data();

	uint8_t opcode = 0x00;
	instruction* curr_instruction;

private:
	Bus* bus = nullptr;
	void write(uint16_t addr, uint8_t data);
	void write(uint16_t addr, uint16_t data);
	uint8_t read(uint16_t addr);
	uint16_t read_16b(uint16_t addr);

	bool check_condition(cond_type cc);

	struct INSTRUCTION {
		std::string name;
		std::function<void()> function;
	};

	std::array<INSTRUCTION, 2> lookup = { {"NOP", } };
};

