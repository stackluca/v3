#ifndef _V3_PARSER_H
#define _V3_PARSER_H 

typedef enum {
    V3_SYNTAX_VARIABLE_DECLARATION,
    V3_SYNTAX_VARIABLE_DECLARATOR,
    V3_SYNTAX_MEMBER_EXPR,
    V3_SYNTAX_PROGRAM,
} v3_syntax_t;

// var a=1, b=2
typedef struct {
    v3_node_t   node;
    v3_str_t    kind;
    v3_vector_t *declarations; 
} v3_variable_statement_t;

typedef struct {
    v3_node_t   node;
    v3_vector_t *body; 
} v3_program_node_t;

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

v3_int_t 
v3_parse(const char *code, size_t len, v3_program_node_t **program);
#endif
