#pragma once

#include "common.h"

enum reg_type {
    RT_NONE,
    RT_A,
    RT_F,
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_AF,
    RT_BC,
    RT_DE,
    RT_HL,
    RT_SP,
    RT_PC
};
enum in_type {
    IN_NONE,
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_RLCA,
    IN_ADD,
    IN_ADD_SP,
    IN_RRCA,
    IN_STOP,
    IN_RLA,
    IN_JR,
    IN_RRA,
    IN_DAA,
    IN_CPL,
    IN_SCF,
    IN_CCF,
    IN_HALT,
    IN_ADC,
    IN_SUB,
    IN_SBC,
    IN_AND,
    IN_XOR,
    IN_OR,
    IN_CP,
    IN_POP,
    IN_JP,
    IN_PUSH,
    IN_RET,
    //IN_CB,
    IN_CALL,
    IN_RETI,
    IN_LDH,
    IN_JPHL,
    IN_DI,
    IN_EI,
    IN_RST,
    IN_ERR,
    //CB instructions...
    IN_RLC,
    IN_RRC,
    IN_RL,
    IN_RR,
    IN_SLA,
    IN_SRA,
    IN_SWAP,
    IN_SRL,
    IN_BIT,
    IN_RES,
    IN_SET
};

enum cond_type {
    CT_NONE,
    CT_NZ,
    CT_Z,
    CT_NC,
    CT_C
};

enum operand_type {
    OT_NONE,   // No operand
    OT_R,      // Register value
    OT_D8,     // 8-bit immediate data
    OT_D16,    // 16-bit immediate data
    OT_MR,     // Memory addressed by a 16-bit register
    OT_MC,     // Memory addressed by 0xFF00 + register C
    OT_A8,     // Memory addressed by 0xFF00 + 8-bit immediate
    OT_A16,    // Memory addressed by a 16-bit immediate
    OT_HLI,    // Memory addressed by HL, then increments HL
    OT_HLD,    // Memory addressed by HL, then decrements HL
    OT_SPR8    // SP plus a signed 8-bit immediate
};

struct operand {
    operand_type type;
    reg_type reg;
};

enum operand_width {
    WIDTH_NONE, // No transferred or operated data
	WIDTH_8, // 8-bit data
	WIDTH_16 // 16-bit data
};

struct instruction {
	in_type type;
	operand destination;
	operand source;
	operand_width width;
	cond_type condition;
	u8 param;
	u8 cycles;
	u8 cycles_if_taken;
};


const instruction* instruction_by_opcode(u8 opcode);
const instruction* cb_instruction_by_opcode(u8 opcode);

char* inst_name(in_type t);