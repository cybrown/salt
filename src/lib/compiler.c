#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "compiler.h"

#include "ast.h"

typedef struct
{
    // TODO: use growable structures
    int current_index;
    char opcodes[10240];
    char strings[10240];
    int new_string_index;
    long breaks[100];
    long current_break;
    long continues[100];
    long current_continue;
} ToOpcodesContext;

void ctx_emit_opcode(ToOpcodesContext *ctx, OpCodes opcode)
{
    ctx->opcodes[ctx->current_index] = opcode;
    ctx->current_index++;
}

void ctx_emit_double(ToOpcodesContext *ctx, double value)
{
    memcpy(ctx->opcodes + ctx->current_index, &value, sizeof(value));
    ctx->current_index += sizeof(value);
}

void ctx_emit_long(ToOpcodesContext *ctx, long value)
{
    memcpy(ctx->opcodes + ctx->current_index, &value, sizeof(value));
    ctx->current_index += sizeof(value);
}

void ctx_emit_break_later(ToOpcodesContext *ctx)
{
    ctx->breaks[ctx->current_break++] = ctx->current_index;
    ctx_emit_long(ctx, 0);
}

void ctx_emit_continue_later(ToOpcodesContext *ctx)
{
    ctx->continues[ctx->current_continue++] = ctx->current_index;
    ctx_emit_long(ctx, 0);
}

void ctx_patch_long(ToOpcodesContext *ctx, long index, long value)
{
    memcpy(ctx->opcodes + index, &value, sizeof(value));
}

void ctx_emit_string(ToOpcodesContext *ctx, char *input_string)
{
    long input_string_length = strlen(input_string);
    long current_index = 0;
    while (current_index < ctx->new_string_index)
    {
        long current_string_length = strlen(ctx->strings + current_index);
        if (current_string_length == 0)
        {
            puts("BREAK");
            break;
        }

        if (input_string_length == current_string_length && !memcmp(input_string, ctx->strings + current_index, input_string_length))
        {
            ctx_emit_long(ctx, current_index);
            return;
        }

        current_index += current_string_length + 1;
    }

    ctx->new_string_index += input_string_length + 1;
    memcpy(ctx->strings + current_index, input_string, input_string_length);
    ctx->strings[current_index + input_string_length] = '\0';

    ctx_emit_long(ctx, current_index);
}

void ctx_push_char(ToOpcodesContext *ctx, char value)
{
    memcpy(ctx->opcodes + ctx->current_index, &value, sizeof(value));
    ctx->current_index += sizeof(value);
}

void to_opcodes_on_nop(void *_ctx, AstNode *node)
{
    // ToOpcodesContext *ctx = _ctx;
}

void to_opcodes_on_number(void *_ctx, AstNode *node, NodeNumber *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_LOAD_DOUBLE);
    ctx_emit_double(ctx, data->value);
}

void to_opcodes_on_boolean(void *_ctx, AstNode *node, NodeBoolean *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_LOAD_BOOLEAN);
    ctx_push_char(ctx, data->value);
}

void to_opcodes_on_null(void *_ctx, AstNode *node)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_LOAD_NULL);
}

void to_opcodes_on_identifier(void *_ctx, AstNode *node, NodeIdentifier *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_LOAD_VARIABLE);
    ctx_emit_string(ctx, data->name);
}

void to_opcodes_on_string(void *_ctx, AstNode *node, NodeString *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_LOAD_STRING);
    ctx_emit_string(ctx, data->value);
}

void to_opcodes_on_args_start(void *_ctx, AstNode *node, NodeArgs *data)
{
    // ToOpcodesContext *ctx = _ctx;
}

void to_opcodes_on_args_end(void *_ctx, AstNode *node, NodeArgs *data)
{
    // ToOpcodesContext *ctx = _ctx;
}

void to_opcodes_on_call_start(void *_ctx, AstNode *node, NodeCall *data)
{
    // ToOpcodesContext *ctx = _ctx;
}

