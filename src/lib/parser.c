#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

long token_string(char *source, long size, long index, char *str)
{
    long len = strlen(str);
    return size - index >= len && !memcmp(source + index, str, len);
}

long token_one_digit(ParseContext *ctx, char *source, long size, long index)
{
    char cur = source[index];
    if (cur >= '0' && cur <= '9')
    {
        return 1;
    }
    return 0;
}

long token_digits(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    while (token_one_digit(ctx, source, size, current_index))
    {
        current_index++;
    }
    return current_index - index;
}

long token_one_letter(ParseContext *ctx, char *source, long size, long index)
{
    char cur = source[index];
    if ((cur >= 'a' && cur <= 'z') || (cur >= 'A' && cur <= 'Z'))
    {
        return 1;
    }
    return 0;
}

long token_one_letter_or_underscore(ParseContext *ctx, char *source, long size, long index)
{
    char cur = source[index];
    if (token_one_letter(ctx, source, size, index) || cur == '_')
    {
        return 1;
    }
    return 0;
}

long token_one_alnum_or_underscore(ParseContext *ctx, char *source, long size, long index)
{
    if (token_one_letter_or_underscore(ctx, source, size, index) || token_one_digit(ctx, source, size, index))
    {
        return 1;
    }
    return 0;
}

long token_alnum_or_underscore(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    while (token_one_alnum_or_underscore(ctx, source, size, current_index))
    {
        current_index++;
    }
    return current_index - index;
}

long token_letters(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    while (token_one_letter(ctx, source, size, current_index))
    {
        current_index++;
    }
    return current_index - index;
}

long token_spaces(ParseContext *ctx, char *source, long size, long index)
{
    for (long i = index; i < size; i++)
    {
        if (source[i] != ' ' && source[i] != '\n' && source[i] != '\t' && source[i] != '\r')
        {
            return i - index;
        }
    }
    return size - index;
}

long token_identifier(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    if (!token_one_letter_or_underscore(ctx, source, size, current_index))
    {
        return 0;
    }

    current_index++;
    current_index += token_alnum_or_underscore(ctx, source, size, current_index);

    return current_index - index;
}

AstUnaryOperator parse_unary_operator(ParseContext *ctx, char *source, long size, long index)
{
    char cur = source[index];
    AstUnaryOperator result;
    result.kind = UNARY_NONE;
    // TODO handle multi char operators
    result.length = 1;
    switch (cur)
    {
    case '-':
        result.kind = UNARY_NEG;
        break;
    case '+':
        result.kind = UNARY_POS;
        break;
    case '!':
        result.kind = UNARY_NOT;
        break;
    case '~':
        result.kind = UNARY_CMP;
        break;
    default:
        result.kind = UNARY_NONE;
        break;
    }
    return result;
}

typedef struct
{
    int precedence;
    int length;
    int right_associative;
} OperatorInfo;

OperatorInfo operator_info(AstBinaryOperatorKind op)
{
    OperatorInfo result = {0};
    switch (op)
    {
    case BINARY_MUL:
        result.precedence = 5;
        result.length = 1;
        break;
    case BINARY_DIV:
        result.precedence = 5;
        result.length = 1;
        break;
    case BINARY_MOD:
        result.precedence = 5;
        result.length = 1;
        break;
    case BINARY_ADD:
        result.precedence = 6;
        result.length = 1;
        break;
    case BINARY_SUB:
        result.precedence = 6;
        result.length = 1;
        break;
    case BINARY_SHL:
        result.precedence = 7;
        result.length = 2;
        break;
    case BINARY_SHLU:
        result.precedence = 7;
        result.length = 3;
        break;
    case BINARY_SHR:
        result.precedence = 7;
        result.length = 2;
        break;
    case BINARY_LT:
        result.precedence = 8;
        result.length = 1;
        break;
    case BINARY_LTE:
        result.precedence = 8;
        result.length = 2;
        break;
    case BINARY_GT:
        result.precedence = 8;
        result.length = 1;
        break;
    case BINARY_GTE:
        result.precedence = 8;
        result.length = 2;
        break;
    case BINARY_EQ:
        result.precedence = 9;
        result.length = 2;
        break;
    case BINARY_NE:
        result.precedence = 9;
        result.length = 2;
        break;
    case BINARY_BAND:
        result.precedence = 10;
        result.length = 1;
        break;
    case BINARY_BXOR:
        result.precedence = 11;
        result.length = 1;
        break;
    case BINARY_BOR:
        result.precedence = 12;
        result.length = 1;
        break;
    case BINARY_AND:
        result.precedence = 13;
        result.length = 2;
        break;
    case BINARY_OR:
        result.precedence = 14;
        result.length = 2;
        break;
    case BINARY_ASSIGN:
        result.precedence = 15;
        result.length = 1;
        result.right_associative = 1;
        break;
    case BINARY_NONE:
        break;
    }
    return result;
}

