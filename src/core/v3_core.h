#ifndef _V3_FOUNDATION_H
#define _V3_FOUNDATION_H 1

typedef struct v3_options_s v3_options_t;
typedef struct v3_vector_s  v3_vector_t;
typedef struct v3_vector_s  *v3_vector_pt;
typedef struct v3_pool_s    *v3_pool_t;
typedef struct v3_dict_s    v3_dict_t;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <v3_pool.h>
#include <v3_string.h>
#include <v3_vector.h>
#include <v3_dict.h>

typedef enum {
    V3_DATA_TYPE_BOOL,
    V3_DATA_TYPE_NUMBER,
    V3_DATA_TYPE_STRING,
    V3_DATA_TYPE_OBJECT,
    V3_DATA_TYPE_FUNCTION,
    V3_DATA_TYPE_NULL,
    V3_DATA_TYPE_UNDEFINED,
} v3_data_type;

typedef struct {
    int     type;
} v3_node_t;

typedef struct {
    
} v3_program_node_t;

typedef struct v3_object_s v3_object_t;

typedef struct {
    v3_data_type            type;
    v3_object_t             *__proto__;
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
typedef int v3_bool_t;

#define V3_OK 0
#define V3_ERROR -1
#define V3_OUT_OF_MEMORY -2
#define V3_UNEXPEDTED_TOKEN -3
#define V3_ALLOC_ERROR -4
#define V3_NOT_SUPPORT -100

#define V3_FALSE 0
#define V3_TRUE  1
#endif // _V3_FOUNDATION_H 

