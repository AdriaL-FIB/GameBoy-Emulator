#include "instructions.h"

namespace {
    constexpr operand op(operand_type type = OT_NONE, reg_type reg = RT_NONE) {
        return {type, reg};
    }

    constexpr instruction instr(
        in_type type = IN_NONE,
        operand destination = op(),
        operand source = op(),
        operand_width width = WIDTH_NONE,
        cond_type condition = CT_NONE,
        u8 param = 0) {
        return {type, destination, source, width, condition, param};
    }
}

instruction instructions[] = {
    instr(IN_NOP),                                                   // 0x00 NOP
    instr(IN_LD,   op(OT_R, RT_BC), op(OT_D16),       WIDTH_16),    // 0x01 LD BC,d16
    instr(IN_LD,   op(OT_MR, RT_BC), op(OT_R, RT_A),  WIDTH_8),     // 0x02 LD (BC),A
    instr(IN_INC,  op(OT_R, RT_BC),                     op(), WIDTH_16), // 0x03 INC BC
    instr(IN_INC,  op(OT_R, RT_B),                      op(), WIDTH_8),  // 0x04 INC B
    instr(IN_DEC,  op(OT_R, RT_B),                      op(), WIDTH_8),  // 0x05 DEC B
    instr(IN_LD,   op(OT_R, RT_B),  op(OT_D8),        WIDTH_8),     // 0x06 LD B,d8
    instr(IN_RLCA, op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x07 RLCA
    instr(IN_LD,   op(OT_A16),       op(OT_R, RT_SP),  WIDTH_16),    // 0x08 LD (a16),SP
    instr(IN_ADD,  op(OT_R, RT_HL), op(OT_R, RT_BC),  WIDTH_16),    // 0x09 ADD HL,BC
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_MR, RT_BC), WIDTH_8),     // 0x0A LD A,(BC)
    instr(IN_DEC,  op(OT_R, RT_BC),                     op(), WIDTH_16), // 0x0B DEC BC
    instr(IN_INC,  op(OT_R, RT_C),                      op(), WIDTH_8),  // 0x0C INC C
    instr(IN_DEC,  op(OT_R, RT_C),                      op(), WIDTH_8),  // 0x0D DEC C
    instr(IN_LD,   op(OT_R, RT_C),  op(OT_D8),        WIDTH_8),     // 0x0E LD C,d8
    instr(IN_RRCA, op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x0F RRCA

    instr(IN_STOP, op(),             op(OT_D8),        WIDTH_8),     // 0x10 STOP 0
    instr(IN_LD,   op(OT_R, RT_DE), op(OT_D16),       WIDTH_16),    // 0x11 LD DE,d16
    instr(IN_LD,   op(OT_MR, RT_DE), op(OT_R, RT_A),  WIDTH_8),     // 0x12 LD (DE),A
    instr(IN_INC,  op(OT_R, RT_DE),                     op(), WIDTH_16), // 0x13 INC DE
    instr(IN_INC,  op(OT_R, RT_D),                      op(), WIDTH_8),  // 0x14 INC D
    instr(IN_DEC,  op(OT_R, RT_D),                      op(), WIDTH_8),  // 0x15 DEC D
    instr(IN_LD,   op(OT_R, RT_D),  op(OT_D8),        WIDTH_8),     // 0x16 LD D,d8
    instr(IN_RLA,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x17 RLA
    instr(IN_JR,   op(),             op(OT_D8),        WIDTH_8),     // 0x18 JR r8
    instr(IN_ADD,  op(OT_R, RT_HL), op(OT_R, RT_DE),  WIDTH_16),    // 0x19 ADD HL,DE
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_MR, RT_DE), WIDTH_8),     // 0x1A LD A,(DE)
    instr(IN_DEC,  op(OT_R, RT_DE),                     op(), WIDTH_16), // 0x1B DEC DE
    instr(IN_INC,  op(OT_R, RT_E),                      op(), WIDTH_8),  // 0x1C INC E
    instr(IN_DEC,  op(OT_R, RT_E),                      op(), WIDTH_8),  // 0x1D DEC E
    instr(IN_LD,   op(OT_R, RT_E),  op(OT_D8),        WIDTH_8),     // 0x1E LD E,d8
    instr(IN_RRA,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x1F RRA

    instr(IN_JR,   op(),             op(OT_D8),        WIDTH_8, CT_NZ), // 0x20 JR NZ,r8
    instr(IN_LD,   op(OT_R, RT_HL), op(OT_D16),       WIDTH_16),    // 0x21 LD HL,d16
    instr(IN_LD,   op(OT_HLI, RT_HL), op(OT_R, RT_A), WIDTH_8),     // 0x22 LD (HL+),A
    instr(IN_INC,  op(OT_R, RT_HL),                     op(), WIDTH_16), // 0x23 INC HL
    instr(IN_INC,  op(OT_R, RT_H),                      op(), WIDTH_8),  // 0x24 INC H
    instr(IN_DEC,  op(OT_R, RT_H),                      op(), WIDTH_8),  // 0x25 DEC H
    instr(IN_LD,   op(OT_R, RT_H),  op(OT_D8),        WIDTH_8),     // 0x26 LD H,d8
    instr(IN_DAA,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x27 DAA
    instr(IN_JR,   op(),             op(OT_D8),        WIDTH_8, CT_Z),  // 0x28 JR Z,r8
    instr(IN_ADD,  op(OT_R, RT_HL), op(OT_R, RT_HL),  WIDTH_16),    // 0x29 ADD HL,HL
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_HLI, RT_HL), WIDTH_8),    // 0x2A LD A,(HL+)
    instr(IN_DEC,  op(OT_R, RT_HL),                     op(), WIDTH_16), // 0x2B DEC HL
    instr(IN_INC,  op(OT_R, RT_L),                      op(), WIDTH_8),  // 0x2C INC L
    instr(IN_DEC,  op(OT_R, RT_L),                      op(), WIDTH_8),  // 0x2D DEC L
    instr(IN_LD,   op(OT_R, RT_L),  op(OT_D8),        WIDTH_8),     // 0x2E LD L,d8
    instr(IN_CPL,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x2F CPL

    instr(IN_JR,   op(),             op(OT_D8),        WIDTH_8, CT_NC), // 0x30 JR NC,r8
    instr(IN_LD,   op(OT_R, RT_SP), op(OT_D16),       WIDTH_16),    // 0x31 LD SP,d16
    instr(IN_LD,   op(OT_HLD, RT_HL), op(OT_R, RT_A), WIDTH_8),     // 0x32 LD (HL-),A
    instr(IN_INC,  op(OT_R, RT_SP),                     op(), WIDTH_16), // 0x33 INC SP
    instr(IN_INC,  op(OT_MR, RT_HL),                    op(), WIDTH_8),  // 0x34 INC (HL)
    instr(IN_DEC,  op(OT_MR, RT_HL),                    op(), WIDTH_8),  // 0x35 DEC (HL)
    instr(IN_LD,   op(OT_MR, RT_HL), op(OT_D8),       WIDTH_8),     // 0x36 LD (HL),d8
    instr(IN_SCF),                                                   // 0x37 SCF
    instr(IN_JR,   op(),             op(OT_D8),        WIDTH_8, CT_C),  // 0x38 JR C,r8
    instr(IN_ADD,  op(OT_R, RT_HL), op(OT_R, RT_SP),  WIDTH_16),    // 0x39 ADD HL,SP
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_HLD, RT_HL), WIDTH_8),    // 0x3A LD A,(HL-)
    instr(IN_DEC,  op(OT_R, RT_SP),                     op(), WIDTH_16), // 0x3B DEC SP
    instr(IN_INC,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x3C INC A
    instr(IN_DEC,  op(OT_R, RT_A),                      op(), WIDTH_8),  // 0x3D DEC A
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_D8),        WIDTH_8),     // 0x3E LD A,d8
    instr(IN_CCF),                                                   // 0x3F CCF

    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_B), WIDTH_8),          // 0x40 LD B,B
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_C), WIDTH_8),          // 0x41 LD B,C
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_D), WIDTH_8),          // 0x42 LD B,D
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_E), WIDTH_8),          // 0x43 LD B,E
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_H), WIDTH_8),          // 0x44 LD B,H
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_L), WIDTH_8),          // 0x45 LD B,L
    instr(IN_LD, op(OT_R, RT_B), op(OT_MR, RT_HL), WIDTH_8),        // 0x46 LD B,(HL)
    instr(IN_LD, op(OT_R, RT_B), op(OT_R, RT_A), WIDTH_8),          // 0x47 LD B,A
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_B), WIDTH_8),          // 0x48 LD C,B
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_C), WIDTH_8),          // 0x49 LD C,C
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_D), WIDTH_8),          // 0x4A LD C,D
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_E), WIDTH_8),          // 0x4B LD C,E
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_H), WIDTH_8),          // 0x4C LD C,H
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_L), WIDTH_8),          // 0x4D LD C,L
    instr(IN_LD, op(OT_R, RT_C), op(OT_MR, RT_HL), WIDTH_8),        // 0x4E LD C,(HL)
    instr(IN_LD, op(OT_R, RT_C), op(OT_R, RT_A), WIDTH_8),          // 0x4F LD C,A

    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_B), WIDTH_8),          // 0x50 LD D,B
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_C), WIDTH_8),          // 0x51 LD D,C
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_D), WIDTH_8),          // 0x52 LD D,D
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_E), WIDTH_8),          // 0x53 LD D,E
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_H), WIDTH_8),          // 0x54 LD D,H
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_L), WIDTH_8),          // 0x55 LD D,L
    instr(IN_LD, op(OT_R, RT_D), op(OT_MR, RT_HL), WIDTH_8),        // 0x56 LD D,(HL)
    instr(IN_LD, op(OT_R, RT_D), op(OT_R, RT_A), WIDTH_8),          // 0x57 LD D,A
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_B), WIDTH_8),          // 0x58 LD E,B
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_C), WIDTH_8),          // 0x59 LD E,C
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_D), WIDTH_8),          // 0x5A LD E,D
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_E), WIDTH_8),          // 0x5B LD E,E
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_H), WIDTH_8),          // 0x5C LD E,H
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_L), WIDTH_8),          // 0x5D LD E,L
    instr(IN_LD, op(OT_R, RT_E), op(OT_MR, RT_HL), WIDTH_8),        // 0x5E LD E,(HL)
    instr(IN_LD, op(OT_R, RT_E), op(OT_R, RT_A), WIDTH_8),          // 0x5F LD E,A

    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_B), WIDTH_8),          // 0x60 LD H,B
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_C), WIDTH_8),          // 0x61 LD H,C
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_D), WIDTH_8),          // 0x62 LD H,D
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_E), WIDTH_8),          // 0x63 LD H,E
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_H), WIDTH_8),          // 0x64 LD H,H
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_L), WIDTH_8),          // 0x65 LD H,L
    instr(IN_LD, op(OT_R, RT_H), op(OT_MR, RT_HL), WIDTH_8),        // 0x66 LD H,(HL)
    instr(IN_LD, op(OT_R, RT_H), op(OT_R, RT_A), WIDTH_8),          // 0x67 LD H,A
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_B), WIDTH_8),          // 0x68 LD L,B
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_C), WIDTH_8),          // 0x69 LD L,C
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_D), WIDTH_8),          // 0x6A LD L,D
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_E), WIDTH_8),          // 0x6B LD L,E
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_H), WIDTH_8),          // 0x6C LD L,H
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_L), WIDTH_8),          // 0x6D LD L,L
    instr(IN_LD, op(OT_R, RT_L), op(OT_MR, RT_HL), WIDTH_8),        // 0x6E LD L,(HL)
    instr(IN_LD, op(OT_R, RT_L), op(OT_R, RT_A), WIDTH_8),          // 0x6F LD L,A

    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_B), WIDTH_8),        // 0x70 LD (HL),B
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_C), WIDTH_8),        // 0x71 LD (HL),C
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_D), WIDTH_8),        // 0x72 LD (HL),D
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_E), WIDTH_8),        // 0x73 LD (HL),E
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_H), WIDTH_8),        // 0x74 LD (HL),H
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_L), WIDTH_8),        // 0x75 LD (HL),L
    instr(IN_HALT),                                                  // 0x76 HALT
    instr(IN_LD, op(OT_MR, RT_HL), op(OT_R, RT_A), WIDTH_8),        // 0x77 LD (HL),A
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0x78 LD A,B
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0x79 LD A,C
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0x7A LD A,D
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0x7B LD A,E
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0x7C LD A,H
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0x7D LD A,L
    instr(IN_LD, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0x7E LD A,(HL)
    instr(IN_LD, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0x7F LD A,A

    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x80 ADD A,B
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x81 ADD A,C
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x82 ADD A,D
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x83 ADD A,E
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x84 ADD A,H
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x85 ADD A,L
    instr(IN_ADD, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x86 ADD A,(HL)
    instr(IN_ADD, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x87 ADD A,A
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x88 ADC A,B
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x89 ADC A,C
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x8A ADC A,D
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x8B ADC A,E
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x8C ADC A,H
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x8D ADC A,L
    instr(IN_ADC, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x8E ADC A,(HL)
    instr(IN_ADC, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x8F ADC A,A

    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x90 SUB A,B
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x91 SUB A,C
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x92 SUB A,D
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x93 SUB A,E
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x94 SUB A,H
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x95 SUB A,L
    instr(IN_SUB, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x96 SUB A,(HL)
    instr(IN_SUB, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x97 SUB A,A
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0x98 SBC A,B
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0x99 SBC A,C
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0x9A SBC A,D
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0x9B SBC A,E
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0x9C SBC A,H
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0x9D SBC A,L
    instr(IN_SBC, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0x9E SBC A,(HL)
    instr(IN_SBC, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0x9F SBC A,A

    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0xA0 AND A,B
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0xA1 AND A,C
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0xA2 AND A,D
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0xA3 AND A,E
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0xA4 AND A,H
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0xA5 AND A,L
    instr(IN_AND, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0xA6 AND A,(HL)
    instr(IN_AND, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0xA7 AND A,A
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),         // 0xA8 XOR A,B
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),         // 0xA9 XOR A,C
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),         // 0xAA XOR A,D
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),         // 0xAB XOR A,E
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),         // 0xAC XOR A,H
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),         // 0xAD XOR A,L
    instr(IN_XOR, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),       // 0xAE XOR A,(HL)
    instr(IN_XOR, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),         // 0xAF XOR A,A

    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0xB0 OR A,B
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0xB1 OR A,C
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0xB2 OR A,D
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0xB3 OR A,E
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0xB4 OR A,H
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0xB5 OR A,L
    instr(IN_OR, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0xB6 OR A,(HL)
    instr(IN_OR, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0xB7 OR A,A
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_B), WIDTH_8),          // 0xB8 CP A,B
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_C), WIDTH_8),          // 0xB9 CP A,C
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_D), WIDTH_8),          // 0xBA CP A,D
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_E), WIDTH_8),          // 0xBB CP A,E
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_H), WIDTH_8),          // 0xBC CP A,H
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_L), WIDTH_8),          // 0xBD CP A,L
    instr(IN_CP, op(OT_R, RT_A), op(OT_MR, RT_HL), WIDTH_8),        // 0xBE CP A,(HL)
    instr(IN_CP, op(OT_R, RT_A), op(OT_R, RT_A), WIDTH_8),          // 0xBF CP A,A

    instr(IN_RET,  op(),             op(),                  WIDTH_NONE, CT_NZ), // 0xC0 RET NZ
    instr(IN_POP,  op(OT_R, RT_BC), op(),                  WIDTH_16),          // 0xC1 POP BC
    instr(IN_JP,   op(),             op(OT_D16),            WIDTH_16, CT_NZ),   // 0xC2 JP NZ,a16
    instr(IN_JP,   op(),             op(OT_D16),            WIDTH_16),          // 0xC3 JP a16
    instr(IN_CALL, op(),             op(OT_D16),            WIDTH_16, CT_NZ),   // 0xC4 CALL NZ,a16
    instr(IN_PUSH, op(),             op(OT_R, RT_BC),       WIDTH_16),          // 0xC5 PUSH BC
    instr(IN_ADD,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xC6 ADD A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x00), // 0xC7 RST 00H
    instr(IN_RET,  op(),             op(),                  WIDTH_NONE, CT_Z),  // 0xC8 RET Z
    instr(IN_RET),                                                            // 0xC9 RET
    instr(IN_JP,   op(),             op(OT_D16),            WIDTH_16, CT_Z),    // 0xCA JP Z,a16
    instr(IN_CB,   op(),             op(OT_D8),             WIDTH_8),           // 0xCB PREFIX CB
    instr(IN_CALL, op(),             op(OT_D16),            WIDTH_16, CT_Z),    // 0xCC CALL Z,a16
    instr(IN_CALL, op(),             op(OT_D16),            WIDTH_16),          // 0xCD CALL a16
    instr(IN_ADC,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xCE ADC A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x08), // 0xCF RST 08H

    instr(IN_RET,  op(),             op(),                  WIDTH_NONE, CT_NC), // 0xD0 RET NC
    instr(IN_POP,  op(OT_R, RT_DE), op(),                  WIDTH_16),          // 0xD1 POP DE
    instr(IN_JP,   op(),             op(OT_D16),            WIDTH_16, CT_NC),   // 0xD2 JP NC,a16
    instr(),                                                                    // 0xD3 ILLEGAL
    instr(IN_CALL, op(),             op(OT_D16),            WIDTH_16, CT_NC),   // 0xD4 CALL NC,a16
    instr(IN_PUSH, op(),             op(OT_R, RT_DE),       WIDTH_16),          // 0xD5 PUSH DE
    instr(IN_SUB,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xD6 SUB A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x10), // 0xD7 RST 10H
    instr(IN_RET,  op(),             op(),                  WIDTH_NONE, CT_C),  // 0xD8 RET C
    instr(IN_RETI),                                                           // 0xD9 RETI
    instr(IN_JP,   op(),             op(OT_D16),            WIDTH_16, CT_C),    // 0xDA JP C,a16
    instr(),                                                                    // 0xDB ILLEGAL
    instr(IN_CALL, op(),             op(OT_D16),            WIDTH_16, CT_C),    // 0xDC CALL C,a16
    instr(),                                                                    // 0xDD ILLEGAL
    instr(IN_SBC,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xDE SBC A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x18), // 0xDF RST 18H

    instr(IN_LDH,  op(OT_A8),       op(OT_R, RT_A),        WIDTH_8),           // 0xE0 LDH (a8),A
    instr(IN_POP,  op(OT_R, RT_HL), op(),                  WIDTH_16),          // 0xE1 POP HL
    instr(IN_LD,   op(OT_MC, RT_C), op(OT_R, RT_A),        WIDTH_8),           // 0xE2 LD (C),A
    instr(),                                                                    // 0xE3 ILLEGAL
    instr(),                                                                    // 0xE4 ILLEGAL
    instr(IN_PUSH, op(),             op(OT_R, RT_HL),       WIDTH_16),          // 0xE5 PUSH HL
    instr(IN_AND,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xE6 AND A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x20), // 0xE7 RST 20H
    instr(IN_ADD_SP, op(OT_R, RT_SP), op(OT_D8),           WIDTH_16),          // 0xE8 ADD SP,r8
    instr(IN_JP,   op(),             op(OT_R, RT_HL),       WIDTH_16),          // 0xE9 JP HL
    instr(IN_LD,   op(OT_A16),       op(OT_R, RT_A),        WIDTH_8),           // 0xEA LD (a16),A
    instr(),                                                                    // 0xEB ILLEGAL
    instr(),                                                                    // 0xEC ILLEGAL
    instr(),                                                                    // 0xED ILLEGAL
    instr(IN_XOR,  op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xEE XOR A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x28), // 0xEF RST 28H

    instr(IN_LDH,  op(OT_R, RT_A),  op(OT_A8),             WIDTH_8),           // 0xF0 LDH A,(a8)
    instr(IN_POP,  op(OT_R, RT_AF), op(),                  WIDTH_16),          // 0xF1 POP AF
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_MC, RT_C),       WIDTH_8),           // 0xF2 LD A,(C)
    instr(IN_DI),                                                             // 0xF3 DI
    instr(),                                                                    // 0xF4 ILLEGAL
    instr(IN_PUSH, op(),             op(OT_R, RT_AF),       WIDTH_16),          // 0xF5 PUSH AF
    instr(IN_OR,   op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xF6 OR A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x30), // 0xF7 RST 30H
    instr(IN_LD,   op(OT_R, RT_HL), op(OT_SPR8, RT_SP),    WIDTH_16),          // 0xF8 LD HL,SP+r8
    instr(IN_LD,   op(OT_R, RT_SP), op(OT_R, RT_HL),       WIDTH_16),          // 0xF9 LD SP,HL
    instr(IN_LD,   op(OT_R, RT_A),  op(OT_A16),            WIDTH_8),           // 0xFA LD A,(a16)
    instr(IN_EI),                                                             // 0xFB EI
    instr(),                                                                    // 0xFC ILLEGAL
    instr(),                                                                    // 0xFD ILLEGAL
    instr(IN_CP,   op(OT_R, RT_A),  op(OT_D8),             WIDTH_8),           // 0xFE CP A,d8
    instr(IN_RST,  op(),             op(),                  WIDTH_NONE, CT_NONE, 0x38)  // 0xFF RST 38H
};

static_assert(sizeof(instructions) / sizeof(instructions[0]) == 0x100,
    "The base opcode table must contain exactly 256 instructions");

instruction* instruction_by_opcode(u8 opcode) {
    return &instructions[opcode];
}
