#include <v3_core.h>
#include "v3_tokenizer.h"


typedef enum {
    V3_SYNTAX_VARIABLE_DECLARATION,
    V3_SYNTAX_VARIABLE_DECLARATOR,
    V3_SYNTAX_MEMBER_EXPR,
} v3_syntax_t;

// var a=1, b=2
typedef struct {
    v3_node_t   node;
    v3_vector_t *declarations; 
} v3_variable_statement_t;

// foo=1
typedef struct {
    v3_node_t           node;
    v3_idetifier_t      *id;
    v3_vector_t         *init;
} v3_variable_declarator_t;

// foo
typedef struct {
    v3_node_t   node;
    v3_str_t    *name;
} v3_idetifier_t;

#define c_lookahead ctx->tokenier->lookahead
#define is_left_hand_side(node) (node->type == V3_SYNTAX_IDENTIFIER \
                                || node->type == V3_SYNTAX_MEMBER_EXPR)

// Return true if the next token matches the specified punctuator.
#define match(punctuator) \
        (c_lookahead->type == V3_TOKEN_Punctuator  \
        && c_lookahead->value->len == (sizeof(punctuator) - 1) \
        && strncmp(c_lookahead->value, punctuator, sizeof(punctuator) ==0))

typedef struct {
    v3_pool_t       *pool;
    v3_tokenizer_t  tokenizer;
} v3_ctx_t;

static v3_node_t *parse_program(v3_ctx_t *ctx);


// v3_object_t v3_global;

static void v3_ctx_init(v3_ctx_t *ctx)
{
    memset(&ctx, 0, sizeof(v3_ctx_t));
}

static void* v3_palloc_wrapper(void *userdata, size_t size)
{
    v3_pool_t *pool;
    pool = userdata;
    return v3_palloc(pool, size);
}

static void v3_pdealloc_wrapper(void *userdata, void *value)
{
    v3_pool_t *pool;
    pool = userdata;
    v3_pfree(pool, value);
}

v3_node_t *parse(const char *code, size_t len)
{
    v3_node_t   *program;
    v3_ctx_t    context;

    context.pool = v3_pool_create();

    v3_options_t    options = {
        v3_palloc_wrapper,
        v3_pdealloc_wrapper,
        context.pool
    };

    if (context.pool == NULL) {
        return NULL;
    }

    if (v3_tokenizer_init(&context.tokenizer, &options, code, len) != V3_OK) {
        return NULL;
    }

    v3_ctx_init(&context);
    
    program = parse_program(&context);

    return program;
}

static v3_node_t *parse_program(v3_ctx_t *ctx)
{
    v3_node_t   *node;
    v3_node_t   *body;
    skipComment(&ctx->tokenizer);    
    peek(&ctx->tokenizer);
    node = v3_node_create(ctx);
    if (node == NULL) return NULL;

    body = parse_source_elements(ctx);
    if (body == NULL) return NULL;

    
}

static v3_node_t *parse_source_elements(v3_ctx_t *ctx)
{
    v3_token_t  *token;
    while (ctx->tokenizer->index < ctx->tokenizer->source.lengh) {
        token = c_lookahead;
        
    }
}

static v3_node_t *parse_source_element(v3_ctx_t *ctx)
{
    if (c_lookahead.type == V3_TOKEN_Keyword) {
        // TODO: let
        // TODO: const
        // TODO: function
        return parse_statement(ctx);
    }

    if (c_lookahead->type != V3_TOKEN_EOF) {
        return parse_statement(ctx);
    }
}

static v3_node_t *parse_statement(v3_ctx_t *ctx)
{
    int type = c_lookahead->type;
    v3_node_t   *node;
    
    if (type == V3_TOKEN_EOF) {
        // TODO: throw
    }

    if (type == V3_TOKEN_Punctuator 
        && lookahead.value == '{') {
        return parse_block(ctx);
    }

    node = v3_node_create(ctx);

    if (type == V3_TOKEN_Punctuator) {
        // if (strncm
        // TODO:
    } else if (type == V3_TOKEN_Keyword) {
        int keyword = *(int *)lookahead.value;

        switch (keyword) {
        case V3_KEYWORD_BREAK:
            return parse_break_statement(ctx, node);
        case V3_KEYWORD_CONTINUE:
            return parse_continue_statement(ctx, node);
        case V3_KEYWORD_DEBUGGER:
            return parse_debugger_statement(ctx, node);
        case V3_KEYWORD_VAR:
            return parse_variable_statement(ctx, node);

        // TODO:
        default:
            break;
        }
    }

    // TODO:
}

