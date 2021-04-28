#include "opcode.h"

#include "ast.h"

char *opcode_to_str(OpCodes opcode)
{
    switch (opcode)
    {
    case OPCODE_NONE:
        return "NONE";
    case OPCODE_LOAD_DOUBLE:
        return "LOAD_DOUBLE";
    case OPCODE_LOAD_BOOLEAN:
        return "LOAD_BOOLEAN";
    case OPCODE_LOAD_NULL:
        return "LOAD_NULL";
    case OPCODE_MUL:
        return "MUL";
    case OPCODE_DIV:
        return "DIV";
    case OPCODE_MOD:
        return "MOD";
    case OPCODE_ADD:
        return "ADD";
    case OPCODE_SUB:
        return "SUB";
    case OPCODE_SHL:
        return "SHL";
    case OPCODE_SHLU:
        return "SHLU";
    case OPCODE_SHR:
        return "SHR";
    case OPCODE_LT:
        return "LT";
    case OPCODE_LTE:
        return "LTE";
    case OPCODE_GT:
        return "GT";
    case OPCODE_GTE:
        return "GTE";
    case OPCODE_EQ:
        return "EQ";
    case OPCODE_NE:
        return "NE";
    case OPCODE_BAND:
        return "BAND";
    case OPCODE_BXOR:
        return "BXOR";
    case OPCODE_BOR:
        return "BOR";
    case OPCODE_AND:
        return "AND";
    case OPCODE_OR:
        return "OR";
    case OPCODE_CALL:
        return "CALL";
    case OPCODE_INDEX:
        return "INDEX";
    case OPCODE_POS:
        return "POS";
    case OPCODE_NEG:
        return "NEG";
    case OPCODE_NOT:
        return "NOT";
    case OPCODE_CMP:
        return "CMP";
    case OPCODE_POP:
        return "POP";
    case OPCODE_LOAD_STRING:
        return "LOAD_STRING";
    case OPCODE_LOAD_VARIABLE:
        return "LOAD_VARIABLE";
    case OPCODE_LOAD_FUNCTION:
        return "LOAD_FUNCTION";
    case OPCODE_STORE_VARIABLE:
        return "STORE_VARIABLE";
    case OPCODE_JUMP:
        return "JUMP";
    case OPCODE_JUMP_ZERO:
        return "JUMP_ZERO";
    case OPCODE_RETURN:
        return "RETURN";
    case OPCODE_LOAD_ARG:
        return "LOAD_ARG";
    default:
        return "<Unknown opcode>";
    }
}
