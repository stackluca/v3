#include <v3_core.h>
#include <assert.h>
#include "v3_tokenizer.h"
#include "v3_parser.h"

typedef struct {
    v3_vector_t     *params;
    v3_vector_t     *defaults;
} v3_params_t;

static v3_node_t v3_arrow_parameter_place_holder;
static v3_int_t binary_precedence(v3_ctx_t *ctx, v3_token_t *token, v3_bool_t allowin);

#define is_left_hand_side(node) (node->type == V3_SYNTAX_IDENTIFIER \
                                || node->type == V3_SYNTAX_MEMBER_EXPR)

// Return true if the next token matches the specified punctuator.
#define match(punctuator) \
        (c_lookahead->type == V3_TOKEN_Punctuator  \
        && c_lookahead->value.length == (sizeof(punctuator) - 1) \
        && strncmp(c_lookahead->value.data, punctuator, sizeof(punctuator) - 1) ==0)

#define token_match(token, punctuator) \
        (token->value.length == (sizeof(punctuator) - 1) \
        && strncmp(token->value.data, punctuator, sizeof(punctuator) - 1) ==0)

#define match_keyword(token, keyword_type) \
                    (token->type == V3_TOKEN_Keyword \
                    && token->v.keyword == keyword_type)

#define is_identifier_name(token) \
    (token->type == V3_TOKEN_Identifier \
    || token->type == V3_TOKEN_Keyword \
    || token->type == V3_TOKEN_BooleanLiteral \
    || token->type == V3_TOKEN_NullLiteral)

v3_str_t dec_kind_var = {"var", 3};


static v3_int_t consume_semicolon(v3_ctx_t *ctx);
static v3_int_t expect(v3_ctx_t *ctx, char *punctuator, size_t len);
static v3_int_t expect_keyword(v3_ctx_t *ctx, v3_keyword_t keyword);
static v3_bool_t match_assign(v3_ctx_t *ctx);