static v3_int_t 
parse_variable_statement(v3_ctx_t *ctx)
{
    v3_vector_t     *declarations;
    expect_keyword(ctx, V3_KEYWORD_VAR);

    declarations = parse_variable_declarations(ctx);
    
    if (!consume_semicolon(ctx)) {
        // TODO:
        return NULL;
    }
}

static v3_int_t 
parse_variable_declarations(v3_ctx_t *ctx, v3_vector_t **list)
{
    *list = v3_vector_new(ctx->options, sizeof(void *), 1);
    if (*list == NULL) return V3_OUT_OF_MEMORY;

    do {
         
    } while (ctx->tokenier->index < ctx->tokenier->data.length)
    
    return V3_OK;
}

static v3_int_t 
parse_variable_declaration(v3_ctx_t *ctx, v3_variable_declarator_t **node)
{
    v3_int_t    rc;
    v3_node_t   *init = NULL;
    v3_idetifier_t  *id;

    node = v3_palloc(ctx->pool, sizeof(v3_variable_declarator_t));
    if (node == NULL) return V3_OUT_OF_MEMORY;

    rc = parse_variable_identifier(ctx, &id);
    if (rc != V3_OK) { return rc; }

    if (match("=")) {
        // eg: var a=1;
        rc = lex(ctx->tokenier);
        if (rc != V3_OK) { return rc; }
        
        rc = parse_assignment_expr(ctx, &init);
        if (rc != V3_OK) return rc;
    } // else var a; just dec without init, so init will be null
    
    node->type = V3_SYNTAX_VARIABLE_DECLARATOR;
    node->id = id;
    node->init = init;
    return 
}

static v3_int_t 
parse_assignment_expr(v3_ctx_t *ctx, v3_node_t **node)
{
    int oldParenthesisCount;
    v3_expr_t   *expr;
    v3_token_t  *startToken, *token;

    oldParenthesisCount = ctx->parenthesis_count;
    startToken = token = c_lookahead;
    rc = parse_conditional_expr(ctx, &expr);
    if (rc != V3_OK) return rc;

    // TODO: if expr == 

    if (match_assign(ctx)) {
        if (!is_left_hand_side(expr)) {
            return throw_error(ctx, Msg_InvalidLHSInAssignment);
        }
        // TODO: strict

        rc = lex(ctx->tokenizer, &token);
        if (rc != V3_OK) return rc;

    }
}

static v3_int_t 
parse_conditional_expr(v3_ctx_t *ctx, v3_exprt_t **expr)
{
    v3_int_t    rc;

    rc = parse_binary_expr(ctx, expr);
    if (rc != V3_OK) return rc;
}

static v3_int_t 
parse_binary_expr(v3_ctx_t *ctx, v3_exprt_t **expr)
{
    v3_token_t  *marker;
    v3_int_t    rc;
    v3_expr_t   *left;

    marker = c_lookahead;
    
    rc = parse_unary_expr(ctx, &left); 
    if (rc != V3_OK) return rc;

    if (left == NULL) {
        
    }
}

static v3_int_t 
parse_unary_expr(v3_ctx_t *ctx, v3_expr_t **expr)
{
    v3_int_t        rc;

    if (c_lookahead->type != V3_TOKEN_Punctuator
        && c_lookahead->type != V3_TOKEN_Keyword) {
        return parse_postfix_expr(ctx, expr);
    }

    // TODO:
    return V3_NOT_SUPPORT;
}

#define match_keyword(token, type) \
                    (token->type == V3_TOKEN_Keyword \
                    && token->v.keyword == type)

