#ifndef H_VM
#define H_VM

#include "opcode.h"

typedef enum
{
    KIND_NULL,
    KIND_NUMBER,
    KIND_BOOLEAN,
    KIND_STRING,
} ValueKind;

typedef struct
{
    ValueKind kind;
    union
    {
        double d;
        int b;
        long s;
    } value;
} Value;

typedef struct
{
    int sp;
    int pc;
    Value *stack;
    Value *global_variables;
} VM;

VM *create_vm();
void vm_run(VM *vm, char *opcodes);
Value create_value_boolean(int b);
Value create_value_number(double d);
Value create_value_string(long index);

#endif
