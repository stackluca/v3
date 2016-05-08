#include <math.h>
#include <v3_core.h>
#include "v3_number_object.h"

v3_number_object_t       v3_number_pool[V3_NUMBER_POOL_MAX];

v3_number_object_t *v3_number_create(v3_ctx_t *ctx, double num)
{
    v3_number_object_t  *anum;

    if (num < V3_NUMBER_POOL_MAX
        && round(num) == num) {
        return &v3_number_pool[(int)num];
    }

    anum = v3_palloc(ctx->pool, sizeof(*anum));
    if (anum == NULL) return NULL;

    v3_number_init(ctx, anum);
    anum->value = num;
    return anum;
}
