#include <stdio.h>

#include "ast_pretty_print.h"

typedef struct
{
    int tab;
} PrettyPrintContext;

const int PRETTY_PRINT_SPACES = 2;

int pretty_print_on_source_root_start(void *_ctx, AstNode *node, NodeSourceRoot *data, AstWalkCallbacks *callbacks)
{
    return 0;
}

void pretty_print_on_source_root_end(void *_ctx, AstNode *node, NodeSourceRoot *data)
{
}

void pretty_print_on_expression_start(void *_ctx, AstNode *node, NodeExpression *data)
{
}

void pretty_print_on_expression_end(void *_ctx, AstNode *node, NodeExpression *data)
{
}

void pretty_print_on_nop(void *_ctx, AstNode *node)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("nop\n");
    ctx->tab++;
}

void pretty_print_on_block_start(void *_ctx, AstNode *node, NodeBlock *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("block\n");
    ctx->tab++;
}

void pretty_print_on_block_end(void *_ctx, AstNode *node, NodeBlock *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

int pretty_print_on_if_start(void *_ctx, AstNode *node, NodeIf *data, AstWalkCallbacks *callbacks)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("if\n");
    ctx->tab++;
    return 0;
}

void pretty_print_on_if_end(void *_ctx, AstNode *node, NodeIf *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

int pretty_print_on_for_start(void *_ctx, AstNode *node, NodeFor *data, AstWalkCallbacks *callbacks)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("for\n");
    ctx->tab++;

    return 0;
}

void pretty_print_on_for_end(void *_ctx, AstNode *node, NodeFor *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

int pretty_print_on_while_start(void *_ctx, AstNode *node, NodeWhile *data, AstWalkCallbacks *callbacks)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("while\n");
    ctx->tab++;

    return 0;
}