AstBinaryOperator parse_binary_operator(ParseContext *ctx, char *source, long size, long index)
{
    /*
        List of operators by precedence:

         5: * / %
         6: + -
         7: << >> <<<
         8: < <= > >=
         9: == !=
        10: &
        11: ^
        12: |
        13: &&
        14: ||
        15: =
   */
    AstBinaryOperator result;
    result.kind = BINARY_NONE;
    result.precedence = 0;
    result.right_associative = 0;
    switch (source[index])
    {
    case '*':
        result.kind = BINARY_MUL;
        break;
    case '/':
        result.kind = BINARY_DIV;
        break;
    case '%':
        result.kind = BINARY_MOD;
        break;

    case '+':
        result.kind = BINARY_ADD;
        break;
    case '-':
        result.kind = BINARY_SUB;
        break;
    case '^':
        result.kind = BINARY_BXOR;
        break;

    case '<':
        result.kind = BINARY_LT;
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '<':
                if (index + 2 < size && source[index + 2] == '<')
                {
                    result.kind = BINARY_SHLU;
                }
                else
                {
                    result.kind = BINARY_SHL;
                }
                break;
            case '=':
                result.kind = BINARY_LTE;
                break;
            }
        }
        break;
    case '>':
        result.kind = BINARY_GT;
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '>':
                result.kind = BINARY_SHR;
                break;
            case '=':
                result.kind = BINARY_GTE;
                break;
            }
        }
        break;
    case '=':
        result.kind = BINARY_ASSIGN;
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '=':
                result.kind = BINARY_EQ;
                break;
            }
        }
        break;
    case '&':
        result.kind = BINARY_BAND;
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '&':
                result.kind = BINARY_AND;
                break;
            }
        }
        break;
    case '|':
        result.kind = BINARY_BOR;
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '|':
                result.kind = BINARY_OR;
                break;
            }
        }
        break;
    case '!':
        if (index + 1 < size)
        {
            switch (source[index + 1])
            {
            case '=':
                result.kind = BINARY_NE;
                break;
            }
        }
        break;
    }

    OperatorInfo op_info = operator_info(result.kind);
    result.precedence = op_info.precedence;
    result.length = op_info.length;
    result.right_associative = op_info.right_associative;
    return result;
}

AstNode *parse_literal_number(ParseContext *ctx, char *source, long size, long index)
{
    long len = token_digits(ctx, source, size, index);
    if (len == 0)
    {
        return 0;
    }

    AstNode *node = create_ast_node(AST_NUMBER, source, index);
    node->position.length = len;
    double value;
    sscanf_s(source + index, "%lf", &value);
    node->data.number.value = value;

    return node;
}

AstNode *parse_literal_boolean(ParseContext *ctx, char *source, long size, long index)
{
    AstNode *node = create_ast_node(AST_BOOLEAN, source, index);

    if (size - index >= 4 && !memcmp(source + index, "true", 4))
    {
        node->position.length = 4;
        node->data.boolean.value = 1;
        return node;
    }

    if (size - index >= 5 && !memcmp(source + index, "false", 5))
    {
        node->position.length = 5;
        node->data.boolean.value = 0;
        return node;
    }

    free(node);
    return 0;
}

AstNode *parse_literal_null(ParseContext *ctx, char *source, long size, long index)
{
    if (size - index >= 4 && !memcmp(source + index, "null", 4))
    {
        AstNode *node = create_ast_node(AST_NULL, source, index);
        node->position.length = 4;
        return node;
    }

    return 0;
}

