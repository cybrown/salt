#ifndef H_AST
#define H_AST

typedef enum
{
    AST_NONE,
    AST_SOURCE_ROOT,
    AST_NUMBER,
    AST_BOOLEAN,
    AST_NULL,
    AST_STRING,
    AST_IDENTIFIER,
    AST_UNARY,
    AST_BINARY,
    AST_CALL,
    AST_INDEX,
    AST_ARGS,
    AST_BLOCK,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_BREAK,
    AST_CONTINUE,
    AST_PAREN,
    AST_EXPRESSION,
    AST_FUNCTION,
    AST_RETURN,
    AST_NOP
} AstNodeKind;

typedef struct
{
    char *source;
    long index;
    int length;
} AstNodePosition;

typedef enum
{
    UNARY_NONE,
    UNARY_POS,
    UNARY_NEG,
    UNARY_NOT,
    UNARY_CMP
} AstUnaryOperatorKind;

typedef enum
{
    BINARY_NONE,

    BINARY_MUL,
    BINARY_DIV,
    BINARY_MOD,

    BINARY_ADD,
    BINARY_SUB,

    BINARY_SHL,
    BINARY_SHLU,
    BINARY_SHR,

    BINARY_LT,
    BINARY_LTE,
    BINARY_GT,
    BINARY_GTE,

    BINARY_EQ,
    BINARY_NE,

    BINARY_BAND,
    BINARY_BXOR,
    BINARY_BOR,

    BINARY_AND,
    BINARY_OR,

    BINARY_ASSIGN,
} AstBinaryOperatorKind;

typedef struct
{
    AstUnaryOperatorKind kind;
    int length;
} AstUnaryOperator;

typedef struct
{
    AstBinaryOperatorKind kind;
    int precedence;
    int right_associative;
    int length;
} AstBinaryOperator;

typedef struct
{
    struct AstNode *expr;
    AstUnaryOperator op;
} NodeUnary;

typedef struct
{
    struct AstNode *left;
    struct AstNode *right;
    AstBinaryOperator op;
} NodeBinary;

typedef struct
{
    struct AstNode *expr;
} NodeParen;

typedef struct
{
    double value;
} NodeNumber;

typedef struct
{
    char *name;
} NodeIdentifier;

typedef struct
{
    struct AstNode *args;
    int count;
} NodeArgs;

typedef struct
{
    struct AstNode *callee;
    struct AstNode *args;
} NodeCall;

typedef struct
{
    struct AstNode *indexable;
    struct AstNode *index;
} NodeIndex;

typedef struct
{
    int value;
} NodeBoolean;

typedef struct
{
    char *value;
} NodeString;

typedef struct
{
    struct AstNode *statements;
    long count;
} NodeSourceRoot;

typedef struct
{
    struct AstNode *condition;
    struct AstNode *on_true;
    struct AstNode *on_false;
} NodeIf;

typedef struct
{
    struct AstNode *initialization;
    struct AstNode *condition;
    struct AstNode *each;
    struct AstNode *body;
} NodeFor;

typedef struct
{
    struct AstNode *condition;
    struct AstNode *body;
} NodeWhile;

typedef struct
{
    int no_data;
} NodeBreak;

typedef struct
{
    int no_data;
} NodeContinue;

typedef struct
{
    struct AstNode *statements;
    long count;
} NodeBlock;

typedef struct
{
    struct AstNode *expr;
} NodeExpression;

typedef struct
{
    char *name;
    struct AstNode *body;
    char **parameters;
    long parameter_count;
} NodeFunction;

typedef struct
{
    struct AstNode *expr;
} NodeReturn;

typedef union
{
    NodeSourceRoot source_root;
    NodeNumber number;
    NodeIdentifier identifier;
    NodeUnary unary;
    NodeBinary binary;
    NodeParen paren;
    NodeCall call;
    NodeIndex index;
    NodeArgs args;
    NodeBoolean boolean;
    NodeString string;
    NodeBlock block;
    NodeIf data_if;
    NodeFor data_for;
    NodeWhile data_while;
    NodeBreak data_break;
    NodeContinue data_continue;
    NodeExpression expression;
    NodeFunction function;
    NodeReturn data_return;
} AstData;

