#ifndef _V3_STRING_OBJECT_H
#define _V3_STRING_OBJECT_H 
#include <v3_base_type.h>

struct v3_string_object_s {
    v3_base_object_t    base;
    v3_str_t            value;
};

static inline void v3_string_init(v3_ctx_t *ctx, v3_string_object_t *str)
{
    memset(str, 0, sizeof(*str));
    str->base.type = V3_DATA_TYPE_STRING; 
}
#endif