AstNode *parse_literal_string(ParseContext *ctx, char *source, long size, long index)
{
    // TODO: Escape characters
    // TODO: Single quotes ?

    long current_index = index;

    if (source[index] == '"')
    {
        AstNode *node = create_ast_node(AST_STRING, source, index);

        current_index++;
        while (current_index < size && source[current_index] != '"')
        {
            current_index++;
        }

        if (source[current_index] == '"')
        {
            current_index++;
        }

        node->position.length = current_index - index;
        long string_size = current_index - index - 2;
        char *value = malloc(string_size + 1);
        memcpy(value, source + index + 1, string_size);
        value[string_size] = '\0';
        node->data.string.value = value;
        return node;
    }

    return 0;
}

AstNode *parse_identifier(ParseContext *ctx, char *source, long size, long index)
{
    long length = token_identifier(ctx, source, size, index);
    if (length == 0)
    {
        return 0;
    }

    AstNode *node = create_ast_node(AST_IDENTIFIER, source, index);
    node->position.length = length;

    node->data.identifier.name = malloc(length + 1);
    memcpy(node->data.identifier.name, source + index, length);
    node->data.identifier.name[length] = '\0';

    return node;
}

AstNode *parse_expression(ParseContext *ctx, char *source, long size, long index);

AstNode *parse_parenthesis(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    if (source[current_index] == '(')
    {
        current_index++;

        current_index += token_spaces(ctx, source, size, current_index);

        AstNode *inner_node = parse_expression(ctx, source, size, current_index);
        if (inner_node == 0)
        {
            printf("failed to parse expression: %s\n", source + current_index);
            return 0;
        }
        current_index += inner_node->position.length;

        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] != ')')
        {
            printf("expected ')'\n");
            return 0;
        }

        current_index++;

        AstNode *node = create_ast_node(AST_PAREN, source, index);

        node->position.length = current_index - index;
        node->data.paren.expr = inner_node;

        return node;
    }
    else
    {
        AstNode *node;

        node = parse_literal_number(ctx, source, size, index);
        if (node != 0)
        {
            return node;
        }

        node = parse_literal_boolean(ctx, source, size, index);
        if (node != 0)
        {
            return node;
        }

        node = parse_literal_null(ctx, source, size, index);
        if (node != 0)
        {
            return node;
        }

        node = parse_literal_string(ctx, source, size, index);
        if (node != 0)
        {
            return node;
        }

        node = parse_identifier(ctx, source, size, index);
        if (node != 0)
        {
            return node;
        }

        // TODO: Parse error
        return 0;
    }
}

typedef struct
{
    AstNode *node;
    int is_call;
} ParseArgumentListReturn;

ParseArgumentListReturn parse_argument_list(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;
    ParseArgumentListReturn result = {0};

    if (source[current_index] == '(' || source[current_index] == '[')
    {
        char end_char = ']';

        if (source[current_index] == '(')
        {
            end_char = ')';
            result.is_call = 1;
        }
        current_index++;

        current_index += token_spaces(ctx, source, size, current_index);

        AstNode *node = create_ast_node(AST_ARGS, source, index);
        node->position.length = current_index - index;
        node->data.args.args = 0;
        node->data.args.count = 0;

        if (source[current_index] == end_char)
        {
            current_index++;
            node->position.length = current_index - index;
            result.node = node;
            return result;
        }

        // TODO: use a growable list
        AstNode *args = malloc(100 * sizeof(AstNode));
        long count = 0;

        while (1)
        {
            AstNode *arg = parse_expression(ctx, source, size, current_index);
            if (arg == 0)
            {
                result.node = 0;
                return result;
            }
            current_index += arg->position.length;
            args[count++] = *arg;

            current_index += token_spaces(ctx, source, size, current_index);

            if (source[current_index] == end_char)
            {
                current_index++;
                break;
            }
            else if (source[current_index] == ',')
            {
                current_index++;
                current_index += token_spaces(ctx, source, size, current_index);
                continue;
            }
            result.node = 0;
            return result;
        }

        node->data.args.count = count;
        node->data.args.args = args;
        node->position.length = current_index - index;

        result.node = node;
        return result;
    }

    result.node = 0;
    return result;
}

AstNode *parse_unary(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    AstUnaryOperator op = parse_unary_operator(ctx, source, size, index);
    if (op.kind == UNARY_NONE)
    {
        return parse_parenthesis(ctx, source, size, index);
    }

    current_index += op.length;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *expr = parse_unary(ctx, source, size, current_index);
    current_index += expr->position.length;

    AstNode *node = create_ast_node(AST_UNARY, source, index);

    node->position.length = current_index - index;
    node->data.unary.expr = expr;
    node->data.unary.op = op;

    return node;
}

