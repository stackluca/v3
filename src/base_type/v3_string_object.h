#ifndef _V3_STRING_OBJECT_H
#define _V3_STRING_OBJECT_H 
#include <v3_base_type.h>

struct v3_string_object_s {
    v3_base_object_t    base;
    size_t              len;
    char*               data;
};

static inline void v3_string_object_init(v3_string_object_t *str)
{
    str->base.type = V3_DATA_TYPE_STRING; 
    str->len = 0;
    str->data = NULL;
}
#endif