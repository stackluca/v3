#ifndef _V3_PARSER_H
#define _V3_PARSER_H 


extern v3_str_t dec_kind_var;
// var a=1, b=2
typedef struct {
    v3_node_t   node;
    v3_str_t    kind;
    v3_vector_t *declarations; 
} v3_variable_statement_t;

// foo
typedef struct {
    v3_node_t   node;
    v3_str_t    name;
} v3_idetifier_t;

// foo=1
typedef struct {
    v3_node_t           node;
    v3_idetifier_t      *id;
    v3_node_t           *init;
} v3_variable_declarator_t;

typedef struct {
    v3_node_t   node;
    v3_base_object_t    *value;
    v3_str_t    raw;
} v3_literal_t;

typedef struct {
    v3_node_t   node;
    int         computed; /* when acceor == [ */
    char        accessor; /* [ or . */
    v3_node_t   *property;
    v3_node_t   *object;
} v3_member_expr_t;

typedef struct {
    v3_node_t   node;
    v3_node_t   *callee;
    v3_vector_t *arguments;
} v3_call_expr_t;

typedef struct {
    v3_node_t   node;
    v3_node_t   *expr;
} v3_expr_statement_t;

typedef struct {
    v3_node_t       node;
    v3_node_t       *callee;
    v3_vector_t     *args;
} v3_new_expr_t;

v3_int_t 
v3_parse(v3_ctx_t *ctx, const char *code, size_t len, v3_program_node_t **program);
#endif