static v3_int_t parse_arguments(v3_ctx_t *ctx, v3_vector_t **args);
static v3_int_t parse_array_init(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_assignment_expr(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_binary_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_conditional_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_computed_member(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_expression(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_function_declaration(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_function_expr(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_function_source_elements(v3_ctx_t *ctx, v3_block_statement_t **block);
static v3_int_t parse_group_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_left_hand_side_expr_allow_call(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_left_hand_side_expr(v3_ctx_t *ctx, v3_node_t **callee);
static v3_int_t parse_new_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_non_computed_member(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_non_computed_property(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_object_init(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_param(v3_ctx_t *ctx, v3_idetifier_t **id, v3_bool_t *end);
static v3_int_t parse_params(v3_ctx_t *ctx, v3_vector_t **params);
static v3_int_t parse_postfix_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_primary_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_source_elements(v3_ctx_t *ctx, v3_vector_t **body);
static v3_int_t parse_source_element(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_statement(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_unary_expr(v3_ctx_t *ctx, v3_node_t **expr);
static v3_int_t parse_variable_declarations(v3_ctx_t *ctx, v3_vector_t **list);
static v3_int_t parse_variable_declaration(v3_ctx_t *ctx, v3_variable_declarator_t **node);
static v3_int_t parse_variable_identifier(v3_ctx_t *ctx, v3_idetifier_t **node);
static v3_int_t parse_variable_statement(v3_ctx_t *ctx, v3_node_t **node);
static v3_int_t parse_return_statement(v3_ctx_t *ctx, v3_node_t **node);

static v3_block_statement_t * v3_create_block_statement(v3_ctx_t *ctx, v3_vector_t *body);
static v3_call_expr_t *v3_create_call_expr(v3_ctx_t *ctx, v3_node_t *callee, v3_vector_t *args);
static v3_expr_statement_t* v3_create_expr_statement(v3_ctx_t *ctx, v3_node_t *expr);
static v3_function_node_t* v3_create_function_node(v3_ctx_t *ctx, v3_idetifier_t *id, v3_vector_t *params, v3_block_statement_t *block, v3_int_t type);
static v3_idetifier_t *v3_create_identifier(v3_ctx_t *ctx, v3_str_t *name);
static v3_literal_t *v3_create_literal(v3_ctx_t *ctx, v3_token_t *token);
static v3_member_expr_t *v3_create_member_expr(v3_ctx_t *ctx, char accessor, v3_node_t *expr, v3_node_t *property);
static v3_new_expr_t* v3_create_new_expr(v3_ctx_t *ctx, v3_node_t *callee, v3_vector_t *arguments);
static v3_return_statement_t *v3_create_return_statement(v3_ctx_t *ctx, v3_node_t *argument);
static v3_binary_expr_t *v3_create_binary_expr(v3_ctx_t *ctx, const char *operator, size_t len, v3_node_t *left, v3_node_t *right);

static v3_number_object_t * v3_number_from_token(v3_ctx_t *ctx, v3_token_t *token);
#if 0
static v3_int_t 
v3_reinterpret_as_cover_formals_list(v3_ctx_t *ctx, v3_vector_t *exprs, v3_params_t **params);
#endif
static v3_bool_t peek_line_terminator(v3_ctx_t *ctx);


static v3_int_t throw_error(v3_ctx_t *ctx, const char *msg, int rc);
static v3_int_t throw_unexpected(v3_ctx_t *ctx, v3_token_t *token);
// v3_object_t v3_global;


void* v3_palloc_wrapper(void *userdata, size_t size)
{
    v3_pool_t *pool;
    pool = userdata;
    return v3_palloc(pool, size);
}

void v3_pdealloc_wrapper(void *userdata, void *value)
{
    v3_pool_t *pool;
    pool = userdata;
    v3_pfree(pool, value);
}

v3_int_t 
v3_parse(v3_ctx_t *ctx, const char *code, size_t len, v3_program_node_t **program)
{
    v3_int_t        rc;
    if (v3_tokenizer_init(&ctx->tokenizer, ctx->options, code, len) != V3_OK) {
        return V3_ERROR;
    }

    rc  = parse_program(ctx, program);

    return rc;
}

v3_int_t 
parse_program(v3_ctx_t *ctx, v3_program_node_t **node)
{
    v3_vector_t         *body;
    v3_int_t             rc;

    skipComment(&ctx->tokenizer);    
    peek(&ctx->tokenizer);

    *node = v3_palloc(ctx->pool, sizeof(v3_program_node_t));
    if (*node == NULL) return V3_ERROR;

    (*node)->node.type = V3_SYNTAX_PROGRAM;

    rc = parse_source_elements(ctx, &body);
    if (rc != V3_OK) return rc;

    (*node)->body = body;

    return V3_OK;
}

static v3_int_t
parse_source_elements(v3_ctx_t *ctx, v3_vector_t **body)
{
    v3_node_t       *source_element, **item;
    v3_int_t        rc;

    *body = v3_vector_new(ctx->options, sizeof(void *), 1);
    if (*body == NULL) return V3_ERROR;
    /*
    while (ctx->tokenizer->index < ctx->tokenizer->source.lengh) {
        token = c_lookahead;
        // TODO:          
    }*/

    while (ctx->tokenizer.index < ctx->tokenizer.source.length) {
        rc = parse_source_element(ctx, &source_element);
        if (rc == V3_UNDEFINED) {
            break;
        }
        if (rc != V3_OK) return rc;
        item = v3_vector_push(ctx->options, *body);
        if (item == NULL) return V3_ERROR;
        *item = source_element;
    }

    return V3_OK;
}

static v3_int_t 
parse_source_element(v3_ctx_t *ctx, v3_node_t **node)
{
    if (c_lookahead->type == V3_TOKEN_Keyword) {
        // TODO: let
        // TODO: const
        // TODO: function
        return parse_statement(ctx, node);
    }

    if (c_lookahead->type != V3_TOKEN_EOF) {
        return parse_statement(ctx, node);
    }

    return V3_UNDEFINED;
}

static v3_int_t 
parse_statement(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_int_t    rc;
    int type = c_lookahead->type;
    v3_expr_statement_t     *expr_statement;
    
    if (type == V3_TOKEN_EOF) {
        // TODO: throw
        ctx->err = "unexpected token";
        return V3_UNEXPEDTED_TOKEN;
    }

    if (match("{")) {
        // TODO:return parse_block(ctx, node);
        return V3_NOT_SUPPORT;
    }

    if (type == V3_TOKEN_Punctuator) {
        // if (strncm
        // TODO:
        return V3_NOT_SUPPORT;
    } else if (type == V3_TOKEN_Keyword) {
        int keyword = c_lookahead->v.keyword;

        switch (keyword) {
        case V3_KEYWORD_BREAK:
            return V3_NOT_SUPPORT;
            // TODO:return parse_break_statement(ctx, node);
        case V3_KEYWORD_CONTINUE:
            return V3_NOT_SUPPORT;
            // TODO:return parse_continue_statement(ctx, node);
        case V3_KEYWORD_DEBUGGER:
            return V3_NOT_SUPPORT;
            // TODO: return parse_debugger_statement(ctx, node);
        case V3_KEYWORD_VAR:
            return parse_variable_statement(ctx, node);
        case V3_KEYWORD_FUNCTION:
            return parse_function_declaration(ctx, node);
        case V3_KEYWORD_RETURN:
            return parse_return_statement(ctx, node);
        // TODO:
        default:
            break;
        }
    }

    rc = parse_expression(ctx, node);
    if (rc != V3_OK) return rc;

    expr_statement = v3_create_expr_statement(ctx, *node);
    if (expr_statement == NULL) return V3_ERROR;
    return V3_OK;
}

static v3_int_t
parse_return_statement(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_node_t           *argument = NULL;
    v3_tokenizer_t      *tokenizer;

    CHECK_FCT(expect_keyword(ctx, V3_KEYWORD_RETURN));
    if (!ctx->state.in_function_body) {
        return V3_EUNEXPETED_TOKEN;
    }

    tokenizer = &ctx->tokenizer;

    // 'return' followed by a space and an identifier is very common.
    if (CURRENT_CHAR == ' ') {
        if (isIdentifierStart(NEXT_CHAR)) {
            CHECK_FCT(parse_expression(ctx, &argument));
            consume_semicolon(ctx);
            *node = to_node(v3_create_return_statement(ctx, argument));
            if (*node == NULL) return V3_ERROR;
            return V3_OK;
        }
    }
    
    if (peek_line_terminator(ctx)) {
        *node = to_node(v3_create_return_statement(ctx, NULL));
        if (*node == NULL) return V3_ERROR;
        return V3_OK;
    }

    if (!match(";")) {
        if (!match("}") && c_lookahead->type != V3_TOKEN_EOF) {
            CHECK_FCT(parse_expression(ctx, &argument));
        }
    }

    consume_semicolon(ctx);
    *node = to_node(v3_create_return_statement(ctx, argument));
    if (*node == NULL) return V3_ERROR;
    return V3_OK;
}

static v3_int_t
parse_function_expr(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_idetifier_t          *id;
    v3_vector_t             *params;
    v3_block_statement_t    *block;
    // v3_token_t              *token;

    CHECK_FCT(expect_keyword(ctx, V3_KEYWORD_FUNCTION));

    id = NULL;
    if (!match("(")) {
        // token = c_lookahead;
        CHECK_FCT(parse_variable_identifier(ctx, &id));
    }

    // TODO:
    CHECK_FCT(parse_params(ctx, &params));

    CHECK_FCT(parse_function_source_elements(ctx, &block));

    *node = (v3_node_t *)v3_create_function_node(ctx, 
                            id, params, block, 
                            V3_SYNTAX_FUNCTION_EXPR);

    return *node == NULL ? V3_ERROR : V3_OK;
}

static v3_int_t
parse_function_declaration(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_idetifier_t          *id;
    v3_vector_t             *params;
    v3_block_statement_t    *block;

    CHECK_FCT(expect_keyword(ctx, V3_KEYWORD_FUNCTION));

    CHECK_FCT(parse_variable_identifier(ctx, &id));

    // TODO:
    CHECK_FCT(parse_params(ctx, &params));

    CHECK_FCT(parse_function_source_elements(ctx, &block));

    // TODO:
    // TODO:
    *node = (v3_node_t *)v3_create_function_node(ctx, 
                        id, params, block, 
                        V3_SYNTAX_FUNCTION_DECLARATION);

    return *node == NULL ? V3_ERROR : V3_OK;
}

static v3_int_t 
parse_params(v3_ctx_t *ctx, v3_vector_t **params)
{
    v3_idetifier_t          **id, *idr;
    v3_bool_t               end;

    *params = NULL;
    CHECK_FCT(expect(ctx, "(", 1));

    if (!match(")")) {
        *params = v3_vector_new(ctx->options, sizeof(void *), 5);
        if (params == NULL) return V3_ERROR;

        while (ctx->tokenizer.index < ctx->tokenizer.source.length) {
            id = v3_vector_push(ctx->options, *params);
            if (id == NULL) return V3_ERROR;
            CHECK_FCT(parse_param(ctx, &idr, &end));
            *id = idr;
            if (end == V3_TRUE) {
                break;
            }

            CHECK_FCT(expect(ctx, ",", 1));
        }
    }

    CHECK_FCT(expect(ctx, ")", 1));
    if (*params == NULL) *params = v3_vector_new(ctx->options, sizeof(void *), 0);

    return V3_OK;
}

static v3_int_t 
parse_param(v3_ctx_t *ctx, v3_idetifier_t **id, v3_bool_t *end)
{
    CHECK_FCT(parse_variable_identifier(ctx, id)); 
    
    *end = match(")") ? V3_TRUE : V3_FALSE;

    return V3_OK;
}

static v3_int_t 
parse_function_source_elements(v3_ctx_t *ctx, v3_block_statement_t **block)
{
    v3_node_t       **source_element;
    v3_vector_t     *body;
    v3_bool_t       in_switch, in_iteration, in_function_body; 
    v3_int_t        parenthesis_count;
    v3_vector_t     *labels;

    body = v3_vector_new(ctx->options, sizeof(void *), 5);
    if (body == NULL) return V3_ERROR;

    //v3_block_statement_t    *block;
    CHECK_FCT(expect(ctx, "{", 1));

    in_iteration = ctx->state.in_iteration;
    in_switch = ctx->state.in_switch;
    in_function_body = ctx->state.in_function_body;
    parenthesis_count = ctx->state.parenthesis_count;
    labels = ctx->state.labels;

    ctx->state.labels = NULL;
    ctx->state.in_iteration = V3_FALSE;
    ctx->state.in_switch = V3_FALSE;
    ctx->state.in_function_body = V3_TRUE;
    ctx->state.parenthesis_count = 0;

    while (ctx->tokenizer.index < ctx->tokenizer.source.length) {
        if (match("}")) {
            break;
        }

        // token = c_lookahead;
        source_element = v3_vector_push(ctx->options, body);
        if (source_element == NULL) return V3_ERROR;
        CHECK_FCT(parse_source_element(ctx, source_element));
    }

    CHECK_FCT(expect(ctx, "}", 1));

    ctx->state.labels = labels;
    ctx->state.in_iteration = in_iteration;
    ctx->state.in_switch = in_switch;
    ctx->state.in_function_body = in_function_body;
    ctx->state.parenthesis_count = parenthesis_count;

    *block = v3_create_block_statement(ctx, body);
    if (*block == NULL) return V3_ERROR;

    return V3_OK;
}

static v3_int_t
parse_expression(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_int_t    rc;
    rc = parse_assignment_expr(ctx, node);

    if (match(",")) {
        // a=1, b=2 
        ctx->err = "unsupported sequence expression yet";
        return V3_UNEXPEDTED_TOKEN;
        // TODO:
    }

    return rc;
}

static v3_int_t 
parse_variable_statement(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_vector_t     *declarations;
    v3_int_t        rc;
    v3_variable_statement_t *statement;
    
    rc = expect_keyword(ctx, V3_KEYWORD_VAR);
    if (rc != V3_OK) return rc;

    statement = v3_palloc(ctx->pool, sizeof(v3_variable_statement_t));

    if (statement == NULL) return V3_ERROR;

    statement->node.type = V3_SYNTAX_VARIABLE_DECLARATION;

    rc = parse_variable_declarations(ctx, &declarations);
    if (rc != V3_OK) return rc;

    statement->declarations = declarations;
    statement->kind = dec_kind_var;

    *node = (v3_node_t *)statement;
    
    if (!consume_semicolon(ctx)) {
        // TODO:
        return V3_OK;
    }

    return V3_OK;
}

static v3_int_t 
parse_variable_declarations(v3_ctx_t *ctx, v3_vector_t **list)
{
    v3_int_t        rc;
    v3_node_t        **item;
    v3_variable_declarator_t    *dec;

    *list = v3_vector_new(ctx->options, sizeof(void *), 1);
    if (*list == NULL) return V3_OUT_OF_MEMORY;

    do {
        rc = parse_variable_declaration(ctx, &dec);
        if (rc != V3_OK) return rc;

        item = v3_vector_push(ctx->options, *list);
        if (item == NULL) return rc;
        *item = (v3_node_t *)dec;

        if (!match(",")) {
            break;
        }

        if (lex(&ctx->tokenizer, NULL) == V3_ERROR) {
            return V3_ERROR;
        }

    } while (ctx->tokenizer.index < ctx->tokenizer.source.length);
    
    return V3_OK;
}

static v3_int_t 
parse_variable_declaration(v3_ctx_t *ctx, v3_variable_declarator_t **node)
{
    v3_int_t        rc;
    v3_node_t       *init = NULL; // var a, may be null;
    v3_idetifier_t  *id;

    *node = v3_palloc(ctx->pool, sizeof(v3_variable_declarator_t));
    if (*node == NULL) return V3_OUT_OF_MEMORY;

    rc = parse_variable_identifier(ctx, &id);
    if (rc != V3_OK) { return rc; }

    // TODO:
    if (match("=")) { // lookahead is =
        // eg: var a=1;
        rc = lex(&ctx->tokenizer, NULL); // so ignore it 
        if (rc != V3_OK) { return rc; }
        
        rc = parse_assignment_expr(ctx, &init);
        if (rc != V3_OK) return rc;
    } // else var a; just dec without init, so init will be null
    
    (*node)->node.type = V3_SYNTAX_VARIABLE_DECLARATOR;
    (*node)->id     = id;
    (*node)->init   = init;
    return V3_OK;
}

static v3_int_t 
parse_assignment_expr(v3_ctx_t *ctx, v3_node_t **node)
{
    int oldParenthesisCount;
    v3_node_t   *expr;
    v3_int_t    rc;
    v3_token_t  *token;
    // v3_vector_t *params;

    oldParenthesisCount = ctx->state.parenthesis_count;
    token = c_lookahead;
    rc = parse_conditional_expr(ctx, node);

    // TODO:
    if (rc != V3_OK) return rc;
    expr = *node;

    if (*node == &v3_arrow_parameter_place_holder  /* eg: a.b() */
        //|| match("=>")) {
        ) {
        if (ctx->state.parenthesis_count == oldParenthesisCount
            || ctx->state.parenthesis_count + 1) {
            if (expr->type == V3_SYNTAX_IDENTIFIER) {
                // eg: foo(a);
                // TODO:
                return V3_NOT_SUPPORT;
            } else if(expr->type == V3_SYNTAX_ASSIGNMENT_EXPR) {
                // eg: 
                // TODO: 
                return V3_NOT_SUPPORT;
            } else if(expr->type == V3_SYNTAX_SEQUENCE_EXPR) {
                // eg: foo(a, b, 2);
                // TODO:
                return V3_NOT_SUPPORT;
            } else if (expr == &v3_arrow_parameter_place_holder) {
                // eg: foo();
                // TODO:
                //rc = v3_reinterpret_as_cover_formals_list(ctx, NULL, params);
                //if (rc != V3_OK) return V3_ERROR;
            }

#if 0
            if (params != NULL) {
                rc =  
                return rc;
            }
#endif
        }
    }

    // TODO: if expr == 

    if (match_assign(ctx)) {
        if (!is_left_hand_side(expr)) {
            return throw_error(ctx, Msg_InvalidLHSInAssignment, V3_SYNTAX_ERROR);
        }
        // TODO: strict

        rc = lex(&ctx->tokenizer, &token);
        if (rc != V3_OK) return rc;
    }

    return V3_OK;
}

#if 0
static v3_int_t 
v3_reinterpret_as_cover_formals_list(v3_ctx_t *ctx, v3_vector_t *exprs, v3_params_t **params)
{
#if 0
    // TODO:
    *params = v3_palloc(ctx->pool, sizeof(v3_params_t));
    if (*params == NULL) return V3_ERROR;
    memset(*params, 0, sizeof(v3_params_t));
    return V3_OK;
#endif
    return V3_NOT_SUPPORT;
}
#endif

static v3_int_t 
parse_conditional_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t    rc;

    rc = parse_binary_expr(ctx, expr);
    if (rc != V3_OK) return rc;
    if (*expr == &v3_arrow_parameter_place_holder) {
        return V3_OK;
    }
    return rc;
    // TODO:
    return V3_NOT_SUPPORT;
}

static v3_int_t 
parse_binary_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    // v3_token_t  *marker;
    v3_int_t    rc, prec;
    v3_node_t   *left, *right;
    v3_token_t  *token;

    // marker = c_lookahead;
    
    rc = parse_unary_expr(ctx, &left); 
    if (rc != V3_OK) return rc;
    if (left == &v3_arrow_parameter_place_holder) {
        *expr = left;
        return V3_OK;
    }

    token = c_lookahead;
    prec = binary_precedence(ctx, token, ctx->state.allowin);
    if (prec < 0) return prec;
    if (prec == 0) {
        *expr = left;
        return  V3_OK;
    }

    token->prec = prec;
    lex(&ctx->tokenizer, NULL);
    CHECK_FCT(parse_unary_expr(ctx, &right));
    // TODO:
    
    *expr = (v3_node_t *)v3_create_binary_expr(ctx, token->value.data, token->value.length, left, right);
    if (*expr == NULL) return V3_ERROR;
#if 0
    if (left == NULL) {
        
    }
#endif

    return V3_OK;
}

static v3_int_t 
binary_precedence(v3_ctx_t *ctx, v3_token_t *token, v3_bool_t allowin)
{
    v3_int_t    prec = 0;
    
    if (token->type != V3_TOKEN_Punctuator
        && token->type != V3_TOKEN_Keyword) {
        return 0;
    }

    if (token_match(token, "+")
        || token_match(token, "-")) {
        prec = 9;
    } else {
        // ctx->err = "not support precedence";
        // return V3_NOT_SUPPORT;
    }

    // may be ,
    return prec;
}

static v3_int_t 
parse_unary_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    // v3_int_t        rc;

    if (c_lookahead->type != V3_TOKEN_Punctuator
        && c_lookahead->type != V3_TOKEN_Keyword) {
        return parse_postfix_expr(ctx, expr);
    } else if (match("++") || match("--")) {
        // TODO:
        return V3_NOT_SUPPORT;
    } else if (match("+") || match("-") || match("~") || match("!")) {
        // TODO:
        return V3_NOT_SUPPORT;
    } else if (match_keyword(c_lookahead, V3_KEYWORD_DELETE) 
                || match_keyword(c_lookahead, V3_KEYWORD_VOID) 
                || match_keyword(c_lookahead, V3_KEYWORD_TYPEOF)) {
        // TODO:
        return V3_NOT_SUPPORT;
    } else {
        return parse_postfix_expr(ctx, expr);
    }
}

static v3_int_t 
parse_postfix_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t    rc;
    v3_token_t  *token;

    rc = parse_left_hand_side_expr_allow_call(ctx, expr);
    if (rc != V3_OK) return rc;

    if (c_lookahead->type == V3_TOKEN_Punctuator) {
        // eg: a.b();
        if ((match("++") || match("--"))
            && !peek_line_terminator(ctx)) {
            
            if (!is_left_hand_side((*expr))) {
                return throw_error(ctx, Msg_InvalidLHSInAssignment, V3_UNEXPEDTED_TOKEN);
            }

            rc = lex(&ctx->tokenizer, &token);
            if (rc != V3_OK) return rc;
            
            // TODO:
        }
    }

    return V3_OK;
}

// Return true if there is a line terminator before the next token.
static v3_bool_t peek_line_terminator(v3_ctx_t *ctx)
{
    v3_int_t    pos, line, start, found;

    pos = ctx->tokenizer.index;
    line = ctx->tokenizer.lineNumber;
    start = ctx->tokenizer.lineStart;

    skipComment(&ctx->tokenizer);
    found = ctx->tokenizer.lineNumber != line;

    ctx->tokenizer.index = pos;
    ctx->tokenizer.lineNumber = line;
    ctx->tokenizer.lineStart = start;

    return found;
}

static v3_int_t
parse_left_hand_side_expr_allow_call(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_bool_t   prev_allow_in;
    // v3_token_t  *start_token;
    v3_int_t    rc;
    v3_node_t   *property;
    v3_vector_t  *args;

    prev_allow_in = ctx->state.allowin;
    ctx->state.allowin = V3_TRUE;
    rc = match_keyword(c_lookahead, V3_KEYWORD_NEW) 
         ? parse_new_expr(ctx, expr)
         : parse_primary_expr(ctx, expr);
    if (rc != V3_OK) return rc;
    ctx->state.allowin = prev_allow_in;

    for (;;) {
        if (match(".")) {
            // eg: a.foo
            CHECK_FCT(parse_non_computed_member(ctx, &property));
            
            *expr = (v3_node_t *)v3_create_member_expr(ctx, '.', *expr, property);
            if (*expr == NULL) return V3_ERROR;
        } else if(match("(")) {
            // eg: a.foo(), a()
            CHECK_FCT(parse_arguments(ctx, &args)); 
            *expr = (v3_node_t *)v3_create_call_expr(ctx, *expr, args);
            if (*expr == NULL) return V3_ERROR;
        } else if (match("[")) {
            CHECK_FCT(parse_computed_member(ctx, &property));
            
            *expr = (v3_node_t *)v3_create_member_expr(ctx, '[', *expr, property);
            if (*expr == NULL) return V3_ERROR;
        
        } else {
            break;
        }
    }
    // start_token = c_lookahead;
    // TODO:
    return V3_OK;
}

static v3_int_t 
parse_arguments(v3_ctx_t *ctx, v3_vector_t **args)
{
    v3_int_t    rc;
    v3_node_t   **item, *arg;

    *args = v3_vector_new(ctx->options, sizeof(void *), 4);
    if (*args == NULL) return V3_ERROR;

    CHECK_FCT(expect(ctx, "(", 1));

    if (!(match(")"))) {
        while (ctx->tokenizer.index < ctx->tokenizer.source.length) {
            rc = parse_assignment_expr(ctx, &arg);
            if (rc != V3_OK) return rc;

            item = v3_vector_push(ctx->options, *args);
            if (item == NULL) return V3_ERROR;

            *item = arg;
            if (match(")")) {
                break;
            }

            CHECK_FCT(expect(ctx, ",", 1));
        }
    }

    return expect(ctx, ")", 1);
}

static v3_function_node_t*
v3_create_function_node(v3_ctx_t *ctx, v3_idetifier_t *id, v3_vector_t *params, v3_block_statement_t *block, v3_int_t type)
{
    v3_function_node_t    *node;  

    node = v3_palloc(ctx->pool, sizeof(*node));
    if (node == NULL) return NULL;
    node->node.type = type;
    node->id = id;
    node->params = params;
    node->body = block;

    return node;
}

static v3_return_statement_t *
v3_create_return_statement(v3_ctx_t *ctx, v3_node_t *argument)
{
    v3_return_statement_t    *node;  

    node = v3_palloc(ctx->pool, sizeof(*node));
    if (node == NULL) return NULL;
    node->node.type = V3_SYNTAX_RETURN_STATEMENT;
    node->argument = argument;

    return node;
}

static v3_block_statement_t *
v3_create_block_statement(v3_ctx_t *ctx, v3_vector_t *body)
{
    v3_block_statement_t    *node;  

    node = v3_palloc(ctx->pool, sizeof(*node));
    if (node == NULL) return NULL;
    node->node.type = V3_SYNTAX_BLOCK_STATEMENT;
    node->body = body;

    return node;
}

static v3_binary_expr_t *
v3_create_binary_expr(v3_ctx_t *ctx, const char *operator, size_t len, v3_node_t *left, v3_node_t *right)
{
    v3_binary_expr_t    *expr;  

    expr = v3_palloc(ctx->pool, sizeof(*expr));
    if (expr == NULL) return NULL;

    expr->node.type = V3_SYNTAX_BINARY_EXPR;
    expr->_operator.data = operator;
    expr->_operator.length = len;
    expr->left = left;
    expr->right = right;

    return expr;
}

static v3_call_expr_t *
v3_create_call_expr(v3_ctx_t *ctx, v3_node_t *callee, v3_vector_t *args)
{
    v3_call_expr_t    *expr;  

    expr = v3_palloc(ctx->pool, sizeof(*expr));
    if (expr == NULL) return NULL;
    expr->node.type = V3_SYNTAX_CALL_EXPR;
    expr->callee = callee;
    expr->arguments = args;

    return expr;
}

static v3_new_expr_t*
v3_create_new_expr(v3_ctx_t *ctx, v3_node_t *callee, v3_vector_t *arguments)
{
    v3_new_expr_t    *node;  

    node = v3_palloc(ctx->pool, sizeof(*node));
    if (node == NULL) return NULL;
    node->node.type = V3_SYNTAX_NEW_EXPR;
    node->callee = callee;
    node->arguments = arguments;

    return node;
}

static v3_expr_statement_t*
v3_create_expr_statement(v3_ctx_t *ctx, v3_node_t *expr)
{
    v3_expr_statement_t    *node;  

    node = v3_palloc(ctx->pool, sizeof(*node));
    if (node == NULL) return NULL;
    node->node.type = V3_SYNTAX_EXPR_STATEMENT;
    node->expr = expr;

    return node;
}

static v3_member_expr_t*
v3_create_member_expr(v3_ctx_t *ctx, char accessor, v3_node_t *expr, v3_node_t *property)
{
    v3_member_expr_t    *mem_expr;  

    mem_expr = v3_palloc(ctx->pool, sizeof(*mem_expr));
    if (mem_expr == NULL) return NULL;
    mem_expr->node.type = V3_SYNTAX_MEMBER_EXPR;
    mem_expr->accessor = accessor;
    mem_expr->object = expr;
    mem_expr->property = property;

    return mem_expr;
}

static v3_int_t
parse_computed_member(v3_ctx_t *ctx, v3_node_t **node)
{
    CHECK_FCT(expect(ctx, "[", 1));

    CHECK_FCT(parse_expression(ctx, node));

    CHECK_FCT(expect(ctx, "]", 1));

    return V3_OK;
}

// parse .foo
static v3_int_t
parse_non_computed_member(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_int_t    rc;
    rc = expect(ctx, ".", 1);
    if (rc != V3_OK) return rc;

    return parse_non_computed_property(ctx, node);
}

static v3_int_t
parse_non_computed_property(v3_ctx_t *ctx, v3_node_t **node)
{
    v3_int_t    rc; 
    v3_token_t  *token;

    rc = lex(&ctx->tokenizer, &token);
    if (rc != V3_OK) return rc;

    if (!is_identifier_name(token)) {
        return throw_unexpected(ctx, token);
    }

    *node = (v3_node_t *)v3_create_identifier(ctx, &token->value);
    if (*node == NULL) return V3_ERROR;
    return V3_OK;
}

// Expect the next token to match the specified punctuator.
// If not, an exception will be thrown.
static v3_int_t
expect(v3_ctx_t *ctx, char *punctuator, size_t len)
{
    v3_int_t    rc; 
    v3_token_t  *token;

    rc = lex(&ctx->tokenizer, &token);
    if (rc != V3_OK) return rc;
    
    if (token->type != V3_TOKEN_Punctuator
        || (token->value.length != len 
            || strncmp(token->value.data, punctuator, len) != 0)) {
        return throw_unexpected(ctx, token);
    }

    return V3_OK;
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
static v3_int_t
expect_keyword(v3_ctx_t *ctx, v3_keyword_t keyword)
{
    v3_int_t    rc; 
    v3_token_t  *token;

    rc = lex(&ctx->tokenizer, &token);
    if (rc != V3_OK) return rc;

    if (token->type != V3_TOKEN_Keyword
        || token->v.keyword != keyword) {
        return throw_unexpected(ctx, token);
    }

    return V3_OK;
}

static v3_int_t
parse_new_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t    rc;
    v3_node_t   *callee;
    v3_vector_t *args = NULL;
    rc = expect_keyword(ctx, V3_KEYWORD_NEW);
    if (rc != V3_OK) return rc;

    rc = parse_left_hand_side_expr(ctx, &callee);
    if (rc != V3_OK) return rc;
    
    if (match("(")) {
        rc = parse_arguments(ctx, &args);
        if (rc != V3_OK) return rc;
    }

    *expr = (v3_node_t *)v3_create_new_expr(ctx, callee, args);
    return *expr == NULL ? V3_ERROR: V3_OK;
}

static v3_int_t 
parse_left_hand_side_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t        prev_allow_in, rc;
    v3_node_t       *property;

    prev_allow_in = ctx->state.allowin;
    rc = match_keyword(c_lookahead, V3_KEYWORD_NEW)
        ? parse_new_expr(ctx, expr)
        : parse_primary_expr(ctx, expr);
    ctx->state.allowin = prev_allow_in;

    // TODO:
    for (;;) {
        if (match(".")) {
            // eg: a.foo
            CHECK_FCT(parse_non_computed_member(ctx, &property));
            
            *expr = (v3_node_t *)v3_create_member_expr(ctx, '.', *expr, property);
            if (*expr == NULL) return V3_ERROR;
        } else if (match("[")) {
            // eg: a["foo"]["bar"]
            CHECK_FCT(parse_computed_member(ctx, &property));
            
            *expr = (v3_node_t *)v3_create_member_expr(ctx, '[', *expr, property);
            if (*expr == NULL) return V3_ERROR;
        
        } else {
            break;
        }
    }

    return rc;
}

static v3_int_t
parse_group_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t        rc;
    rc = expect(ctx, "(", 1);
    if (rc != V3_OK) return rc;

    if (match(")")) {
        rc = lex(&ctx->tokenizer, NULL);
        if (rc != V3_OK) return rc;
        *expr = &v3_arrow_parameter_place_holder;
        return V3_OK;
    }

    ctx->state.parenthesis_count++;

    // TODO:
    //rc = parse_expression(ctx, expr);
    ////if (rc != V3_OK) return rc;

    return expect(ctx, ")", 1); 
}

static v3_int_t
parse_array_init(v3_ctx_t *ctx, v3_node_t **expr)
{
    return V3_NOT_SUPPORT;
}

static v3_int_t
parse_object_init(v3_ctx_t *ctx, v3_node_t **expr)
{
    return V3_NOT_SUPPORT;
}

static v3_int_t
parse_primary_expr(v3_ctx_t *ctx, v3_node_t **expr)
{
    v3_int_t        type, rc;
    v3_token_t      *token;
    //v3_node_t       *

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
        rc = lex(&ctx->tokenizer, &token);
        if (rc != V3_OK) return rc;
        *expr = (v3_node_t *)v3_create_identifier(ctx, &token->value);
        if (expr == NULL) return V3_ALLOC_ERROR;

        return V3_OK;
    } else if (type == V3_TOKEN_StringLiteral
                || type == V3_TOKEN_NumericLiteral) {
        // a = "abc", or a = 1;
        // TODO: strict
        rc = lex(&ctx->tokenizer, &token);
        if (rc != V3_OK) return rc;

        *expr = (v3_node_t *)v3_create_literal(ctx, token);
        if (expr == NULL) return V3_ALLOC_ERROR;

        return V3_OK;
    } else if (type == V3_TOKEN_Keyword) {
        if (match_keyword(c_lookahead, V3_KEYWORD_FUNCTION)) {
            return parse_function_expr(ctx, expr);
        }
    }

    // TODO:
    return V3_NOT_SUPPORT;
}

static v3_idetifier_t *v3_create_identifier(v3_ctx_t *ctx, v3_str_t *name)
{
    v3_idetifier_t  *id;
    id = v3_palloc(ctx->pool, sizeof(v3_idetifier_t));
    if (id == NULL) return NULL;

    id->node.type = V3_SYNTAX_IDENTIFIER;
    id->name = *name;
    return id;
}

static v3_number_object_t *
v3_number_from_token(v3_ctx_t *ctx, v3_token_t *token)
{
    return v3_number_create(ctx, token->v.num);
}

static v3_literal_t*
v3_create_literal(v3_ctx_t *ctx, v3_token_t *token) 
{
    v3_literal_t    *literal;

    literal =  v3_palloc(ctx->pool, sizeof(v3_literal_t)); 
    if (literal == NULL) return NULL; 
    
    literal->node.type = V3_SYNTAX_LITERAL;

    if (token->type == V3_TOKEN_NumericLiteral) {
        literal->value = (v3_base_object_t *)v3_number_from_token(ctx, token);
        if (literal->value == NULL) return NULL;
    } else {
        return NULL;
        // TODO:
        // literal->value = v3_create_string(ctx->pool, value, token_type);
        // if (literal->value == NULL) return NULL;
    }

    // literal->value = v3_create_primitive_value(ctx->pool, value, token_type);
    literal->raw = token->value;

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

    op  = &c_lookahead->value;
    if (op->length == 1) {
        return strncmp(op->data, "=", 1) == 0;
    } else if(op->length == 2) {
        return strncmp(op->data, "*=", 2) == 0
            || strncmp(op->data, "/=", 2) == 0
            || strncmp(op->data, "%=", 2) == 0
            || strncmp(op->data, "+=", 2) == 0
            || strncmp(op->data, "-=", 2) == 0
            || strncmp(op->data, "&=", 2) == 0
            || strncmp(op->data, "^=", 2) == 0
            || strncmp(op->data, "|=", 2) == 0;
    } else if(op->length == 3) {
        return strncmp(op->data, ">>=", 3) == 0
            || strncmp(op->data, "<<=", 3) == 0;
    } else if(op->length == 4) {
        return strncmp(op->data, ">>>=", 4) == 0;
    } 

    return V3_FALSE;
}

static v3_int_t 
parse_variable_identifier(v3_ctx_t *ctx, v3_idetifier_t **node)
{
    v3_token_t  *token;
    // v3_str_t    *name;
    v3_int_t    rc;

    rc = lex(&ctx->tokenizer, &token);
    if (rc != V3_OK) return rc;

    if (token->type != V3_TOKEN_Identifier) {
        return throw_unexpected(ctx, token);    
    }

    *node = v3_palloc(ctx->pool, sizeof(v3_idetifier_t));
    if (*node == NULL) return V3_ALLOC_ERROR;
    
    (*node)->node.type = V3_SYNTAX_IDENTIFIER;
    (*node)->name = token->value;

    return V3_OK;
}

static v3_int_t 
consume_semicolon(v3_ctx_t *ctx)
{
    v3_int_t        line;
    if (match(";")) {
        return lex(&ctx->tokenizer, NULL);
    }

    line = ctx->tokenizer.lineNumber;
    skipComment(&ctx->tokenizer);
    if (ctx->tokenizer.lineNumber != line) {
        return V3_OK;
    }

    if (c_lookahead->type != V3_TOKEN_EOF && !match("}")) {
        return V3_EUNEXPETED_TOKEN;
    }
    
    return V3_OK;
}


static v3_int_t throw_error(v3_ctx_t *ctx, const char *msg, int rc)
{
    // TODO:
    ctx->err = msg;
    return rc;
}

static v3_int_t throw_unexpected(v3_ctx_t *ctx, v3_token_t *token)
{
    // TODO:
    //ctx->err = msg;
    return V3_UNEXPEDTED_TOKEN;
}
