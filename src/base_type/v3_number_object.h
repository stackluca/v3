#ifndef _V3_NUMBER_OBJECT_H
#define _V3_NUMBER_OBJECT_H 

#include <v3_core.h>
#include <v3_base_type.h>

#define V3_NUMBER_POOL_MAX      100
struct v3_number_object_s {
    v3_base_object_t    base;
    double              value;
};

extern v3_object_t  *Number_prototype;
extern v3_int_t v3_init_Number(v3_ctx_t *ctx);
extern v3_number_object_t *v3_number_create(v3_ctx_t *ctx, double num);
extern v3_number_object_t v3_number_pool[];

static inline void v3_number_init(v3_ctx_t *ctx, v3_number_object_t *num)
{
    num->base.type = V3_DATA_TYPE_NUMBER;
    num->base.__proto__ = Number_prototype; //v3_object_get("prototype", ctx->globals);
    num->base.ref_count = 0;
    num->value = 0;
}

#define v3_numobj(num) v3_number_create(ctx, num)

#endif
