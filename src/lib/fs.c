#include <stdlib.h>

#include "fs.h"

int fsize(FILE *fp)
{
    long prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return sz;
}

char *fread_all(FILE *fp)
{
    long size = fsize(fp);
    char *buffer = malloc(size + 1);
    buffer[size] = '\0';
    fread_s(buffer, size, size, 1, fp);
    return buffer;
}