typedef struct AstNode
{
    AstNodeKind kind;
    AstNodePosition position;
    AstData data;
} AstNode;

typedef struct AstWalkCallbacks
{
    void (*on_nop)(void *ctx, AstNode *ast);
    int (*on_source_root_start)(void *ctx, AstNode *ast, NodeSourceRoot *node, struct AstWalkCallbacks *callbacks);
    void (*on_source_root_end)(void *ctx, AstNode *ast, NodeSourceRoot *node);
    void (*on_expression_start)(void *ctx, AstNode *ast, NodeExpression *node);
    void (*on_expression_end)(void *ctx, AstNode *ast, NodeExpression *node);
    void (*on_number)(void *ctx, AstNode *ast, NodeNumber *node);
    void (*on_boolean)(void *ctx, AstNode *ast, NodeBoolean *node);
    void (*on_string)(void *ctx, AstNode *ast, NodeString *node);
    void (*on_null)(void *ctx, AstNode *ast);
    void (*on_identifier)(void *ctx, AstNode *ast, NodeIdentifier *node);
    void (*on_args_start)(void *ctx, AstNode *ast, NodeArgs *node);
    void (*on_args_end)(void *ctx, AstNode *ast, NodeArgs *node);
    void (*on_call_start)(void *ctx, AstNode *ast, NodeCall *node);
    void (*on_call_end)(void *ctx, AstNode *ast, NodeCall *node);
    void (*on_index_start)(void *ctx, AstNode *ast, NodeIndex *node);
    void (*on_index_end)(void *ctx, AstNode *ast, NodeIndex *node);
    void (*on_unary_start)(void *ctx, AstNode *ast, NodeUnary *node);
    void (*on_unary_end)(void *ctx, AstNode *ast, NodeUnary *node);
    int (*on_binary_start)(void *ctx, AstNode *ast, NodeBinary *node);
    void (*on_binary_end)(void *ctx, AstNode *ast, NodeBinary *node, struct AstWalkCallbacks *callbacks);
    void (*on_paren_start)(void *ctx, AstNode *ast, NodeParen *node);
    void (*on_paren_end)(void *ctx, AstNode *ast, NodeParen *node);
    void (*on_block_start)(void *ctx, AstNode *ast, NodeBlock *node);
    void (*on_block_end)(void *ctx, AstNode *ast, NodeBlock *node);
    int (*on_if_start)(void *ctx, AstNode *node, NodeIf *data, struct AstWalkCallbacks *callbacks);
    void (*on_if_end)(void *ctx, AstNode *ast, NodeIf *node);
    int (*on_for_start)(void *ctx, AstNode *ast, NodeFor *node, struct AstWalkCallbacks *callbacks);
    void (*on_for_end)(void *ctx, AstNode *ast, NodeFor *node);
    int (*on_while_start)(void *ctx, AstNode *ast, NodeWhile *node, struct AstWalkCallbacks *callbacks);
    void (*on_while_end)(void *ctx, AstNode *ast, NodeWhile *node);
    void (*on_break)(void *ctx, AstNode *ast, NodeBreak *node);
    void (*on_continue)(void *ctx, AstNode *ast, NodeContinue *node);
    void (*on_function_start)(void *ctx, AstNode *ast, NodeFunction *node);
    void (*on_function_end)(void *ctx, AstNode *ast, NodeFunction *node);
    void (*on_return_start)(void *ctx, AstNode *ast, NodeReturn *node);
    void (*on_return_end)(void *ctx, AstNode *ast, NodeReturn *node);
} AstWalkCallbacks;

AstNode *create_ast_node(AstNodeKind kind, char *source, long index);
char *unary_op_to_str(AstUnaryOperatorKind kind);
char *binary_op_to_str(AstBinaryOperatorKind kind);
void ast_walk(void *ctx, AstNode *ast, AstWalkCallbacks *callbacks);

#endif
