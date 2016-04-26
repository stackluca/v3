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

v3_int_t 
v3_parse(v3_ctx_t *ctx, const char *code, size_t len, v3_program_node_t **program);
#endif
