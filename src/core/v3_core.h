#ifndef _V3_FOUNDATION_H
#define _V3_FOUNDATION_H 1

typedef struct v3_options_s v3_options_t;
typedef struct v3_vector_s  v3_vector_t;
typedef struct v3_vector_s  *v3_vector_pt;
typedef struct v3_pool_s    *v3_pool_t;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <v3_pool.h>
#include <v3_string.h>
#include <v3_vector.h>

typedef struct {
    int     type;
} v3_node_t;

typedef struct {
    
} v3_program_node_t;

typedef struct {

} v3_base_object_t;

typedef struct {
    void    *result;
} v3_expr_t;

typedef void *(*v3_alloc_pt)(void *userdata, size_t size);
typedef void (*v3_dealloc_pt)(void *userdata, void *ptr);

typedef struct v3_options_s {
    v3_alloc_pt alloc;
    v3_dealloc_pt dealloc;
    void *userdata; 
} v3_options_t;

typedef int v3_int_t;
typedef unsigned int v3_uint_t;

#define V3_OK 0
#define V3_ERROR -1

#define V3_FALSE 0
#define V3_TRUE  1
#endif // _V3_FOUNDATION_H 

