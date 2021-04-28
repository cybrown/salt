#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "ast.h"

AstNode *create_ast_node(AstNodeKind kind, char *source, long index)
{
    AstNode *node = malloc(sizeof(AstNode));
    memset(node, 0, sizeof(AstNode));
    node->kind = kind;
    node->position.source = source;
    node->position.index = index;
    return node;
}

char *unary_op_to_str(AstUnaryOperatorKind kind)
{
    switch (kind)
    {
    case UNARY_NONE:
        return "<unary op none>";
    case UNARY_POS:
        return "+";
    case UNARY_NEG:
        return "-";
    case UNARY_NOT:
        return "!";
    case UNARY_CMP:
        return "~";
    default:
        return "<unknown unary operator>";
    }
}

char *binary_op_to_str(AstBinaryOperatorKind kind)
{
    switch (kind)
    {
    case BINARY_NONE:
        return "<binary op none>";
    case BINARY_MUL:
        return "*";
    case BINARY_DIV:
        return "/";
    case BINARY_MOD:
        return "%";
    case BINARY_ADD:
        return "+";
    case BINARY_SUB:
        return "-";
    case BINARY_SHL:
        return "<<";
    case BINARY_SHLU:
        return "<<<";
    case BINARY_SHR:
        return ">>";
    case BINARY_LT:
        return "<";
    case BINARY_LTE:
        return "<=";
    case BINARY_GT:
        return ">";
    case BINARY_GTE:
        return ">=";
    case BINARY_EQ:
        return "==";
    case BINARY_NE:
        return "!=";
    case BINARY_BAND:
        return "&";
    case BINARY_BXOR:
        return "^";
    case BINARY_BOR:
        return "|";
    case BINARY_AND:
        return "&&";
    case BINARY_OR:
        return "||";
    case BINARY_ASSIGN:
        return "=";
    default:
        return "<unknown binary operator>";
    }
}

