#ifndef _V3_BASE_TYPE_H
#define _V3_BASE_TYPE_H 1

typedef enum {
    V3_DATA_TYPE_BOOL,
    V3_DATA_TYPE_NUMBER,
    V3_DATA_TYPE_STRING,
    V3_DATA_TYPE_OBJECT,
    V3_DATA_TYPE_FUNCTION,
    V3_DATA_TYPE_NULL,
    V3_DATA_TYPE_UNDEFINED,
} v3_data_type;

typedef struct v3_base_object_s v3_base_object_t;
typedef struct v3_number_object_s v3_number_object_t;
typedef struct v3_string_object_s v3_string_object_t;
typedef struct v3_function_object_s v3_function_object_t;
typedef struct v3_object_s v3_object_t;

struct v3_base_object_s {
    v3_data_type            type;
    v3_object_t             *__proto__;
};

#include <v3_base_object.h>
#include <v3_object.h>
#include <v3_number_object.h>
#include <v3_string_object.h>
#include <v3_function_object.h>

#endif
