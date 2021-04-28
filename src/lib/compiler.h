#ifndef H_COMPILER
#define H_COMPILER

#include "opcode.h"

typedef struct
{
    char *opcodes;
    long size;
    char *strings;
} CompilationResult;

CompilationResult compile_ast(AstNode *ast);

#endif