AstNode *parse_call(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    AstNode *node = parse_unary(ctx, source, size, index);
    if (node == 0)
    {
        return 0;
    }

    current_index = index + node->position.length;

    current_index += token_spaces(ctx, source, size, current_index);

    ParseArgumentListReturn result;
    while ((result = parse_argument_list(ctx, source, size, current_index), result.node))
    {
        AstNode *args = result.node;
        current_index += args->position.length;
        AstNode *call_node = create_ast_node(AST_NONE, source, index); // Node kind not known
        call_node->position.length = current_index - index;

        if (result.is_call)
        {
            call_node->kind = AST_CALL;
            call_node->data.call.callee = node;
            call_node->data.call.args = args;
        }
        else
        {
            call_node->kind = AST_INDEX;
            call_node->data.index.indexable = node;
            call_node->data.index.index = args;
        }

        node = call_node;

        current_index += token_spaces(ctx, source, size, current_index);
    }

    return node;
}

AstNode *parse_binary(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    AstNode *left = parse_call(ctx, source, size, current_index);
    if (left == 0)
    {
        printf("failed to parse left expression\n");
        return 0;
    }
    current_index += left->position.length;
    current_index += token_spaces(ctx, source, size, current_index);

    AstBinaryOperator op = parse_binary_operator(ctx, source, size, current_index);
    if (op.kind == BINARY_NONE)
    {
        // if no operator was parsed, return left as a standalone expression
        return left;
    }
    current_index += op.length;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *right = parse_expression(ctx, source, size, current_index);
    if (right == 0)
    {
        printf("failed to parse right\n");
        return 0;
    }
    current_index += right->position.length;

    AstNode *node = create_ast_node(AST_BINARY, source, index);
    node->position.length = current_index - index;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->data.binary.op = op;

    // TODO: what happens when only one is left associative ?
    // TODO: always when precedence is higher on the left ?
    if (right->kind == AST_BINARY &&
        ((op.precedence == right->data.binary.op.precedence &&
          !op.right_associative &&
          !right->data.binary.op.right_associative) ||
         (op.precedence < right->data.binary.op.precedence)))
    {
        node->data.binary.right = right->data.binary.left;
        right->data.binary.left = node;
        right->position.length += right->position.index - node->position.index;
        right->position.index = node->position.index;
        node = right;
    }

    return node;
}

AstNode *parse_expression(ParseContext *ctx, char *source, long size, long index)
{
    return parse_binary(ctx, source, size, index);
}

AstNode *parse_statement(ParseContext *ctx, char *source, long size, long index);

AstNode *parse_block(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (source[current_index] != '{')
    {
        return 0;
    }

    current_index++;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *node = create_ast_node(AST_BLOCK, source, current_index);

    if (source[current_index] == '}')
    {
        current_index++;
        node->position.length = current_index - index;
        node->data.block.statements = 0;
        node->data.block.count = 0;
        return node;
    }

    // TODO: Use a growable data structure
    AstNode *statements = malloc(sizeof(AstNode) * 100);
    long count = 0;

    while (current_index < size)
    {
        AstNode *statement = parse_statement(ctx, source, size, current_index);
        if (statement == 0)
        {
            return 0;
        }

        statements[count] = *statement;
        // TODO: free statement ast node, or use an array of pointers
        count++;

        current_index += statement->position.length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] == '}')
        {
            current_index++;
            node->position.length = current_index - index;
            node->data.block.statements = statements;
            node->data.block.count = count;
            return node;
        }

        current_index += token_spaces(ctx, source, size, current_index);
    }

    // TODO: parse error, expected } ?
    return 0;
}

