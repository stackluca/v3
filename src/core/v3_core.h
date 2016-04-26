#ifndef _V3_FOUNDATION_H
#define _V3_FOUNDATION_H 1

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
#include <v3_foundation.h>
#include <v3_base_type.h>
#include "../v3_tokenizer.h"

typedef enum {
    V3_SYNTAX_VARIABLE_DECLARATION,
    V3_SYNTAX_VARIABLE_DECLARATOR,
    V3_SYNTAX_MEMBER_EXPR,
    V3_SYNTAX_PROGRAM,
    V3_SYNTAX_IDENTIFIER,
    V3_SYNTAX_LITERAL,
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

typedef void *(*v3_alloc_pt)(void *userdata, size_t size);
typedef void (*v3_dealloc_pt)(void *userdata, void *ptr);

struct v3_options_s {
    v3_alloc_pt     alloc;
    v3_dealloc_pt   dealloc;
    void            *userdata; 
};

struct v3_ctx_s {
    v3_pool_t           *pool; 
    v3_tokenizer_t      tokenizer;
    v3_options_t        *options;
    v3_object_t         *global;
    const char          *err;
    v3_error_object_t   *err_obj;
};

#define V3_OK 0
#define V3_ERROR -1
#define V3_OUT_OF_MEMORY -2
#define V3_UNEXPEDTED_TOKEN -3
#define V3_ALLOC_ERROR -4
#define V3_SYNTAX_ERROR -5
#define V3_UNDEFINED -6
#define V3_NOT_SUPPORT -100

#define V3_FALSE 0
#define V3_TRUE  1
#endif // _V3_FOUNDATION_H 

