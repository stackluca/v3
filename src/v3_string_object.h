#ifndef _V3_STRING_OBJECT_H
#define _V3_STRING_OBJECT_H 

typedef struct {
    v3_base_object_t    base;
    size_t              len;
    char*               data;
} v3_string_object_t;

static inline v3_string_object_init(v3_string_object_t *str)
{
    str->base.type = V3_TYPE_STRING; 
    str->len = 0;
    str->data = NULL;
}
#endif