void ast_walk(void *ctx, AstNode *ast, AstWalkCallbacks *callbacks)
{
    switch (ast->kind)
    {
    case AST_NOP:
        if (callbacks->on_nop == 0)
        {
            puts("on_nop not defined");
            return;
        }
        callbacks->on_nop(ctx, ast);
        break;
    case AST_NUMBER:
        if (callbacks->on_number == 0)
        {
            puts("on_number not defined");
            return;
        }
        callbacks->on_number(ctx, ast, &ast->data.number);
        break;
    case AST_BOOLEAN:
        if (callbacks->on_boolean == 0)
        {
            puts("on_boolean not defined");
            return;
        }
        callbacks->on_boolean(ctx, ast, &ast->data.boolean);
        break;
    case AST_NULL:
        if (callbacks->on_null == 0)
        {
            puts("on_null not defined");
            return;
        }
        callbacks->on_null(ctx, ast);
        break;
    case AST_STRING:
        if (callbacks->on_string == 0)
        {
            puts("on_string not defined");
            return;
        }
        callbacks->on_string(ctx, ast, &ast->data.string);
        break;
    case AST_IDENTIFIER:
        if (callbacks->on_identifier == 0)
        {
            puts("on_identifier not defined");
            return;
        }
        callbacks->on_identifier(ctx, ast, &ast->data.identifier);
        break;
    case AST_CALL:
        if (callbacks->on_call_start == 0)
        {
            puts("on_call_start not defined");
            return;
        }
        callbacks->on_call_start(ctx, ast, &ast->data.call);
        ast_walk(ctx, ast->data.call.callee, callbacks);
        ast_walk(ctx, ast->data.call.args, callbacks);
        if (callbacks->on_call_end == 0)
        {
            puts("on_call_end not defined");
            return;
        }
        callbacks->on_call_end(ctx, ast, &ast->data.call);
        break;
    case AST_INDEX:
        if (callbacks->on_index_start == 0)
        {
            puts("on_index_start not defined");
            return;
        }
        callbacks->on_index_start(ctx, ast, &ast->data.index);
        ast_walk(ctx, ast->data.index.indexable, callbacks);
        ast_walk(ctx, ast->data.index.index, callbacks);
        if (callbacks->on_index_end == 0)
        {
            puts("on_index_end not defined");
            return;
        }
        callbacks->on_index_end(ctx, ast, &ast->data.index);
        break;
    case AST_ARGS:
        if (callbacks->on_args_start == 0)
        {
            puts("on_args_start not defined");
            return;
        }
        callbacks->on_args_start(ctx, ast, &ast->data.args);
        for (int i = 0; i < ast->data.args.count; i++)
        {
            ast_walk(ctx, &ast->data.args.args[i], callbacks);
        }
        if (callbacks->on_args_end == 0)
        {
            puts("on_args_end not defined");
            return;
        }
        callbacks->on_args_end(ctx, ast, &ast->data.args);
        break;
    case AST_UNARY:
        if (callbacks->on_unary_start == 0)
        {
            puts("on_unary_start not defined");
            return;
        }
        callbacks->on_unary_start(ctx, ast, &ast->data.unary);
        ast_walk(ctx, ast->data.unary.expr, callbacks);
        if (callbacks->on_unary_end == 0)
        {
            puts("on_unary_end not defined");
            return;
        }
        callbacks->on_unary_end(ctx, ast, &ast->data.unary);
        break;
    case AST_BINARY:
        if (callbacks->on_binary_start == 0)
        {
            puts("on_binary_start not defined");
            return;
        }
        int skip_children = callbacks->on_binary_start(ctx, ast, &ast->data.binary);
        if (!skip_children)
        {
            ast_walk(ctx, ast->data.binary.left, callbacks);
            ast_walk(ctx, ast->data.binary.right, callbacks);
        }
        if (callbacks->on_binary_end == 0)
        {
            puts("on_binary_end not defined");
            return;
        }
        callbacks->on_binary_end(ctx, ast, &ast->data.binary, callbacks);
        break;
    case AST_PAREN:
        if (callbacks->on_paren_start == 0)
        {
            puts("on_paren_start not defined");
            return;
        }
        callbacks->on_paren_start(ctx, ast, &ast->data.paren);
        ast_walk(ctx, ast->data.paren.expr, callbacks);
        if (callbacks->on_paren_end == 0)
        {
            puts("on_paren_end not defined");
            return;
        }
        callbacks->on_paren_end(ctx, ast, &ast->data.paren);
        break;
    case AST_SOURCE_ROOT:
        if (callbacks->on_source_root_start == 0)
        {
            puts("on_source_root_start not defined");
            return;
        }
        if (!callbacks->on_source_root_start(ctx, ast, &ast->data.source_root, callbacks))
        {
            for (long i = 0; i < ast->data.source_root.count; i++)
            {
                ast_walk(ctx, ast->data.source_root.statements + i, callbacks);
            }
        }
        if (callbacks->on_source_root_end == 0)
        {
            puts("on_source_root_end not defined");
            return;
        }
        callbacks->on_source_root_end(ctx, ast, &ast->data.source_root);
        break;
    case AST_EXPRESSION:
        if (callbacks->on_expression_start == 0)
        {
            puts("on_expression_start not defined");
            return;
        }
        callbacks->on_expression_start(ctx, ast, &ast->data.expression);
        ast_walk(ctx, ast->data.expression.expr, callbacks);
        if (callbacks->on_expression_end == 0)
        {
            puts("on_expression_end not defined");
            return;
        }
        callbacks->on_expression_end(ctx, ast, &ast->data.expression);
        break;
    case AST_BLOCK:
        if (callbacks->on_block_start == 0)
        {
            puts("on_block_start not defined");
            return;
        }
        callbacks->on_block_start(ctx, ast, &ast->data.block);
        for (long i = 0; i < ast->data.block.count; i++)
        {
            ast_walk(ctx, ast->data.block.statements + i, callbacks);
        }
        if (callbacks->on_block_end == 0)
        {
            puts("on_block_end not defined");
            return;
        }
        callbacks->on_block_end(ctx, ast, &ast->data.block);
        break;
    case AST_IF:
        if (callbacks->on_if_start == 0)
        {
            puts("on_if_start not defined");
            return;
        }
        if (!callbacks->on_if_start(ctx, ast, &ast->data.data_if, callbacks))
        {
            ast_walk(ctx, ast->data.data_if.condition, callbacks);
            ast_walk(ctx, ast->data.data_if.on_true, callbacks);
            if (ast->data.data_if.on_false != 0)
            {
                ast_walk(ctx, ast->data.data_if.on_false, callbacks);
            }
        }
        if (callbacks->on_if_end == 0)
        {
            puts("on_if_end not defined");
            return;
        }
        callbacks->on_if_end(ctx, ast, &ast->data.data_if);
        break;
    case AST_FOR:
        if (callbacks->on_for_start == 0)
        {
            puts("on_for_start not defined");
            return;
        }
        if (!callbacks->on_for_start(ctx, ast, &ast->data.data_for, callbacks))
        {
            if (ast->data.data_for.initialization != 0)
            {
                ast_walk(ctx, ast->data.data_for.initialization, callbacks);
            }
            if (ast->data.data_for.condition != 0)
            {
                ast_walk(ctx, ast->data.data_for.condition, callbacks);
            }
            if (ast->data.data_for.each != 0)
            {
                ast_walk(ctx, ast->data.data_for.each, callbacks);
            }
            ast_walk(ctx, ast->data.data_for.body, callbacks);
        }
        if (callbacks->on_for_end == 0)
        {
            puts("on_for_end not defined");
            return;
        }
        callbacks->on_for_end(ctx, ast, &ast->data.data_for);
        break;
    case AST_WHILE:
        if (callbacks->on_while_start == 0)
        {
            puts("on_while_start not defined");
            return;
        }
        if (!callbacks->on_while_start(ctx, ast, &ast->data.data_while, callbacks))
        {
            ast_walk(ctx, ast->data.data_while.condition, callbacks);
            ast_walk(ctx, ast->data.data_while.body, callbacks);
        }
        if (callbacks->on_while_end == 0)
        {
            puts("on_while_end not defined");
            return;
        }
        callbacks->on_while_end(ctx, ast, &ast->data.data_while);
        break;
    case AST_BREAK:
        if (callbacks->on_break == 0)
        {
            puts("on_break not defined");
            return;
        }
        callbacks->on_break(ctx, ast, &ast->data.data_break);
        break;
    case AST_CONTINUE:
        if (callbacks->on_continue == 0)
        {
            puts("on_continue not defined");
            return;
        }
        callbacks->on_continue(ctx, ast, &ast->data.data_continue);
        break;
    case AST_FUNCTION:
        if (callbacks->on_function_start == 0)
        {
            puts("on_function_start not defined");
            return;
        }
        callbacks->on_function_start(ctx, ast, &ast->data.function);
        ast_walk(ctx, ast->data.function.body, callbacks);
        if (callbacks->on_function_end == 0)
        {
            puts("on_function_end not defined");
            return;
        }
        callbacks->on_function_end(ctx, ast, &ast->data.function);
        break;
    case AST_RETURN:
        if (callbacks->on_return_start == 0)
        {
            puts("on_return_start not defined");
            return;
        }
        callbacks->on_return_start(ctx, ast, &ast->data.data_return);
        if (ast->data.data_return.expr != 0)
        {
            ast_walk(ctx, ast->data.data_return.expr, callbacks);
        }
        if (callbacks->on_return_end == 0)
        {
            puts("on_return_end not defined");
            return;
        }
        callbacks->on_return_end(ctx, ast, &ast->data.data_return);
        break;
    default:
        puts("walk not implemented for node");
    }
}
