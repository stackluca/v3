#ifndef _V3_STRING_OBJECT_H
#define _V3_STRING_OBJECT_H 
#include <v3_base_type.h>

struct v3_string_object_s {
    v3_base_object_t    base;
    v3_str_t            value;
};

#define to_string(value) ((v3_string_object_t *)(value))
// extern v3_object_t  *String_prototype;

v3_string_object_t *v3_string_create(v3_ctx_t *ctx, const char *value, size_t len);
v3_bool_t v3_strobj_equal(v3_string_object_t *str1, v3_string_object_t *str2);

static inline void v3_string_init(v3_ctx_t *ctx, v3_string_object_t *str)
{
    memset(str, 0, sizeof(*str));
    str->base.type = V3_DATA_TYPE_STRING; 
    // str->base.__proto__ = String_prototype;
}

#define v3_strobj(str) v3_string_create(ctx, str, sizeof(str) - 1)
#define v3_str2string(str) v3_string_create(ctx, (str)->data, (str)->length)
#endif
