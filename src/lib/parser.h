#ifndef H_PARSER
#define H_PARSER

#include "ast.h"

typedef struct
{
    int ok;
} ParseContext;

AstNode *parse_source_root(char *source, long size);

#endif