void to_opcodes_on_call_end(void *_ctx, AstNode *node, NodeCall *data)
{
    ToOpcodesContext *ctx = _ctx;

    // TODO: Load the number of arguments
    ctx_emit_opcode(ctx, OPCODE_CALL);
    ctx_emit_long(ctx, data->args->data.args.count);
}

void to_opcodes_on_index_start(void *_ctx, AstNode *node, NodeIndex *data)
{
    // ToOpcodesContext *ctx = _ctx;

    // TODO: Check only one argment
}

void to_opcodes_on_index_end(void *_ctx, AstNode *node, NodeIndex *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_INDEX);
}

void to_opcodes_on_unary_start(void *_ctx, AstNode *node, NodeUnary *data)
{
    // ToOpcodesContext *ctx = _ctx;
}

void to_opcodes_on_unary_end(void *_ctx, AstNode *node, NodeUnary *data)
{
    ToOpcodesContext *ctx = _ctx;

    switch (data->op.kind)
    {
    case UNARY_POS:
        ctx_emit_opcode(ctx, OPCODE_POS);
        break;
    case UNARY_NEG:
        ctx_emit_opcode(ctx, OPCODE_NEG);
        break;
    case UNARY_NOT:
        ctx_emit_opcode(ctx, OPCODE_NOT);
        break;
    case UNARY_CMP:
        ctx_emit_opcode(ctx, OPCODE_CMP);
        break;
    default:
        printf("Operator not supported for compilation: %s\n", unary_op_to_str(data->op.kind));
        break;
    }
}

int to_opcodes_on_binary_start(void *_ctx, AstNode *node, NodeBinary *data)
{
    // ToOpcodesContext *ctx = _ctx;

    switch (data->op.kind)
    {
    case BINARY_ASSIGN:
        if (data->left->kind != AST_IDENTIFIER)
        {
            puts("Only identifier are supported on LHS of assignements");
            return 0;
        }
        return 1;
    default:
        break;
    }
    return 0;
}

void to_opcodes_on_binary_end(void *_ctx, AstNode *node, NodeBinary *data, AstWalkCallbacks *callbacks)
{
    ToOpcodesContext *ctx = _ctx;

    switch (data->op.kind)
    {
    case BINARY_MUL:
        ctx_emit_opcode(ctx, OPCODE_MUL);
        break;
    case BINARY_DIV:
        ctx_emit_opcode(ctx, OPCODE_DIV);
        break;
    case BINARY_MOD:
        ctx_emit_opcode(ctx, OPCODE_MOD);
        break;
    case BINARY_ADD:
        ctx_emit_opcode(ctx, OPCODE_ADD);
        break;
    case BINARY_SUB:
        ctx_emit_opcode(ctx, OPCODE_SUB);
        break;
    case BINARY_SHL:
        ctx_emit_opcode(ctx, OPCODE_SHL);
        break;
    case BINARY_SHLU:
        ctx_emit_opcode(ctx, OPCODE_SHLU);
        break;
    case BINARY_SHR:
        ctx_emit_opcode(ctx, OPCODE_SHR);
        break;
    case BINARY_LT:
        ctx_emit_opcode(ctx, OPCODE_LT);
        break;
    case BINARY_LTE:
        ctx_emit_opcode(ctx, OPCODE_LTE);
        break;
    case BINARY_GT:
        ctx_emit_opcode(ctx, OPCODE_GT);
        break;
    case BINARY_GTE:
        ctx_emit_opcode(ctx, OPCODE_GTE);
        break;
    case BINARY_EQ:
        ctx_emit_opcode(ctx, OPCODE_EQ);
        break;
    case BINARY_NE:
        ctx_emit_opcode(ctx, OPCODE_NE);
        break;
    case BINARY_BAND:
        ctx_emit_opcode(ctx, OPCODE_BAND);
        break;
    case BINARY_BXOR:
        ctx_emit_opcode(ctx, OPCODE_BXOR);
        break;
    case BINARY_BOR:
        ctx_emit_opcode(ctx, OPCODE_BOR);
        break;
    case BINARY_AND:
        ctx_emit_opcode(ctx, OPCODE_AND);
        break;
    case BINARY_OR:
        ctx_emit_opcode(ctx, OPCODE_OR);
        break;
    case BINARY_ASSIGN:
        if (data->left->kind != AST_IDENTIFIER)
        {
            puts("Only identifier are supported on LHS of assignements");
            return;
        }
        ast_walk(ctx, data->right, callbacks);
        ctx_emit_opcode(ctx, OPCODE_STORE_VARIABLE);
        ctx_emit_string(ctx, data->left->data.identifier.name);
        break;
    default:
        printf("Operator not supported for compilation: %s\n", binary_op_to_str(data->op.kind));
        break;
    }
}

