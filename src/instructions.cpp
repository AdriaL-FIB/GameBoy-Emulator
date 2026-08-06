#include "instructions.h"

namespace {
    constexpr operand op(operand_type type = OT_NONE, reg_type reg = RT_NONE) {
        return {type, reg};
    }

    constexpr instruction instr(
        in_type type = IN_NONE,
        u8 cycles = 0,
        operand destination = op(),
        operand source = op(),
        operand_width width = WIDTH_NONE,
        cond_type condition = CT_NONE,
        u8 param = 0) {
        return {
            type,
            destination,
            source,
            width,
            condition,
            param,
            cycles,
            cycles
        };
    }

    constexpr instruction instr_cond(
        in_type type,
        u8 cycles,
        u8 cycles_if_taken,
        operand destination = op(),
        operand source = op(),
        operand_width width = WIDTH_NONE,
        cond_type condition = CT_NONE,
        u8 param = 0) {
        return {
            type,
            destination,
            source,
            width,
            condition,
            param,
            cycles,
            cycles_if_taken
        };
    }
}

constexpr instruction instructions[] = {
    instr(IN_NOP, 4),                                                   // 0x00 NOP
    instr(IN_LD, 12,   op(OT_R, RT_BC), op(OT_D16),       WIDTH_16),    // 0x01 LD BC,d16
    instr(IN_LD, 8,   op(OT_MR, RT_BC), op(OT_R, RT_A),  WIDTH_8),     // 0x02 LD (BC),A
    instr(IN_INC, 8,  op(OT_R, RT_BC),                     op(), WIDTH_16), // 0x03 INC BC
    instr(IN_INC, 4,  op(OT_R, RT_B),                      op(), WIDTH_8),  // 0x04 INC B
    instr(IN_DEC, 4,  op(OT_R, RT_B),                      op(), WIDTH_8),  // 0x05 DEC B
    instr(IN_LD, 8,   op(OT_R, RT_B),  op(OT_D8),        WIDTH_8),     // 0x06 LD B,d8
    instr(IN_RLCA, 4, op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x07 RLCA
    instr(IN_LD, 20,   op(OT_A16),       op(OT_R, RT_SP),  WIDTH_16),    // 0x08 LD (a16),SP
    instr(IN_ADD, 8,  op(OT_R, RT_HL), op(OT_R, RT_BC),  WIDTH_16),    // 0x09 ADD HL,BC
    instr(IN_LD, 8,   op(OT_R, RT_A),  op(OT_MR, RT_BC), WIDTH_8),     // 0x0A LD A,(BC)
    instr(IN_DEC, 8,  op(OT_R, RT_BC),                     op(), WIDTH_16), // 0x0B DEC BC
    instr(IN_INC, 4,  op(OT_R, RT_C),                      op(), WIDTH_8),  // 0x0C INC C
    instr(IN_DEC, 4,  op(OT_R, RT_C),                      op(), WIDTH_8),  // 0x0D DEC C
    instr(IN_LD, 8,   op(OT_R, RT_C),  op(OT_D8),        WIDTH_8),     // 0x0E LD C,d8
    instr(IN_RRCA, 4, op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x0F RRCA

    instr(IN_STOP, 4, op(),             op(OT_D8),        WIDTH_8),     // 0x10 STOP n8
    instr(IN_LD, 12,   op(OT_R, RT_DE), op(OT_D16),       WIDTH_16),    // 0x11 LD DE,d16
    instr(IN_LD, 8,   op(OT_MR, RT_DE), op(OT_R, RT_A),  WIDTH_8),     // 0x12 LD (DE),A
    instr(IN_INC, 8,  op(OT_R, RT_DE),                     op(), WIDTH_16), // 0x13 INC DE
    instr(IN_INC, 4,  op(OT_R, RT_D),                      op(), WIDTH_8),  // 0x14 INC D
    instr(IN_DEC, 4,  op(OT_R, RT_D),                      op(), WIDTH_8),  // 0x15 DEC D
    instr(IN_LD, 8,   op(OT_R, RT_D),  op(OT_D8),        WIDTH_8),     // 0x16 LD D,d8
    instr(IN_RLA, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x17 RLA
    instr(IN_JR, 12,   op(),             op(OT_D8),        WIDTH_8),     // 0x18 JR r8
    instr(IN_ADD, 8,  op(OT_R, RT_HL), op(OT_R, RT_DE),  WIDTH_16),    // 0x19 ADD HL,DE
    instr(IN_LD, 8,   op(OT_R, RT_A),  op(OT_MR, RT_DE), WIDTH_8),     // 0x1A LD A,(DE)
    instr(IN_DEC, 8,  op(OT_R, RT_DE),                     op(), WIDTH_16), // 0x1B DEC DE
    instr(IN_INC, 4,  op(OT_R, RT_E),                      op(), WIDTH_8),  // 0x1C INC E
    instr(IN_DEC, 4,  op(OT_R, RT_E),                      op(), WIDTH_8),  // 0x1D DEC E
    instr(IN_LD, 8,   op(OT_R, RT_E),  op(OT_D8),        WIDTH_8),     // 0x1E LD E,d8
    instr(IN_RRA, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x1F RRA

    instr_cond(IN_JR, 8, 12,   op(),             op(OT_D8),        WIDTH_8, CT_NZ), // 0x20 JR NZ,r8
    instr(IN_LD, 12,   op(OT_R, RT_HL), op(OT_D16),       WIDTH_16),    // 0x21 LD HL,d16
    instr(IN_LD, 8,   op(OT_HLI, RT_HL), op(OT_R, RT_A), WIDTH_8),     // 0x22 LD (HL+),A
    instr(IN_INC, 8,  op(OT_R, RT_HL),                     op(), WIDTH_16), // 0x23 INC HL
    instr(IN_INC, 4,  op(OT_R, RT_H),                      op(), WIDTH_8),  // 0x24 INC H
    instr(IN_DEC, 4,  op(OT_R, RT_H),                      op(), WIDTH_8),  // 0x25 DEC H
    instr(IN_LD, 8,   op(OT_R, RT_H),  op(OT_D8),        WIDTH_8),     // 0x26 LD H,d8
    instr(IN_DAA, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x27 DAA
    instr_cond(IN_JR, 8, 12,   op(),             op(OT_D8),        WIDTH_8, CT_Z),  // 0x28 JR Z,r8
    instr(IN_ADD, 8,  op(OT_R, RT_HL), op(OT_R, RT_HL),  WIDTH_16),    // 0x29 ADD HL,HL
    instr(IN_LD, 8,   op(OT_R, RT_A),  op(OT_HLI, RT_HL), WIDTH_8),    // 0x2A LD A,(HL+)
    instr(IN_DEC, 8,  op(OT_R, RT_HL),                     op(), WIDTH_16), // 0x2B DEC HL
    instr(IN_INC, 4,  op(OT_R, RT_L),                      op(), WIDTH_8),  // 0x2C INC L
    instr(IN_DEC, 4,  op(OT_R, RT_L),                      op(), WIDTH_8),  // 0x2D DEC L
    instr(IN_LD, 8,   op(OT_R, RT_L),  op(OT_D8),        WIDTH_8),     // 0x2E LD L,d8
    instr(IN_CPL, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x2F CPL

    instr_cond(IN_JR, 8, 12,   op(),             op(OT_D8),        WIDTH_8, CT_NC), // 0x30 JR NC,r8
    instr(IN_LD, 12,   op(OT_R, RT_SP), op(OT_D16),       WIDTH_16),    // 0x31 LD SP,d16
    instr(IN_LD, 8,   op(OT_HLD, RT_HL), op(OT_R, RT_A), WIDTH_8),     // 0x32 LD (HL-),A
    instr(IN_INC, 8,  op(OT_R, RT_SP),                     op(), WIDTH_16), // 0x33 INC SP
    instr(IN_INC, 12,  op(OT_MR, RT_HL),                    op(), WIDTH_8),  // 0x34 INC (HL)
    instr(IN_DEC, 12,  op(OT_MR, RT_HL),                    op(), WIDTH_8),  // 0x35 DEC (HL)
    instr(IN_LD, 12,   op(OT_MR, RT_HL), op(OT_D8),       WIDTH_8),     // 0x36 LD (HL),d8
    instr(IN_SCF, 4),                                                   // 0x37 SCF
    instr_cond(IN_JR, 8, 12,   op(),             op(OT_D8),        WIDTH_8, CT_C),  // 0x38 JR C,r8
    instr(IN_ADD, 8,  op(OT_R, RT_HL), op(OT_R, RT_SP),  WIDTH_16),    // 0x39 ADD HL,SP
    instr(IN_LD, 8,   op(OT_R, RT_A),  op(OT_HLD, RT_HL), WIDTH_8),    // 0x3A LD A,(HL-)
    instr(IN_DEC, 8,  op(OT_R, RT_SP),                     op(), WIDTH_16), // 0x3B DEC SP
    instr(IN_INC, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x3C INC A
    instr(IN_DEC, 4,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x3D DEC A
    instr(IN_LD, 8,   op(OT_R, RT_A),  op(OT_D8),        WIDTH_8),     // 0x3E LD A,d8
    instr(IN_CCF, 4),                                                   // 0x3F CCF

    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_B), WIDTH_8),          // 0x40 LD B,B
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_C), WIDTH_8),          // 0x41 LD B,C
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_D), WIDTH_8),          // 0x42 LD B,D
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_E), WIDTH_8),          // 0x43 LD B,E
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_H), WIDTH_8),          // 0x44 LD B,H
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_L), WIDTH_8),          // 0x45 LD B,L
    instr(IN_LD, 8, op(OT_R, RT_B), op(OT_MR, RT_HL), WIDTH_8),        // 0x46 LD B,(HL)
    instr(IN_LD, 4, op(OT_R, RT_B), op(OT_R, RT_A), WIDTH_8),          // 0x47 LD B,A
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_B), WIDTH_8),          // 0x48 LD C,B
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_C), WIDTH_8),          // 0x49 LD C,C
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_D), WIDTH_8),          // 0x4A LD C,D
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_E), WIDTH_8),          // 0x4B LD C,E
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_H), WIDTH_8),          // 0x4C LD C,H
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_L), WIDTH_8),          // 0x4D LD C,L
    instr(IN_LD, 8, op(OT_R, RT_C), op(OT_MR, RT_HL), WIDTH_8),        // 0x4E LD C,(HL)
    instr(IN_LD, 4, op(OT_R, RT_C), op(OT_R, RT_A), WIDTH_8),          // 0x4F LD C,A

    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_B), WIDTH_8),          // 0x50 LD D,B
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_C), WIDTH_8),          // 0x51 LD D,C
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_D), WIDTH_8),          // 0x52 LD D,D
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_E), WIDTH_8),          // 0x53 LD D,E
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_H), WIDTH_8),          // 0x54 LD D,H
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_L), WIDTH_8),          // 0x55 LD D,L
    instr(IN_LD, 8, op(OT_R, RT_D), op(OT_MR, RT_HL), WIDTH_8),        // 0x56 LD D,(HL)
    instr(IN_LD, 4, op(OT_R, RT_D), op(OT_R, RT_A), WIDTH_8),          // 0x57 LD D,A
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_B), WIDTH_8),          // 0x58 LD E,B
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_C), WIDTH_8),          // 0x59 LD E,C
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_D), WIDTH_8),          // 0x5A LD E,D
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_E), WIDTH_8),          // 0x5B LD E,E
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_H), WIDTH_8),          // 0x5C LD E,H
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_L), WIDTH_8),          // 0x5D LD E,L
    instr(IN_LD, 8, op(OT_R, RT_E), op(OT_MR, RT_HL), WIDTH_8),        // 0x5E LD E,(HL)
    instr(IN_LD, 4, op(OT_R, RT_E), op(OT_R, RT_A), WIDTH_8),          // 0x5F LD E,A

    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_B), WIDTH_8),          // 0x60 LD H,B
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_C), WIDTH_8),          // 0x61 LD H,C
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_D), WIDTH_8),          // 0x62 LD H,D
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_E), WIDTH_8),          // 0x63 LD H,E
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_H), WIDTH_8),          // 0x64 LD H,H
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_L), WIDTH_8),          // 0x65 LD H,L
    instr(IN_LD, 8, op(OT_R, RT_H), op(OT_MR, RT_HL), WIDTH_8),        // 0x66 LD H,(HL)
    instr(IN_LD, 4, op(OT_R, RT_H), op(OT_R, RT_A), WIDTH_8),          // 0x67 LD H,A
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_B), WIDTH_8),          // 0x68 LD L,B
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_C), WIDTH_8),          // 0x69 LD L,C
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_D), WIDTH_8),          // 0x6A LD L,D
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_E), WIDTH_8),          // 0x6B LD L,E
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_H), WIDTH_8),          // 0x6C LD L,H
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_L), WIDTH_8),          // 0x6D LD L,L
    instr(IN_LD, 8, op(OT_R, RT_L), op(OT_MR, RT_HL), WIDTH_8),        // 0x6E LD L,(HL)
    instr(IN_LD, 4, op(OT_R, RT_L), op(OT_R, RT_A), WIDTH_8),          // 0x6F LD L,A

    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_B), WIDTH_8),        // 0x70 LD (HL),B
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_C), WIDTH_8),        // 0x71 LD (HL),C
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_D), WIDTH_8),        // 0x72 LD (HL),D
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_E), WIDTH_8),        // 0x73 LD (HL),E
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_H), WIDTH_8),        // 0x74 LD (HL),H
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_L), WIDTH_8),        // 0x75 LD (HL),L
    instr(IN_HALT, 4),                                                  // 0x76 HALT
    instr(IN_LD, 8, op(OT_MR, RT_HL), op(OT_R, RT_A), WIDTH_8),        // 0x77 LD (HL),A
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0x78 LD A,B
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0x79 LD A,C
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0x7A LD A,D
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0x7B LD A,E
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0x7C LD A,H
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0x7D LD A,L
    instr(IN_LD, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0x7E LD A,(HL)
    instr(IN_LD, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0x7F LD A,A

    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x80 ADD A,B
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x81 ADD A,C
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x82 ADD A,D
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x83 ADD A,E
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x84 ADD A,H
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x85 ADD A,L
    instr(IN_ADD, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x86 ADD A,(HL)
    instr(IN_ADD, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x87 ADD A,A
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x88 ADC A,B
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x89 ADC A,C
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x8A ADC A,D
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x8B ADC A,E
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x8C ADC A,H
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x8D ADC A,L
    instr(IN_ADC, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x8E ADC A,(HL)
    instr(IN_ADC, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x8F ADC A,A

    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x90 SUB A,B
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x91 SUB A,C
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x92 SUB A,D
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x93 SUB A,E
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x94 SUB A,H
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x95 SUB A,L
    instr(IN_SUB, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x96 SUB A,(HL)
    instr(IN_SUB, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x97 SUB A,A
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x98 SBC A,B
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x99 SBC A,C
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x9A SBC A,D
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x9B SBC A,E
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x9C SBC A,H
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x9D SBC A,L
    instr(IN_SBC, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x9E SBC A,(HL)
    instr(IN_SBC, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x9F SBC A,A

    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0xA0 AND A,B
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0xA1 AND A,C
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0xA2 AND A,D
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0xA3 AND A,E
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0xA4 AND A,H
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0xA5 AND A,L
    instr(IN_AND, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0xA6 AND A,(HL)
    instr(IN_AND, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0xA7 AND A,A
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0xA8 XOR A,B
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0xA9 XOR A,C
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0xAA XOR A,D
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0xAB XOR A,E
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0xAC XOR A,H
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0xAD XOR A,L
    instr(IN_XOR, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0xAE XOR A,(HL)
    instr(IN_XOR, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0xAF XOR A,A

    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0xB0 OR A,B
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0xB1 OR A,C
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0xB2 OR A,D
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0xB3 OR A,E
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0xB4 OR A,H
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0xB5 OR A,L
    instr(IN_OR, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0xB6 OR A,(HL)
    instr(IN_OR, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0xB7 OR A,A
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0xB8 CP A,B
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0xB9 CP A,C
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0xBA CP A,D
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0xBB CP A,E
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0xBC CP A,H
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0xBD CP A,L
    instr(IN_CP, 8, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0xBE CP A,(HL)
    instr(IN_CP, 4, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0xBF CP A,A

    instr_cond(IN_RET, 8, 20,  op(),             op(),                  WIDTH_NONE, CT_NZ), // 0xC0 RET NZ
    instr(IN_POP, 12,  op(OT_R, RT_BC), op(),                  WIDTH_16),          // 0xC1 POP BC
    instr_cond(IN_JP, 12, 16,   op(),             op(OT_D16),            WIDTH_16, CT_NZ),   // 0xC2 JP NZ,a16
    instr(IN_JP, 16,   op(),             op(OT_D16),            WIDTH_16),          // 0xC3 JP a16
    instr_cond(IN_CALL, 12, 24, op(),             op(OT_D16),            WIDTH_16, CT_NZ),   // 0xC4 CALL NZ,a16
    instr(IN_PUSH, 16, op(),             op(OT_R, RT_BC),       WIDTH_16),          // 0xC5 PUSH BC
    instr(IN_ADD, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xC6 ADD A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x00), // 0xC7 RST 00H
    instr_cond(IN_RET, 8, 20,  op(),             op(),                  WIDTH_NONE, CT_Z),  // 0xC8 RET Z
    instr(IN_RET, 16),                                                            // 0xC9 RET
    instr_cond(IN_JP, 12, 16,   op(),             op(OT_D16),            WIDTH_16, CT_Z),    // 0xCA JP Z,a16
    instr(IN_CB, 4,   op(),             op(OT_D8),             WIDTH_8),           // 0xCB PREFIX CB
    instr_cond(IN_CALL, 12, 24, op(),             op(OT_D16),            WIDTH_16, CT_Z),    // 0xCC CALL Z,a16
    instr(IN_CALL, 24, op(),             op(OT_D16),            WIDTH_16),          // 0xCD CALL a16
    instr(IN_ADC, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xCE ADC A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x08), // 0xCF RST 08H

    instr_cond(IN_RET, 8, 20,  op(),             op(),                  WIDTH_NONE, CT_NC), // 0xD0 RET NC
    instr(IN_POP, 12,  op(OT_R, RT_DE), op(),                  WIDTH_16),          // 0xD1 POP DE
    instr_cond(IN_JP, 12, 16,   op(),             op(OT_D16),            WIDTH_16, CT_NC),   // 0xD2 JP NC,a16
    instr(IN_ERR, 4),                                                              // 0xD3 ILLEGAL
    instr_cond(IN_CALL, 12, 24, op(),             op(OT_D16),            WIDTH_16, CT_NC),   // 0xD4 CALL NC,a16
    instr(IN_PUSH, 16, op(),             op(OT_R, RT_DE),       WIDTH_16),          // 0xD5 PUSH DE
    instr(IN_SUB, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xD6 SUB A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x10), // 0xD7 RST 10H
    instr_cond(IN_RET, 8, 20,  op(),             op(),                  WIDTH_NONE, CT_C),  // 0xD8 RET C
    instr(IN_RETI, 16),                                                           // 0xD9 RETI
    instr_cond(IN_JP, 12, 16,   op(),             op(OT_D16),            WIDTH_16, CT_C),    // 0xDA JP C,a16
    instr(IN_ERR, 4),                                                              // 0xDB ILLEGAL
    instr_cond(IN_CALL, 12, 24, op(),             op(OT_D16),            WIDTH_16, CT_C),    // 0xDC CALL C,a16
    instr(IN_ERR, 4),                                                              // 0xDD ILLEGAL
    instr(IN_SBC, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xDE SBC A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x18), // 0xDF RST 18H

    instr(IN_LDH, 12,  op(OT_A8),       op(OT_R, RT_A),        WIDTH_8),           // 0xE0 LDH (a8),A
    instr(IN_POP, 12,  op(OT_R, RT_HL), op(),                  WIDTH_16),          // 0xE1 POP HL
    instr(IN_LDH, 8,  op(OT_MC, RT_C), op(OT_R, RT_A),        WIDTH_8),           // 0xE2 LDH (C),A
    instr(IN_ERR, 4),                                                              // 0xE3 ILLEGAL
    instr(IN_ERR, 4),                                                              // 0xE4 ILLEGAL
    instr(IN_PUSH, 16, op(),             op(OT_R, RT_HL),       WIDTH_16),          // 0xE5 PUSH HL
    instr(IN_AND, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xE6 AND A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x20), // 0xE7 RST 20H
    instr(IN_ADD_SP, 16, op(OT_R, RT_SP), op(OT_D8),           WIDTH_16),          // 0xE8 ADD SP,r8
    instr(IN_JP, 4,   op(),             op(OT_R, RT_HL),       WIDTH_16),          // 0xE9 JP HL
    instr(IN_LD, 16,   op(OT_A16),       op(OT_R, RT_A),        WIDTH_8),           // 0xEA LD (a16),A
    instr(IN_ERR, 4),                                                              // 0xEB ILLEGAL
    instr(IN_ERR, 4),                                                              // 0xEC ILLEGAL
    instr(IN_ERR, 4),                                                              // 0xED ILLEGAL
    instr(IN_XOR, 8,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xEE XOR A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x28), // 0xEF RST 28H

    instr(IN_LDH, 12,  op(OT_R, RT_A),  op(OT_A8),             WIDTH_8),           // 0xF0 LDH A,(a8)
    instr(IN_POP, 12,  op(OT_R, RT_AF), op(),                  WIDTH_16),          // 0xF1 POP AF
    instr(IN_LDH, 8,  op(OT_R, RT_A),  op(OT_MC, RT_C),       WIDTH_8),           // 0xF2 LDH A,(C)
    instr(IN_DI, 4),                                                             // 0xF3 DI
    instr(IN_ERR, 4),                                                              // 0xF4 ILLEGAL
    instr(IN_PUSH, 16, op(),             op(OT_R, RT_AF),       WIDTH_16),          // 0xF5 PUSH AF
    instr(IN_OR, 8,   op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xF6 OR A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x30), // 0xF7 RST 30H
    instr(IN_LD, 12,   op(OT_R, RT_HL), op(OT_SPR8, RT_SP),    WIDTH_16),          // 0xF8 LD HL,SP+r8
    instr(IN_LD, 8,   op(OT_R, RT_SP), op(OT_R, RT_HL),       WIDTH_16),          // 0xF9 LD SP,HL
    instr(IN_LD, 16,   op(OT_R, RT_A),  op(OT_A16),            WIDTH_8),           // 0xFA LD A,(a16)
    instr(IN_EI, 4),                                                             // 0xFB EI
    instr(IN_ERR, 4),                                                              // 0xFC ILLEGAL
    instr(IN_ERR, 4),                                                              // 0xFD ILLEGAL
    instr(IN_CP, 8,   op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xFE CP A,d8
    instr(IN_RST, 16,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x38)  // 0xFF RST 38H
};


constexpr instruction cb_instructions[] = {
    instr(IN_RLC, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x00 RLC B
    instr(IN_RLC, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x01 RLC C
    instr(IN_RLC, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x02 RLC D
    instr(IN_RLC, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x03 RLC E
    instr(IN_RLC, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x04 RLC H
    instr(IN_RLC, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x05 RLC L
    instr(IN_RLC, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x06 RLC (HL)
    instr(IN_RLC, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x07 RLC A
    instr(IN_RRC, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x08 RRC B
    instr(IN_RRC, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x09 RRC C
    instr(IN_RRC, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x0A RRC D
    instr(IN_RRC, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x0B RRC E
    instr(IN_RRC, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x0C RRC H
    instr(IN_RRC, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x0D RRC L
    instr(IN_RRC, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x0E RRC (HL)
    instr(IN_RRC, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x0F RRC A
    instr(IN_RL, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x10 RL B
    instr(IN_RL, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x11 RL C
    instr(IN_RL, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x12 RL D
    instr(IN_RL, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x13 RL E
    instr(IN_RL, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x14 RL H
    instr(IN_RL, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x15 RL L
    instr(IN_RL, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x16 RL (HL)
    instr(IN_RL, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x17 RL A
    instr(IN_RR, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x18 RR B
    instr(IN_RR, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x19 RR C
    instr(IN_RR, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x1A RR D
    instr(IN_RR, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x1B RR E
    instr(IN_RR, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x1C RR H
    instr(IN_RR, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x1D RR L
    instr(IN_RR, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x1E RR (HL)
    instr(IN_RR, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x1F RR A
    instr(IN_SLA, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x20 SLA B
    instr(IN_SLA, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x21 SLA C
    instr(IN_SLA, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x22 SLA D
    instr(IN_SLA, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x23 SLA E
    instr(IN_SLA, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x24 SLA H
    instr(IN_SLA, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x25 SLA L
    instr(IN_SLA, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x26 SLA (HL)
    instr(IN_SLA, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x27 SLA A
    instr(IN_SRA, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x28 SRA B
    instr(IN_SRA, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x29 SRA C
    instr(IN_SRA, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x2A SRA D
    instr(IN_SRA, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x2B SRA E
    instr(IN_SRA, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x2C SRA H
    instr(IN_SRA, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x2D SRA L
    instr(IN_SRA, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x2E SRA (HL)
    instr(IN_SRA, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x2F SRA A
    instr(IN_SWAP, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x30 SWAP B
    instr(IN_SWAP, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x31 SWAP C
    instr(IN_SWAP, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x32 SWAP D
    instr(IN_SWAP, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x33 SWAP E
    instr(IN_SWAP, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x34 SWAP H
    instr(IN_SWAP, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x35 SWAP L
    instr(IN_SWAP, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x36 SWAP (HL)
    instr(IN_SWAP, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x37 SWAP A
    instr(IN_SRL, 8, op(OT_R, RT_B), op(), WIDTH_8), // 0x38 SRL B
    instr(IN_SRL, 8, op(OT_R, RT_C), op(), WIDTH_8), // 0x39 SRL C
    instr(IN_SRL, 8, op(OT_R, RT_D), op(), WIDTH_8), // 0x3A SRL D
    instr(IN_SRL, 8, op(OT_R, RT_E), op(), WIDTH_8), // 0x3B SRL E
    instr(IN_SRL, 8, op(OT_R, RT_H), op(), WIDTH_8), // 0x3C SRL H
    instr(IN_SRL, 8, op(OT_R, RT_L), op(), WIDTH_8), // 0x3D SRL L
    instr(IN_SRL, 16, op(OT_MR, RT_HL), op(), WIDTH_8), // 0x3E SRL (HL)
    instr(IN_SRL, 8, op(OT_R, RT_A), op(), WIDTH_8), // 0x3F SRL A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 0), // 0x40 BIT 0,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 0), // 0x41 BIT 0,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 0), // 0x42 BIT 0,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 0), // 0x43 BIT 0,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 0), // 0x44 BIT 0,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 0), // 0x45 BIT 0,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 0), // 0x46 BIT 0,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 0), // 0x47 BIT 0,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 1), // 0x48 BIT 1,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 1), // 0x49 BIT 1,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 1), // 0x4A BIT 1,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 1), // 0x4B BIT 1,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 1), // 0x4C BIT 1,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 1), // 0x4D BIT 1,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 1), // 0x4E BIT 1,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 1), // 0x4F BIT 1,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 2), // 0x50 BIT 2,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 2), // 0x51 BIT 2,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 2), // 0x52 BIT 2,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 2), // 0x53 BIT 2,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 2), // 0x54 BIT 2,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 2), // 0x55 BIT 2,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 2), // 0x56 BIT 2,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 2), // 0x57 BIT 2,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 3), // 0x58 BIT 3,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 3), // 0x59 BIT 3,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 3), // 0x5A BIT 3,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 3), // 0x5B BIT 3,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 3), // 0x5C BIT 3,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 3), // 0x5D BIT 3,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 3), // 0x5E BIT 3,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 3), // 0x5F BIT 3,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 4), // 0x60 BIT 4,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 4), // 0x61 BIT 4,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 4), // 0x62 BIT 4,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 4), // 0x63 BIT 4,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 4), // 0x64 BIT 4,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 4), // 0x65 BIT 4,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 4), // 0x66 BIT 4,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 4), // 0x67 BIT 4,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 5), // 0x68 BIT 5,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 5), // 0x69 BIT 5,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 5), // 0x6A BIT 5,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 5), // 0x6B BIT 5,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 5), // 0x6C BIT 5,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 5), // 0x6D BIT 5,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 5), // 0x6E BIT 5,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 5), // 0x6F BIT 5,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 6), // 0x70 BIT 6,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 6), // 0x71 BIT 6,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 6), // 0x72 BIT 6,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 6), // 0x73 BIT 6,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 6), // 0x74 BIT 6,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 6), // 0x75 BIT 6,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 6), // 0x76 BIT 6,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 6), // 0x77 BIT 6,A
    instr(IN_BIT, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 7), // 0x78 BIT 7,B
    instr(IN_BIT, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 7), // 0x79 BIT 7,C
    instr(IN_BIT, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 7), // 0x7A BIT 7,D
    instr(IN_BIT, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 7), // 0x7B BIT 7,E
    instr(IN_BIT, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 7), // 0x7C BIT 7,H
    instr(IN_BIT, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 7), // 0x7D BIT 7,L
    instr(IN_BIT, 12, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 7), // 0x7E BIT 7,(HL)
    instr(IN_BIT, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 7), // 0x7F BIT 7,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 0), // 0x80 RES 0,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 0), // 0x81 RES 0,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 0), // 0x82 RES 0,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 0), // 0x83 RES 0,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 0), // 0x84 RES 0,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 0), // 0x85 RES 0,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 0), // 0x86 RES 0,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 0), // 0x87 RES 0,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 1), // 0x88 RES 1,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 1), // 0x89 RES 1,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 1), // 0x8A RES 1,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 1), // 0x8B RES 1,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 1), // 0x8C RES 1,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 1), // 0x8D RES 1,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 1), // 0x8E RES 1,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 1), // 0x8F RES 1,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 2), // 0x90 RES 2,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 2), // 0x91 RES 2,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 2), // 0x92 RES 2,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 2), // 0x93 RES 2,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 2), // 0x94 RES 2,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 2), // 0x95 RES 2,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 2), // 0x96 RES 2,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 2), // 0x97 RES 2,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 3), // 0x98 RES 3,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 3), // 0x99 RES 3,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 3), // 0x9A RES 3,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 3), // 0x9B RES 3,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 3), // 0x9C RES 3,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 3), // 0x9D RES 3,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 3), // 0x9E RES 3,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 3), // 0x9F RES 3,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 4), // 0xA0 RES 4,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 4), // 0xA1 RES 4,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 4), // 0xA2 RES 4,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 4), // 0xA3 RES 4,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 4), // 0xA4 RES 4,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 4), // 0xA5 RES 4,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 4), // 0xA6 RES 4,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 4), // 0xA7 RES 4,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 5), // 0xA8 RES 5,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 5), // 0xA9 RES 5,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 5), // 0xAA RES 5,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 5), // 0xAB RES 5,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 5), // 0xAC RES 5,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 5), // 0xAD RES 5,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 5), // 0xAE RES 5,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 5), // 0xAF RES 5,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 6), // 0xB0 RES 6,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 6), // 0xB1 RES 6,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 6), // 0xB2 RES 6,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 6), // 0xB3 RES 6,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 6), // 0xB4 RES 6,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 6), // 0xB5 RES 6,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 6), // 0xB6 RES 6,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 6), // 0xB7 RES 6,A
    instr(IN_RES, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 7), // 0xB8 RES 7,B
    instr(IN_RES, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 7), // 0xB9 RES 7,C
    instr(IN_RES, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 7), // 0xBA RES 7,D
    instr(IN_RES, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 7), // 0xBB RES 7,E
    instr(IN_RES, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 7), // 0xBC RES 7,H
    instr(IN_RES, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 7), // 0xBD RES 7,L
    instr(IN_RES, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 7), // 0xBE RES 7,(HL)
    instr(IN_RES, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 7), // 0xBF RES 7,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 0), // 0xC0 SET 0,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 0), // 0xC1 SET 0,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 0), // 0xC2 SET 0,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 0), // 0xC3 SET 0,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 0), // 0xC4 SET 0,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 0), // 0xC5 SET 0,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 0), // 0xC6 SET 0,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 0), // 0xC7 SET 0,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 1), // 0xC8 SET 1,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 1), // 0xC9 SET 1,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 1), // 0xCA SET 1,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 1), // 0xCB SET 1,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 1), // 0xCC SET 1,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 1), // 0xCD SET 1,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 1), // 0xCE SET 1,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 1), // 0xCF SET 1,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 2), // 0xD0 SET 2,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 2), // 0xD1 SET 2,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 2), // 0xD2 SET 2,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 2), // 0xD3 SET 2,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 2), // 0xD4 SET 2,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 2), // 0xD5 SET 2,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 2), // 0xD6 SET 2,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 2), // 0xD7 SET 2,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 3), // 0xD8 SET 3,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 3), // 0xD9 SET 3,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 3), // 0xDA SET 3,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 3), // 0xDB SET 3,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 3), // 0xDC SET 3,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 3), // 0xDD SET 3,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 3), // 0xDE SET 3,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 3), // 0xDF SET 3,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 4), // 0xE0 SET 4,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 4), // 0xE1 SET 4,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 4), // 0xE2 SET 4,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 4), // 0xE3 SET 4,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 4), // 0xE4 SET 4,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 4), // 0xE5 SET 4,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 4), // 0xE6 SET 4,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 4), // 0xE7 SET 4,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 5), // 0xE8 SET 5,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 5), // 0xE9 SET 5,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 5), // 0xEA SET 5,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 5), // 0xEB SET 5,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 5), // 0xEC SET 5,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 5), // 0xED SET 5,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 5), // 0xEE SET 5,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 5), // 0xEF SET 5,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 6), // 0xF0 SET 6,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 6), // 0xF1 SET 6,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 6), // 0xF2 SET 6,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 6), // 0xF3 SET 6,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 6), // 0xF4 SET 6,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 6), // 0xF5 SET 6,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 6), // 0xF6 SET 6,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 6), // 0xF7 SET 6,A
    instr(IN_SET, 8, op(OT_R, RT_B), op(), WIDTH_8, CT_NONE, 7), // 0xF8 SET 7,B
    instr(IN_SET, 8, op(OT_R, RT_C), op(), WIDTH_8, CT_NONE, 7), // 0xF9 SET 7,C
    instr(IN_SET, 8, op(OT_R, RT_D), op(), WIDTH_8, CT_NONE, 7), // 0xFA SET 7,D
    instr(IN_SET, 8, op(OT_R, RT_E), op(), WIDTH_8, CT_NONE, 7), // 0xFB SET 7,E
    instr(IN_SET, 8, op(OT_R, RT_H), op(), WIDTH_8, CT_NONE, 7), // 0xFC SET 7,H
    instr(IN_SET, 8, op(OT_R, RT_L), op(), WIDTH_8, CT_NONE, 7), // 0xFD SET 7,L
    instr(IN_SET, 16, op(OT_MR, RT_HL), op(), WIDTH_8, CT_NONE, 7), // 0xFE SET 7,(HL)
    instr(IN_SET, 8, op(OT_R, RT_A), op(), WIDTH_8, CT_NONE, 7), // 0xFF SET 7,A
};

static_assert(sizeof(instructions) / sizeof(instructions[0]) == 0x100,
    "The base opcode table must contain exactly 256 instructions");

const instruction* instruction_by_opcode(u8 opcode) {
    return &instructions[opcode];
}

const instruction* cb_instruction_by_opcode(u8 opcode) {
    return &cb_instructions[opcode];
}