static v3_int_t 
parse_postfix_expr(v3_ctx_t *ctx, v3_expr_t **expr)
{
    v3_bool_t   prev_allow_in;
    v3_token_t  *start_token;
    v3_int_t    rc;
    prev_allow_in = ctx->state.allowin;
    ctx->state.allowin = V3_TRUE;

    rc = match_keyword(c_lookahead, V3_KEYWORD_NEW) 
         ? parse_new_expr(ctx, expr)
         : parse_primary_expr(ctx, expr);
    if (rc != V3_OK) return rc;

    ctx->state.allowin = prev_allow_in;

    start_token = c_lookahead;
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
static v3_int_t
expect_keyword(v3_ctx_t *ctx, v3_keyword_t keyword)
{
    v3_int_t    rc; 
    v3_token_t  *token;

    rc = lex(ctx, &token);
    if (rc != V3_OK) return rc;

    if (token->type != V3_TOKEN_Keyword
        || toke->v.keyword == keyword) {
        return throw_unexpected(ctx, token);
    }

    return V3_OK;
}

static v3_int_t
parse_new_expr(v3_ctx_t *ctx, v3_expr_t **expr)
{
    v3_int_t    rc;
    rc = expect_keyword(ctx, V3_KEYWORD_NEW);
    // TODO:
    return rc;
}

static v3_int_t
parse_primary_expr(v3_ctx_t *ctx, v3_expr_t **expr)
{
    v3_int_t        type;
    v3_node_t       *

    if (match("(")) {
        return parse_group_expr(ctx, expr);
    }

    if (match("[")) {
        return parse_array_init(ctx, expr);
    }

    if (match("{")) {
        return parse_object_init(ctx, expr);
    }

    type = c_lookahead->type;
    if (type == V3_TOKEN_Identifier) {
        // a=b;
        rc = lex(ctx, &token);
        if (rc != V3_OK) return rc;
        *expr = v3_create_identifier(ctx, token->value);
        if (expr == NULL) return V3_ALLOC_ERROR;
        return V3_OK;
    } else if (type == V3_TOKEN_StringLiteral
                || type == V3_TOKEN_NumericLiteral) {
        // a = "abc", or a = 1;
        // TODO: strict
        rc = lex(ctx, &token);
        if (rc != V3_OK) return rc;
        *expr = v3_create_literal(ctx, token);
        if (expr == NULL) return V3_ALLOC_ERROR;
        return V3_OK;
    }
}

v3_create_number(v3_ctx_t *ctx, v3_token_t *token)
{
    v3_number_object_t *num;
    num = v3_pcalloc(ctx->pool);
    if (num == NULL) return NULL;
    num->value = token->v.num;
    num->__proto__ = ctx->Number->protoype;

    return num;
}

static v3_literal_t 
v3_create_literal(v3_ctx_t *ctx, v3_token_t *token) 
{
    v3_literal_t    *literal;
    literal =  v3_pcalloc(ctx->pool); 
    if (literal == NULL) return NULL; 
    
    literal->node.type = V3_SYNTAX_LITERAL;
    if (token->type == V3_TOKEN_NumericLiteral) {
        literal->value = v3_create_number(ctx->pool, value, token_type);
        if (literal->value == NULL) return NULL;
    } else {
        // literal->value = v3_create_string(ctx->pool, value, token_type);
        // if (literal->value == NULL) return NULL;
    }

    // literal->value = v3_create_primitive_value(ctx->pool, value, token_type);
    literal->raw = value;

    return literal;
}

// if match assign operator
static v3_bool_t 
match_assign(v3_ctx_t *ctx)
{
    v3_str_t    *op;
    if (c_lookahead->type != V3_TOKEN_Punctuator) {
        return V3_FALSE;
    }

    op  = c_lookahead->value;
    if (op->length == 1) {
        return strncmp(op->data, "=", 1) == 0;
    } else if(op->len == 2) {
        return strncmp(op->data, "*=", 2) == 0
            || strncmp(op->data, "/=", 2) == 0
            || strncmp(op->data, "%=", 2) == 0
            || strncmp(op->data, "+=", 2) == 0
            || strncmp(op->data, "-=", 2) == 0
            || strncmp(op->data, "&=", 2) == 0
            || strncmp(op->data, "^=", 2) == 0
            || strncmp(op->data, "|=", 2) == 0;
    } else if(op->len == 3) {
        return strncmp(op->data, ">>=", 3) == 0
            || strncmp(op->data, "<<=", 3) == 0;
    } else if(op->len == 4) {
        return strncmp(op->data, ">>>=", 4) == 0;
    } 

    return V3_FALSE;
}

static v3_int_t 
parse_variable_identifier(v3_ctx_t *ctx, v3_idetifier_t **node)
{
    v3_token_t  *token;
    v3_str_t    *name;

    rc = lex(ctx->tokenier, &token);
    if (rc != V3_OK) return rc;

    if (token->type != V3_TOKEN_Identifier) {
        return throw_unexpected(ctx, token);    
    }

    *node = v3_palloc(ctx->pool, sizeof(v3_idetifier_t));
    if (*node == NULL) return V3_ALLOC_ERROR;
    
    *node->node.type = V3_SYNTAX_IDENTIFIER;
    *node->name = (ngx_str_t *)token->value;

    return V3_OK;
}

static v3_int_t 
consume_semicolon(ctx)
{
    // TODO:
    return V3_OK;
}