AstNode *parse_if(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (!token_string(source, size, current_index, "if"))
    {
        // TODO parse error
        return 0;
    }

    current_index += 2;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != '(')
    {
        // TODO expected '('
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *condition = parse_expression(ctx, source, size, current_index);
    if (condition == 0)
    {
        return 0;
    }

    current_index += condition->position.length;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != ')')
    {
        // TODO expected ')'
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *on_true = parse_statement(ctx, source, size, current_index);
    if (on_true == 0)
    {
        return 0;
    }

    current_index += on_true->position.length;

    AstNode *on_false = 0;

    long current_index_before_potential_else = current_index + token_spaces(ctx, source, size, current_index);

    if (token_string(source, size, current_index_before_potential_else, "else"))
    {
        current_index = current_index_before_potential_else;
        current_index += 4;
        current_index += token_spaces(ctx, source, size, current_index);

        on_false = parse_statement(ctx, source, size, current_index);
        if (on_false == 0)
        {
            return 0;
        }

        current_index += on_false->position.length;
    }

    AstNode *node = create_ast_node(AST_IF, source, index);
    node->position.length = current_index - index;
    node->data.data_if.condition = condition;
    node->data.data_if.on_true = on_true;
    node->data.data_if.on_false = on_false;

    return node;
}

AstNode *parse_for(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (!token_string(source, size, current_index, "for"))
    {
        // TODO parse error
        return 0;
    }

    current_index += 3;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != '(')
    {
        // TODO expected '('
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *initialization = 0;

    if (source[current_index] != ';')
    {
        initialization = parse_expression(ctx, source, size, current_index);
        if (initialization == 0)
        {
            return 0;
        }
        current_index += initialization->position.length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] != ';')
        {
            // TODO expected ';'
            return 0;
        }
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *condition = 0;
    if (source[current_index] != ';')
    {
        condition = parse_expression(ctx, source, size, current_index);
        if (condition == 0)
        {
            return 0;
        }

        current_index += condition->position.length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] != ';')
        {
            // TODO expected ';'
            return 0;
        }
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *each = 0;

    if (source[current_index] != ')')
    {
        each = parse_expression(ctx, source, size, current_index);
        if (each == 0)
        {
            return 0;
        }

        current_index += each->position.length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] != ')')
        {
            // TODO expected ')'
            return 0;
        }
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *body = parse_statement(ctx, source, size, current_index);
    if (body == 0)
    {
        return 0;
    }

    current_index += body->position.length;

    AstNode *node = create_ast_node(AST_FOR, source, index);
    node->position.length = current_index - index;
    node->data.data_for.initialization = initialization;
    node->data.data_for.condition = condition;
    node->data.data_for.each = each;
    node->data.data_for.body = body;

    return node;
}

AstNode *parse_while(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (!token_string(source, size, current_index, "while"))
    {
        // TODO parse error
        return 0;
    }

    current_index += 5;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != '(')
    {
        // TODO expected '('
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *condition = parse_expression(ctx, source, size, current_index);
    if (condition == 0)
    {
        return 0;
    }

    current_index += condition->position.length;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != ')')
    {
        // TODO expected ')'
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *body = parse_statement(ctx, source, size, current_index);
    if (body == 0)
    {
        return 0;
    }

    current_index += body->position.length;

    AstNode *node = create_ast_node(AST_WHILE, source, index);
    node->position.length = current_index - index;
    node->data.data_while.condition = condition;
    node->data.data_while.body = body;

    return node;
}

AstNode *parse_function(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (!token_string(source, size, current_index, "function"))
    {
        // TODO parse error
        return 0;
    }

    current_index += 8;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *identifier = parse_identifier(ctx, source, size, current_index);
    if (identifier == 0)
    {
        // TODO: parse error expected identifier
        return 0;
    }

    char *name = identifier->data.identifier.name;

    current_index += identifier->position.length;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != '(')
    {
        // TODO expected '('
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    long count = 0;
    // TODO: use a growable array
    char **parameters = malloc(sizeof(char *) * 100);

    long length = 0;

    while ((length = token_identifier(ctx, source, size, current_index)) != 0)
    {
        char *parameter_name = malloc(sizeof(char) * length + 1);
        memcpy(parameter_name, source + current_index, length);
        parameter_name[length] = '\0';
        parameters[count] = parameter_name;
        count++;

        current_index += length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] == ',')
        {
            current_index++;
            current_index += token_spaces(ctx, source, size, current_index);
            continue;
        }
        else
        {
            break;
        }
    }

    if (source[current_index] != ')')
    {
        // TODO expected ')' or ','
        return 0;
    }

    current_index += 1;
    current_index += token_spaces(ctx, source, size, current_index);

    AstNode *body = parse_block(ctx, source, size, current_index);
    if (body == 0)
    {
        return 0;
    }

    current_index += body->position.length;

    AstNode *node = create_ast_node(AST_FUNCTION, source, index);
    node->position.length = current_index - index;
    node->data.function.name = name;
    node->data.function.body = body;
    node->data.function.parameter_count = count;
    node->data.function.parameters = parameters;

    return node;
}