void pretty_print_on_while_end(void *_ctx, AstNode *node, NodeWhile *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_break(void *_ctx, AstNode *node, NodeBreak *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("break\n");
}

void pretty_print_on_continue(void *_ctx, AstNode *node, NodeContinue *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("continue\n");
}

void pretty_print_on_number(void *_ctx, AstNode *node, NodeNumber *data)
{
    PrettyPrintContext *ctx = _ctx;

    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("number (%lf)\n", data->value);
}

void pretty_print_on_boolean(void *_ctx, AstNode *node, NodeBoolean *data)
{
    PrettyPrintContext *ctx = _ctx;

    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("boolean (%s)\n", data->value ? "true" : "false");
}

void pretty_print_on_string(void *_ctx, AstNode *node, NodeString *data)
{
    PrettyPrintContext *ctx = _ctx;

    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("string (\"%s\")\n", data->value);
}

void pretty_print_on_null(void *_ctx, AstNode *node)
{
    PrettyPrintContext *ctx = _ctx;

    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("null\n");
}

void pretty_print_on_identifier(void *_ctx, AstNode *node, NodeIdentifier *data)
{
    PrettyPrintContext *ctx = _ctx;

    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("identifier (%s)\n", data->name);
}

void pretty_print_on_args_start(void *_ctx, AstNode *node, NodeArgs *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("args (%d)\n", data->count);
    ctx->tab++;
}

void pretty_print_on_args_end(void *_ctx, AstNode *node, NodeArgs *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_call_start(void *_ctx, AstNode *node, NodeCall *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("call\n");
    ctx->tab++;
}

void pretty_print_on_call_end(void *_ctx, AstNode *node, NodeCall *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_index_start(void *_ctx, AstNode *node, NodeIndex *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("index\n");
    ctx->tab++;
}

void pretty_print_on_index_end(void *_ctx, AstNode *node, NodeIndex *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_unary_start(void *_ctx, AstNode *node, NodeUnary *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("unary (%s)\n", unary_op_to_str(data->op.kind));
    ctx->tab++;
}

void pretty_print_on_unary_end(void *_ctx, AstNode *node, NodeUnary *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

int pretty_print_on_binary_start(void *_ctx, AstNode *node, NodeBinary *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("binary (%s)\n", binary_op_to_str(data->op.kind));
    ctx->tab++;
    return 0;
}

void pretty_print_on_binary_end(void *_ctx, AstNode *node, NodeBinary *data, AstWalkCallbacks *callbacks)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_paren_start(void *_ctx, AstNode *node, NodeParen *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("paren\n");
    ctx->tab++;
}

void pretty_print_on_paren_end(void *_ctx, AstNode *node, NodeParen *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_function_start(void *_ctx, AstNode *node, NodeFunction *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("function '%s' (", data->name);
    for (long i = 0; i < data->parameter_count; i++)
    {
        if (i != 0)
        {
            printf(", ");
        }
        printf("%s", data->parameters[i]);
    }
    printf(")\n");
    ctx->tab++;
}

void pretty_print_on_function_end(void *_ctx, AstNode *node, NodeFunction *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void pretty_print_on_return_start(void *_ctx, AstNode *node, NodeReturn *data)
{
    PrettyPrintContext *ctx = _ctx;
    for (int i = 0; i < ctx->tab * PRETTY_PRINT_SPACES; i++)
    {
        printf(" ");
    }
    printf("return\n");
    ctx->tab++;
}

void pretty_print_on_return_end(void *_ctx, AstNode *node, NodeReturn *data)
{
    PrettyPrintContext *ctx = _ctx;
    ctx->tab--;
}

void ast_pretty_print(AstNode *ast)
{
    AstWalkCallbacks pretty_print_callbacks = {0};
    pretty_print_callbacks.on_source_root_start = pretty_print_on_source_root_start;
    pretty_print_callbacks.on_source_root_end = pretty_print_on_source_root_end;
    pretty_print_callbacks.on_expression_start = pretty_print_on_expression_start;
    pretty_print_callbacks.on_expression_end = pretty_print_on_expression_end;
    pretty_print_callbacks.on_number = pretty_print_on_number;
    pretty_print_callbacks.on_boolean = pretty_print_on_boolean;
    pretty_print_callbacks.on_string = pretty_print_on_string;
    pretty_print_callbacks.on_null = pretty_print_on_null;
    pretty_print_callbacks.on_identifier = pretty_print_on_identifier;
    pretty_print_callbacks.on_args_start = pretty_print_on_args_start;
    pretty_print_callbacks.on_args_end = pretty_print_on_args_end;
    pretty_print_callbacks.on_call_start = pretty_print_on_call_start;
    pretty_print_callbacks.on_call_end = pretty_print_on_call_end;
    pretty_print_callbacks.on_index_start = pretty_print_on_index_start;
    pretty_print_callbacks.on_index_end = pretty_print_on_index_end;
    pretty_print_callbacks.on_unary_start = pretty_print_on_unary_start;
    pretty_print_callbacks.on_unary_end = pretty_print_on_unary_end;
    pretty_print_callbacks.on_binary_start = pretty_print_on_binary_start;
    pretty_print_callbacks.on_binary_end = pretty_print_on_binary_end;
    pretty_print_callbacks.on_paren_start = pretty_print_on_paren_start;
    pretty_print_callbacks.on_paren_end = pretty_print_on_paren_end;
    pretty_print_callbacks.on_block_start = pretty_print_on_block_start;
    pretty_print_callbacks.on_block_end = pretty_print_on_block_end;
    pretty_print_callbacks.on_if_start = pretty_print_on_if_start;
    pretty_print_callbacks.on_if_end = pretty_print_on_if_end;
    pretty_print_callbacks.on_for_start = pretty_print_on_for_start;
    pretty_print_callbacks.on_for_end = pretty_print_on_for_end;
    pretty_print_callbacks.on_while_start = pretty_print_on_while_start;
    pretty_print_callbacks.on_while_end = pretty_print_on_while_end;
    pretty_print_callbacks.on_break = pretty_print_on_break;
    pretty_print_callbacks.on_continue = pretty_print_on_continue;
    pretty_print_callbacks.on_function_start = pretty_print_on_function_start;
    pretty_print_callbacks.on_function_end = pretty_print_on_function_end;
    pretty_print_callbacks.on_return_start = pretty_print_on_return_start;
    pretty_print_callbacks.on_return_end = pretty_print_on_return_end;
    pretty_print_callbacks.on_nop = pretty_print_on_nop;

    PrettyPrintContext pretty_print_ctx;
    pretty_print_ctx.tab = 0;

    ast_walk(&pretty_print_ctx, ast, &pretty_print_callbacks);
}