void to_opcodes_on_paren_start(void *_ctx, AstNode *node, NodeParen *data)
{
    // Nothing to do here
}

void to_opcodes_on_paren_end(void *_ctx, AstNode *node, NodeParen *data)
{
    // Nothing to do here
}

int to_opcodes_on_source_root_start(void *_ctx, AstNode *node, NodeSourceRoot *data, AstWalkCallbacks *callbacks)
{
    ToOpcodesContext *ctx = _ctx;
    long *func_starts = malloc(sizeof(long) * data->count);
    char **func_names = malloc(sizeof(char *) * data->count);
    long function_count = 0;

    int has_function = 0;

    for (long i = 0; i < data->count; i++)
    {
        if (data->statements[i].kind == AST_FUNCTION)
        {
            has_function = 1;
            break;
        }
    }

    long patch_jump = 0;
    if (has_function)
    {
        ctx_emit_opcode(ctx, OPCODE_JUMP);
        patch_jump = ctx->current_index;
        ctx_emit_long(ctx, 0);
    }

    for (long i = 0; i < data->count; i++)
    {
        if (data->statements[i].kind == AST_FUNCTION)
        {
            func_starts[function_count] = ctx->current_index;
            func_names[function_count] = data->statements[i].data.function.name;
            function_count++;
            ast_walk(ctx, &data->statements[i], callbacks);
        }
    }

    for (long i = 0; i < function_count; i++)
    {
        ctx_emit_opcode(ctx, OPCODE_LOAD_FUNCTION);
        ctx_emit_long(ctx, func_starts[i]);
        ctx_emit_opcode(ctx, OPCODE_STORE_VARIABLE);
        ctx_emit_string(ctx, func_names[i]);
        ctx_emit_opcode(ctx, OPCODE_POP);
    }

    if (patch_jump != 0)
    {
        ctx_patch_long(ctx, patch_jump, ctx->current_index);
    }

    free(func_starts);
    free(func_names);

    for (long i = 0; i < data->count; i++)
    {
        if (data->statements[i].kind != AST_FUNCTION)
        {
            ast_walk(ctx, &data->statements[i], callbacks);
        }
    }

    return 1;
}

void to_opcodes_on_source_root_end(void *_ctx, AstNode *node, NodeSourceRoot *data)
{
    // Nothing to do here
}

void to_opcodes_on_expression_start(void *_ctx, AstNode *node, NodeExpression *data)
{
    // Nothing to do here
}

void to_opcodes_on_expression_end(void *_ctx, AstNode *node, NodeExpression *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_POP);
}

void to_opcodes_on_block_start(void *_ctx, AstNode *node, NodeBlock *data)
{
    // Nothing to do here
}

void to_opcodes_on_block_end(void *_ctx, AstNode *node, NodeBlock *data)
{
    // Nothing to do here
}

