#ifndef H_OPCODE
#define H_OPCODE

#include "parser.h"

typedef enum
{
    OPCODE_NONE,
    OPCODE_LOAD_DOUBLE,
    OPCODE_LOAD_BOOLEAN,
    OPCODE_LOAD_NULL,
    OPCODE_LOAD_STRING,
    OPCODE_LOAD_VARIABLE,
    OPCODE_LOAD_FUNCTION,
    OPCODE_MUL,
    OPCODE_DIV,
    OPCODE_MOD,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_SHL,
    OPCODE_SHLU,
    OPCODE_SHR,
    OPCODE_LT,
    OPCODE_LTE,
    OPCODE_GT,
    OPCODE_GTE,
    OPCODE_EQ,
    OPCODE_NE,
    OPCODE_BAND,
    OPCODE_BXOR,
    OPCODE_BOR,
    OPCODE_AND,
    OPCODE_OR,
    OPCODE_CALL,
    OPCODE_INDEX,
    OPCODE_POS,
    OPCODE_NEG,
    OPCODE_NOT,
    OPCODE_CMP,
    OPCODE_POP,
    OPCODE_STORE_VARIABLE,
    OPCODE_JUMP_ZERO,
    OPCODE_JUMP,
    OPCODE_RETURN,
    OPCODE_LOAD_ARG,
} OpCodes;

char *opcode_to_str(OpCodes opcode);

#endif
