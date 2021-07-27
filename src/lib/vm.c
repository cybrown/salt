#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vm.h"

#include "ast.h"

long read_long_from_opcodes_and_increment(VM *vm, char *opcodes);
void vm_store_variable(VM *vm, long key_index, Value value);
Value vm_load_variable(VM *vm, long key_index);

VM *create_vm()
{
    VM *vm = malloc(sizeof(VM));
    memset(vm, 0, sizeof(VM));
    vm->stack = malloc(sizeof(double) * 100);
    memset(vm->stack, 0, sizeof(double) * 100);
    vm->global_variables = malloc(sizeof(Value) * 1024);
    memset(vm->global_variables, 0, sizeof(Value) * 1024);
    return vm;
}

Value value_add(Value a, Value b)
{
    // TODO: allow strings
    if (a.kind != KIND_NUMBER || b.kind != KIND_NUMBER)
    {
        puts("Error: only numbers are allowed for additions");
    }
    Value result = create_value_number(a.value.d + b.value.d);
    return result;
}

Value value_sub(Value a, Value b)
{
    if (a.kind != KIND_NUMBER || b.kind != KIND_NUMBER)
    {
        puts("Error: only numbers are allowed for substractions");
    }
    Value result = create_value_number(a.value.d - b.value.d);
    return result;
}

Value value_mul(Value a, Value b)
{
    if (a.kind != KIND_NUMBER || b.kind != KIND_NUMBER)
    {
        puts("Error: only numbers are allowed for multiplications");
    }
    Value result = create_value_number(a.value.d * b.value.d);
    return result;
}

Value value_mod(Value a, Value b)
{
    if (a.kind != KIND_NUMBER || b.kind != KIND_NUMBER)
    {
        puts("Error: only numbers are allowed for modulo");
    }
    Value result = create_value_number((double)(((int)a.value.d) % ((int)b.value.d)));
    return result;
}

Value value_band(Value a, Value b)
{
    // TODO: allow numbers
    if (a.kind != KIND_BOOLEAN || b.kind != KIND_BOOLEAN)
    {
        puts("Error: only booleans are allowed for binary and");
    }
    Value result = create_value_boolean(a.value.b & b.value.b);
    return result;
}

Value value_bor(Value a, Value b)
{
    // TODO: allow numbers
    if (a.kind != KIND_BOOLEAN || b.kind != KIND_BOOLEAN)
    {
        puts("Error: only booleans are allowed for binary or");
    }
    Value result = create_value_boolean(a.value.b | b.value.b);
    return result;
}

void vm_run(VM *vm, char *opcodes)
{
    while (opcodes[vm->pc] != 0)
    {
        char current_opcode = opcodes[vm->pc++];
        switch (current_opcode)
        {
        case OPCODE_LOAD_NULL:
        {
            Value value = {0};
            value.kind = KIND_NULL;
            vm->stack[vm->sp++] = value;
            break;
        }
        case OPCODE_LOAD_BOOLEAN:
        {
            char value;
            memcpy(&value, opcodes + vm->pc, sizeof(value));
            vm->pc += sizeof(value);
            vm->stack[vm->sp++] = create_value_boolean(value);
            break;
        }
        case OPCODE_LOAD_DOUBLE:
        {
            double value;
            memcpy(&value, opcodes + vm->pc, sizeof(value));
            vm->pc += sizeof(value);
            vm->stack[vm->sp++] = create_value_number(value);
            break;
        }
        case OPCODE_LOAD_STRING:
        {
            long index = read_long_from_opcodes_and_increment(vm, opcodes);
            Value value = create_value_string(index);
            vm->stack[vm->sp++] = value;
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
        case OPCODE_MOD:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_mod(a, b);
            break;
        }
        case OPCODE_POP:
        {
            vm->sp--;
            break;
        }
        case OPCODE_STORE_VARIABLE:
        {
            vm_store_variable(
                vm,
                read_long_from_opcodes_and_increment(vm, opcodes),
                vm->stack[vm->sp]);
            break;
        }
        case OPCODE_LOAD_VARIABLE:
        {
            vm->stack[vm->sp++] = vm_load_variable(
                vm,
                read_long_from_opcodes_and_increment(vm, opcodes));
            break;
        }
        case OPCODE_BAND:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_band(a, b);
            break;
        }
        case OPCODE_BOR:
        {
            Value b = vm->stack[--vm->sp];
            Value a = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = value_bor(a, b);
            break;
        }
        case OPCODE_POS:
        {
            Value value = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = create_value_number(+value.value.d);
            break;
        }
        case OPCODE_NEG:
        {
            Value value = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = create_value_number(-value.value.d);
            break;
        }
        case OPCODE_CMP:
        {
            Value value = vm->stack[--vm->sp];
            vm->stack[vm->sp++] = create_value_number(~((int)value.value.d));
            break;
        }
        default:
            printf("opcode not known: %d\n", (unsigned char)current_opcode);
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

Value create_value_string(long index)
{
    Value value;
    value.kind = KIND_STRING;
    value.value.s = index;
    return value;
}

void vm_store_variable(VM *vm, long key_index, Value value)
{
    vm->global_variables[key_index] = value;
}

Value vm_load_variable(VM *vm, long key_index)
{
    return vm->global_variables[key_index];
}

long read_long_from_opcodes_and_increment(VM *vm, char *opcodes)
{
    long value;
    memcpy(&value, opcodes + vm->pc, sizeof(value));
    vm->pc += sizeof(value);
    return value;
}