int to_opcodes_on_if_start(void *_ctx, AstNode *node, NodeIf *data, AstWalkCallbacks *callbacks)
{
    ToOpcodesContext *ctx = _ctx;

    ast_walk(ctx, data->condition, callbacks);
    ctx_emit_opcode(ctx, OPCODE_JUMP_ZERO);
    long index_to_replace = ctx->current_index;
    ctx_emit_long(ctx, 0);
    ast_walk(ctx, data->on_true, callbacks);
    if (data->on_false != 0)
    {
        ctx_emit_opcode(ctx, OPCODE_JUMP);
        long index_to_replace2 = ctx->current_index;
        ctx_emit_long(ctx, 0);
        ast_walk(ctx, data->on_false, callbacks);
        ctx_patch_long(ctx, index_to_replace2, ctx->current_index);
    }
    ctx_patch_long(ctx, index_to_replace, ctx->current_index);

    return 1;
}

void to_opcodes_on_if_end(void *_ctx, AstNode *node, NodeIf *data)
{
    // Nothing to do here
}

int to_opcodes_on_for_start(void *_ctx, AstNode *node, NodeFor *data, AstWalkCallbacks *callbacks)
{
    ToOpcodesContext *ctx = _ctx;

    long current_breaks_index = ctx->current_break;
    long current_continues_index = ctx->current_continue;

    // run initialization
    if (data->initialization != 0)
    {
        ast_walk(ctx, data->initialization, callbacks);
    }

    // get current label as start label
    long start_label = ctx->current_index;

    // run condition
    long index_jump_end = 0;
    if (data->condition != 0)
    {
        ast_walk(ctx, data->condition, callbacks);

        // jump zero end
        ctx_emit_opcode(ctx, OPCODE_JUMP_ZERO);
        index_jump_end = ctx->current_index;
        ctx_emit_long(ctx, 0);
    }

    // run body
    ast_walk(ctx, data->body, callbacks);

    long continue_label = ctx->current_index;

    // run each
    if (data->each != 0)
    {
        ast_walk(ctx, data->each, callbacks);
    }

    // jump start label
    ctx_emit_opcode(ctx, OPCODE_JUMP);
    ctx_emit_long(ctx, start_label);

    // set end label
    if (index_jump_end != 0)
    {
        ctx_patch_long(ctx, index_jump_end, ctx->current_index);
    }

    for (long i = ctx->current_break - 1; i >= current_breaks_index; i--)
    {
        ctx_patch_long(ctx, ctx->breaks[i], ctx->current_index);
    }

    for (long i = ctx->current_continue - 1; i >= current_continues_index; i--)
    {
        ctx_patch_long(ctx, ctx->continues[i], continue_label);
    }

    return 1;
}

void to_opcodes_on_for_end(void *_ctx, AstNode *node, NodeFor *data)
{
    // Nothing to do here
}

int to_opcodes_on_while_start(void *_ctx, AstNode *node, NodeWhile *data, AstWalkCallbacks *callbacks)
{
    ToOpcodesContext *ctx = _ctx;

    long current_breaks_index = ctx->current_break;
    long current_continues_index = ctx->current_continue;

    long start_label = ctx->current_index;
    ast_walk(ctx, data->condition, callbacks);
    ctx_emit_opcode(ctx, OPCODE_JUMP_ZERO);
    long index_to_replace = ctx->current_index;
    ctx_emit_long(ctx, 0);
    ast_walk(ctx, data->body, callbacks);
    ctx_emit_opcode(ctx, OPCODE_JUMP);
    ctx_emit_long(ctx, start_label);
    ctx_patch_long(ctx, index_to_replace, ctx->current_index);

    for (long i = ctx->current_break - 1; i >= current_breaks_index; i--)
    {
        ctx_patch_long(ctx, ctx->breaks[i], ctx->current_index);
    }

    for (long i = ctx->current_continue - 1; i >= current_continues_index; i--)
    {
        ctx_patch_long(ctx, ctx->continues[i], start_label);
    }

    return 1;
}

void to_opcodes_on_while_end(void *_ctx, AstNode *node, NodeWhile *data)
{
    // Nothing to do here
}

void to_opcodes_on_break(void *_ctx, AstNode *node, NodeBreak *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_JUMP);
    ctx_emit_break_later(ctx);
}

