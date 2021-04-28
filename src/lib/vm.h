#ifndef H_VM
#define H_VM

#include "opcode.h"

typedef enum
{
    KIND_NULL,
    KIND_NUMBER,
    KIND_BOOLEAN,
} ValueKind;

typedef struct
{
    ValueKind kind;
    union
    {
        double d;
        int b;
    } value;
} Value;

typedef struct
{
    int sp;
    int pc;
    Value *stack;
} VM;

VM *create_vm();
void vm_run(VM *vm, char *opcodes);
Value create_value_number(double d);
Value create_value_boolean(int b);

#endif
