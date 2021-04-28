#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vm.h"

#include "ast.h"

VM *create_vm()
{
    VM *vm = malloc(sizeof(VM));
    memset(vm, 0, sizeof(VM));
    vm->stack = malloc(sizeof(double) * 100);
    return vm;
}

Value value_add(Value a, Value b)
{
    Value result = create_value_number(a.value.d + b.value.d);
    return result;
}

Value value_sub(Value a, Value b)
{
    Value result = create_value_number(a.value.d - b.value.d);
    return result;
}

Value value_mul(Value a, Value b)
{
    Value result = create_value_number(a.value.d * b.value.d);
    return result;
}

void vm_run(VM *vm, char *opcodes)
{
    while (opcodes[vm->pc] != 0)
    {
        char current_opcode = opcodes[vm->pc++];
        switch (current_opcode)
        {
        case OPCODE_LOAD_DOUBLE:
        {
            double value;
            memcpy(&value, opcodes + vm->pc, sizeof(value));
            vm->pc += sizeof(value);
            vm->stack[vm->sp++] = create_value_number(value);
            break;
        }
        case OPCODE_LOAD_BOOLEAN:
        {
            int value;
            memcpy(&value, opcodes + vm->pc, sizeof(value));
            vm->pc += sizeof(value);
            vm->stack[vm->sp++] = create_value_boolean(value);
            break;
        }
        case OPCODE_ADD:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_add(a, b);
            break;
        }
        case OPCODE_SUB:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_sub(a, b);
            break;
        }
        case OPCODE_MUL:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_mul(a, b);
            break;
        }
        case OPCODE_POP:
        {
            vm->sp--;
            break;
        }
        default:
            printf("opcode not known: %d", (unsigned char)current_opcode);
        }
    }
}

Value create_value_number(double d)
{
    Value value;
    value.kind = KIND_NUMBER;
    value.value.d = d;
    return value;
}

Value create_value_boolean(int b)
{
    Value value;
    value.kind = KIND_BOOLEAN;
    value.value.b = b;
    return value;
}

Value create_value_null()
{
    Value value;
    value.kind = KIND_NULL;
    return value;
}
