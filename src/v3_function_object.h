#ifndef _V3_FUNCTION_OBJECT_H
#define _V3_FUNCTION_OBJECT_H 1

#include <v3_core.h>

typedef struct {
    v3_object_t         *__proto__;
    v3_string_object_t  name;
    v3_number_object_t  length;
    constructor         *v3_function_object_t;
    v3_object_t         prototype;
    unsigned int        is_native;
    v3_function_block_t function;
    v3_func_pt          *native_func;
} v3_function_object_t;

typedef struct {
    size_t          len; 
    v3_vector_t     *args;
} v3_arguments_t;

v3_base_object_t (*v3_func_pt)(v3_ctx_t *ctx, v3_base_object_t *self, v3_arguments_t *args);

extern v3_function_object_t 
v3_create_native_func(v3_ctx_t *ctx, char *name, size_t arg_count, v3_func_pt *pt);
#endif
