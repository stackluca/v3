#ifndef _V3_OBJECT_H
#define _V3_OBJECT_H 

#include <v3_core.h>
#include <v3_base_type.h>

extern v3_object_t  Object_prototype;

struct v3_object_s {
    v3_base_object_t    base; 
    v3_dict_t           *__attrs__;
};

v3_int_t v3_object_init(v3_ctx_t *ctx, v3_object_t *obj, size_t capacity);

extern void v3_init_Object(v3_ctx_t *ctx);
// key could be number_object (used in array) and string_object
void v3_objec_set(v3_ctx_t *ctx, v3_object_t* obj, v3_str_t *key, v3_base_object_t *value);
#endif
