#pragma once
#include "../commons.h"

struct ModRM_t {
    uint8_t mod : 2;     // 2 bits for the Mod field: Specifies the addressing mode used by the instruction.
    uint8_t reg : 3;     // 3 bits for the Reg/Opcode field: Specifies either a register operand or an extension of the opcode.
    uint8_t rm : 3;      // 3 bits for the R/M field: Usually serves as an extension of the Mod field, providing additional information about the operand.
};