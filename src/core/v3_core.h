#ifndef _V3_CORE_H
#define _V3_CORE_H 1

#define V3_DEBUG

#define CHECK_FCT2(__call__, __return__) { \
    v3_int_t    __rc__; \
    __rc__ = __call__; \
    if (__rc__ != V3_OK) { \
        return __return__; \
    }\
}

#define CHECK_FCT(__call__) { \
    v3_int_t    __rc__; \
    __rc__ = __call__; \
    if (__rc__ != V3_OK) { \
        return __rc__; \
    }\
}

typedef struct v3_options_s v3_options_t;
typedef struct v3_vector_s  v3_vector_t;
typedef struct v3_vector_s  *v3_vector_pt;
typedef struct v3_pool_s    *v3_pool_t;
typedef struct v3_dict_s    v3_dict_t;
typedef int v3_int_t;
typedef unsigned int v3_uint_t;
typedef int    v3_bool_t;
typedef struct v3_tokenizer_s v3_tokenizer_t;
typedef struct v3_ctx_s     v3_ctx_t;
typedef struct v3_error_object_s    v3_error_object_t;
typedef struct v3_program_node_s v3_program_node_t;
typedef struct v3_function_node_s v3_function_node_t;
extern v3_int_t parse_program(v3_ctx_t *ctx, v3_program_node_t **node);
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern v3_int_t v3_parse(v3_ctx_t *ctx, const char *code, size_t len, v3_program_node_t **program);
extern const char *Msg_InvalidLHSInAssignment;
void* v3_palloc_wrapper(void *userdata, size_t size);
void v3_pdealloc_wrapper(void *userdata, void *value);

#include <v3_pool.h>
#include <v3_string.h>
#include <v3_vector.h>
#include <v3_dict.h>
#include <v3_list.h>
#include <v3_foundation.h>
#include <v3_reference.h>
#include <v3_base_type.h>
#include "../v3_tokenizer.h"

typedef enum {
    V3_SYNTAX_ASSIGNMENT_EXPR = 0,
    V3_SYNTAX_CALL_EXPR = 1,
    V3_SYNTAX_IDENTIFIER = 2,
    V3_SYNTAX_LITERAL = 3,
    V3_SYNTAX_MEMBER_EXPR = 4,
    V3_SYNTAX_PROGRAM = 5,
    V3_SYNTAX_SEQUENCE_EXPR = 6,
    V3_SYNTAX_VARIABLE_DECLARATION = 7, // var a=a, b=2
    V3_SYNTAX_VARIABLE_DECLARATOR = 8,  // a=1
    V3_SYNTAX_EXPR_STATEMENT = 9,   // 
    V3_SYNTAX_NEW_EXPR = 10,
    V3_SYNTAX_BLOCK_STATEMENT = 11,
    V3_SYNTAX_FUNCTION_DECLARATION = 12, // function statement, eg: var function foo(){}
    V3_SYNTAX_FUNCTION_EXPR = 13, // functin expr, eg: var foo = function(){}
    V3_SYNTAX_RETURN_STATEMENT = 13, 
} v3_syntax_t;

typedef enum {
    V3_KEYWORD_IF = 1, 
    V3_KEYWORD_IN,
    V3_KEYWORD_DO,
                    
    V3_KEYWORD_VAR,
    V3_KEYWORD_FOR,
    V3_KEYWORD_NEW,
    V3_KEYWORD_TRY,
    V3_KEYWORD_LET,
                    
    V3_KEYWORD_THIS,
    V3_KEYWORD_ELSE,
    V3_KEYWORD_CASE,
    V3_KEYWORD_VOID,
    V3_KEYWORD_WITH,
    V3_KEYWORD_ENUM,

    V3_KEYWORD_WHILE,
    V3_KEYWORD_BREAK,
    V3_KEYWORD_CATCH,
    V3_KEYWORD_THROW,
    V3_KEYWORD_CONST,
    V3_KEYWORD_YIELD,
    V3_KEYWORD_CLASS,
    V3_KEYWORD_SUPER,

    V3_KEYWORD_RETURN,
    V3_KEYWORD_TYPEOF,
    V3_KEYWORD_DELETE,
    V3_KEYWORD_SWITCH,
    V3_KEYWORD_EXPORT,
    V3_KEYWORD_IMPORT,

    V3_KEYWORD_DEFAULT,
    V3_KEYWORD_FINALLY,
    V3_KEYWORD_EXTENDS,

    V3_KEYWORD_FUNCTION,
    V3_KEYWORD_CONTINUE,
    V3_KEYWORD_DEBUGGER,

    V3_KEYWORD_INTANCEOF,
} v3_keyword_t;

typedef struct {
    int     type;
} v3_node_t;

struct v3_program_node_s {
    v3_node_t   node;
    v3_vector_t *body; 
};

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
    v3_vector_t *arguments; /** item is assigment expr */
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

typedef struct {
    v3_node_t       node;
    v3_vector_t     *body;
} v3_block_statement_t;

struct v3_function_node_s {
    v3_node_t               node;
    v3_idetifier_t          *id;
    v3_vector_t             *params;
    v3_block_statement_t    *body;
    //v3_list_t               *scope_chain;
};
typedef void *(*v3_alloc_pt)(void *userdata, size_t size);
typedef void (*v3_dealloc_pt)(void *userdata, void *ptr);

struct v3_options_s {
    v3_alloc_pt     alloc;
    v3_dealloc_pt   dealloc;
    void            *userdata; 
};

typedef struct {
    size_t              parenthesis_count;
    v3_vector_t         *labels;
    v3_bool_t           in_iteration;
    v3_bool_t           in_switch;
    v3_bool_t           in_function_body;
    unsigned int        allowin:1;

} v3_tokenizer_state_t;

struct v3_ctx_s {
    v3_pool_t           *pool; 
    v3_tokenizer_t      tokenizer;
    v3_options_t        *options;
    v3_object_t         *global;
    const char          *err;
    v3_error_object_t   *err_obj;
    v3_tokenizer_state_t    state;
    v3_reference_t      ret;
    v3_frame_t          *frame;
};

#define V3_OK 0
#define V3_ERROR -1
#define V3_OUT_OF_MEMORY -2
#define V3_UNEXPEDTED_TOKEN -3
#define V3_ALLOC_ERROR -4
#define V3_SYNTAX_ERROR -5
#define V3_ESYNTAX          V3_SYNTAX_ERROR
#define V3_UNDEFINED -6
/** Reference Error*/
#define V3_EREF          -7
/** Type Error */
#define V3_ETYPE          -8
#define V3_NOT_SUPPORT -100

#define V3_FALSE 0
#define V3_TRUE  1
#endif // _V3_CORE_H 