AstNode *parse_return(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (!token_string(source, size, current_index, "return"))
    {
        // TODO: parse error, expected return
        return 0;
    }
    current_index += 6;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] == ';')
    {
        current_index++;
        AstNode *node = create_ast_node(AST_RETURN, source, index);
        node->position.length = current_index - index;
        node->data.data_return.expr = 0;
        return node;
    }

    AstNode *expr = parse_expression(ctx, source, size, current_index);
    if (expr == 0)
    {
        return 0;
    }
    current_index += expr->position.length;
    current_index += token_spaces(ctx, source, size, current_index);

    if (source[current_index] != ';')
    {
        // TODO: parse error, expected ';';
        return 0;
    }
    current_index++;
    AstNode *node = create_ast_node(AST_RETURN, source, index);
    node->position.length = current_index - index;
    node->data.data_return.expr = expr;
    return node;
}

AstNode *parse_statement(ParseContext *ctx, char *source, long size, long index)
{
    long current_index = index;

    if (source[current_index] == '{')
    {
        return parse_block(ctx, source, size, current_index);
    }
    else if (source[current_index] == ';')
    {
        AstNode *node = create_ast_node(AST_NOP, source, index);
        node->position.length = 1;
        return node;
    }
    else if (token_string(source, size, current_index, "if"))
    {
        return parse_if(ctx, source, size, current_index);
    }
    else if (token_string(source, size, current_index, "for"))
    {
        return parse_for(ctx, source, size, current_index);
    }
    else if (token_string(source, size, current_index, "while"))
    {
        return parse_while(ctx, source, size, current_index);
    }
    else if (token_string(source, size, current_index, "break"))
    {
        current_index += 5;
        current_index += token_spaces(ctx, source, size, current_index);
        if (source[current_index] != ';')
        {
            // TODO: parse error, expected ';';
            return 0;
        }
        current_index++;
        AstNode *node = create_ast_node(AST_BREAK, source, index);
        node->position.length = current_index - index;
        return node;
    }
    else if (token_string(source, size, current_index, "continue"))
    {
        current_index += 8;
        current_index += token_spaces(ctx, source, size, current_index);
        if (source[current_index] != ';')
        {
            // TODO: parse error, expected ';';
            return 0;
        }
        current_index++;
        AstNode *node = create_ast_node(AST_CONTINUE, source, index);
        node->position.length = current_index - index;
        return node;
    }
    else if (token_string(source, size, current_index, "function"))
    {
        return parse_function(ctx, source, size, current_index);
    }
    else if (token_string(source, size, current_index, "return"))
    {
        return parse_return(ctx, source, size, current_index);
    }
    else
    {
        AstNode *expr = 0;

        expr = parse_expression(ctx, source, size, current_index);
        if (expr == 0)
        {
            return 0;
        }

        current_index += expr->position.length;
        current_index += token_spaces(ctx, source, size, current_index);

        if (source[current_index] != ';')
        {
            puts("Expected ';'");
            return 0;
        }

        current_index++;

        AstNode *statement = create_ast_node(AST_EXPRESSION, source, index);
        statement->position.length = current_index - index;
        statement->data.expression.expr = expr;

        return statement;
    }
}

AstNode *parse_source_root(char *source, long size)
{
    ParseContext ctx;
    long current_index = 0;

    current_index += token_spaces(&ctx, source, size, current_index);

    AstNode *node = create_ast_node(AST_SOURCE_ROOT, source, current_index);

    // TODO: Use a growable data structure
    AstNode *statements = malloc(sizeof(AstNode) * 100);
    long count = 0;

    while (current_index < size)
    {
        AstNode *statement = parse_statement(&ctx, source, size, current_index);
        if (statement == 0)
        {
            return 0;
        }

        statements[count] = *statement;
        // TODO: free statement or use array of pointers
        count++;
        current_index += statement->position.length;
        current_index += token_spaces(&ctx, source, size, current_index);
    }

    node->position.length = current_index;
    node->data.source_root.statements = statements;
    node->data.source_root.count = count;

    return node;
}