void to_opcodes_on_continue(void *_ctx, AstNode *node, NodeContinue *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_JUMP);
    ctx_emit_continue_later(ctx);
}

void to_opcodes_on_function_start(void *_ctx, AstNode *node, NodeFunction *data)
{
    ToOpcodesContext *ctx = _ctx;

    for (long i = 0; i < data->parameter_count; i++)
    {
        ctx_emit_opcode(ctx, OPCODE_LOAD_ARG);
        ctx_emit_long(ctx, i);
        ctx_emit_opcode(ctx, OPCODE_STORE_VARIABLE);
        ctx_emit_string(ctx, data->parameters[i]);
    }
}

void to_opcodes_on_function_end(void *_ctx, AstNode *node, NodeFunction *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_RETURN);
}

void to_opcodes_on_return_start(void *_ctx, AstNode *node, NodeReturn *data)
{
    // Nothing to do here
}

void to_opcodes_on_return_end(void *_ctx, AstNode *node, NodeReturn *data)
{
    ToOpcodesContext *ctx = _ctx;

    ctx_emit_opcode(ctx, OPCODE_RETURN);
}

AstWalkCallbacks to_opcodes_callbacks()
{
    AstWalkCallbacks value = {0};
    value.on_nop = to_opcodes_on_nop;
    value.on_number = to_opcodes_on_number;
    value.on_boolean = to_opcodes_on_boolean;
    value.on_null = to_opcodes_on_null;
    value.on_string = to_opcodes_on_string;
    value.on_identifier = to_opcodes_on_identifier;
    value.on_args_start = to_opcodes_on_args_start;
    value.on_args_end = to_opcodes_on_args_end;
    value.on_call_start = to_opcodes_on_call_start;
    value.on_call_end = to_opcodes_on_call_end;
    value.on_index_start = to_opcodes_on_index_start;
    value.on_index_end = to_opcodes_on_index_end;
    value.on_unary_start = to_opcodes_on_unary_start;
    value.on_unary_end = to_opcodes_on_unary_end;
    value.on_binary_start = to_opcodes_on_binary_start;
    value.on_binary_end = to_opcodes_on_binary_end;
    value.on_paren_start = to_opcodes_on_paren_start;
    value.on_paren_end = to_opcodes_on_paren_end;
    value.on_source_root_start = to_opcodes_on_source_root_start;
    value.on_source_root_end = to_opcodes_on_source_root_end;
    value.on_expression_start = to_opcodes_on_expression_start;
    value.on_expression_end = to_opcodes_on_expression_end;
    value.on_block_start = to_opcodes_on_block_start;
    value.on_block_end = to_opcodes_on_block_end;
    value.on_if_start = to_opcodes_on_if_start;
    value.on_if_end = to_opcodes_on_if_end;
    value.on_for_start = to_opcodes_on_for_start;
    value.on_for_end = to_opcodes_on_for_end;
    value.on_while_start = to_opcodes_on_while_start;
    value.on_while_end = to_opcodes_on_while_end;
    value.on_break = to_opcodes_on_break;
    value.on_continue = to_opcodes_on_continue;
    value.on_function_start = to_opcodes_on_function_start;
    value.on_function_end = to_opcodes_on_function_end;
    value.on_return_start = to_opcodes_on_return_start;
    value.on_return_end = to_opcodes_on_return_end;
    return value;
}

CompilationResult compile_ast(AstNode *ast)
{
    CompilationResult result;

    AstWalkCallbacks callbacks = to_opcodes_callbacks();
    ToOpcodesContext ctx = {0};

    ast_walk(&ctx, ast, &callbacks);

    char *opcodes = malloc(ctx.current_index + 1);
    memcpy(opcodes, ctx.opcodes, ctx.current_index);
    opcodes[ctx.current_index] = '\0';

    result.opcodes = opcodes;
    result.size = ctx.current_index;
    result.strings = ctx.strings;
    return result;
}
