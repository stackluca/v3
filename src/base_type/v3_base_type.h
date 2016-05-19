#ifndef _V3_BASE_TYPE_H
#define _V3_BASE_TYPE_H 1

typedef enum {
    V3_DATA_TYPE_BOOL = 0,
    V3_DATA_TYPE_NUMBER = 1,
    V3_DATA_TYPE_STRING = 2,
    V3_DATA_TYPE_OBJECT = 3,
    V3_DATA_TYPE_FUNCTION = 4,
    V3_DATA_TYPE_NULL = 5,
    V3_DATA_TYPE_UNDEFINED = 6,
    V3_DATA_TYPE_VECTOR = 7,
    V3_DATA_TYPE_REF = 8,
    V3_DATA_TYPE_STATMENT_RESULT = 9,
} v3_data_type;

#define v3_is_base_type(value)  \
   ((value)->type == V3_DATA_TYPE_BOOL \
    || (value)->type == V3_DATA_TYPE_NUMBER \
    || (value)->type == V3_DATA_TYPE_NULL \
    || (value)->type == V3_DATA_TYPE_UNDEFINED \
    || (value)->type == V3_DATA_TYPE_STRING)

#define v3_is_js_type(value)  \
    (v3_is_base_type(value)  \
        || value->type == V3_DATA_TYPE_OBJECT \
        || value->type == V3_DATA_TYPE_FUNCTION)

typedef struct v3_base_object_s v3_base_object_t;
typedef struct v3_number_object_s v3_number_object_t;
typedef struct v3_string_object_s v3_string_object_t;
typedef struct v3_function_object_s v3_function_object_t;
typedef struct v3_object_s v3_object_t;

struct v3_base_object_s {
    v3_data_type            type;
    v3_int_t                ref_count;
    v3_object_t             *__proto__;
};

#include <v3_base_object.h>
#include <v3_undefined.h>
#include <v3_null.h>
#include <v3_bool.h>
#include <v3_object.h>
#include <v3_function_object.h>
#include <v3_number_object.h>
#include <v3_string_object.h>
#include <v3_reference.h>
#include <v3_vector.h>

#endif
