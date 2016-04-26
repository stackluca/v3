#ifndef _V3_FUNCTION_OBJECT_H
#define _V3_FUNCTION_OBJECT_H 1

#include <v3_core.h>
#include <v3_base_type.h>

typedef struct {
    size_t          len; 
    v3_vector_t     *args;
} v3_arguments_t;

typedef v3_base_object_t* (*v3_func_pt)(v3_ctx_t *ctx, v3_base_object_t *self, v3_arguments_t *args);

struct v3_function_object_s {
    v3_object_t         base;
    v3_str_t            name;
    size_t              length;
    // v3_string_object_t  name;
    // v3_number_object_t  length;
    // constructor         *v3_function_object_t;
    v3_object_t         *prototype;
    unsigned int        is_native;
    // v3_function_block_t function;
    v3_func_pt          *native_func;
};

extern v3_function_object_t *
v3_create_native_func(v3_ctx_t *ctx, char *name, size_t arg_count, v3_func_pt *pt);
#endif
