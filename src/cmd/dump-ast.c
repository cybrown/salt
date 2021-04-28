#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/err.h"
#include "../lib/fs.h"
#include "../lib/parser.h"
#include "../lib/opcode.h"
#include "../lib/compiler.h"
#include "../lib/vm.h"
#include "../lib/ast_pretty_print.h"

void dump_opcodes(CompilationResult result)
{
    char *opcodes = result.opcodes;
    long size = result.size;

    long i = 0;
    while (i < size)
    {
        char current_opcode = opcodes[i];
        printf("%04lu: ", i);
        printf("%s\n", opcode_to_str(current_opcode));
        i++;
        switch (current_opcode)
        {
        case OPCODE_LOAD_DOUBLE:
            printf("%04lu: ", i);
            printf("double (%lf)\n", *(double *)(opcodes + i));
            i += sizeof(double);
            break;
        case OPCODE_LOAD_BOOLEAN:
            printf("%04lu: ", i);
            printf("boolean (%d)\n", *(char *)(opcodes + i));
            i += sizeof(char);
            break;
        case OPCODE_LOAD_STRING:
            printf("%04lu: ", i);
            printf("index (%lu) '%s'\n", *(long *)(opcodes + i), (result.strings + *(long *)(opcodes + i)));
            i += sizeof(long);
            break;
        case OPCODE_LOAD_VARIABLE:
            printf("%04lu: ", i);
            printf("index (%lu) '%s'\n", *(long *)(opcodes + i), (result.strings + *(long *)(opcodes + i)));
            i += sizeof(long);
            break;
        case OPCODE_STORE_VARIABLE:
            printf("%04lu: ", i);
            printf("index (%lu) '%s'\n", *(long *)(opcodes + i), (result.strings + *(long *)(opcodes + i)));
            i += sizeof(long);
            break;
        case OPCODE_CALL:
            printf("%04lu: ", i);
            printf("arity (%lu)\n", *(long *)(opcodes + i));
            i += sizeof(long);
            break;
        case OPCODE_JUMP:
            printf("%04lu: ", i);
            printf("index (%lu)\n", *(long *)(opcodes + i));
            i += sizeof(long);
            break;
        case OPCODE_JUMP_ZERO:
            printf("%04lu: ", i);
            printf("index (%lu)\n", *(long *)(opcodes + i));
            i += sizeof(long);
            break;
        case OPCODE_LOAD_ARG:
            printf("%04lu: ", i);
            printf("index (%lu)\n", *(long *)(opcodes + i));
            i += sizeof(long);
            break;
        case OPCODE_LOAD_FUNCTION:
            printf("%04lu: ", i);
            printf("index (%lu)\n", *(long *)(opcodes + i));
            i += sizeof(long);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    char *source_path = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strlen(argv[i]) > 0)
        {
            if (argv[i][0] != '-')
            {
                source_path = argv[i];
            }
        }
    }

    FILE *source_file;

    if (source_path != 0)
    {
        errno_t err = fopen_s(&source_file, source_path, "rb");
        if (err != 0)
        {
            print_err(__FILE__, __LINE__, "fopen_s", err);
            return 1;
        }
    }
    else
    {
        source_file = stdin;
    }

    char *source = fread_all(source_file);
    long source_size = strlen(source);
    AstNode *ast = parse_source_root(source, source_size);

    if (ast == 0)
    {
        print_err(__FILE__, __LINE__, "parse_source_root", 0);
        return 1;
    }

    ast_pretty_print(ast);

    return 0;
}
