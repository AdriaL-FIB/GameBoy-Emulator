#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include "instructions.h"

// http://www.codeslinger.co.uk/pages/projects/gameboy/files/GB.pdf

class Bus;

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
			uint8_t unused : 4;
			bool C : 1; // This bit is set if a carry occurred from the last math operation or if register A is the smaller value when executing the CP instruction
			bool H : 1; // This bit is set if a carry occurred from the lower nibble in the last math operation.
			bool N : 1; // This bit is set if a subtraction was performed in the last math instruction.
			bool Z : 1; // This bit is set when the result of a math operation is zero or two values match when using the CP instruction.

		};
		uint8_t reg;
	};

	// Registers
	union AF {
		struct {
			FlagRegister F;
			uint8_t A;
		};
		uint16_t reg;
	};
	union BC {
		struct {
			uint8_t C;
			uint8_t B;
		};
		uint16_t reg;
	};
	union DE {
		struct {
			uint8_t E;
			uint8_t D;
		};
		uint16_t reg;
	};
	union HL {
		struct {
			uint8_t L;
			uint8_t H;
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

	// 8-bit and 16-bit load
	void LD();

	// 16-bit load
	void PUSH();
	void POP();

	// 8-bit ALU
	uint8_t add_8b(uint8_t dest, uint8_t value, uint8_t carry = 0);
	uint16_t add_16bs(uint16_t dest, int8_t value);
	uint8_t sub_8b(uint8_t dest, uint8_t value, uint8_t borrow = 0);

	// 8-bit: Add value to A. 16-bit: Add value to HL.
	void ADD(); 
	// Add value + Carry flag to A
	void ADC(); 
	// Subtract value from A
	void SUB(); 
	// Subtract value + Carry flag from A.
	void SBC(); 
	// Logically AND value with A, result in A.
	void AND(); 
	// Logical OR value with register A, result in A.
	void OR();
	// Logical exclusive OR value with register A, result in A.
	void XOR();
	// Compare A with value. This is basically an A - value subtraction instruction but the results are thrown away.
	void CP(); 
	// Increment register n or nn
	void INC(); 
	// Decrement register n or nn
	void DEC(); 

	// 16-bit Arithmetic
	
	u16 add_16b(u16 dest, u16 value);

	// Add one byte signed immediate value to SP.
	void ADD_SP(); 

	// Miscellaneous
	
	// Swap upper & lower nibbles of register n.
	void SWAP(); 
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
	// Enable interrupts. This instruction enables interrupts but not immediately.Interrupts are enabled after instruction after EI is executed.
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
	void RLC();
	// Rotate n left through Carry flag.
	void RL(); 
	// Rotate n right. Old bit 0 to Carry flag.
	void RRC(); 
	// Rotate n right through Carry flag.
	void RR(); 
	// Shift n left into Carry. LSB of n set to 0.
	void SLA(); 
	// Shift n right into Carry. MSB doesn't change.
	void SRA(); 
	// Shift n right into Carry. MSB set to 0.
	void SRL(); 

	// Bit Opcodes
	void BIT(); // Test bit b in register r.
	void SET(); // Set bit b in register r.
	void RES(); // Reset bit b in register r.

	// Jumps

	// Jump to address nn. two byte immediate value. (LS byte first.)
	void JP(); 
	// If following condition is true then add n to current address and jump to it
	void JR(); 

	// Calls

	void CALL(u16 address);

	// Push address of next instruction onto stack and then jump to address nn. two byte immediate value. (LS byte first.)
	// Call address nn if following condition is true. two byte immediate value. (LS byte first.)
	void CALL();

	// Restarts

	// Push present address onto stack. Jump to address $0000 + n. n = $00,$08,$10,$18,$20,$28,$30,$38
	void RST(); 

	// Returns

	// Pop two bytes from stack & jump to that address.
	void RET(); 
	// Pop two bytes from stack & jump to that address then enable interrupts.
	void RETI(); 




	u8 clock();


private:
	struct data {
		u16 fetched_data;
		u16 mem_dest;
		bool dest_is_mem;
	};
	data curr_data;

	void fetch_data();
	void execute_instr();
	void post_process();
	uint8_t opcode = 0x00;
	const instruction* curr_instruction;
	Bus* bus = nullptr;

	void* get_reg_from_enum(reg_type reg);
	u16 read_reg_from_enum(reg_type reg) const;
	//bool is_reg_16_bit(reg_type reg) const;

	//uint16_t* get_reg16_from_enum(reg_type reg);

	void write(uint16_t addr, uint8_t data);
	void write(uint16_t addr, uint16_t data);
	uint8_t read(uint16_t addr);
	uint16_t read_16b(uint16_t addr);

	// Converts 16-bit address fake ram to system address
	//uint8_t* get_addr(uint16_t addr);

	bool check_condition(cond_type cc) const;

	// Interrupts
	bool IME = false; // Interrupts Master Enable
	bool IME_scheduled = false;

};

