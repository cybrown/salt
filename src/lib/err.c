#include <stdio.h>

void print_err(char *file_name, int line, char *function_name, int err_code)
{
    printf("Function '%s' returned with error code: '%d' at %s:%d\n", function_name, err_code, file_name, line);
}
